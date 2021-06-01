#ifndef GAUTO_LAYOUT_H
#define GAUTO_LAYOUT_H

#include <QVector>
#include <QPair>
#include <QPolygonF>
#include <QImage>
#include "gnest_global.h"

// ����90����ת���
#define		LAYOUT_OPT_ROTATE90		0x0001
// ����180����ת���
#define		LAYOUT_OPT_ROTATE180	0x0002
// ����270����ת���
#define		LAYOUT_OPT_ROTATE270	0x0004

// �Ƿ�������������OOBBox, ������ת���
#define		LAYOUT_OPT_OOBB_ROT		0x0008

// X������������(Ĭ��)
#define		LAYOUT_OPT_ORDER_BY_X	0x0010
// Y������������
#define		LAYOUT_OPT_ORDER_BY_Y	0x0020

// ���
class GNEST_EXPORT GPart: public QPolygonF
{
public:
	int						mSortIndex;
	int						mPartID;		// ���ID
	int						mPageIndex;		// ĸ���±�
	QVector<QPolygonF>		mHoles;			// ������ڿ׶����
	QMatrix					mTransMat;		// �任����
	QPointF					mCenter;		// �������
	qint32					mLayoutFlag;	// ����������
	int						mPriority;		// ���ȼ�

	int						mSubGroupIndex;	// ��������С�������
	int						mGroupIndex;	// �������ִ��������

	GPart();

	void cloneGeometry(const GPart &other);
};

// �Զ��Ű�
class GNEST_EXPORT GAutoLayout
{
public:
	// ��������
	enum LayoutDirect {LEFT_TOP, LEFT_BOTTOM, RIGHT_TOP, RIGHT_BOTTOM};
	typedef QList< QVector< QPair<int, int> > > LMask;//����ģ��

	struct PartTemplate
	{
		int					mPartIndex;			// ����±�

		QVector< QSize >	mMaskSizeList;
		QList< GPart >		mPolygonList;		// ��¼������̬�Ķ����
		//QList< QPolygon >	mMaskPosList;		// ��mPolygonList��Ӧ�������
		QVector< QMatrix >	mTransMats;			// �������̬����ζ�Ӧ�ı任����
		QList< LMask >		mLMaskList;			// x���Ӧ�ĸ��������������
		QList< LMask >      mLMaskListY;		// y���Ӧ�ĸ��������������

		int					mPriority;			// ���ȼ�
		int					mMaskArea;			// �������
		double				mPolyArea;			// ��������
		//double				mRatio;				// ��Ч���ռ��
		double				mAspect;			// �����
		long				mPerimeter;			// ������ܳ�

		PartTemplate();
		~PartTemplate();
	};

	struct PageTemplate
	{
		int			mPageMaskIndex;		// ��Ӧ��ĸ���±�
		int			mPageIndex;			// ҳ���±�
		//bool **		mMask;				// ҳ������
		int			mRemains;			// ʣ���������
		int			mMaxWidth;
		int			mMaxHeight;
		LMask		mLMaskX;			// X������������
		LMask		mLMaskY;			// y������������

		LayoutDirect	mLayoutDirect;	// ��������
		QVector<int>	mForwardLine;	// ǰ��������(��¼ÿ��ʹ�õ�����±�)

		PageTemplate();
		~PageTemplate();
	};

	struct EvaluateResult
	{
		int mRemains;		//ʣ��ռ�
		int mForwardLine;	//ǰ����
		int mNeatDegrees;	//������
		bool mIsLay;
		int mMaxWidth;
		int mMaxHeight;
		QVector<int> mForLine;
		QPoint mPos;	//���õ�

		EvaluateResult();
		~EvaluateResult();
	};

	struct PageMask
	{
		//bool **			mMask;					// ĸ������
		LMask			mLMask;					// ĸ����������
		double			mPageWidth;
		double			mPageHeight;
		double			mPageX0, mPageY0;		// ĸ��ƽ����
		int				mPageNumX;
		int				mPageNumY;

		PageMask();
		~PageMask();
	};


private:
	QVector<GPart>				mPagePlates;			// ĸ���
	QVector<int>				mPageNums;				// ĸ������
	QVector<int>				mUsedPageNums;			// �Ѿ�ʹ�õ�ĸ������
	QVector<PageMask>			mPageMasks;				// ĸ������
	double						mGridDeltaX;
	double						mGridDeltaY;
	quint32						mLayoutFlag;			// ����������
	QVector< GPart >			mParts;					// �������
	QVector< int >				mPartNums;				// �������
	QVector< PartTemplate * >	mPartTemplates;
	bool						mIsSelfPartTemplates;	// ���ģ����Ƿ�Ϊ�Լ�����		
	QVector< PageTemplate * >	mPageTemplates;
	bool						mIsAbort;				// �Ƿ��ж�����
	QVector< quint32 >			mRotFlags;				// ��ת���Ʊ��

	QVector<QPair<int, int> >    mGroupIndices;			// ���������Ϣ
	QVector<QPair<int, int> >	 mSubGroupIndices;		// ����С����Ϣ
	int						     mFirstPageMaxHeight;	// ��¼�Ű���һҳ����͸߶�


protected:

	// �Զ���ν�������任
	void transform(GPart &poly, double &w, double &h, QMatrix &matrix);

	//Ϊ����δ���Lmask�ṹ
	void createPolyLMask(const QImage &maskImage, LMask &lmask, LMask &lmaskY, int &nmx, int &nmy);

	// Ϊ����δ���PolyMask�ṹ
	PartTemplate * createPolyMask(GPart &poly, double w, double h, const QMatrix &matrix);

	// ��ĸ�����Ԥ����
	void preprocessPage();

	// ��ָ��ĸ�����ŷ�һ�������
	void putPoly(const QPoint &pos, PartTemplate *pPolyTemp, int tempIndex, PageTemplate *pPageTemp, GPart *pPart);

	bool layoutPoly(int iPage, PartTemplate *pPolyTemp, GPart *pPart);

	// ���һ����Page
	void addPage(int pageMaskIndex);

	// ����ǰ���߲���
	void calcForwardLineParam(int nx, int ny, const QVector<int> &forwardLine, int &remains, int &len);

	// ���㴹ֱ����ǰ���߳���
	int calcYForwardLineLen(const LMask &pageLMask, int nx);

	//��ͬ��������
	void tryLayout(PartTemplate *pPolyTemp, int k, PageTemplate *pPageTemp,
		quint32 layoutOrderconst, const int &mPageNumX, const int &mPageNumY, EvaluateResult &result);


public:
	GAutoLayout();
	~GAutoLayout();

	void clear();

	quint32 getLayoutFlag() const;

	void setLayoutFlag(quint32 flag);

	// �õ����ģ���
	const QVector< PartTemplate * > & getPartTemplates() const;

	// �������ģ���
	void setPartTemplates(const QVector< PartTemplate * > &partTemplates);

	// ���������Ԥ����
	void preprocess();

	// ����������ĵ�
	void calcPartsCenter(QVector< GPart > &parts);

	// ������������
	void setLayoutParam(const QVector<GPart> &pagePlates, 
						const QVector<int> &pageNums,
						double gridDelta,
						const QVector< GPart > &parts,
						const QVector< int > &partNums);

	// �Զ�����
	// pagePoly:		ĸ������
	// pageHoles:		ĸ��׶������
	// gridDelta:		����ɨ����
	// parts:			��������б�
	// priorityOrder:	���������������ű�(�㷨�����ñ��г��ֵ�������˳�������������)
	// layoutPolygons:	������������
	// layoutPages:		������������ڵ�ĸ�����
	// pageNum:			���������ĸ��ҳ��
	void layout(const QVector<int> &orders,
		const QVector<quint32> &rots,
		QVector< GPart > &nestParts,
		QVector<int> &usedPageNums,
		QVector<int> &plateIndices,
		double &nestRating, int &lastForwardLineLen,
		int *pProgress);

	void abort();

	// ���þ��������±�(���ڲ���)
	void setGroupIndices(const QVector<QPair<int, int> > &indices);

	// ���þ���С�����±�(���ڲ���)
	void setSubGroupIndices(const QVector<QPair<int, int > > &indices);

	// ��õ�һҳ�Ű�ĸ߶�
	int getFirstPageMaxHeight() const;
};

#endif
