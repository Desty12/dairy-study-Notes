#ifndef SAMULATED_ANNEALING_H
#define SAMULATED_ANNEALING_H

#include "gfirefly.h"
#include "glayoutoptimizer.h"

class GSAOptimizer : public GLayoutOptimizer
{
private:
	// 定义模拟退火需要的参数
	double			maxTemperature;				// 退火最大温度
	double          minTemperature;				// 退火最低温度
	int             saPerIters;					// 每次温度一定时需要迭代的最大次数
	float		    mControlA;  				// 温度退火系数						

												// 主要用于测试
	QVector<int>	layoutOrders;			// 对外传递零件排样先后顺序

public:

	GSAOptimizer();
	~GSAOptimizer();

	// 执行模拟退火算法
	double optimize(int maxIter,			// 最大迭代次数
		double maxLayoutRate,					// 最大排版率
		QVector< GPart > &nestParts,			// 排样零件
		QVector<int> &usedPageNums,				// 板材使用数量
		QVector<int> &plateIndices,				// 板材索引
		double &layoutRate,						// 排版率
		int *pProgress);

	// 主要用于测试
	const QVector<int>	& getLayoutSequence() const;

};
#endif