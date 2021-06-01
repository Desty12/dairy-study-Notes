#ifndef GAUTO_LAYOUT_H
#define GAUTO_LAYOUT_H

#include <QVector>
#include <QPair>
#include <QPolygonF>
#include <QImage>
#include "gnest_global.h"

// 允许90度旋转零件
#define		LAYOUT_OPT_ROTATE90		0x0001
// 允许180度旋转零件
#define		LAYOUT_OPT_ROTATE180	0x0002
// 允许270度旋转零件
#define		LAYOUT_OPT_ROTATE270	0x0004

// 是否允许计算零件的OOBBox, 并且旋转零件
#define		LAYOUT_OPT_OOBB_ROT		0x0008

// X方向优先排样(默认)
#define		LAYOUT_OPT_ORDER_BY_X	0x0010
// Y方向优先排样
#define		LAYOUT_OPT_ORDER_BY_Y	0x0020

// 零件
class GNEST_EXPORT GPart: public QPolygonF
{
public:
	int						mSortIndex;
	int						mPartID;		// 零件ID
	int						mPageIndex;		// 母版下标
	QVector<QPolygonF>		mHoles;			// 零件的内孔多边形
	QMatrix					mTransMat;		// 变换矩阵
	QPointF					mCenter;		// 零件中心
	qint32					mLayoutFlag;	// 排样旗标参数
	int						mPriority;		// 优先级

	int						mSubGroupIndex;	// 零件聚类分小组的类别号
	int						mGroupIndex;	// 零件聚类分大组的类别号

	GPart();

	void cloneGeometry(const GPart &other);
};

// 自动排版
class GNEST_EXPORT GAutoLayout
{
public:
	// 排样方向
	enum LayoutDirect {LEFT_TOP, LEFT_BOTTOM, RIGHT_TOP, RIGHT_BOTTOM};
	typedef QList< QVector< QPair<int, int> > > LMask;//线性模板

	struct PartTemplate
	{
		int					mPartIndex;			// 零件下标

		QVector< QSize >	mMaskSizeList;
		QList< GPart >		mPolygonList;		// 记录各种姿态的多边形
		//QList< QPolygon >	mMaskPosList;		// 与mPolygonList对应的掩码表
		QVector< QMatrix >	mTransMats;			// 与各种姿态多边形对应的变换矩阵
		QList< LMask >		mLMaskList;			// x轴对应四个方向的线性掩码
		QList< LMask >      mLMaskListY;		// y轴对应四个方向的线性掩码

		int					mPriority;			// 优先级
		int					mMaskArea;			// 掩码面积
		double				mPolyArea;			// 多边形面积
		//double				mRatio;				// 有效面积占比
		double				mAspect;			// 长宽比
		long				mPerimeter;			// 多边形周长

		PartTemplate();
		~PartTemplate();
	};

	struct PageTemplate
	{
		int			mPageMaskIndex;		// 对应的母版下标
		int			mPageIndex;			// 页面下标
		//bool **		mMask;				// 页面掩码
		int			mRemains;			// 剩余可用网格
		int			mMaxWidth;
		int			mMaxHeight;
		LMask		mLMaskX;			// X方向线性掩码
		LMask		mLMaskY;			// y方向线性掩码

		LayoutDirect	mLayoutDirect;	// 排样方向
		QVector<int>	mForwardLine;	// 前锋线数组(记录每行使用的最大下标)

		PageTemplate();
		~PageTemplate();
	};

	struct EvaluateResult
	{
		int mRemains;		//剩余空间
		int mForwardLine;	//前锋线
		int mNeatDegrees;	//齐整度
		bool mIsLay;
		int mMaxWidth;
		int mMaxHeight;
		QVector<int> mForLine;
		QPoint mPos;	//放置点

		EvaluateResult();
		~EvaluateResult();
	};

	struct PageMask
	{
		//bool **			mMask;					// 母版掩码
		LMask			mLMask;					// 母版线性掩码
		double			mPageWidth;
		double			mPageHeight;
		double			mPageX0, mPageY0;		// 母板平移量
		int				mPageNumX;
		int				mPageNumY;

		PageMask();
		~PageMask();
	};


private:
	QVector<GPart>				mPagePlates;			// 母版表
	QVector<int>				mPageNums;				// 母版数量
	QVector<int>				mUsedPageNums;			// 已经使用的母版数量
	QVector<PageMask>			mPageMasks;				// 母版掩码
	double						mGridDeltaX;
	double						mGridDeltaY;
	quint32						mLayoutFlag;			// 排样旗标参数
	QVector< GPart >			mParts;					// 输入零件
	QVector< int >				mPartNums;				// 零件数量
	QVector< PartTemplate * >	mPartTemplates;
	bool						mIsSelfPartTemplates;	// 零件模板表是否为自己创建		
	QVector< PageTemplate * >	mPageTemplates;
	bool						mIsAbort;				// 是否中断排样
	QVector< quint32 >			mRotFlags;				// 旋转限制标记

	QVector<QPair<int, int> >    mGroupIndices;			// 聚类大组信息
	QVector<QPair<int, int> >	 mSubGroupIndices;		// 聚类小组信息
	int						     mFirstPageMaxHeight;	// 记录排版后第一页的最低高度


protected:

	// 对多边形进行坐标变换
	void transform(GPart &poly, double &w, double &h, QMatrix &matrix);

	//为多边形创建Lmask结构
	void createPolyLMask(const QImage &maskImage, LMask &lmask, LMask &lmaskY, int &nmx, int &nmy);

	// 为多边形创建PolyMask结构
	PartTemplate * createPolyMask(GPart &poly, double w, double h, const QMatrix &matrix);

	// 多母版进行预处理
	void preprocessPage();

	// 在指定母版上排放一个多边形
	void putPoly(const QPoint &pos, PartTemplate *pPolyTemp, int tempIndex, PageTemplate *pPageTemp, GPart *pPart);

	bool layoutPoly(int iPage, PartTemplate *pPolyTemp, GPart *pPart);

	// 添加一个新Page
	void addPage(int pageMaskIndex);

	// 计算前锋线参数
	void calcForwardLineParam(int nx, int ny, const QVector<int> &forwardLine, int &remains, int &len);

	// 计算垂直方向前锋线长度
	int calcYForwardLineLen(const LMask &pageLMask, int nx);

	//不同方向排样
	void tryLayout(PartTemplate *pPolyTemp, int k, PageTemplate *pPageTemp,
		quint32 layoutOrderconst, const int &mPageNumX, const int &mPageNumY, EvaluateResult &result);


public:
	GAutoLayout();
	~GAutoLayout();

	void clear();

	quint32 getLayoutFlag() const;

	void setLayoutFlag(quint32 flag);

	// 得到零件模板表
	const QVector< PartTemplate * > & getPartTemplates() const;

	// 设置零件模板表
	void setPartTemplates(const QVector< PartTemplate * > &partTemplates);

	// 对零件进行预处理
	void preprocess();

	// 计算零件重心点
	void calcPartsCenter(QVector< GPart > &parts);

	// 设置排样参数
	void setLayoutParam(const QVector<GPart> &pagePlates, 
						const QVector<int> &pageNums,
						double gridDelta,
						const QVector< GPart > &parts,
						const QVector< int > &partNums);

	// 自动排样
	// pagePoly:		母版多边形
	// pageHoles:		母版孔洞多边形
	// gridDelta:		排样扫描间隔
	// parts:			排样零件列表
	// priorityOrder:	优先排样零件的序号表(算法将按该表中出现的零件序号顺序进行优先排样)
	// layoutPolygons:	排样后的零件表
	// layoutPages:		排样后零件所在的母版序号
	// pageNum:			排样所需的母版页数
	void layout(const QVector<int> &orders,
		const QVector<quint32> &rots,
		QVector< GPart > &nestParts,
		QVector<int> &usedPageNums,
		QVector<int> &plateIndices,
		double &nestRating, int &lastForwardLineLen,
		int *pProgress);

	void abort();

	// 设置聚类大分组下标(用于测试)
	void setGroupIndices(const QVector<QPair<int, int> > &indices);

	// 设置聚类小分组下标(用于测试)
	void setSubGroupIndices(const QVector<QPair<int, int > > &indices);

	// 获得第一页排版的高度
	int getFirstPageMaxHeight() const;
};

#endif
