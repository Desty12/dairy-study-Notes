#ifndef GLAYOUT_OPTIMIZER_H
#define GLAYOUT_OPTIMIZER_H

#include <QVector>
#include <QHash>
#include <QVariant>
#include "gautolayout.h"

class GLayoutOptimizer
{
protected:
	const QVector<GPart> *								mpPagePlates;		// 母版表指针
	const QVector<int> *								mpPageNums;			// 母版数量表指针
	const QVector<GPart> *								mpParts;			// 零件表指针
	const QVector<int> *								mpPartNums;			// 零件数量表指针
	const QVector<GAutoLayout::PartTemplate *> *		mpPartTemplates;	// 零件模板表指针
	double												mGridDelta;			// 排样网格间距
	QVector<QList<QVector<int> > >						mClusterGroups;		// 聚类后零件号分组
	quint32												mLayoutFlag;		// 排样标志
	QHash<QString, QVariant>							mOptParams;			// 优化参数
	bool												mIsAbort;			// 是否中断优化排样

	QHash<QVector<int>, double>							mRecordLayoutSeQ;	// 记录全部排版序列和排版率
	QList<QVector<quint32> >							mFeasibleRots;		// 记录所有零件的可旋转角度
	QHash<QVector<int>, int >							mMarkSeqHashs;		// 记录初始种群的零件序列，如果零件都相同就重新初始序列


	//////  优化构成中需要用到的工具函数 ///////////////

	// 选择需要调整的零件大组
	// isHashFind为true表示生成的零件需要在hash表中查找序列是否重复，为false表示不需要执行查找
	void changeGroupsDatas(QVector<QList<QVector<int> > > &aimSequence, int changeNum, quint32 mark, bool isHashFind);

	// 选择需要调整的零件小组进行调整
	void changSmallGroupsData(QVector<int> &tmpList, quint32 mark);

	// 改变大组内的小组前后顺序
	void changeSmallGroupsOrders(QVector<QList<QVector<int> > > &tempSeQ);

	// 随机调整小组内的零件序列
	void randomChangeData(QVector<int> &arr1);

	// 随机选择组内两个数据进行改变
	void changeTwoData(QVector<int> &arr);

	// 对组内数据进行微调
	void fewChangeData(QVector<int> &arr);

	// 在寻优算法内执行萤火虫算法
	double layout(QVector<QList<QVector<int> > >  layoutSeq, QVector< GPart > &tmpNestParts, QVector<int> &tmpUsedPageNums, QVector<int> &tmpPlateIndices);

	// 三层分组零件序列转换为一层零件序列
	void  groupsSeq2Seq(const QVector<QList<QVector<int> > > &sequence, QVector<int> &orders);

	// 随机选择一个小组进行组内零件乱序
	void  smallGroupSeqAdjust(QVector<QList<QVector<int> > >  layoutSeq);

public:
	GLayoutOptimizer();
	virtual ~GLayoutOptimizer();

	// 设置排样参数
	void setLayoutParam(quint32 layoutFlag,
		const QVector<GPart> *	pPagePlates,
		const QVector<int> *	pPageNums,
		const QVector<GPart> *	pParts,
		const QVector<int> *	pPartNums,
		const QVector< GAutoLayout::PartTemplate * > *pPartTemplates,
		double					gridDelta);

	// 设置聚类分组
	void setClusterGroup(const QVector< QList< QVector<int> > > &	clusterGroups);

	// 设置优化参数
	void setOptimizeParam(const QHash<QString, QVariant> &params);

	// 执行优化算法
	virtual void optimize(int maxIter,			// 最大迭代次数
		double maxLayoutRate,					// 最大排版率
		QVector< GPart > &nestParts,			// 排样零件
		QVector<int> &usedPageNums,				// 板材使用数量
		QVector<int> &plateIndices,				// 板材索引
		double &layoutRate,						// 排版率
		int *pProgress);						// 排版进度

	void abort();
};
#endif