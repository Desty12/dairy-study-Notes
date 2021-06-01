#include "firefly.h"
#include "time.h"
#include "omp.h"
#include "gmath.h"
#include <iostream>
#include <cstdlib>
#include <gautolayout.h>
#include <QTime>

#define e 2.718
#define MAXITERM 10    // 定义萤火虫最大迭代次数

using namespace  std;
int numProcs = omp_get_num_procs();

// 保存排版结果
struct SaveLayoutData
{
	QVector<GPart>	nestParts;
	QVector<int>	usedPageNums;
	QVector<int>	plateIndices;
	double          nestRating;

	SaveLayoutData()
	{
		nestRating = 0.0;
	}

};


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
	for (int i = 0; i < 8; i++)
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
	for (int i = 0; i < 8; i++)
	{
		FireWorm fireWorm1;
		quint32  selectFlag = 0x01;
		int	changeNum = ceil(allGroupsNum*(1 / 3.0));
		//if (changeNum > allGroupsNum) changeNum = allGroupsNum;

		if (i >= 4) selectFlag = 0x02;
		fireWorm1.sequenceGroups = mClusterGroups;
		changeGroupsDatas(fireWorm1.sequenceGroups, changeNum, selectFlag, true);
		mFireFlySerises << fireWorm1;
	}

	// 随机调整两个小组内的零件，产生第22-第31个种群
	for (int i = 0; i < 8; i++)
	{
		FireWorm fireWorm1;
		quint32  selectFlag = 0x01;
		//if (changeNum > allGroupsNum) changeNum = allGroupsNum;

		if (i >= 4) selectFlag = 0x02;
		fireWorm1.sequenceGroups = mClusterGroups;
		changeGroupsDatas(fireWorm1.sequenceGroups, 2, selectFlag, true);
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
		if (seQ1[i1] == seQ2[i1])
			sum++;
	}

	allDistances = seQ1.count();

	return sum;
}


// 计算两只萤火虫每个小组内的吸引度，吸引度决定萤火虫移动的距离大小
double FireFly::calAttraction_i_j(int rI_J, int allDistance)
{
	float attractionResult = 0.0;
	// 取值范[0, 2.3]
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
		if ((!tmpMarks) && (sequence[j0] != -1))
		{
			sequence[j0] = -1;
		}
	}
}

// 去除sequence中重复的值，并将其设置为-1，将去重后的序列保存到noRepeate中
void polishRepeate(QVector<int>	&sequence, QVector<int> &noRepeate)
{
	noRepeate.clear();
	int num;
	for (int i = 0; i < sequence.count(); i++)
	{
		num = i;
		if (sequence[i] != -1)
		{
			noRepeate.append(sequence[i]);
			num++;
			break;
		}
	}
	for (int i = num; i < sequence.count(); i++)
	{
		if (sequence[i] != -1)
		{
			bool mark = false;
			for (int j = 0; j < noRepeate.count(); j++)
			{
				if (sequence[i] == noRepeate[j])
				{
					mark = true;
					break;
				}
			}
			if (!mark)
			{
				noRepeate.append(sequence[i]);
			}
			else
			{
				sequence[i] = -1;
			}
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
		QVector<int>	&tepSeq0 = seq[i];
		sequence << tepSeq0;

		const QVector<int>    &tepSeq1 = orignalSeq[i];
		orignalSequence << tepSeq1;
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
	QVector<int> noRepeateSequence;
	polishRepeate(sequence, noRepeateSequence);
	if (noRepeateSequence.count() == orignalSequence.count()) // 若两个序列的值完全相等则直接返回
	{
		return;
	}
	QVector<int> noScanIndex;
	for (int i = 0; i < orignalSequence.count(); i++)
	{
		bool mark = false;
		for (int j = 0; j < noRepeateSequence.count(); j++)
		{
			if (orignalSequence[i] == noRepeateSequence[j])
			{
				mark = true;
				break;
			}
		}
		if (!mark)
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

	QVector<int>		seQ1;
	QVector<int>		seQ2;
	for (int i1 = 0; i1 < sequenceI.count(); i1++)
	{
		for (int j1 = 0; j1 < sequenceI[i1].count(); j1++)
		{
			QVector<int>	&temp = sequenceI[i1][j1];
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
		float a = (rand() % 11) / 10.0;
		float rands = (rand() % 11) / 10.0;
		seQ1[i1] = (1 - attraction)*seQ1[i1] + attraction*seQ2[i1] + a*(rands - 0.5);
		//cout << "打印seQ1中的每一个元素值" <<seQ1[i1]<< endl;
	}
	//cout << endl;

	int len0 = 0;
	for (int i2 = 0; i2 < sequenceI.count(); i2++)
	{
		for (int j2 = 0; j2 < sequenceI[i2].count(); j2++)
		{
			for (int k2 = 0; k2 < sequenceI[i2][j2].count(); k2++)
			{
				sequenceI[i2][j2][k2] = seQ1[len0];
				len0++;
				//cout << "打印seQ1中的每一个元素值" << sequenceI[i2][j2][k2] << endl;
			}
		}
	}

	// 对不可行编码执行更新
	for (int i2 = 0; i2 < sequenceI.count(); i2++)
	{
		dealInfeasibleSequence(sequenceI[i2], tempS[i2]);
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

	changeGroupsDatas(sequences, GroupsNum, 0x02, false);
}

// 执行萤火虫算法
void FireFly::optimize(int maxIter,			// 最大迭代次数
	double maxLayoutRate,					// 最大排版率
	QVector< GPart > &nestParts,			// 排样零件
	QVector<int> &usedPageNums,				// 板材使用数量
	QVector<int> &plateIndices,				// 板材索引
	double &layoutRate,						// 排版率
	int *pProgress)
{
	srand((unsigned)time(NULL));			// 初始化随机数种子
	quint32		selectMark = 0x01;			// 当两个萤火虫荧光亮度一致时，标记采用哪一种方式微调零件序列
	int   allSubGroupNums = 0;
	for (int i = 0; i < mClusterGroups.count(); i++)
	{
		allSubGroupNums += mClusterGroups[i].count();
	}

	InitFireFlies();
	mFireFlyNumbers = mFireFlySerises.count();

	QVector<SaveLayoutData>		saveLayoutData(mFireFlyNumbers);
	// 根据聚类结果计算第一个萤火虫的排版率，并将其作为适应度值
	QTime layoutTime0;
	layoutTime0.start();

	mFireFlySerises[0].layoutRating = layout(mFireFlySerises[0].sequenceGroups, saveLayoutData[0].nestParts,
		saveLayoutData[0].usedPageNums, saveLayoutData[0].plateIndices);
	saveLayoutData[0].nestRating = mFireFlySerises[0].layoutRating;
	QVector<int>		firstSequence;
	groupsSeq2Seq(mFireFlySerises[0].sequenceGroups, firstSequence);
	mRecordLayoutSeQ.insert(firstSequence, mFireFlySerises[0].layoutRating);
	printf("运行排样所需时间:%d\n", layoutTime0.elapsed());

	// 使用openmp进行并行计算，计算每一只萤火虫的初始排版率
	// #pragma omp parallel for num_threads(numProcs)
	for (int i = 1; i < mFireFlyNumbers; i++)
	{
		QVector<int>	tempSequence;
		groupsSeq2Seq(mFireFlySerises[i].sequenceGroups, tempSequence);
		QHash<QVector<int>, double>::iterator iter = mRecordLayoutSeQ.find(tempSequence);
		if (iter != mRecordLayoutSeQ.end())
		{
			mFireFlySerises[i].layoutRating = iter.value();
		}
		else
		{
			mFireFlySerises[i].layoutRating = layout(mFireFlySerises[i].sequenceGroups, saveLayoutData[i].nestParts,
				saveLayoutData[i].usedPageNums, saveLayoutData[i].plateIndices);
			saveLayoutData[i].nestRating = mFireFlySerises[i].layoutRating;

			mRecordLayoutSeQ.insert(tempSequence, mFireFlySerises[i].layoutRating);
		}
	}

	for (int i = 0; i < mFireFlyNumbers; i++)
	{
		printf("打印第%d个种群的排版率为：%f\n", i + 1, mFireFlySerises[i].layoutRating);
	}

	// 进行萤火虫寻优
	int				t = 0;
	int				bestLocationMark = 0;				// 记录最优的排样序列的萤火虫下标
	float			bestRating = 0;					    // 记录全局最佳排版率
														//QVector<int>	bestLayoutSequence;				    // 记录零件最佳排样序列
	QVector<QList<QVector<int> > >	globalBestLayoutSequence;	// 暂存全局最优排版序列
	MaxIterm = maxIter;

	QTime   layoutTime2;
	layoutTime2.start();
	int numMark = 0;

	while (t < MaxIterm)
	{
		if (pProgress) // 计算目前的迭代进度
		{
			*pProgress = qRound((t + 1) * 100.0 / maxIter);
		}
		for (int i2 = 0; i2 < mFireFlyNumbers && (!mIsAbort); i2++)
		{
			// 进行萤火虫位置更新
#pragma omp parallel for  num_threads(numProcs)
			for (int j3 = 0; j3 < i2; j3++)
			{
				if (mFireFlySerises[i2].layoutRating > mFireFlySerises[j3].layoutRating)
				{
					int allSequences = 0;
					int tempDistance = calDistance_i_j(mFireFlySerises[j3].sequenceGroups, mFireFlySerises[i2].sequenceGroups, allSequences);
					double tempAttraction = calAttraction_i_j(tempDistance, allSequences);
					updateLocationItoJ(mFireFlySerises[j3].sequenceGroups, mFireFlySerises[i2].sequenceGroups, tempAttraction);
				}
				else if (mFireFlySerises[i2].layoutRating == mFireFlySerises[j3].layoutRating)
				{
					// 当两只萤火虫荧光亮度相等时，选择其中一只萤火虫的排样序列进行微调
					// 计算迭代次数为t时，需要调整的小组零件个数
					int classNum = 0;
					double  minvalue = 2 / allSubGroupNums*1.0;
					double  tempValue = 0.66 - minvalue;

					if (t < 0.7*maxIter) classNum = ceil((0.66 - (tempValue / maxIter)*t)*allSubGroupNums);
					else classNum = 2;

					if (classNum >= allSubGroupNums) classNum = ceil(0.66*allSubGroupNums);
					else if (classNum <= 0) classNum = 1;

					if (0x01 & selectMark)
					{
						changeGroupsDatas(mFireFlySerises[j3].sequenceGroups, classNum, selectMark, false);
						selectMark <<= 1;
					}
					else if (0x02 & selectMark)
					{
						changeGroupsDatas(mFireFlySerises[j3].sequenceGroups, classNum, selectMark, false);
						selectMark <<= 1;
					}
					else if ((0x04 & selectMark) || (selectMark & 0x08))
					{
						// 随机选择一个小组，对小组内的零件进行乱序调整
						smallGroupSeqAdjust(mFireFlySerises[j3].sequenceGroups);
						// changeGroupsDatas(mFireFlySerises[j3].sequenceGroups, classNum, selectMark,false);
						selectMark <<= 1;
						if (selectMark > 0x08)
							selectMark = 0x01;
					}
				} // end if-else	
			}// end for

			 // 计算萤火虫位置更新后的排版率
#pragma omp parallel for  num_threads(numProcs)
			for (int j3 = 0; j3 < i2; j3++)
			{
				QVector<int>	tmpSequence;
				groupsSeq2Seq(mFireFlySerises[j3].sequenceGroups, tmpSequence);
				QHash<QVector<int>, double>::iterator iter = mRecordLayoutSeQ.find(tmpSequence);
				if (iter != mRecordLayoutSeQ.end())
				{
					mFireFlySerises[j3].layoutRating = iter.value();
				}
				else
				{
					mFireFlySerises[j3].layoutRating = layout(mFireFlySerises[j3].sequenceGroups, saveLayoutData[j3].nestParts,
						saveLayoutData[j3].usedPageNums, saveLayoutData[j3].plateIndices);
					saveLayoutData[j3].nestRating = mFireFlySerises[j3].layoutRating;

#pragma omp critical
					mRecordLayoutSeQ.insert(tmpSequence, mFireFlySerises[j3].layoutRating);
				}
			}
			numMark += i2;
			//printf("计算排版率的时间%d\n", t2.elapsed());
		}// end for

		 // 记录最佳排版率的萤火虫下标
		float maxPageRating = mFireFlySerises[0].layoutRating;
		int   tmpMarkBestRating = 0;
		float  minPageRating = maxPageRating;
		int   tmpMinWorst = 0;	// 记录排版最差的萤火虫下标
		for (int i = 1; i < mFireFlyNumbers; i++)
		{
			if (mFireFlySerises[i].layoutRating > maxPageRating)
			{
				maxPageRating = mFireFlySerises[i].layoutRating;
				tmpMarkBestRating = i;
			}
			else
			{
				minPageRating = mFireFlySerises[i].layoutRating;
				tmpMinWorst = i;
			}
		}

		if (maxPageRating > bestRating)
		{
			bestRating = maxPageRating;
			globalBestLayoutSequence = mFireFlySerises[tmpMarkBestRating].sequenceGroups;
			bestLocationMark = tmpMarkBestRating;
			layoutRate = bestRating;

			if (mIsAbort) return;   // 强制退出迭代
			// 达到目标排版率直接退出迭代
			// if (bestRating >= maxLayoutRate) return;
		}
		else if (maxPageRating < bestRating)  // 如果寻优结果变差则立即返回排样最终结果
		{
			mFireFlySerises[tmpMarkBestRating].sequenceGroups = globalBestLayoutSequence;
			mFireFlySerises[tmpMarkBestRating].layoutRating = bestRating;
		}
		// 初始化最差的萤火虫排版序列
		initWorstFireWormSequence(mFireFlySerises[tmpMinWorst].sequenceGroups, globalBestLayoutSequence);
		printf("迭代第%d次, 最佳萤火虫位置: %d, 最佳排版率为: %f\n", t + 1, bestLocationMark, bestRating);
		t++;

	}// end while

	printf("总共执行%d次排样所需的时间为:%d\n", numMark, layoutTime2.elapsed());

	printf("hash表的总长度：%d\n", mRecordLayoutSeQ.count());
	printf("打印改变序列hash表的总长度:%d\n", mMarkSeqHashs.count());
	mRecordLayoutSeQ.clear();
	mMarkSeqHashs.clear();

	int  mark0 = 0;
	double mark1 = saveLayoutData[0].nestRating;

	for (int i = 1; i < saveLayoutData.count(); i++)
	{
		if (saveLayoutData[i].nestParts.count() != 0 && saveLayoutData[i].nestRating != 0.0)
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

	// 用于测试大小组显示情况
	mLayoutOrders.clear();
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
	}
}

const QVector<int>	& FireFly::getLayoutSequence() const
{
	return mLayoutOrders;
}