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


double GSAOptimizer::optimize(int maxIter,							// ����������
	double maxLayoutRate,					// ����Ű���
	QVector< GPart > &nestParts,			// �������
	QVector<int> &usedPageNums,				// ���ʹ������
	QVector<int> &plateIndices,				// �������
	double &layoutRate,						// �Ű���
	int *pProgress)
{
	QVector<QList<QVector<int> > >		globalBestSeq;				// ȫ������������
	double								globalBestRating = 0.0;		// ȫ�������Ű���
	int									globalBestFLen = 100000000;
	double								globalBestFirstPageHeight;	// ȫ������ʱ��һҳ�Ű�߶�
	QVector<QList<QVector<int> > >		localBestSeq;				// �ֲ�����������
	double								localBestRating = 0.0;		// �ֲ�����������
	int									localBestFLen = 100000000;
	double								localBestFirstPageHeight;	// �ֲ�����������
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

	layout(localBestSeq, tmpNestParts, pageNum, plateNum, localBestRating, localBestFirstPageHeight, localBestFLen);

	QVector<int>	tempSorts0;
	groupsSeq2Seq(localBestSeq, tempSorts0);

	SaveLayoutDatas saveDatas0;
	saveDatas0.nestRating = localBestRating;
	saveDatas0.firstPageNestHeight = localBestFirstPageHeight;
	saveDatas0.lastPageForwardLines = localBestFLen;

	mRecordLayoutSeQ.insert(tempSorts0, saveDatas0);

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
	globalBestFLen = localBestFLen;
	globalBestFirstPageHeight = localBestFirstPageHeight;

	int perIterm = 0;			// ͳ�Ƶ�������

	double  tempData = round((log(minTemperature) - log(maxTemperature)) / log(mControlA));
	saPerIters = ceil(maxIter / tempData);

	double t = maxTemperature;
	int aiter = 0;
	while (t > minTemperature && aiter < maxIter)
	{
		aiter++;
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
			double							tmpRating;		// ��ʱ����Ű���
			int								tmpFLen;		
			double							tmpFirstPageHeight;
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
			int		markFLen = 0;

			QHash<QVector<int>, SaveLayoutDatas >::iterator hashIter = mRecordLayoutSeQ.find(tmpSequence);
			if (hashIter != mRecordLayoutSeQ.end())
			{
				markRating = hashIter->nestRating;
				markFLen = hashIter->lastPageForwardLines;
				isSame = true;
			}

			if (isSame)
			{
				tmpRating = markRating;
				tmpFLen = markFLen;
				isSameNum++;
				//printf("\n��%d�Σ���hash�����ҵ���ͬ����\n\n", isSameNum);
			}
			else
			{
				layout(tmpSeq, tmnestParts, tmusedPageNums, tmplateIndices, tmpRating, tmpFirstPageHeight, tmpFLen);
				SaveLayoutDatas saveDatasTmp;
				saveDatasTmp.nestRating = tmpRating;
				saveDatasTmp.firstPageNestHeight = tmpFirstPageHeight;
				saveDatasTmp.lastPageForwardLines = tmpFLen;

				mRecordLayoutSeQ.insert(tmpSequence, saveDatasTmp);
			}

			float  dE = tmpRating - localBestRating;
			if (dE > 1e-6)
			{
				localBestSeq = tmpSeq;
				localBestRating = tmpRating;
				localBestFLen = tmpFLen;
				localBestFirstPageHeight = tmpFirstPageHeight;
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
					localBestFLen = tmpFLen;
					localBestFirstPageHeight = tmpFirstPageHeight;
				}
			}

			if (//localBestRating > globalBestRating ||
				((localBestRating - globalBestRating) > 1e-4 )|| (fabs(localBestRating - globalBestRating) < 1e-4 && localBestFLen < globalBestFLen))
			{
				globalBestSeq = localBestSeq;
				globalBestRating = localBestRating;
				globalBestFLen = localBestFLen;
				globalBestFirstPageHeight = localBestFirstPageHeight;
				layoutRate = globalBestRating;

				nestParts = tmnestParts;
				usedPageNums = tmusedPageNums;
				plateIndices = tmplateIndices;

				// ����ﵽ����Ű���ֱ���˳�����
				if (globalBestRating > maxLayoutRate)
					return globalBestFirstPageHeight;
			}
			printf("������%d��ʱ����ǰ�����ȫ�������Ű���Ϊ��%f, %d\t,�ֲ������Ű���Ϊ��%f, %d\n", 
				perIterm + 1, globalBestRating, globalBestFLen, localBestRating, localBestFLen);

			perIterm++;
		}// end for

		 // ǿ�ƽ�������Ѱ��
		if (mIsAbort) return globalBestFirstPageHeight;

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