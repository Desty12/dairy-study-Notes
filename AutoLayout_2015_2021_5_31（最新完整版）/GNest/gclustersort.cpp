#include "gclustersort.h"
#include "cluster.h"

struct ClusterData
{
	double		mPolyArea;		// ������
	double      mAspect;		// ��������
	double      mRatio;			// �����Χ��ռ��
	double      mPerimeter;		// ����ܳ�

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
	int GGroupIndex = 1;	//����������
	int SGroupIndex = 1;	//С��������
	for (int i = 0; i < groupSeq.count(); i++)
	{
		SGroupIndex = 1;
		for (int j = 0; j < groupSeq[i].count(); j++)
		{
			sequence.append(groupSeq[i][j]);
			for (int k = 0; k < groupSeq[i][j].count(); k++)
			{
				int temp = groupSeq[i][j][k];
				mClusterBGroups.append(QPair<int, int>(temp, GGroupIndex));  	//�������
				mClusterSGroups.append(QPair<int, int>(temp, SGroupIndex));		//����С��
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

	QVector<double>					polyAreas;						// ��������������
	QVector<double>					polyAspects;						// ��������ĳ����
	QVector<int>					partIndices;					// ��¼�����������֮������ʵ�����к�
	QList< QVector<int> >			priGroupIndices;				// �����������Ȩ���������������к�
	QVector< QList< QVector<int> > >	areaClusterGroups;			// �ݴ��������ִ���Ľ��
	QVector< GAutoLayout::PartTemplate * > partTemps = *mpPartTemplates;
	// ������������ȼ���������г�ʼ����
	qSort(partTemps.begin(), partTemps.end(), polyLessThan);

	for (int i = 0; i < partTemps.count(); i++)
	{
		polyAreas.append(partTemps[i]->mPolyArea);
		polyAspects.append(partTemps[i]->mAspect);
		partIndices << partTemps[i]->mPartIndex;
		// ���������������±�
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

	// �����������ִ���
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

	QVector< QList< QVector<int> > >  tmpClusterSequences;  // �ݴ�����ִ��������н��
	
	for (int i = 0; i < areaClusterGroups.count(); i++)
	{
		for (int j = 0; j < areaClusterGroups[i].count(); j++)
		{
			tmpClusterSequences << QList< QVector<int> >();
			tmpClusterSequences.last() << areaClusterGroups[i][j];
		}
	}

	//quint32  grotFlag = LAYOUT_OPT_ROTATE90 | LAYOUT_OPT_ROTATE270;
	// ������ת�����, ��������ٻ���С��
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

	// ������ת��Ϊ����±�
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