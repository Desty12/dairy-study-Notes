#include "ugroupfirefly.h"
#include "time.h"
#include "gmath.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <gautolayout.h>
#include <QTime>
#include <QSet>

#define e 2.718
#define MAXITERM 5000    // 定义萤火虫最大迭代次数

using namespace  std;

UgroupFireFly::UgroupFireWorm::UgroupFireWorm()
{
	layoutRating = 0;
}

UgroupFireFly::UgroupFireWorm::~UgroupFireWorm()
{
}

UgroupFireFly::UgroupFireFly()
{
	r = 1;
	B0 = 1;
	m = 20;
	MaxIterm = MAXITERM;
	I0 = 0.0;
	mIsAbort = false;
	//mDifSeqNums = -1;
}

UgroupFireFly::~UgroupFireFly()
{
}

// 萤火虫种群初始化
void UgroupFireFly::InitFireFlies()
{
	// 第一个萤火虫初始种群按照聚类分组数据产生
	UgroupFireWorm	fireWorm;
	fireWorm.sequenceGroups = mSortSequences;
	mFireFlySerises << fireWorm;

	// 随机调整2/3的allGroupsNum数，产生第2-第11个种群
	for (int i = 0; i < 10; i++)
	{
		UgroupFireWorm fireWorm1;
		fireWorm1.sequenceGroups = mSortSequences;
		randomChangeData(fireWorm1.sequenceGroups);
		mFireFlySerises << fireWorm1;
	}

	//  随机调整1/3的allGroupsNum数，产生第12-第21个种群
	for (int i = 0; i < 19; i++)
	{
		UgroupFireWorm fireWorm1;
		fireWorm1.sequenceGroups = mSortSequences;
		fewChangeData(fireWorm1.sequenceGroups);
		mFireFlySerises << fireWorm1;
	}

	// 随机调整两个小组内的零件，产生第22-第31个种群
	for (int i = 0; i <10; i++)
	{
		UgroupFireWorm fireWorm1;
		fireWorm1.sequenceGroups = mSortSequences;
		changeTwoData(fireWorm1.sequenceGroups);
		mFireFlySerises << fireWorm1;
	}
}

// 计算两只萤火虫每个小组seQuenceI内的相对荧光亮度
double UgroupFireFly::calDistance_i_j(const QVector<int> &seQuenceI, const QVector<int> &sequenceJ)
{
	int allDistances = 0;
	int sum = 0;

	for (int i1 = 0; i1 < seQuenceI.count(); i1++)
	{
		if (seQuenceI[i1] != sequenceJ[i1]) sum++;
	}
	allDistances = seQuenceI.count();

	return (sum*1.0) / allDistances;
}

// 计算两只萤火虫每个小组内的吸引度，吸引度决定萤火虫移动的距离大小
double UgroupFireFly::calAttraction_i_j(double rI_J)
{
	double attractionResult = 0.0;
	double doubleR = rI_J*rI_J;
	attractionResult = B0*pow(e, -r*doubleR);
	// attractionResult = 0.3697为距离rI_J等于0时的吸引度大小
	if ((attractionResult > 0.37) && (attractionResult < 0.5)) attractionResult *= 1.7;		// 避免萤火虫寻优后期大量相同序列时，零件序列改变较少

	return attractionResult;
}

// 去除sequence中不存在的值，并将其值设置为-1
void UgroupPolishNoUsed(QVector<int>	&sequence, const QVector<int>	&goalSequence)
{
	for (int j0 = 0; j0 < sequence.count(); j0++)
	{
		bool tmpMarks = false;
		for (int n = 0; n < goalSequence.count(); n++)
		{
			if (sequence[j0] == goalSequence[n])
			{
				tmpMarks = true;
				break;
			}
		}
		if ((!tmpMarks) && (sequence[j0] != -1)) sequence[j0] = -1;
	}
}

// 去除sequence中重复的值，并将其设置为-1，将去重后的序列保存到noRepeate中
void UgroupPolishRepeate(QVector<int>	&sequence, QSet<int> &noRepeate)
{
	noRepeate.clear();
	for (int i = 0; i < sequence.count(); i++)
	{
		if (sequence[i] != -1)
		{
			bool mark = false;
			if (noRepeate.count() == 0)
			{
				noRepeate.insert(sequence[i]);
				continue;
			}

			QSet<int>::iterator it = noRepeate.find(sequence[i]);
			if (it != noRepeate.end()) mark = true;
			else
			{
				noRepeate.insert(sequence[i]);
				continue;
			}

			if (mark) sequence[i] = -1;
		}
	}
}

// 处理序列sequence中不可编码的值，eg:原始待处理序列(3,1,4,1,2,5,2,8,15) 
//（待处理的序列取值应该为（1-9）） ==> 处理后的序列为（3,1,4,6,2,5,7,8，9）
void UgroupFireFly::dealInfeasibleSequence(QVector<int> &seq, const QVector<int> &orignalSeq)
{
	QVector<int>	sequence = seq;
	QVector<int>	orignalSequence = orignalSeq;

	int sameNum = 0;
	for (int j0 = 0; j0 < orignalSequence.count(); j0++)
	{
		if (sequence[j0] == orignalSequence[j0])
			sameNum++;
	}
	if (sameNum == orignalSequence.count()) // 如果当前分组零件都相同,则直接返回
	{
		return;
	}
	if (sequence.count() == 1)
	{
		sequence[0] = orignalSequence[0];
		return;
	}

	UgroupPolishNoUsed(sequence, orignalSequence);
	QSet<int> noRepeateSequence;
	UgroupPolishRepeate(sequence, noRepeateSequence);
	if (noRepeateSequence.count() == orignalSequence.count()) // 若两个序列的值完全相等则直接返回
	{
		return;
	}

	for (int i = 0; i < orignalSequence.count(); i++)
	{
		bool mark = false;
		QSet<int>::iterator iter = noRepeateSequence.find(orignalSequence[i]);
		if (iter != noRepeateSequence.end())
		{
			mark = true;
		}

		if (!mark) // 没有找到则在sequence的对应位置更新该值
		{
			for (int k = 0; k < sequence.count(); k++)
			{
				if (sequence[k] == -1)
				{
					sequence[k] = orignalSequence[i];
					break;
				}
			}
		}
	}

	// 处理完毕之后，将sequence中对应的值在seq表序列中进行相应的更新
	int   tempIndex = 0;		// 标记目前在sequence的第tempIndex出进行值更新
	for (int i = 0; i < seq.count(); i++)
	{
		seq[i] = sequence[tempIndex];
		tempIndex++;
	}
}

// 序列sequenceI向sequenceJ移动更新
void UgroupFireFly::updateLocationItoJ(QVector<int> &sequenceI, const QVector<int> &sequenceJ, double attraction)
{
	for (int i = 0; i < sequenceI.count(); i++)
	{
		double a = (rand() % 1000) / 1000.0;
		double rands = (rand() % 1000) / 1000.0;
		sequenceI[i] = (1 - attraction)*sequenceI[i] + attraction*sequenceJ[i] + a*(rands - 0.5);
	}

	// 对不可行编码执行更新
	dealInfeasibleSequence(sequenceI, sequenceJ);
}

// 初始化最差的萤火虫排样序列
void	UgroupFireFly::initWorstFireWormSequence(QVector<int> &sequences, const QVector<int>  &bestSeq)
{
	sequences.clear();
	sequences = mSortSequences;

	fewChangeData(sequences);
}


// 执行萤火虫算法
double UgroupFireFly::optimize(int maxIter,			// 最大迭代次数
	double maxLayoutRating,					// 最大排版率
	QVector< GPart > &nestParts,			// 排样零件
	QVector<int> &usedPageNums,				// 板材使用数量
	QVector<int> &plateIndices,				// 板材索引
	double &layoutRate,						// 排版率
	int *pProgress)
{
	srand((unsigned)time(NULL));			// 初始化随机数种子
	quint32	selectFlag = 0x01;			// 当两个萤火虫荧光亮度一致时，标记采用哪一种方式微调零件序列

	int minFirstPageNestHeight = 100000;
	mFireFlySerises.clear();
	mFireFlyNumbers = 0;
	mRecordLayoutSeQ.clear();

	InitFireFlies();
	mFireFlyNumbers = mFireFlySerises.count();
	QVector<int>	initPartsSeq;
	for (int i = 0; i < mpParts->count(); i++)
	{
		initPartsSeq.append(mpParts->at(i).mPartID);
	}
	QVector<SaveLayoutDatas>		saveLayoutData(mFireFlyNumbers);
	// 根据聚类结果计算第一个萤火虫的排版率，并将其作为适应度值
	QTime layoutTime0;
	layoutTime0.start();
	GAutoLayout gauto0;
	gauto0.setLayoutParam((*mpPagePlates), (*mpPageNums), mGridDelta, *mpParts, *mpPartNums);
	gauto0.setPartTemplates(*mpPartTemplates);
	gauto0.setLayoutFlag(mLayoutFlag);

	QVector<quint32>	rots;
	gauto0.layout(mFireFlySerises[0].sequenceGroups, rots, saveLayoutData[0].nestParts, saveLayoutData[0].usedPageNums, saveLayoutData[0].plateIndices, mFireFlySerises[0].layoutRating, saveLayoutData[0].lastPageForwardLines, 0);
	saveLayoutData[0].firstPageNestHeight = gauto0.getFirstPageMaxHeight();
	saveLayoutData[0].nestRating = mFireFlySerises[0].layoutRating;

	mRecordLayoutSeQ.insert(mFireFlySerises[0].sequenceGroups, saveLayoutData[0]);
	printf("运行排样所需时间:%d\n", layoutTime0.elapsed());

	QVector<int>  tIndexsArr;
	for (int i = 1; i < mFireFlyNumbers; i++)
	{
		QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(mFireFlySerises[i].sequenceGroups);
		if (iter != mRecordLayoutSeQ.end())
		{
			SaveLayoutDatas save0 = iter.value();
			mFireFlySerises[i].layoutRating = save0.nestRating;
			saveLayoutData[i].nestParts = save0.nestParts;
			saveLayoutData[i].plateIndices = save0.plateIndices;
			saveLayoutData[i].usedPageNums = save0.usedPageNums;
			saveLayoutData[i].firstPageNestHeight = save0.firstPageNestHeight;
			saveLayoutData[i].nestRating = save0.nestRating;

		}
		else tIndexsArr.append(i);
	}

#pragma omp parallel for  num_threads(OMP_NUM_THREADS)
	for (int n = 0; n < tIndexsArr.count(); n++)
	{
		int temp = tIndexsArr[n];
		GAutoLayout gauto1;
		gauto1.setLayoutParam((*mpPagePlates), (*mpPageNums), mGridDelta, *mpParts, *mpPartNums);
		gauto1.setPartTemplates(*mpPartTemplates);
		gauto1.setLayoutFlag(mLayoutFlag);

		QVector<quint32>	rots1;
		gauto1.layout(mFireFlySerises[temp].sequenceGroups, rots1, saveLayoutData[temp].nestParts, saveLayoutData[temp].usedPageNums, saveLayoutData[temp].plateIndices, mFireFlySerises[temp].layoutRating, saveLayoutData[temp].lastPageForwardLines, 0);
		saveLayoutData[temp].firstPageNestHeight = gauto1.getFirstPageMaxHeight();
		saveLayoutData[temp].nestRating = mFireFlySerises[temp].layoutRating;
	}

	for (int n = 0; n < tIndexsArr.count(); n++)
	{
		int temp = tIndexsArr[n];
		mRecordLayoutSeQ.insert(mFireFlySerises[temp].sequenceGroups, saveLayoutData[temp]);
	}
	tIndexsArr.clear();

	double gBestLayoutRating = 0.0;
	QVector<int>	globalBestLayoutSequence;	// 暂存全局最优排版序列
	QVector<GPart>	gNestParts;
	QVector<int>	gUsedPageNums;
	QVector<int>	gPlateIndices;
	int				gFirstPageNestHeight;
	int				gLastPageForwardLines= 100000;

	for (int i = 0; i < mFireFlyNumbers; i++)
	{
		printf("打印第%d个种群的排版率为：%f, 排样高度为：%f\n", i + 1, mFireFlySerises[i].layoutRating, saveLayoutData[i].firstPageNestHeight);
		if (saveLayoutData[i].firstPageNestHeight < minFirstPageNestHeight) minFirstPageNestHeight = saveLayoutData[i].firstPageNestHeight;

		if (((mFireFlySerises[i].layoutRating- gBestLayoutRating)>1e-6)|| (fabs(mFireFlySerises[i].layoutRating - gBestLayoutRating)<1e-6 && saveLayoutData[i].lastPageForwardLines < gLastPageForwardLines))
		{
			gBestLayoutRating = mFireFlySerises[i].layoutRating;
			gLastPageForwardLines = saveLayoutData[i].lastPageForwardLines;
			// 从hash表中查找对应排版数据
			QVector<int> tmpSeq = mFireFlySerises[i].sequenceGroups;
			QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(tmpSeq);
			if (iter != mRecordLayoutSeQ.end())
			{
				SaveLayoutDatas save0 = iter.value();
				gNestParts = save0.nestParts;
				gUsedPageNums = save0.usedPageNums;
				gPlateIndices = save0.plateIndices;
				gFirstPageNestHeight = save0.firstPageNestHeight;
			}
		}

		if ((gBestLayoutRating - maxLayoutRating)>=1e-6)
		{
			layoutRate = gBestLayoutRating;
			nestParts = gNestParts;
			usedPageNums = gUsedPageNums;
			plateIndices = gPlateIndices;
			//mbestSequences = mFireFlySerises[i].sequenceGroups;

			return gFirstPageNestHeight;
		}
	}

	// 进行萤火虫寻优
	int				t = 0;
	int				bestLocationMark = 0;						// 记录最优的排											// 样序列的萤火虫下标
																//double			bestRating = 0;								// 记录全局最佳排版率
	MaxIterm = maxIter;

	int numMark = 0;
	while (t < MaxIterm)
	{
		if (pProgress) // 计算目前的迭代进度
		{
			*pProgress = qRound((t + 1) * 100.0 / maxIter);
		}
		for (int m = 0; (m < mFireFlyNumbers) && (!mIsAbort); m++)
		{
			// 进行萤火虫位置更新
			QVector<int>	tmpMoreLightIndexs;
			for (int n = 0; n < m; n++)
			{
				if ((mFireFlySerises[m].layoutRating - mFireFlySerises[n].layoutRating)>1e-6)
				{
					double tempDistance = calDistance_i_j(mFireFlySerises[n].sequenceGroups, mFireFlySerises[m].sequenceGroups);
					double tempAttraction = calAttraction_i_j(tempDistance);
					updateLocationItoJ(mFireFlySerises[n].sequenceGroups, mFireFlySerises[m].sequenceGroups, tempAttraction);
				}
				else if (fabs(mFireFlySerises[m].layoutRating - mFireFlySerises[n].layoutRating) <= 1e-6)
				{
					// 当两只萤火虫荧光亮度相等时，选择其中一只萤火虫的排样序列进行微调
					// 计算迭代次数为t时，需要调整的小组零件个数
					//printf("每次需要调整的零件总个数：%d, 调整的零件个数：%d\n", allSubGroupNums , classNum);
					if (0x01 & selectFlag)
					{
						randomChangeData(mFireFlySerises[n].sequenceGroups);
						selectFlag <<= 1;
					}
					else if (0x02 & selectFlag)
					{
						fewChangeData(mFireFlySerises[n].sequenceGroups);
						selectFlag <<= 1;
					}
					else if ((0x04 & selectFlag) || (0x08 & selectFlag))
					{
						changeTwoData(mFireFlySerises[n].sequenceGroups);
						// changeGroupsDatas(mFireFlySerises[j3].sequenceGroups, classNum, selectFlag,false);
						selectFlag <<= 1;
						if (selectFlag > 0x08)
							selectFlag = 0x01;
					}
				} // end if-else	
			}// end for

			tIndexsArr.clear();
			for (int n = 0; n < m; n++)
			{
				QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(mFireFlySerises[n].sequenceGroups);
				if (iter != mRecordLayoutSeQ.end())
				{
					SaveLayoutDatas saveDatas = iter.value();
					mFireFlySerises[n].layoutRating = saveDatas.nestRating;
					saveLayoutData[n].nestParts = saveDatas.nestParts;
					saveLayoutData[n].plateIndices = saveDatas.plateIndices;
					saveLayoutData[n].usedPageNums = saveDatas.usedPageNums;
					saveLayoutData[n].firstPageNestHeight = saveDatas.firstPageNestHeight;
					saveLayoutData[n].nestRating = saveDatas.nestRating;
				}
				else tIndexsArr.append(n);
			}

#pragma omp parallel for  num_threads(OMP_NUM_THREADS)
			for (int n = 0; n < tIndexsArr.count(); n++)
			{
				int temp = tIndexsArr[n];
				GAutoLayout gauto1;
				gauto1.setLayoutParam((*mpPagePlates), (*mpPageNums), mGridDelta, *mpParts, *mpPartNums);
				gauto1.setPartTemplates(*mpPartTemplates);
				gauto1.setLayoutFlag(mLayoutFlag);

				QVector<quint32>	rots1;
				gauto1.layout(mFireFlySerises[temp].sequenceGroups, rots1, saveLayoutData[temp].nestParts, saveLayoutData[temp].usedPageNums, saveLayoutData[temp].plateIndices, mFireFlySerises[temp].layoutRating, saveLayoutData[temp].lastPageForwardLines, 0);
				saveLayoutData[temp].firstPageNestHeight = gauto1.getFirstPageMaxHeight();
				saveLayoutData[temp].nestRating = mFireFlySerises[temp].layoutRating;
			}

			for (int n = 0; n < tIndexsArr.count(); n++)
			{
				int temp = tIndexsArr[n];
				mRecordLayoutSeQ.insert(mFireFlySerises[temp].sequenceGroups, saveLayoutData[temp]);
			}

			for (int n = 0; n < tIndexsArr.count(); n++)
			{
				int temp = tIndexsArr[n];
				if (((mFireFlySerises[temp].layoutRating - gBestLayoutRating)>1e-4)||(fabs(mFireFlySerises[temp].layoutRating - gBestLayoutRating)<1e-6 && saveLayoutData[temp].lastPageForwardLines < gLastPageForwardLines))
				{
					gBestLayoutRating = mFireFlySerises[temp].layoutRating;
					gLastPageForwardLines = saveLayoutData[temp].lastPageForwardLines;
					// 从hash表中查找对应排版数据
					QVector<int> tmpSeq = mFireFlySerises[temp].sequenceGroups;
					QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(tmpSeq);
					if (iter != mRecordLayoutSeQ.end())
					{
						SaveLayoutDatas save0 = iter.value();
						gNestParts = save0.nestParts;
						gUsedPageNums = save0.usedPageNums;
						gPlateIndices = save0.plateIndices;
						gFirstPageNestHeight = save0.firstPageNestHeight;
					}
				}

				if ((gBestLayoutRating - maxLayoutRating)>=1e-6)
				{
					layoutRate = gBestLayoutRating;
					nestParts = gNestParts;
					usedPageNums = gUsedPageNums;
					plateIndices = gPlateIndices;
					//mbestSequences = mFireFlySerises[i].sequenceGroups;

					return gFirstPageNestHeight;
				}
			}
		}// end for

		 // 记录最佳排版率的萤火虫下标
		double maxPageRating = mFireFlySerises[0].layoutRating;
		int   tmpBestRatingMark = 0;
		int   tmpLastPageForwardLines = 100000;
		float  minPageRating = maxPageRating;
		int   tmpMinWorst = 0;	// 记录排版最差的萤火虫下标

		for (int i = 1; i < mFireFlyNumbers; i++)
		{
			if (((mFireFlySerises[i].layoutRating - maxPageRating)>1e-4) || (fabs(mFireFlySerises[i].layoutRating - maxPageRating)< 1e-6 && saveLayoutData[i].lastPageForwardLines < tmpLastPageForwardLines))
			{
				maxPageRating = mFireFlySerises[i].layoutRating;
				tmpLastPageForwardLines = saveLayoutData[i].lastPageForwardLines;
				tmpBestRatingMark = i;
			}
			else
			{
				minPageRating = mFireFlySerises[i].layoutRating;
				tmpMinWorst = i;
			}
		}

		if (((maxPageRating - gBestLayoutRating)>1e-4) && (fabs(maxPageRating - gBestLayoutRating)<1e-6 && tmpLastPageForwardLines < gLastPageForwardLines))
		{
			gBestLayoutRating = maxPageRating;
			gLastPageForwardLines = tmpLastPageForwardLines;
			globalBestLayoutSequence = mFireFlySerises[tmpBestRatingMark].sequenceGroups;

			//从hash表中最佳获取排版信息
			QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(globalBestLayoutSequence);
			if (iter != mRecordLayoutSeQ.end())
			{
				SaveLayoutDatas saveDatas = iter.value();
				gNestParts = saveDatas.nestParts;
				gPlateIndices = saveDatas.plateIndices;
				gUsedPageNums = saveDatas.usedPageNums;
				gFirstPageNestHeight = saveDatas.firstPageNestHeight;
			}

			bestLocationMark = tmpBestRatingMark;
			if (mIsAbort || (gBestLayoutRating - maxLayoutRating)>=1e-6)   // 达到目标排版率直接退出迭代,获知外界直接强制退出迭代
			{
				layoutRate = gBestLayoutRating;
				nestParts = gNestParts;
				usedPageNums = gUsedPageNums;
				plateIndices = gPlateIndices;

				return gFirstPageNestHeight;
			}
		}
		else if ((maxPageRating - gBestLayoutRating) <-1e-6) {
			mFireFlySerises[tmpBestRatingMark].sequenceGroups = globalBestLayoutSequence;
			mFireFlySerises[tmpBestRatingMark].layoutRating = gBestLayoutRating;
		}

		// 初始化最差的萤火虫排版序列
		initWorstFireWormSequence(mFireFlySerises[tmpMinWorst].sequenceGroups, globalBestLayoutSequence);
		printf("迭代第%d次, 最佳萤火虫位置: %d, 最佳排版率为: %f\n", t + 1, bestLocationMark, gBestLayoutRating);
		t++;
	}// end while

	printf("hash表的总长度：%d\n", mRecordLayoutSeQ.count());
	printf("打印改变序列hash表的总长度:%d\n", mMarkSeqHashs.count());
	mRecordLayoutSeQ.clear();
	mMarkSeqHashs.clear();

	layoutRate = gBestLayoutRating;
	nestParts = gNestParts;
	usedPageNums = gUsedPageNums;
	plateIndices = gPlateIndices;

	//mbestSequences = globalBestLayoutSequence;   // 保存全局最优零件序列
	/*mLayoutOrders.clear();
	for (int i = 0; i < globalBestLayoutSequence.count(); i++)
	{
	for (int j = 0; j < globalBestLayoutSequence[i].count(); j++)
	{
	for (int k = 0; k < globalBestLayoutSequence[i][j].count(); k++)
	{
	int temp = globalBestLayoutSequence[i][j][k];
	mLayoutOrders.append(temp);
	}
	}
	}*/

	return gFirstPageNestHeight;
}

const QVector<int>	& UgroupFireFly::getLayoutSequence() const
{
	return mLayoutOrders;
}
