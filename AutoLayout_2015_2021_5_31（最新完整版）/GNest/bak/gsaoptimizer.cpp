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


void GSAOptimizer::optimize(int maxIter,							// 最大迭代次数
	double maxLayoutRate,					// 最大排版率
	QVector< GPart > &nestParts,			// 排样零件
	QVector<int> &usedPageNums,				// 板材使用数量
	QVector<int> &plateIndices,				// 板材索引
	double &layoutRate,						// 排版率
	int *pProgress)
{
	QVector<QList<QVector<int> > >		globalBestSeq;				// 全局最优排序结果
	float								globalBestRating = 0.0;			// 全局最优排版率
	QVector<QList<QVector<int> > >		localBestSeq;				// 局部最优排序结果
	float								localBestRating = 0.0;			// 局部最优排序结果
	quint32								selectMark = 0x01;			// 随机调整零件的旗标

	int   isSameNum = 0;
	srand((unsigned)time(NULL));
	// 模拟退火算法主体

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

	int perIterm = 0;			// 统计迭代次数

								// 最大温度与外界传进来的最大迭代次数转化
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
			if (pProgress) // 计算目前的迭代进度
			{
				*pProgress = qRound((perIterm + 1) * 100.0 / maxIter);
			}
			QVector< GPart >	tmnestParts;
			QVector<int>		tmusedPageNums;
			QVector<int>		tmplateIndices;

			QVector<QList<QVector<int> > >	tmpSeq;			// 临时零件排版序列
			float							tmpRating;		// 临时零件排版率
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

			// 0x01表示随机调整一个小组，0x02表示对一个小组进行略微调整，
			// 0x04和0x08表示任意交换一个小组内的两个数据
			if (perIterm > ceil(0.8*maxIter)) // 零件迭代到后期将只对一个大组内的小组零件进行调整
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
				//printf("\n第%d次，在hash表中找到相同序列\n\n", isSameNum);
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
				// printf("根据当前温度计算的概率%f\n",P);

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

				// 如果达到最大排版率直接退出迭代
				if (globalBestRating > maxLayoutRate)
					return;
			}
			printf("迭代第%d次时，当前零件的全局最优排版率为：%f\t,局部最优排版率为：%f\n", perIterm + 1, globalBestRating, localBestRating);

			perIterm++;
		}// end for

		 // 强制结束迭代寻优
		if (mIsAbort) return;

		t = t * mControlA; // 执行降温操作
	}// end while

	printf("打印总的迭代次数%d\n", perIterm);
	mRecordLayoutSeQ.clear();
	// 用于测试大小组显示情况
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