#include "cluster.h"

Cluster::Cluster()
{
}


Cluster::~Cluster()
{
}


void Cluster::setClusterDatas(const QVector<double> &datas)
{
	mClusterDatas = datas;
}

void Cluster::normalize(const QVector<double> &clusterDatas, QVector<double> &normalizationResult)
{
	normalizationResult.clear();
	double maxValue = *std::max_element(std::begin(clusterDatas), std::end(clusterDatas));
	double minValue = *std::min_element(std::begin(clusterDatas), std::end(clusterDatas));

	double scale = 1;
	if ((maxValue - minValue) > 1e-6) scale = 1.0 / (maxValue - minValue);
	for (int i = 0; i < clusterDatas.count(); i++)
	{
		double v = (clusterDatas[i] - minValue) * scale;
		normalizationResult.append(v);
	}
}


bool Cluster::initCenters(int nk, const QVector<double>	&datas, QVector<double> &kCenters)
{
	double		aveRadius;	// ����������뾶
	int			minNPt;	// �������ܶ���ֵ
	int			n = datas.count();
	double		tempSum = 0;

	// ͳ�����ݵ���ƽ������
	for (int i = 0; i < datas.count(); i++)
	{
		for (int j = i+1; j < datas.count(); j++)
		{
			tempSum += 2*fabs(datas[i] - datas[j]);
		}
	}
	aveRadius = tempSum / int(n*n*1.2);

	// ͳ����ÿ������Ϊ���ģ���aveRadiusΪ�뾶��Բ�����ݵ����
	QVector<int> nearNPts(n, 0); 
	for (int i = 0; i < datas.count(); i++)
	{
		for (int j = 0; j < datas.count(); j++)
		{
			if (fabs(datas[i] - datas[j]) <= aveRadius)
			{
				nearNPts[i]++;
			}
		}
	}

	// ͳ����С�ܶ���ֵminNPt
	minNPt = 0;
	for (int i = 0; i < nearNPts.count(); i++)
	{
		minNPt += nearNPts[i];
	}
	minNPt = ceil(minNPt / (n * 3));

	// ��������и��ܶȵ������㼯
	QVector< QPair<int, int> >	hDensPts;		//��¼���ܶȵ���±꼰���Ӧ���ܶȴ�С 
	int    maxDens = 0;							// ��¼����ܶȵ�
	int    maxDensPtIdx;						// ��¼�ܶ�������±�
	for (int i = 0; i < datas.count(); i++)
	{
		if (nearNPts[i] > minNPt)
		{
			hDensPts << QPair<int, int>(i, nearNPts[i]);
			if (nearNPts[i] >= maxDens)
			{
				maxDens = nearNPts[i];
				maxDensPtIdx = i;
			}
		}
	}
	if (hDensPts.count() < nk) return false;

	// ��¼���ܶȵ��ڰ������������ܶȵ�
	QList<QVector<int> >	circlePoints; 
	for (int i = 0; i < hDensPts.count(); i++)
	{
		circlePoints.append(QVector<int>());
		for (int j = 0; j < hDensPts.count(); j++)
		{
			if (i == j) continue;
			int ip = hDensPts[i].first;
			int iq = hDensPts[j].first;
			double dist = fabs(datas[ip] - datas[iq]);
			if (dist < aveRadius)
			{
				circlePoints[i].append(j);
			}
		}
	}

	// ��ȡ���ܶȵ��о�����Զ��K�����������Ϊ��ʼ��������

	// ���ܶ����ĵ���Ϊ��1��������
	kCenters[0] = datas[maxDensPtIdx];
	// printf("��ӡ��ʼ��ʱ�ľ������ģ�%f\n",orignalCenters[0]);
	int nhd = hDensPts.count();
	QVector<bool> clusterCenterTag(nhd, false);
	for (int i = 0; i < hDensPts.count(); i++)
	{
		if (maxDensPtIdx == hDensPts[i].first)
		{
			clusterCenterTag[i] = true;
			break;
		}
	}

	double maxDistance = 0;
	int ifar = 0;

	// Ѱ�Ҿ��1����������Զ�ĵ���Ϊ��2��������
	for (int j = 0; j < nhd; j++)
	{
		if (clusterCenterTag[j]) continue;

		int ip = hDensPts[j].first;
		double dist = fabs(kCenters[0] - datas[ip]);
		if (maxDistance < dist)
		{
			maxDistance = dist;
			ifar = j;
		}
	}
	kCenters[1] = datas[ifar];
	//printf("��ӡ��ʼ��ʱ�ľ������ģ�%f\n", orignalCenters[m]);
	clusterCenterTag[ifar] = true;

	// Ѱ��ʣ�µ�k-2���������ģ����þ���˻����ֵΪ��һ����Ѿ�������
	for (int ik = 2; ik < nk; ik++)
	{
		double  maxMutiply = 0;
		for (int i = 0; i < nhd; i++)
		{
			if (clusterCenterTag[i]) continue;
			QVector<double>	distIK;		// ��¼��i�����ܶȵ㵽��ȷ����ik���������ĵľ���
			for (int j = 0; j < ik; j++)
			{
				double d = fabs(datas[hDensPts[i].first] - kCenters[j]);
				distIK.append(d);
			}
			double tempMax = distIK[0];
			for (int j = 1; j < distIK.count(); j++)
			{
				tempMax *= distIK[j];
			}
			if (tempMax > maxMutiply)
			{
				ifar = i;
			}
			else if (tempMax == maxMutiply) // ���������ʱ���Ƚϸ��ܶȵ���ܶȴ�С
			{
				if (nearNPts[i] > nearNPts[ifar])
				{
					ifar = i;
				}
			}
		}
		kCenters[ik] = datas[ifar];
		//printf("��ӡ��ʼ��ʱ�ľ������ģ�%f\n", orignalCenters[m]);
		clusterCenterTag[ifar] = true;
	}// end for

	return true;
}

bool lessThanDatas(double a1, double a2)
{
	if (a1 > a2) return true;
	else return false;
}

bool Cluster::kmeans(int nk, int niter, const QVector<double> &noRepeatDatas, const QVector<double> &datas, 
					 QList<QVector<int> > &clusterResult, QVector<double> &clusterCenters)
{
	clusterCenters.clear();
	clusterResult.clear();
	for (int i = 0; i < nk; i++)
	{
		clusterResult.append(QVector<int>());
	}

	QVector<double> kCenters(nk, 0);
	// ��ʼ��K����������
	if (!initCenters(nk, noRepeatDatas, kCenters))
	{
		return false;
	}

	QVector<double>  tempClusterCenters(nk, 0);

	int iter = 0;
	for (int iter = 0; iter <= niter; iter++)
	{
		tempClusterCenters = kCenters;
		for (int i = 0; i < nk; i++)
		{
			clusterResult[i].clear();
		}

		for (int i = 0; i < datas.count(); i++)
		{
			double minDist = HUGE_VAL;
			int minIndex = -1;
			for (int j = 0; j < nk; j++)
			{
				double temp = fabs(kCenters[j] - datas[i]);
				if (minDist > temp)
				{
					minDist = temp;
					minIndex = j;
				}
			}
			clusterResult[minIndex].append(i);
		}
		// ���¼����������
		int sameClusterCenters = 0;
		for (int i = 0; i < nk; i++)
		{
			double sum = 0;
			for (int j = 0; j < clusterResult[i].count(); j++)
			{
				int temp = clusterResult[i][j];
				sum += datas[temp];
			}
			kCenters[i] = sum / clusterResult[i].count();

			if (fabs(kCenters[i] - tempClusterCenters[i]) < 1e-6)
			{
				sameClusterCenters++;
			}
		}
		if (sameClusterCenters == nk) // ����ľ��������Ѿ����ڱ仯����������
		{
			clusterCenters = kCenters;
			break;
		}
	}
	//qSort(kOrignalCenters.begin(), kOrignalCenters.end(), lessThanDatas);
	clusterCenters = kCenters;
	return true;
}


double Cluster::evaluateResult(const QVector<double> &normalDatas, const QVector<double> &orignalCenters, 
							   QList< QVector<int> > &clusterResults, int K)
{
	double center = 0;  // ��¼�������ݼ��ϵ�����
	for (int i = 0; i < normalDatas.count(); i++)
	{
		center += normalDatas[i];
	}
	center = center / normalDatas.count();
	double tempResult = 0;
	double L = 0;
	for (int i = 0; i < K; i++)
	{
		L += fabs(orignalCenters[i] - center);
		double temp = 0;
		for (int j = 0; j < clusterResults[i].count(); j++)
		{
			temp += fabs(orignalCenters[i] - normalDatas[clusterResults[i][j]]);
		}
		tempResult += (clusterResults[i].count()*temp) / normalDatas.count();
	}
	return (L + tempResult);
}

void rmRepeatDatas(const QVector<double> &orignalDatas, QVector<double> &classdata)
{
	int i, j;
	classdata.clear();
	classdata << orignalDatas[0];
	for (i = 1; i < orignalDatas.count(); i++)
	{
		for (j = 0; j < classdata.count(); j++)
		{
			if (fabs(orignalDatas[i] - classdata[j]) < 1e-6) break;
		}
		if (j >= classdata.count())
		{
			classdata.append(orignalDatas[i]);
		}
	}
}


bool lessThanAreas(QVector<int> p1, QVector<int> p2)
{
	double a1 = 0;
	double a2 = 0;
	for (int i = 0; i < p1.count(); i++)
	{
		a1 += p1[i];
	}
	a1 = a1 / p1.count();

	for (int i = 0; i < p2.count(); i++)
	{
		a2 += p2[i];
	}
	a2 = a2 / p2.count();

	if (a1 < a2) return true;
	else return false;
}


void Cluster::dataCluster(QList< QVector<int> > &clusterGroups)
{
	QVector<double> normalDatas;		// ��Ź�һ��֮����������
	normalize(mClusterDatas, normalDatas);
	int clusterGroupsNum = 0;    // ��¼�����ʵ�ʵķ�����

	// Ĭ�Ϸ�1�����
	clusterGroups.append(QVector<int>());
	for (int i = 0; i < mClusterDatas.count(); i++)
	{
		clusterGroups.last() << i;
	}

	// ���������Ŀ<=5ֱ�ӷ���1������
	if (mClusterDatas.count() <= 5) return;

	QVector<double>		noRepeatDatas;		// ���ȥ�غ������
	rmRepeatDatas(normalDatas, noRepeatDatas);
	int maxNK = round(sqrt(noRepeatDatas.count()));
	if (maxNK == 1) return;

	int maxIter = 100;			// ����������
	QList< QVector<int> >		clusterResult;		// ��ž�����
	QList< QVector<int> >		bestClusterResult;  // �����Ѿ�����
	int bestNK;
	double minEvaluate = HUGE_VALL;
	QVector<double>		bestClusterCenters;
	for (int k = 2; k <= maxNK; k++)
	{
		QVector<double>  tempClusterCenters;
		if (!kmeans(k, maxIter, noRepeatDatas, normalDatas, clusterResult, tempClusterCenters))
		{
			break;
		}

		// �Ծ�������������
		double temp = evaluateResult(normalDatas, tempClusterCenters, clusterResult, k);
		if (minEvaluate > temp)
		{
			bestNK = k;
			minEvaluate = temp;
			bestClusterResult = clusterResult;
			bestClusterCenters = tempClusterCenters;
		}
	}
	//qSort(bestClusterResult.begin(), bestClusterResult.end(), lessThanAreas);
	if (bestClusterResult.count() > 0)
	{
		clusterGroups = bestClusterResult;
	}

	/*cout << "��ӡ��Ѿ�����" << bestK << endl;
	cout << "��ӡ��ѵ�k����������" << endl;
	for (int j = 0; j < bestK; j++)
	{
	cout << bestClusterCenters[j] << "\t";
	}
	cout << endl;*/
}