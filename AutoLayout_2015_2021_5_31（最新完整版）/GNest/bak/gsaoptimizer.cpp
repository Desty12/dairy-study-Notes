#include "GSAOptimizer.h"
#include <time.h>
#include "gautolayout.h"

#define e 2.718

GSAOptimizer::GSAOptimizer()
{
	//maxTemperature = 800000.0;
	maxTemperature = 750.0;
	minTemperature = 1.0;
	saPerIters = 1;
	mControlA = 0.9359;

	mIsAbort = false;
}

GSAOptimizer::~GSAOptimizer()
{
}


void GSAOptimizer::optimize(int maxIter,							// ����������
	double maxLayoutRate,					// ����Ű���
	QVector< GPart > &nestParts,			// �������
	QVector<int> &usedPageNums,				// ���ʹ������
	QVector<int> &plateIndices,				// �������
	double &layoutRate,						// �Ű���
	int *pProgress)
{
	QVector<QList<QVector<int> > >		globalBestSeq;				// ȫ������������
	float								globalBestRating = 0.0;			// ȫ�������Ű���
	QVector<QList<QVector<int> > >		localBestSeq;				// �ֲ�����������
	float								localBestRating = 0.0;			// �ֲ�����������
	quint32								selectMark = 0x01;			// ���������������

	int   isSameNum = 0;
	srand((unsigned)time(NULL));
	// ģ���˻��㷨����

	localBestSeq = mClusterGroups;
	QVector<quint32>	rotFlags0;
	QVector<GPart>		tmpNestParts;
	QVector<int>		pageNum;
	QVector<int>		plateNum;
	nestParts.clear();
	usedPageNums.clear();
	plateIndices.clear();

	localBestRating = layout(localBestSeq, tmpNestParts, pageNum, plateNum);

	QVector<int>	tempSorts0;
	groupsSeq2Seq(localBestSeq, tempSorts0);
	mRecordLayoutSeQ.insert(tempSorts0, localBestRating);

	nestParts = tmpNestParts;
	usedPageNums = pageNum;
	plateIndices = plateNum;
	layoutRate = localBestRating;
	plateNum.clear();
	pageNum.clear();
	tmpNestParts.clear();
	tempSorts0.clear();

	globalBestSeq = localBestSeq;
	globalBestRating = localBestRating;

	int perIterm = 0;			// ͳ�Ƶ�������

								// ����¶�����紫����������������ת��
								/*double tempData = (-(maxIter / saPerIters)*log(mControlA) + log(minTemperature) );
								double tempData1 = int(tempData * 100) / 100.0;
								maxTemperature = (pow(e, tempData1));
								double tempD = long long(maxTemperature * 10) / 10.0;
								maxTemperature = tempD;*/

	double  tempData = round((log(minTemperature) - log(maxTemperature)) / log(mControlA));
	saPerIters = ceil(maxIter / tempData);

	double t = maxTemperature;
	while (t > minTemperature)
	{
		for (int iter = 0; iter < saPerIters && (!mIsAbort); iter++)
		{
			if (pProgress) // ����Ŀǰ�ĵ�������
			{
				*pProgress = qRound((perIterm + 1) * 100.0 / maxIter);
			}
			QVector< GPart >	tmnestParts;
			QVector<int>		tmusedPageNums;
			QVector<int>		tmplateIndices;

			QVector<QList<QVector<int> > >	tmpSeq;			// ��ʱ����Ű�����
			float							tmpRating;		// ��ʱ����Ű���
			tmpSeq = mClusterGroups;
			int bigNums = 0;
			int allGroupsNum = 0;
			for (int i = 0; i < mClusterGroups.count(); i++)
			{
				allGroupsNum += mClusterGroups[i].count();
			}

			double  minvalue = 2 / allGroupsNum*1.0;
			double  tempValue = 0.66 - minvalue;

			if (perIterm < 0.7*maxIter) bigNums = ceil((0.66 - (tempValue / maxIter)*perIterm)*allGroupsNum);
			else bigNums = 2;

			if (bigNums >= allGroupsNum) bigNums = ceil(0.66*allGroupsNum);
			else if (bigNums <= 0) bigNums = 1;

			// 0x01��ʾ�������һ��С�飬0x02��ʾ��һ��С�������΢������
			// 0x04��0x08��ʾ���⽻��һ��С���ڵ���������
			if (perIterm > ceil(0.8*maxIter)) // ������������ڽ�ֻ��һ�������ڵ�С��������е���
			{
				bigNums = 1;
				selectMark = 0x01;
			}
			changeGroupsDatas(tmpSeq, bigNums, selectMark, true);

			QVector<int>	tmpSequence;
			groupsSeq2Seq(tmpSeq, tmpSequence);
			selectMark <<= 1;
			if (selectMark > 0x08) selectMark = 0x01;

			bool isSame = false;
			double  markRating = 0.0;

			QHash<QVector<int>, double>::iterator hashIter = mRecordLayoutSeQ.find(tmpSequence);
			if (hashIter != mRecordLayoutSeQ.end())
			{
				markRating = hashIter.value();
				isSame = true;
			}

			if (isSame)
			{
				tmpRating = markRating;
				isSameNum++;
				//printf("\n��%d�Σ���hash�����ҵ���ͬ����\n\n", isSameNum);
			}
			else
			{
				tmpRating = layout(tmpSeq, tmnestParts, tmusedPageNums, tmplateIndices);
				mRecordLayoutSeQ.insert(tmpSequence, tmpRating);
			}

			float  dE = tmpRating - localBestRating;
			if (dE > 1e-6)
			{
				localBestSeq = tmpSeq;
				localBestRating = tmpRating;
			}
			else
			{
				double P = exp(dE / (t));
				// printf("���ݵ�ǰ�¶ȼ���ĸ���%f\n",P);

				double tmpRandom = (rand() % 1000 / 1000.0);
				if (P > tmpRandom)
				{
					localBestSeq = tmpSeq;
					localBestRating = tmpRating;
				}
			}

			if (localBestRating > globalBestRating)
			{
				globalBestSeq = localBestSeq;
				globalBestRating = localBestRating;
				layoutRate = globalBestRating;

				nestParts = tmnestParts;
				usedPageNums = tmusedPageNums;
				plateIndices = tmplateIndices;

				// ����ﵽ����Ű���ֱ���˳�����
				if (globalBestRating > maxLayoutRate)
					return;
			}
			printf("������%d��ʱ����ǰ�����ȫ�������Ű���Ϊ��%f\t,�ֲ������Ű���Ϊ��%f\n", perIterm + 1, globalBestRating, localBestRating);

			perIterm++;
		}// end for

		 // ǿ�ƽ�������Ѱ��
		if (mIsAbort) return;

		t = t * mControlA; // ִ�н��²���
	}// end while

	printf("��ӡ�ܵĵ�������%d\n", perIterm);
	mRecordLayoutSeQ.clear();
	// ���ڲ��Դ�С����ʾ���
	/*layoutOrders.clear();
	for (int i = 0; i < globalBestSeq.count(); i++)
	{
	for (int j = 0; j < globalBestSeq[i].count(); j++)
	{
	for (int k = 0; k < globalBestSeq[i][j].count(); k++)
	{
	layoutOrders.append(globalBestSeq[i][j][k]);
	}
	}
	}*/
}

const QVector<int>	& GSAOptimizer::getLayoutSequence() const
{
	return layoutOrders;
}