#ifndef FIRT_FLY_H
#define FIRT_FLY_H

#include "gautolayout.h"
#include "glayoutoptimizer.h"

class FireFly : public GLayoutOptimizer 
{
public:
	struct FireWorm
	{
		QVector<QList<QVector<int> > >		sequenceGroups;		//  分组零件序号
		double								layoutRating;		//	排版率

		FireWorm();
		~FireWorm();
	};

	FireFly();
	~FireFly();

	// 主要用于测试
	const QVector<int>	& getLayoutSequence() const;

	// 父类执行萤火虫算法
	int optimize(int maxIter,					// 最大迭代次数
		double maxLayoutRate,					// 最大排版率
		QVector< GPart > &nestParts,			// 排样零件
		QVector<int> &usedPageNums,				// 板材使用数量
		QVector<int> &plateIndices,				// 板材索引
		double &layoutRate,						// 排版率
		int *pProgress);


protected:

	// 初始化萤火虫种群
	void InitFireFlies();

	// 计算萤火虫i和萤火虫j之间的海明距离
	int calDistance_i_j(const QVector<QList<QVector<int> > >  &seQuenceI, const QVector<QList<QVector<int> > > &sequenceJ, int &allDistances);

	// 计算萤火虫i和萤火虫j之间的吸引度
	double calAttraction_i_j(int rI_J, int allDistance);

	// 处理排样序列更新之后的不可行解
	// sequence为待处理的序列，orignalSequence为未处理之前的序列用于对比
	//void dealInfeasibleSequence(QVector<QList<QVector<int> > > &sequence, const QVector<QList<QVector<int> > > &orignalSequence);
	void dealInfeasibleSequence(QList<QVector<int> > &sequence, const QList<QVector<int> > &orignalSequence);

	// 计算萤火虫i向萤火虫j移动过程中，位置更新
	void updateLocationItoJ(QVector<QList<QVector<int> > > &sequenceI, const QVector<QList<QVector<int> > > &sequenceJ, double attraction);

	// 计算萤火虫i相对于萤火虫j的相对荧光亮度
	// double  calMutualLight_i_j(int distance, double MaxLight);

	// 初始化最差的萤火虫种群序列
	void	initWorstFireWormSequence(QVector<QList<QVector<int> > > &sequences, const QVector<QList<QVector<int> > > &bestSeq);

private:
	// 主要用于测试
	QVector<int>			   mLayoutOrders;			// 对外传递零件排样先后顺序
	//long long				   mDifSeqNums;				// 记录零件所能产生的不同

	QVector< FireWorm >		   mFireFlySerises;			// 存放萤火虫种群初始化之后的序列						 		 
	int                        mFireFlyNumbers;			//萤火虫种群数量
	double                     r;				   	    // 吸引系数
	double                     B0;					    // 最大吸引度
	int                        m;						// 计算两只萤火虫之间的海明距离参数值
	float                      p;					    // 编码不可行解处理系数p
	int                        MaxIterm;				// 萤火虫最大迭代次数
	double                     I0;						// 最大荧光亮度
};
#endif