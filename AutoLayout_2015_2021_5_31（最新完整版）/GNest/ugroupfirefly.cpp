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
#define MAXITERM 5000    // ����ө�������������

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

// ө�����Ⱥ��ʼ��
void UgroupFireFly::InitFireFlies()
{
	// ��һ��ө����ʼ��Ⱥ���վ���������ݲ���
	UgroupFireWorm	fireWorm;
	fireWorm.sequenceGroups = mSortSequences;
	mFireFlySerises << fireWorm;

	// �������2/3��allGroupsNum����������2-��11����Ⱥ
	for (int i = 0; i < 10; i++)
	{
		UgroupFireWorm fireWorm1;
		fireWorm1.sequenceGroups = mSortSequences;
		randomChangeData(fireWorm1.sequenceGroups);
		mFireFlySerises << fireWorm1;
	}

	//  �������1/3��allGroupsNum����������12-��21����Ⱥ
	for (int i = 0; i < 19; i++)
	{
		UgroupFireWorm fireWorm1;
		fireWorm1.sequenceGroups = mSortSequences;
		fewChangeData(fireWorm1.sequenceGroups);
		mFireFlySerises << fireWorm1;
	}

	// �����������С���ڵ������������22-��31����Ⱥ
	for (int i = 0; i <10; i++)
	{
		UgroupFireWorm fireWorm1;
		fireWorm1.sequenceGroups = mSortSequences;
		changeTwoData(fireWorm1.sequenceGroups);
		mFireFlySerises << fireWorm1;
	}
}

// ������ֻө���ÿ��С��seQuenceI�ڵ����ӫ������
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

// ������ֻө���ÿ��С���ڵ������ȣ������Ⱦ���ө����ƶ��ľ����С
double UgroupFireFly::calAttraction_i_j(double rI_J)
{
	double attractionResult = 0.0;
	double doubleR = rI_J*rI_J;
	attractionResult = B0*pow(e, -r*doubleR);
	// attractionResult = 0.3697Ϊ����rI_J����0ʱ�������ȴ�С
	if ((attractionResult > 0.37) && (attractionResult < 0.5)) attractionResult *= 1.7;		// ����ө���Ѱ�ź��ڴ�����ͬ����ʱ��������иı����

	return attractionResult;
}

// ȥ��sequence�в����ڵ�ֵ��������ֵ����Ϊ-1
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

// ȥ��sequence���ظ���ֵ������������Ϊ-1����ȥ�غ�����б��浽noRepeate��
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

// ��������sequence�в��ɱ����ֵ��eg:ԭʼ����������(3,1,4,1,2,5,2,8,15) 
//�������������ȡֵӦ��Ϊ��1-9���� ==> ����������Ϊ��3,1,4,6,2,5,7,8��9��
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
	if (sameNum == orignalSequence.count()) // �����ǰ�����������ͬ,��ֱ�ӷ���
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
	if (noRepeateSequence.count() == orignalSequence.count()) // ���������е�ֵ��ȫ�����ֱ�ӷ���
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

		if (!mark) // û���ҵ�����sequence�Ķ�Ӧλ�ø��¸�ֵ
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

	// �������֮�󣬽�sequence�ж�Ӧ��ֵ��seq�������н�����Ӧ�ĸ���
	int   tempIndex = 0;		// ���Ŀǰ��sequence�ĵ�tempIndex������ֵ����
	for (int i = 0; i < seq.count(); i++)
	{
		seq[i] = sequence[tempIndex];
		tempIndex++;
	}
}

// ����sequenceI��sequenceJ�ƶ�����
void UgroupFireFly::updateLocationItoJ(QVector<int> &sequenceI, const QVector<int> &sequenceJ, double attraction)
{
	for (int i = 0; i < sequenceI.count(); i++)
	{
		double a = (rand() % 1000) / 1000.0;
		double rands = (rand() % 1000) / 1000.0;
		sequenceI[i] = (1 - attraction)*sequenceI[i] + attraction*sequenceJ[i] + a*(rands - 0.5);
	}

	// �Բ����б���ִ�и���
	dealInfeasibleSequence(sequenceI, sequenceJ);
}

// ��ʼ������ө�����������
void	UgroupFireFly::initWorstFireWormSequence(QVector<int> &sequences, const QVector<int>  &bestSeq)
{
	sequences.clear();
	sequences = mSortSequences;

	fewChangeData(sequences);
}


// ִ��ө����㷨
double UgroupFireFly::optimize(int maxIter,			// ����������
	double maxLayoutRating,					// ����Ű���
	QVector< GPart > &nestParts,			// �������
	QVector<int> &usedPageNums,				// ���ʹ������
	QVector<int> &plateIndices,				// �������
	double &layoutRate,						// �Ű���
	int *pProgress)
{
	srand((unsigned)time(NULL));			// ��ʼ�����������
	quint32	selectFlag = 0x01;			// ������ө���ӫ������һ��ʱ����ǲ�����һ�ַ�ʽ΢���������

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
	// ���ݾ����������һ��ө�����Ű��ʣ���������Ϊ��Ӧ��ֵ
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
	printf("������������ʱ��:%d\n", layoutTime0.elapsed());

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
	QVector<int>	globalBestLayoutSequence;	// �ݴ�ȫ�������Ű�����
	QVector<GPart>	gNestParts;
	QVector<int>	gUsedPageNums;
	QVector<int>	gPlateIndices;
	int				gFirstPageNestHeight;
	int				gLastPageForwardLines= 100000;

	for (int i = 0; i < mFireFlyNumbers; i++)
	{
		printf("��ӡ��%d����Ⱥ���Ű���Ϊ��%f, �����߶�Ϊ��%f\n", i + 1, mFireFlySerises[i].layoutRating, saveLayoutData[i].firstPageNestHeight);
		if (saveLayoutData[i].firstPageNestHeight < minFirstPageNestHeight) minFirstPageNestHeight = saveLayoutData[i].firstPageNestHeight;

		if (((mFireFlySerises[i].layoutRating- gBestLayoutRating)>1e-6)|| (fabs(mFireFlySerises[i].layoutRating - gBestLayoutRating)<1e-6 && saveLayoutData[i].lastPageForwardLines < gLastPageForwardLines))
		{
			gBestLayoutRating = mFireFlySerises[i].layoutRating;
			gLastPageForwardLines = saveLayoutData[i].lastPageForwardLines;
			// ��hash���в��Ҷ�Ӧ�Ű�����
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

	// ����ө���Ѱ��
	int				t = 0;
	int				bestLocationMark = 0;						// ��¼���ŵ���											// �����е�ө����±�
																//double			bestRating = 0;								// ��¼ȫ������Ű���
	MaxIterm = maxIter;

	int numMark = 0;
	while (t < MaxIterm)
	{
		if (pProgress) // ����Ŀǰ�ĵ�������
		{
			*pProgress = qRound((t + 1) * 100.0 / maxIter);
		}
		for (int m = 0; (m < mFireFlyNumbers) && (!mIsAbort); m++)
		{
			// ����ө���λ�ø���
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
					// ����ֻө���ӫ���������ʱ��ѡ������һֻө�����������н���΢��
					// �����������Ϊtʱ����Ҫ������С���������
					//printf("ÿ����Ҫ����������ܸ�����%d, ���������������%d\n", allSubGroupNums , classNum);
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
					// ��hash���в��Ҷ�Ӧ�Ű�����
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

		 // ��¼����Ű��ʵ�ө����±�
		double maxPageRating = mFireFlySerises[0].layoutRating;
		int   tmpBestRatingMark = 0;
		int   tmpLastPageForwardLines = 100000;
		float  minPageRating = maxPageRating;
		int   tmpMinWorst = 0;	// ��¼�Ű�����ө����±�

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

			//��hash������ѻ�ȡ�Ű���Ϣ
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
			if (mIsAbort || (gBestLayoutRating - maxLayoutRating)>=1e-6)   // �ﵽĿ���Ű���ֱ���˳�����,��֪���ֱ��ǿ���˳�����
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

		// ��ʼ������ө����Ű�����
		initWorstFireWormSequence(mFireFlySerises[tmpMinWorst].sequenceGroups, globalBestLayoutSequence);
		printf("������%d��, ���ө���λ��: %d, ����Ű���Ϊ: %f\n", t + 1, bestLocationMark, gBestLayoutRating);
		t++;
	}// end while

	printf("hash����ܳ��ȣ�%d\n", mRecordLayoutSeQ.count());
	printf("��ӡ�ı�����hash����ܳ���:%d\n", mMarkSeqHashs.count());
	mRecordLayoutSeQ.clear();
	mMarkSeqHashs.clear();

	layoutRate = gBestLayoutRating;
	nestParts = gNestParts;
	usedPageNums = gUsedPageNums;
	plateIndices = gPlateIndices;

	//mbestSequences = globalBestLayoutSequence;   // ����ȫ�������������
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
