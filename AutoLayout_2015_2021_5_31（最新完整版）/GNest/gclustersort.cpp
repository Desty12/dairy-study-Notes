#include "gclustersort.h"
#include "cluster.h"

struct ClusterData
{
	double		mPolyArea;		// 零件面积
	double      mAspect;		// 零件长宽比
	double      mRatio;			// 零件包围盒占比
	double      mPerimeter;		// 零件周长

	ClusterData()
	{
		mPolyArea = 0;
		mAspect = 0;
		mRatio = 0;
		mPerimeter = 0;
	}
};

static bool polyLessThan(GAutoLayout::PartTemplate *pMask1, GAutoLayout::PartTemplate *pMask2)
{
	double a1, a2;
	a1 = pMask1->mPolyArea;
	a2 = pMask2->mPolyArea;
	if (pMask1->mPriority > pMask2->mPriority) return true;
	else if (pMask1->mPriority < pMask2->mPriority) return false;

	if (a1 > a2) return true;
	else return false;
}

QVector<int> GClusterSort::sort()
{
	QVector< QList< QVector<int> > > groupSeq;
	sort(groupSeq);
	mClusterGroups = groupSeq;

	QVector<int> sequence;
	mClusterBGroups.clear();
	mClusterSGroups.clear();
	int GGroupIndex = 1;	//大组索引号
	int SGroupIndex = 1;	//小组索引号
	for (int i = 0; i < groupSeq.count(); i++)
	{
		SGroupIndex = 1;
		for (int j = 0; j < groupSeq[i].count(); j++)
		{
			sequence.append(groupSeq[i][j]);
			for (int k = 0; k < groupSeq[i][j].count(); k++)
			{
				int temp = groupSeq[i][j][k];
				mClusterBGroups.append(QPair<int, int>(temp, GGroupIndex));  	//分类大组
				mClusterSGroups.append(QPair<int, int>(temp, SGroupIndex));		//分类小组
			}
			SGroupIndex++;
		}
		GGroupIndex++;
	}

	return sequence;
}

class ClusterGroupLessThan
{
private:
	bool						mIsAsc;
	bool						mIsAdjustAspect;
	const QVector<double>		&mDatas;

protected:
	double calcGroupValue(const QVector<int> &group) const
	{
		double v = 0;
		if (group.count() <= 0) return 0;

		for (int j = 0; j < group.count(); j++)
		{
			double data = mDatas[group[j]];
			if (mIsAdjustAspect)
			{
				if (data < 1) data = 1.0 / data;
			}
			v += data;
		}
		v /= group.count();
		return v;
	}

public:
	ClusterGroupLessThan(const QVector<double> &datas, bool asc, bool isAdjustAspect=false) : mDatas(datas)
	{
		mIsAsc = asc;
		mIsAdjustAspect = isAdjustAspect;
	}

	bool operator () (const QVector<int> &group1, const QVector<int> &group2) const
	{
		double v1 = calcGroupValue(group1);
		double v2 = calcGroupValue(group2);

		if (mIsAsc) return v1 < v2;
		else return v1 > v2;
	}
};


void GClusterSort::sort(QVector< QList< QVector<int> > > &groupSeq)
{
	groupSeq.clear();
	if (mpPartTemplates->count() <= 0) return;

	QVector<double>					polyAreas;						// 保存零件面积数据
	QVector<double>					polyAspects;						// 保存零件的长宽比
	QVector<int>					partIndices;					// 记录按照面积排序之后的零件实际序列号
	QList< QVector<int> >			priGroupIndices;				// 按照零件优先权分组后的零件相对序列号
	QVector< QList< QVector<int> > >	areaClusterGroups;			// 暂存面积聚类分大组的结果
	QVector< GAutoLayout::PartTemplate * > partTemps = *mpPartTemplates;
	// 对零件按照优先级和面积进行初始排序
	qSort(partTemps.begin(), partTemps.end(), polyLessThan);

	for (int i = 0; i < partTemps.count(); i++)
	{
		polyAreas.append(partTemps[i]->mPolyArea);
		polyAspects.append(partTemps[i]->mAspect);
		partIndices << partTemps[i]->mPartIndex;
		// 设置排序后零件的下标
		partTemps[i]->mPartIndex = i;
	}

	priGroupIndices.append(QVector<int>());
	priGroupIndices.last() << 0;
	for (int i = 1; i < partTemps.count(); i++)
	{
		if (partTemps[i]->mPriority != partTemps[i - 1]->mPriority)
		{
			priGroupIndices.append(QVector<int>());
		}
		priGroupIndices.last() << i;
	}
	partTemps.clear();

	// 按照面积聚类分大组
	for (int i = 0; i < priGroupIndices.count(); i++)
	{
		Cluster cluster;
		QVector<double> areas;
		QList< QVector<int> > areaCluster;
		for (int j = 0; j < priGroupIndices[i].count(); j++)
		{
			areas << polyAreas[priGroupIndices[i][j]];
		}
		cluster.setClusterDatas(areas);
		cluster.dataCluster(areaCluster);

		ClusterGroupLessThan lessThan(areas, false);
		qStableSort(areaCluster.begin(), areaCluster.end(), lessThan);

		for (int j = 0; j < areaCluster.count(); j++)
		{
			for (int k = 0; k < areaCluster[j].count(); k++)
			{
				areaCluster[j][k] = priGroupIndices[i][areaCluster[j][k]];
			}
		}
		areaClusterGroups << areaCluster;
	}

	QVector< QList< QVector<int> > >  tmpClusterSequences;  // 暂存面积分大组后的序列结果
	
	for (int i = 0; i < areaClusterGroups.count(); i++)
	{
		for (int j = 0; j < areaClusterGroups[i].count(); j++)
		{
			tmpClusterSequences << QList< QVector<int> >();
			tmpClusterSequences.last() << areaClusterGroups[i][j];
		}
	}

	//quint32  grotFlag = LAYOUT_OPT_ROTATE90 | LAYOUT_OPT_ROTATE270;
	// 不可旋转情况下, 对面积组再划分小组
	//if (!(mLayoutFlag & grotFlag))
	{
		for (int i = 0; i < tmpClusterSequences.count(); i++)
		{
			QVector<int> &sequence = tmpClusterSequences[i][0];
			Cluster cluster;
			QVector<double> aspects;
			QList< QVector<int> > aspectCluster;
			for (int j = 0; j < sequence.count(); j++)
			{
				aspects << polyAspects[sequence[j]];
			}
			cluster.setClusterDatas(aspects);
			cluster.dataCluster(aspectCluster);

			ClusterGroupLessThan lessThan(aspects, true, false);
			qStableSort(aspectCluster.begin(), aspectCluster.end(), lessThan);

			for (int j = 0; j < aspectCluster.count(); j++)
			{
				for (int k = 0; k < aspectCluster[j].count(); k++)
				{
					aspectCluster[j][k] = sequence[aspectCluster[j][k]];
				}
			}
			tmpClusterSequences[i].clear();
			for (int j = 0; j < aspectCluster.count(); j++)
			{
				tmpClusterSequences[i] << aspectCluster[j];
			}
		}
	}

	// 将序列转换为零件下标
	for (int i = 0; i < tmpClusterSequences.count(); i++)
	{
		groupSeq << QList< QVector<int> >();
		for (int j = 0; j < tmpClusterSequences[i].count(); j++)
		{
			groupSeq[i] << QVector<int>();
			for (int k = 0; k < tmpClusterSequences[i][j].count(); k++)
			{
				int temp = tmpClusterSequences[i][j][k];
				groupSeq[i][j] << partIndices[temp];
			}
		}
	}
}