#include "firefly.h"
#include "time.h"
#include "gmath.h"
#include <iostream>
#include <cstdlib>
#include <gautolayout.h>
#include <QTime>
#include <QSet>

#define e 2.718
#define MAXITERM 5000    // 定义萤火虫最大迭代次数

using namespace  std;


FireFly::FireWorm::FireWorm()
{
	layoutRating = 0;
}

FireFly::FireWorm::~FireWorm()
{
}

FireFly::FireFly()
{
	r = 1;
	B0 = 1;
	m = 20;
	MaxIterm = MAXITERM;
	I0 = 0.0;
	mIsAbort = false;
	//mDifSeqNums = -1;
}

FireFly::~FireFly()
{
}

// 萤火虫种群初始化
void FireFly::InitFireFlies()
{
	// 统计所有的小组数
	int allGroupsNum = 0;
	for (int i = 0; i < mClusterGroups.count(); i++)
	{
		allGroupsNum += mClusterGroups[i].count();
	}
	// 第一个萤火虫初始种群按照聚类分组数据产生
	FireWorm	fireWorm;
	fireWorm.sequenceGroups = mClusterGroups;
	mFireFlySerises << fireWorm;

	// 随机调整2/3的allGroupsNum数，产生第2-第11个种群
	for (int i = 0; i < 10; i++)
	{
		FireWorm fireWorm1;
		quint32  selectFlag = 0x01;
		int	changeNum = ceil(allGroupsNum*(2 / 3.0));
		fireWorm1.sequenceGroups = mClusterGroups;
		if (i >= 4) selectFlag = 0x02;
		changeGroupsDatas(fireWorm1.sequenceGroups, changeNum, selectFlag, true);
		mFireFlySerises << fireWorm1;
	}

	//  随机调整1/3的allGroupsNum数，产生第12-第21个种群
	for (int i = 0; i < 19; i++)
	{
		FireWorm fireWorm1;
		quint32  selectFlag = 0x01;
		int	changeNum = ceil(allGroupsNum*(1 / 3.0));
		if (i >= 4) selectFlag = 0x02;
		fireWorm1.sequenceGroups = mClusterGroups;
		changeGroupsDatas(fireWorm1.sequenceGroups, changeNum, selectFlag, true);
		mFireFlySerises << fireWorm1;
	}

	// 随机调整两个小组内的零件，产生第22-第31个种群
	for (int i = 0; i < 10; i++)
	{
		FireWorm fireWorm1;
		quint32  selectFlag = 0x04;
		int changeNum = 2;
		//if (i >= 4) selectFlag = 0x02;
		fireWorm1.sequenceGroups = mClusterGroups;
		changeGroupsDatas(fireWorm1.sequenceGroups, changeNum, selectFlag, true);
		mFireFlySerises << fireWorm1;
	}
}

// 计算两只萤火虫每个小组seQuenceI内的相对荧光亮度
int FireFly::calDistance_i_j(const QVector<QList<QVector<int> > >  &seQuenceI, const QVector<QList<QVector<int> > > &sequenceJ, int &allDistances)
{
	allDistances = 0;
	int sum = 0;
	QVector<int>		seQ1;
	QVector<int>		seQ2;
	for (int i1 = 0; i1 < seQuenceI.count(); i1++)
	{
		for (int j1 = 0; j1 < seQuenceI[i1].count(); j1++)
		{
			const QVector<int>	&temp = seQuenceI[i1][j1];
			seQ1 << temp;
		}
	}

	for (int i1 = 0; i1 < sequenceJ.count(); i1++)
	{
		for (int j1 = 0; j1 < sequenceJ[i1].count(); j1++)
		{
			const QVector<int>	&temp = sequenceJ[i1][j1];
			seQ2 << temp;
		}
	}

	for (int i1 = 0; i1 < seQ1.count(); i1++)
	{
		if (seQ1[i1] == seQ2[i1]) sum++;
	}
	allDistances = seQ1.count();

	return sum;
}


// 计算两只萤火虫每个小组内的吸引度，吸引度决定萤火虫移动的距离大小
double FireFly::calAttraction_i_j(int rI_J, int allDistance)
{
	float attractionResult = 0.0;
	// 取值范围[0, 2.3]
	float temp = rI_J / (allDistance*1.0);
	temp = temp * 3;
	attractionResult = pow(e, -temp);
	/*long doubleR = rI_J*rI_J;
	attractionResult = B0*pow(e, -r*doubleR);*/
	return attractionResult;
}

// 去除sequence中不存在的值，并将其值设置为-1
void polishNoUsed(QVector<int>	&sequence, const QVector<int>	&goalSequence)
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
void polishRepeate(QVector<int>	&sequence, QSet<int> &noRepeate)
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
void FireFly::dealInfeasibleSequence(QList<QVector<int> > &seq, const QList<QVector<int> > &orignalSeq)
{
	QVector<int>	sequence;
	QVector<int>	orignalSequence;
	for (int i = 0; i < seq.count(); i++)
	{
		sequence << seq[i];
	}

	for (int i = 0; i < orignalSeq.count(); i++)
	{
		orignalSequence << orignalSeq[i];
	}

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

	polishNoUsed(sequence, orignalSequence);
	QSet<int> noRepeateSequence;
	polishRepeate(sequence, noRepeateSequence);
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
		for (int j = 0; j < seq[i].count(); j++)
		{
			seq[i][j] = sequence[tempIndex];
			tempIndex++;
		}
	}
}

// 序列sequenceI向sequenceJ移动更新
void FireFly::updateLocationItoJ(QVector<QList<QVector<int> > > &sequenceI, const QVector<QList<QVector<int> > > &sequenceJ, double attraction)
{
	QVector<QList<QVector<int> > > tempS = sequenceJ;
	QVector<int>	startSeq, endSeq;
	QVector<int>		seQ1;
	QVector<int>		seQ2;

	groupsSeq2Seq(sequenceI, seQ1);
	groupsSeq2Seq(sequenceJ, seQ2);
	for (int i1 = 0; i1 < seQ1.count(); i1++)
	{
		double a = (rand() % 1100) / 1000.0;
		double rands = (rand() % 1100) / 1000.0;
		seQ1[i1] = (1 - attraction)*seQ1[i1] + attraction*seQ2[i1] + a*(rands - 0.5);
	}

	int len0 = 0;
	for (int i2 = 0; i2 < sequenceI.count(); i2++)
	{
		for (int j2 = 0; j2 < sequenceI[i2].count(); j2++)
		{
			for (int k2 = 0; k2 < sequenceI[i2][j2].count(); k2++)
			{
				sequenceI[i2][j2][k2] = seQ1[len0];
				len0++;
			}
		}
	}

	// 对不可行编码执行更新
	QSet<int>  quencesSet;
	for (int i2 = 0; i2 < sequenceI.count(); i2++)
	{
		dealInfeasibleSequence(sequenceI[i2], tempS[i2]);
		for (int j = 0; j < sequenceI[i2].count(); j++)
		{
			endSeq << sequenceI[i2][j];
			for (int k = 0; k < sequenceI[i2][j].count(); k++)
			{
				quencesSet.insert(sequenceI[i2][j][k]);
			}
		}
	}

	if (quencesSet.count() < endSeq.count())
	{
		//printf("调整零件序列出错\n");
		// 对零件执行不可形解处理
		startSeq = seQ2;

		QSet<int> noRepeateSequence;
		polishRepeate(endSeq, noRepeateSequence);
		if (noRepeateSequence.count() == startSeq.count()) // 若两个序列的值完全相等则直接返回
		{
			return;
		}

		for (int i = 0; i < startSeq.count(); i++)
		{
			bool mark = false;
			QSet<int>::iterator iter = noRepeateSequence.find(startSeq[i]);
			if (iter != noRepeateSequence.end())
			{
				mark = true;
			}
			if (!mark) // 没有找到则在sequence的对应位置更新该值
			{
				for (int k = 0; k < endSeq.count(); k++)
				{
					if (endSeq[k] == -1)
					{
						endSeq[k] = startSeq[i];
						break;
					}
				}
			}
		}
	}

	// 处理完毕之后，将endSeq中对应的值在sequenceI表序列中进行相应的更新
	int   tempIndex = 0;
	for (int i = 0; i < sequenceI.count(); i++)
	{
		for (int j = 0; j < sequenceI[i].count(); j++)
		{
			for (int k = 0; k < sequenceI[i][j].count(); k++)
			{
				sequenceI[i][j][k] = endSeq[tempIndex];
				tempIndex++;
			}
		}
	}
}

// 初始化最差的萤火虫排样序列
void	FireFly::initWorstFireWormSequence(QVector<QList<QVector<int> > > &sequences, const QVector<QList<QVector<int> > > &bestSeq)
{
	sequences.clear();
	sequences = mClusterGroups;
	int ibigGroupsNum = 0;
	for (int i = 0; i < sequences.count(); i++)
	{
		ibigGroupsNum += sequences[i].count();
	}
	int GroupsNum = ceil(0.3*ibigGroupsNum);
	changeGroupsDatas(sequences, GroupsNum, 0x02, true);
}

// 执行萤火虫算法
int FireFly::optimize(int maxIter,			// 最大迭代次数
	double maxLayoutRating,					// 最大排版率
	QVector< GPart > &nestParts,			// 排样零件
	QVector<int> &usedPageNums,				// 板材使用数量
	QVector<int> &plateIndices,				// 板材索引
	double &layoutRate,						// 排版率
	int *pProgress)
{
	srand((unsigned)time(NULL));			// 初始化随机数种子
	quint32	selectFlag = 0x01;			// 当两个萤火虫荧光亮度一致时，标记采用哪一种方式微调零件序列
	int allSubGroupNums = 0;
	for (int i = 0; i < mClusterGroups.count(); i++)
	{
		allSubGroupNums += mClusterGroups[i].count();
	}
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
	mFireFlySerises[0].layoutRating = layout(mFireFlySerises[0].sequenceGroups, saveLayoutData[0].nestParts,
		saveLayoutData[0].usedPageNums, saveLayoutData[0].plateIndices, saveLayoutData[0].firstPageNestHeight);
	saveLayoutData[0].nestRating = mFireFlySerises[0].layoutRating;
	QVector<int>		firstSequence;
	groupsSeq2Seq(mFireFlySerises[0].sequenceGroups, firstSequence);
	mRecordLayoutSeQ.insert(firstSequence, saveLayoutData[0]);
	printf("运行排样所需时间:%d\n", layoutTime0.elapsed());

	QVector<int>  tIndexsArr;
	for (int i = 1; i < mFireFlyNumbers; i++)
	{
		QVector<int>	tmpSequence;
		groupsSeq2Seq(mFireFlySerises[i].sequenceGroups, tmpSequence);
		QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(tmpSequence);
		if (iter != mRecordLayoutSeQ.end())
		{
			SaveLayoutDatas saveDatas = iter.value();
			mFireFlySerises[i].layoutRating = saveDatas.nestRating;
			saveLayoutData[i].nestParts = saveDatas.nestParts;
			saveLayoutData[i].plateIndices = saveDatas.plateIndices;
			saveLayoutData[i].usedPageNums = saveDatas.usedPageNums;
			saveLayoutData[i].firstPageNestHeight = saveDatas.firstPageNestHeight;
		}
		else tIndexsArr.append(i);
	}

    #pragma omp parallel for  num_threads(OMP_NUM_THREADS)
	for (int j3 = 0; j3 < tIndexsArr.count(); j3++)
	{
		int temp = tIndexsArr[j3];
		mFireFlySerises[temp].layoutRating = layout(mFireFlySerises[temp].sequenceGroups, saveLayoutData[temp].nestParts,
			saveLayoutData[temp].usedPageNums, saveLayoutData[temp].plateIndices, saveLayoutData[temp].firstPageNestHeight);
		saveLayoutData[temp].nestRating = mFireFlySerises[temp].layoutRating;
	}

	for (int j3 = 0; j3 < tIndexsArr.count(); j3++)
	{
		int temp = tIndexsArr[j3];
		QVector<int>	tmpSequences;
		groupsSeq2Seq(mFireFlySerises[temp].sequenceGroups, tmpSequences);
		mRecordLayoutSeQ.insert(tmpSequences, saveLayoutData[temp]);
	}
	tIndexsArr.clear();


	for (int i = 0; i < mFireFlyNumbers; i++)
	{
		printf("打印第%d个种群的排版率为：%f, 排样高度为：%d\n", i + 1, mFireFlySerises[i].layoutRating, saveLayoutData[i].firstPageNestHeight);
		if (saveLayoutData[i].firstPageNestHeight < minFirstPageNestHeight) minFirstPageNestHeight = saveLayoutData[i].firstPageNestHeight;
		
		if (mFireFlySerises[i].layoutRating == maxLayoutRating)
		{
			nestParts = saveLayoutData[i].nestParts;
			usedPageNums = saveLayoutData[i].usedPageNums;
			plateIndices = saveLayoutData[i].plateIndices;
			mbestSequences = mFireFlySerises[i].sequenceGroups;

			return saveLayoutData[i].firstPageNestHeight;
		}
	}

	// 进行萤火虫寻优
	int				t = 0;
	int				bestLocationMark = 0;						    // 记录最优的排
	// 样序列的萤火虫下标
	double			gBestRating = 0;								// 记录全局最佳排版率
	QVector<QList<QVector<int> > >	globalBestLayoutSequence;	    // 全局最优排版序列
	QVector<GPart>	gNestParts;
	QVector<int>	gUsedPageNums;
	QVector<int>	gPlateIndices;
	int				gFirstPageNestHeight;

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
			for (int n = 0; n < m; n++)
			{
				if (mFireFlySerises[m].layoutRating > mFireFlySerises[n].layoutRating)
				{
					int allSequences = 0;
					int tempDistance = calDistance_i_j(mFireFlySerises[n].sequenceGroups, mFireFlySerises[m].sequenceGroups, allSequences);
					double tempAttraction = calAttraction_i_j(tempDistance, allSequences);
					updateLocationItoJ(mFireFlySerises[n].sequenceGroups, mFireFlySerises[m].sequenceGroups, tempAttraction);
				}
				else if (mFireFlySerises[m].layoutRating == mFireFlySerises[n].layoutRating)
				{
					// 当两只萤火虫荧光亮度相等时，选择其中一只萤火虫的排样序列进行微调
					// 计算迭代次数为t时，需要调整的小组零件个数
					int classNum = 0;
					double  minvalue = 1 / allSubGroupNums*1.0;
					//double  tempValue = 0.66 - minvalue;
					double  tempValue = 0.9 - minvalue;

					classNum = round((0.9 - (tempValue / maxIter)*t)*allSubGroupNums);

					if (classNum >= allSubGroupNums) classNum = allSubGroupNums;
					else if (classNum <= 0) classNum = 1;
					//printf("每次需要调整的零件总个数：%d, 调整的零件个数：%d\n", allSubGroupNums , classNum);
					if (0x01 & selectFlag)
					{
						changeGroupsDatas(mFireFlySerises[n].sequenceGroups, classNum, selectFlag, false);
						selectFlag <<= 1;
					}
					else if (0x02 & selectFlag)
					{
						changeGroupsDatas(mFireFlySerises[n].sequenceGroups, classNum, selectFlag, false);
						selectFlag <<= 1;
					}
					else if ((0x04 & selectFlag) || (0x08 & selectFlag))
					{
						smallGroupSeqAdjust(mFireFlySerises[n].sequenceGroups);
						// changeGroupsDatas(mFireFlySerises[j3].sequenceGroups, classNum, selectFlag,false);
						selectFlag <<= 1;
						if (selectFlag > 0x08)
							selectFlag = 0x01;
					}
				} // end if-else
			}// end for

			//QVector<int>  tIndexsArr;
			tIndexsArr.clear();
			for (int n = 0; n < m; n++)
			{
				QVector<int>	tmpSequence;
				groupsSeq2Seq(mFireFlySerises[n].sequenceGroups, tmpSequence);
				QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(tmpSequence);
				if (iter != mRecordLayoutSeQ.end())
				{
					SaveLayoutDatas saveDatas = iter.value();
					mFireFlySerises[n].layoutRating = saveDatas.nestRating;
					saveLayoutData[n].nestParts = saveDatas.nestParts;
					saveLayoutData[n].plateIndices = saveDatas.plateIndices;
					saveLayoutData[n].usedPageNums = saveDatas.usedPageNums;
					saveLayoutData[n].firstPageNestHeight = saveDatas.firstPageNestHeight;
				}
				else tIndexsArr.append(n);
			}

           #pragma omp parallel for  num_threads(OMP_NUM_THREADS)
			for (int n = 0; n < tIndexsArr.count(); n++)
			{
				int temp = tIndexsArr[n];
				mFireFlySerises[temp].layoutRating = layout(mFireFlySerises[temp].sequenceGroups, saveLayoutData[temp].nestParts,
					saveLayoutData[temp].usedPageNums, saveLayoutData[temp].plateIndices, saveLayoutData[temp].firstPageNestHeight);
				saveLayoutData[temp].nestRating = mFireFlySerises[temp].layoutRating;
			}

			for (int n = 0; n < tIndexsArr.count(); n++)
			{
				int temp = tIndexsArr[n];
				QVector<int>	tmpSequences;
				groupsSeq2Seq(mFireFlySerises[temp].sequenceGroups, tmpSequences);
				mRecordLayoutSeQ.insert(tmpSequences, saveLayoutData[temp]);

				if (minFirstPageNestHeight > saveLayoutData[n].firstPageNestHeight)
					minFirstPageNestHeight = saveLayoutData[n].firstPageNestHeight;
			}

		}// end for

		// 记录最佳排版率的萤火虫下标
		double maxPageRating = mFireFlySerises[0].layoutRating;
		int   tmpBestRatingMark = 0;
		double  minPageRating = maxPageRating;
		int   tmpMinWorst = 0;	// 记录排版最差的萤火虫下标

		for (int i = 1; i < mFireFlyNumbers; i++)
		{
			if (mFireFlySerises[i].layoutRating > maxPageRating)
			{
				maxPageRating = mFireFlySerises[i].layoutRating;
				tmpBestRatingMark = i;
			}
			else
			{
				minPageRating = mFireFlySerises[i].layoutRating;
				tmpMinWorst = i;
			}
		}

		if (maxPageRating > gBestRating)
		{
			gBestRating = maxPageRating;
			globalBestLayoutSequence = mFireFlySerises[tmpBestRatingMark].sequenceGroups;

			//从hash表中最佳获取排版信息
			QVector<int>	tmpSequence;
			groupsSeq2Seq(globalBestLayoutSequence, tmpSequence);
			QHash<QVector<int>, SaveLayoutDatas>::iterator iter = mRecordLayoutSeQ.find(tmpSequence);
			if (iter != mRecordLayoutSeQ.end())
			{
				SaveLayoutDatas saveDatas = iter.value();
				gNestParts = saveDatas.nestParts;
				gPlateIndices = saveDatas.plateIndices;
				gUsedPageNums = saveDatas.usedPageNums;
				gFirstPageNestHeight = saveDatas.firstPageNestHeight;
			}

			layoutRate = gBestRating;
			bestLocationMark = tmpBestRatingMark;
			if (mIsAbort || (gBestRating >= maxLayoutRating))   // 达到目标排版率直接退出迭代,获知外界直接强制退出迭代
			{
				nestParts = gNestParts;
				usedPageNums = gUsedPageNums;
				plateIndices = gPlateIndices;
				minFirstPageNestHeight = gFirstPageNestHeight;
				mbestSequences = globalBestLayoutSequence;   // 保存全局最优零件序列
				
				return minFirstPageNestHeight;
			}
		}
		else if (maxPageRating < gBestRating) {
			mFireFlySerises[tmpBestRatingMark].sequenceGroups = globalBestLayoutSequence;
			mFireFlySerises[tmpBestRatingMark].layoutRating = gBestRating;
			saveLayoutData[tmpBestRatingMark].nestParts = gNestParts;
			saveLayoutData[tmpBestRatingMark].plateIndices = gPlateIndices;
			saveLayoutData[tmpBestRatingMark].usedPageNums = gUsedPageNums;
			saveLayoutData[tmpBestRatingMark].nestRating = gBestRating;
			saveLayoutData[tmpBestRatingMark].firstPageNestHeight = gFirstPageNestHeight;
		}

		// 初始化最差的萤火虫排版序列
		initWorstFireWormSequence(mFireFlySerises[tmpMinWorst].sequenceGroups, globalBestLayoutSequence);

		printf("迭代第%d次, 最佳萤火虫位置: %d, 最佳排版率为: %f\n", t + 1, bestLocationMark, gBestRating);
		t++;
	}// end while

	printf("hash表的总长度：%d\n", mRecordLayoutSeQ.count());
	mRecordLayoutSeQ.clear();
	mMarkSeqHashs.clear();

	int  mark0 = 0;
	double mark1 = saveLayoutData[0].nestRating;

	for (int i = 1; i < saveLayoutData.count(); i++)
	{
		if (saveLayoutData[i].nestRating != 0.0)
		{
			if (mark1 < saveLayoutData[i].nestRating)
			{
				mark0 = i;
				mark1 = saveLayoutData[i].nestRating;
			}
		}
	}

	nestParts = saveLayoutData[mark0].nestParts;
	usedPageNums = saveLayoutData[mark0].usedPageNums;
	plateIndices = saveLayoutData[mark0].plateIndices;

	mbestSequences = globalBestLayoutSequence;   // 保存全局最优零件序列
	//mLayoutOrders.clear();
	//for (int i = 0; i < globalBestLayoutSequence.count(); i++)
	//{
	//	for (int j = 0; j < globalBestLayoutSequence[i].count(); j++)
	//	{
	//		for (int k = 0; k < globalBestLayoutSequence[i][j].count(); k++)
	//		{
	//			int temp = globalBestLayoutSequence[i][j][k];
	//			mLayoutOrders.append(temp);
	//		}
	//	}
	//}

	return minFirstPageNestHeight;
}

const QVector<int>	& FireFly::getLayoutSequence() const
{
	return mLayoutOrders;
}
