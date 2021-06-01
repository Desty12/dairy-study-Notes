#include "gpartsort.h"

GPartSort::GPartSort()
{
	mLayoutFlag = 0;
	mpPartTemplates = NULL;
}

GPartSort::~GPartSort()
{
}

void GPartSort::setLayoutParam(quint32 layoutFlag,const QVector< GPart > *pParts, 
							   const QVector< GAutoLayout::PartTemplate * > *pPartTemplates,
							   const QVector< GPart >	   *pPagePlates,
							   const QVector< int >        *pageNums,
							   const QVector< int >	       *pPartNums,
							   double	gridDelta)
{
	mLayoutFlag = layoutFlag;
	mpParts = pParts;
	mpPartTemplates = pPartTemplates;
	mpPagePlates = pPagePlates;
	mpageNums = pageNums;
	mpPartNums = pPartNums;
	mpGridDelta = gridDelta;
}

bool polyMaskLessThan(GAutoLayout::PartTemplate *pMask1, GAutoLayout::PartTemplate *pMask2)
{
	double a1, a2;
	a1 = pMask1->mMaskArea;
	a2 = pMask2->mMaskArea;
	//return a1 > a2;
	double c1 = 0.90;
	if (pMask1->mPriority > pMask2->mPriority) return true;
	else if (pMask1->mPriority < pMask2->mPriority) return false;

	if (c1 * a1 > a2) return true;
	else if (a1 < c1*a2) return false;
	else
	{
		return pMask1->mAspect < pMask2->mAspect;
	}
}

class PolyMaskLessThan
{
public:
	GPartSort *mpPartSort;

public:
	PolyMaskLessThan(GPartSort *pSort)
	{
		mpPartSort = pSort;
	}

	bool operator () (GAutoLayout::PartTemplate *pMask1, GAutoLayout::PartTemplate *pMask2) const
	{
		if (pMask1->mPriority > pMask2->mPriority) return true;
		else if (pMask1->mPriority < pMask2->mPriority) return false;

		int w1, w2, h1, h2;
		w1 = pMask1->mMaskSizeList[0].width();
		h1 = pMask1->mMaskSizeList[0].height();
		w2 = pMask2->mMaskSizeList[0].width();
		h2 = pMask2->mMaskSizeList[0].height();

		double a1, a2;
		a1 = w1*h1;
		a2 = w2*h2;

		double c1 = 0.65;
		double c2 = 0.9;

		if (c1 * a1 > a2) return true;
		else if (a1 < c1*a2) return false;
		else
		{
			if (c2 * h1 > h2) return true;
			else if (h1 < c2 * h2) return false;
			else
			{
				return w1 > w2;
			}
			//return h1 < h2;
		}
	}
};

QVector<int> GPartSort::sort()
{
	// 序列先按优先级分大组，之后再根据聚类分二级大组和小组
	QVector< GAutoLayout::PartTemplate * > tmpTemplates = *mpPartTemplates;

	if (!(mLayoutFlag & LAYOUT_OPT_ROTATE90 || mLayoutFlag & LAYOUT_OPT_ROTATE270))
	{
		PolyMaskLessThan polyMaskLessThan(this);
		qSort(tmpTemplates.begin(), tmpTemplates.end(), polyMaskLessThan);
	}
	else
	{
		qSort(tmpTemplates.begin(), tmpTemplates.end(), polyMaskLessThan);
	}

	QVector<int> layoutOrders;
	layoutOrders.clear();
	for (int i = 0; i < tmpTemplates.count(); i++)
	{
		layoutOrders << tmpTemplates[i]->mPartIndex;
	}
	tmpTemplates.clear();

	mClusterGroups.clear();
	int priority = -1;
	for (int i = 0; i < layoutOrders.count(); i++)
	{
		int index = layoutOrders[i];
		int pri = mpPartTemplates->at(index)->mPriority;
		if (pri != priority)
		{
			mClusterGroups << QList< QVector<int> >();
			mClusterGroups.last().append(QVector<int>());
			priority = pri;
		}
		mClusterGroups.last().last() << index;
	}

	return layoutOrders;
}

const QVector<QList<QVector<int> > > &GPartSort::getClusterGroupDatas() const
{
	return mClusterGroups;
}

const QVector<QPair<int, int> > &GPartSort::getClusterGGroups() const
{
	return mClusterBGroups;
}

const QVector<QPair<int, int> > &GPartSort::getClusterSGroups() const
{
	return mClusterSGroups;
}