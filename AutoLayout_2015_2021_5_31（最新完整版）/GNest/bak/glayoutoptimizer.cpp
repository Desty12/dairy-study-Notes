#include "glayoutoptimizer.h"

GLayoutOptimizer::GLayoutOptimizer()
{
	mIsAbort = false;
}

GLayoutOptimizer::~GLayoutOptimizer()
{

}

void GLayoutOptimizer::setLayoutParam(quint32 layoutFlag,
	const QVector<GPart> *	pPagePlates,
	const QVector<int> *	pPageNum,
	const QVector<GPart> *	pParts,
	const QVector<int> *	pPartNums,
	const QVector< GAutoLayout::PartTemplate * > *pPartTemplates,
	double					gridDelta
)
{
	mLayoutFlag = layoutFlag;
	mpPagePlates = pPagePlates;
	mpPageNums = pPageNum;
	mpParts = pParts;
	mpPartNums = pPartNums;
	mpPartTemplates = pPartTemplates;
	mGridDelta = gridDelta;
}

void GLayoutOptimizer::setClusterGroup(const QVector<QList<QVector<int> > > &	clusterGroups)
{
	mClusterGroups = clusterGroups;
}

void GLayoutOptimizer::setOptimizeParam(const QHash<QString, QVariant> &params)
{
	mOptParams = params;
}

void GLayoutOptimizer::optimize(int maxIter,
	double maxLayoutRate,
	QVector< GPart > &nestParts,
	QVector<int> &usedPageNums,
	QVector<int> &plateIndices,
	double &layoutRate,
	int *pProgress)
{

}

void GLayoutOptimizer::abort()
{
	mIsAbort = true;
}


void dataSwap(int &a1, int &a2)
{
	int a = a1;
	a1 = a2;
	a2 = a;
}

// 随机调整小组内部的零件序列
void GLayoutOptimizer::randomChangeData(QVector<int> &arr1)
{
	QVector<int> tempMark;  // 用于随机取值的临时数组
	for (int i = 0; i < arr1.count(); i++)
	{
		tempMark.append(arr1[i]);
	}
	if (tempMark.count() > 2)
	{
		QVector<int> tempArr;
		int numbers = arr1.count();
		for (int i = numbers - 1; i >= 0; i--)
		{
			int randValue = rand() % tempMark.count();
			tempArr.append(tempMark[randValue]);
			tempMark.remove(randValue);
		}
		arr1.clear();
		arr1 = tempArr;
		tempArr.clear();
	}
	tempMark.clear();
}

// 随机选择组内两个位置进行数据交换
void GLayoutOptimizer::changeTwoData(QVector<int> &arr)
{
	QVector<int> tempSort;
	for (int i = 0; i < arr.count(); i++)
	{
		tempSort.append(i);
	}
	if (tempSort.count() >= 2) // 只对组内数据长度大于等于2的分组进行序列调整
	{
		int temp1 = rand() % tempSort.count();
		int temp11 = tempSort[temp1];
		tempSort.remove(temp1);

		int temp2 = rand() % tempSort.count();
		int temp22 = tempSort[temp2];
		dataSwap(arr[temp11], arr[temp22]);
	}
	else
	{
		tempSort.clear();
		return;
	}
	tempSort.clear();
}

// 对组内数据进行微调
void GLayoutOptimizer::fewChangeData(QVector<int> &arr)
{
	QVector<int> tempSort;
	for (int i = 0; i < arr.count(); i++)
	{
		tempSort.append(i);
	}
	if (tempSort.count() > 2) // 只对组内数据长度大于2的分组进行序列调整
	{
		int changeNum = tempSort.count() / 2;  // 数组微调的次数
		for (int i = 0; i < changeNum; i++) // 微调数组长度的1/2次
		{
			int temp1 = rand() % tempSort.count();
			int temp11 = tempSort[temp1];
			tempSort.remove(temp1);
			if (tempSort.count() == 0)
				break;

			int temp2 = rand() % tempSort.count();
			int temp22 = tempSort[temp2];
			tempSort.remove(temp2);

			dataSwap(arr[temp11], arr[temp22]);
			if (tempSort.count() == 0)
				break;
		}
	}
	tempSort.clear();
}

// 调整大组tmpList中的小组零件排样序号
void GLayoutOptimizer::changSmallGroupsData(QVector<int>  &tmpSequences, quint32 mark)
{
	int partsAllNum = tmpSequences.count();

	if (0x01 & mark)
	{
		if (partsAllNum > 2) randomChangeData(tmpSequences);
	}
	else if (0x02 & mark)
	{
		if (partsAllNum > 2) fewChangeData(tmpSequences);
	}
	else if ((0x04 & mark) || (mark & 0x08))
	{
		changeTwoData(tmpSequences);
	}
}

void GLayoutOptimizer::changeSmallGroupsOrders(QVector<QList<QVector<int> > > &tempSeQ)
{
	// 每一个大组内的小组前后顺序随机调动
	for (int i = 0; i < tempSeQ.count(); i++)
	{
		if (tempSeQ[i].count() >= 2)
		{
			QList<QVector<int> >	tmpGroups;
			QVector<int>	markGroups;		// 记录小组个数
			for (int k = 0; k < tempSeQ[i].count(); k++)
			{
				markGroups.append(k);
			}
			int t = markGroups.count();

			while (t > 0)
			{
				int temp = rand() % markGroups.count();
				int temp1 = markGroups[temp];
				markGroups.remove(temp);
				tmpGroups.append(tempSeQ[i][temp1]);
				t--;
			}
			tempSeQ[i].clear();
			tempSeQ[i] = tmpGroups;
		}
	}
}

void  GLayoutOptimizer::groupsSeq2Seq(const QVector<QList<QVector<int> > > &sequence, QVector<int> &orders)
{
	orders.clear();
	for (int i1 = 0; i1 < sequence.count(); i1++)
	{
		for (int j1 = 0; j1 < sequence[i1].count(); j1++)
		{
			for (int k1 = 0; k1 < sequence[i1][j1].count(); k1++)
			{
				int temp = sequence[i1][j1][k1];
				orders.append(temp);
			}
		}
	}
}

// 选择需要调整的大组序号
void GLayoutOptimizer::changeGroupsDatas(QVector<QList<QVector<int> > > &aimSequence, int changeNum0, quint32 mark, bool isHashFind)
{
	bool isSameSeq = false;
	int nHashFind = 0;
	while (!isSameSeq)
	{
		// 1、选择changeNum个小组
		int	 hasMarkedSum = 1;
		int  smallGroupNum = 0;
		for (int i = 0; i < aimSequence.count(); i++)
		{
			smallGroupNum += aimSequence[i].count();
		}

		if (changeNum0 > smallGroupNum) changeNum0 = round(smallGroupNum / 2.0);
		//printf("\n输出每次需要调整的小组数：%d\t，总的小组个数为：%d\n\n",changeNum0,smallGroupNum);

		QVector<int> tmpGroupsMark;				// 暂存大组组号
		QList<QVector<int> > tmpSubGroupsMark;   // 暂存每个大组中的小组号
		for (int i = 0; i < aimSequence.count(); i++)
		{
			tmpGroupsMark << i;
			QVector<int>  tmps;
			for (int j = 0; j < aimSequence[i].count(); j++)
			{
				tmps << j;
			}
			tmpSubGroupsMark << tmps;
		}

		while (hasMarkedSum <= changeNum0)
		{
			// 先选择能访问的大组号
			int temp0 = rand() % tmpGroupsMark.count();
			int groupIndex = tmpGroupsMark[temp0];

			// 再选择需要调整的小组号
			int subTemp0 = rand() % tmpSubGroupsMark[groupIndex].count();
			int subGroupsIndex = tmpSubGroupsMark[groupIndex][subTemp0];
			tmpSubGroupsMark[groupIndex].remove(subTemp0);

			changSmallGroupsData(aimSequence[groupIndex][subGroupsIndex], mark);

			if (tmpSubGroupsMark[groupIndex].count() == 0)
			{
				tmpGroupsMark.remove(temp0);
				if (tmpGroupsMark.count() == 0) break;
			}
			hasMarkedSum++;

		}// end while

		 // 调整大组内部的小组前后顺序
		changeSmallGroupsOrders(aimSequence);

		if (!isHashFind) break;  // 如果不执行在hash表中查找直接跳出循环

								 // 在hash表中查找当前所有零件序列是否已经重复生成
		QVector<int>	tmpSeq;
		groupsSeq2Seq(aimSequence, tmpSeq);
		if (mMarkSeqHashs.count() != 0)
		{
			QHash<QVector<int>, int >::iterator  hashIter = mMarkSeqHashs.find(tmpSeq);
			if (hashIter != mMarkSeqHashs.end())
			{
				nHashFind++;
				if (nHashFind > 5) break;
				else continue;
			}
			else
			{
				// 当前序列未在hash表中找到，则将当前序列插入hash表
				mMarkSeqHashs.insert(tmpSeq, 0);
				isSameSeq = true;
				break;
			}
		}
		else
		{
			mMarkSeqHashs.insert(tmpSeq, 0);
			break;
		}
	}// end while
}


void  GLayoutOptimizer::smallGroupSeqAdjust(QVector<QList<QVector<int> > >  layoutSeq)
{
	int groupNums = layoutSeq.count();
	int num0 = rand() % groupNums;

	int smallGroupNums = layoutSeq[num0].count();
	int num1 = rand() % smallGroupNums;

	randomChangeData(layoutSeq[num0][num1]);
}


double  GLayoutOptimizer::layout(QVector<QList<QVector<int> > >  layoutSeq,
	QVector< GPart > &tmpNestParts,
	QVector<int> &tmpUsedPageNums,
	QVector<int> &tmpPlateIndices)
{
	double          layoutRaing;
	GAutoLayout		gautoLayout2;
	gautoLayout2.setLayoutParam((*mpPagePlates), (*mpPageNums), mGridDelta, *mpParts, *mpPartNums);
	gautoLayout2.setPartTemplates(*mpPartTemplates);
	gautoLayout2.setLayoutFlag(mLayoutFlag);
	tmpNestParts.clear();
	tmpUsedPageNums.clear();
	tmpPlateIndices.clear();

	QVector<int>  tempSorts2;
	// printf("\n\n打印每次排样时的零件序列\n");
	for (int j = 0; j < layoutSeq.count(); j++)
	{
		for (int k = 0; k < layoutSeq[j].count(); k++)
		{
			for (int m = 0; m < layoutSeq[j][k].count(); m++)
			{
				tempSorts2.append(layoutSeq[j][k][m]);
			}
		}
	}

	QVector<quint32> rotFlags;
	layoutRaing = gautoLayout2.layout(tempSorts2, rotFlags, tmpNestParts, tmpUsedPageNums, tmpPlateIndices, 0);

	return layoutRaing;
}
