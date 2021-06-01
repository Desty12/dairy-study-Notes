#pragma once

#include <QVector>
#include "gautolayout.h"

class GPartSort
{
protected:
	quint32											mLayoutFlag;
	const QVector< GPart > *						mpParts;
	const QVector< GAutoLayout::PartTemplate * > *	mpPartTemplates;
	const QVector<GPart> *						    mpPagePlates;
	const QVector<int>	*							mpageNums;
	const QVector< int > *						    mpPartNums;
	double											mpGridDelta;
	QVector<QList<QVector<int> > >					mClusterGroups;		// 保存最终的聚类分组信息

	QVector<QPair<int, int> >						mClusterBGroups;	//保存大组数据，第一个值表示零件下标，第二个值表示分组类别
	QVector<QPair<int, int> >						mClusterSGroups;	//保存小组数据，第一个值表示零件下标，第二个值表示分组类别

public:
	GPartSort();
	virtual ~GPartSort();

	void setLayoutParam(quint32 layoutFlag, const QVector< GPart > *pParts,
						const QVector< GAutoLayout::PartTemplate * > *pPartTemplates,
						const QVector<GPart>	*pPagePlates,
						const QVector<int>      *pageNums,
						const QVector< int >	*pPartNums,
						double					gridDeltaX
						);
	
	virtual QVector<int> sort();

	const QVector<QList<QVector<int> > > &getClusterGroupDatas() const;

	const QVector<QPair<int, int> > &getClusterGGroups() const;	// 聚类大组信息对外开放

	const QVector<QPair<int, int> > &getClusterSGroups() const;	// 聚类小组信息对外开放
};

