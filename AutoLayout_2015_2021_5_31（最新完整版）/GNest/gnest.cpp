#include <QHash>
#include "gnest.h"
#include "gautolayout.h"
#include "gpartsort.h"
#include "gclustersort.h"
#include <QDateTime>
#include "gfirefly.h"
#include "gsaoptimizer.h"
#include "ugroupfirefly.h"


// ��������
struct NestData
{
public:
	QVector< GPart >	mPagePlates;		// ĸ��
	QVector< int >		mPalteNums;			// ĸ������
	QVector< GPart >	mParts;				// �������
	QVector< int >		mPartNums;			// �������������
	double				mNestGrid;			// ��������
	quint32				mNestFlag;			// ����ѡ�����

	QVector< GPart >	mNestParts;			// ����������
	QVector< int >		mUsedPlateNums;		// ����ʹ�õ�ĸ������
	int					mTotalPageNums;		// ����ĸ������
	QVector< int >		mPlateIndices;		// ����ĸ���±�

	QHash< QPair<int, int>, int >	mPartIndexHash;		// ��������Ĺ�ϣ��
														// QPair<PartNo, index> => ����������±�
	GAutoLayout			mAutoLayout;
	GLayoutOptimizer *	mpOptimizer;		// �����Ż���

	NestData()
	{
		mTotalPageNums = 0;
		mNestFlag = LAYOUT_OPT_ROTATE90 | LAYOUT_OPT_ROTATE180 | LAYOUT_OPT_ROTATE270 |
					LAYOUT_OPT_ORDER_BY_X;
		mNestGrid = 10;
	}
	~NestData() {}
};

// ��������ָ��
NestData * gNestData = NULL;

// �Ƿ���������
bool gIsStartNest = false;

int InitNest()
{
	// ���������������ܳ�ʼ��
	if (gIsStartNest) return -1;

	if (gNestData) delete gNestData;
	gNestData = NULL;
	gNestData = new NestData;

	return 0;
}

int DestroyNest()
{
	// ���������������ܳ�ʼ��
	if (gIsStartNest) return -1;

	if (gNestData) delete gNestData;
	gNestData = NULL;

	ClearParts();
	ClearPlates();

	return 0;
}

int indexOfPlate(int plateID)
{
	for (int i = 0; i < gNestData->mPagePlates.count(); i++)
	{
		if (gNestData->mPagePlates[i].mPartID == plateID)
		{
			return i;
		}
	}
	return -1;
}

int AddNewPlates(int plateID, int NestNum, int nCount, double *data)
{
	if (!gNestData || gIsStartNest || NestNum <= 0) return -1;
	if (indexOfPlate(plateID) >= 0) return -1;

	gNestData->mPagePlates << GPart();
	GPart &poly = gNestData->mPagePlates.last();
	poly.mPartID = plateID;

	for (int i = 0; i < nCount; i++)
	{
		poly << QPointF(data[i * 2], data[i * 2 + 1]);
	}

	gNestData->mPalteNums << NestNum;

	return 0;
}

int AddInnerHoleToPlate(int plateID, int nCount, double *data)
{
	if (!gNestData || gIsStartNest) return -1;

	int index = indexOfPlate(plateID);
	if (index < 0) return -1;

	GPart &plate = gNestData->mPagePlates[index];
	plate.mHoles << QPolygonF();
	QPolygonF &poly = plate.mHoles.last();
	
	for (int i = 0; i < nCount; i++)
	{
		poly << QPointF(data[i * 2], data[i * 2 + 1]);
	}

	return 0;
}

int ClearPlates()
{
	if (!gNestData || gIsStartNest) return -1;

	gNestData->mPagePlates.clear();

	return 0;
}

int ClearParts()
{
	if (!gNestData || gIsStartNest) return -1;

	gNestData->mParts.clear();
	gNestData->mNestParts.clear();
	gNestData->mPartIndexHash.clear();

	return 0;
}

int indexOfPart(int partID)
{
	for (int i = 0; i < gNestData->mParts.count(); i++)
	{
		if (gNestData->mParts[i].mPartID == partID)
		{
			return i;
		}
	}

	return -1;
}

int AddNewSpareParts(int partID, int NestNum, int nCount, double *data)
{
	if (!gNestData || gIsStartNest || NestNum <= 0) return -1;

	if (indexOfPart(partID) >= 0) return -1;

	gNestData->mParts << GPart();
	GPart &part = gNestData->mParts.last();
	for (int i = 0; i < nCount; i++)
	{
		part << QPointF(data[i * 2], data[i * 2 + 1]);
	}
	part.mPartID = partID;
	gNestData->mPartNums << NestNum;

	return 0;
}

int AddInnerHoleByPartNo(int partID, int nCount, double *data)
{
	if (!gNestData || gIsStartNest) return -1;

	int partIndex = indexOfPart(partID);
	if (partIndex < 0) return -1;

	GPart &part = gNestData->mParts[partIndex];
	part.mHoles << QPolygonF();
	QPolygonF &hole = part.mHoles.last();
	for (int i = 0; i < nCount; i++)
	{
		hole << QPointF(data[i * 2], data[i * 2 + 1]);
	}

	return 0;
}

int SetRotAngleByPartNo(int partID, char flag)
{
	if (!gNestData || gIsStartNest) return -1;

	int partIndex = indexOfPart(partID);
	if (partIndex < 0) return -1;

	GPart &part = gNestData->mParts[partIndex];
	part.mLayoutFlag = flag;
	return true;
}

int SetPartPriority(int partID, int priority)
{
	if (!gNestData || gIsStartNest) return -1;

	int index = indexOfPart(partID);
	if (index < 0) return -1;

	gNestData->mParts[index].mPriority = priority;

	return 0;
}

int SetNestPriorityDir(int data)
{
	if (!gNestData || gIsStartNest) return -1;

	quint32 &flag = gNestData->mNestFlag;

	if (data == 1)
	{
		flag &= ~LAYOUT_OPT_ORDER_BY_Y;
		flag |= LAYOUT_OPT_ORDER_BY_X;
	}
	else
	{
		flag &= ~LAYOUT_OPT_ORDER_BY_X;
		flag |= LAYOUT_OPT_ORDER_BY_Y;
	}

	return 0;
}

int SetNestSpacing(double spaceGrid)
{
	if (!gNestData || gIsStartNest) return -1;

	gNestData->mNestGrid = spaceGrid;

	return 0;
}

int SetPartEnablePolarRotation(bool IsEnbale)
{
	if (!gNestData || gIsStartNest) return -1;

	if (IsEnbale)
	{
		gNestData->mNestFlag |= LAYOUT_OPT_OOBB_ROT;
	}
	else
	{
		gNestData->mNestFlag &= ~LAYOUT_OPT_OOBB_ROT;
	}

	return 0;
}

int SetSparePartsAngle(char flag)
{
	if (!gNestData || gIsStartNest) return -1;

	if (flag & 0x01) gNestData->mNestFlag |= LAYOUT_OPT_ROTATE90;
	else gNestData->mNestFlag &= ~LAYOUT_OPT_ROTATE90;

	if (flag & 0x02) gNestData->mNestFlag |= LAYOUT_OPT_ROTATE180;
	else gNestData->mNestFlag &= ~LAYOUT_OPT_ROTATE180;

	if (flag & 0x04) gNestData->mNestFlag |= LAYOUT_OPT_ROTATE270;
	else gNestData->mNestFlag &= ~LAYOUT_OPT_ROTATE270;

	return 0;
}

int PreProcess()
{
	if (!gNestData || gIsStartNest) return -1;

	// �������������
	gNestData->mPartIndexHash.clear();
	int ipart = 0;
	for (int i = 0; i < gNestData->mParts.count(); i++)
	{
		const GPart &part = gNestData->mParts[i];
		for (int j = 0; j < gNestData->mPartNums[i]; j++)
		{
			gNestData->mPartIndexHash[QPair<int, int>(part.mPartID, j)] = ipart++;
		}
	}

	GAutoLayout &autoLayout = gNestData->mAutoLayout;
	autoLayout.setLayoutFlag(gNestData->mNestFlag);
	autoLayout.setLayoutParam(gNestData->mPagePlates, gNestData->mPalteNums, gNestData->mNestGrid,
		gNestData->mParts, gNestData->mPartNums);
	autoLayout.preprocess();

	return 0;
}

int StartNest(int *progress)
{
	if (!gNestData || gIsStartNest) return -1;

	gIsStartNest = true;

	GAutoLayout &autoLayout = gNestData->mAutoLayout;

	//GPartSort partSort;
	GClusterSort partSort;
	partSort.setLayoutParam(autoLayout.getLayoutFlag(), &gNestData->mParts, &autoLayout.getPartTemplates(), 
							&gNestData->mPagePlates, &gNestData->mPalteNums, &gNestData->mPartNums, gNestData->mNestGrid);

	QVector<int> orders = partSort.sort();

	QVector<quint32> rotFlags;
	double nestRate;
	int lastForwardLineLen;
	autoLayout.layout(orders, rotFlags, gNestData->mNestParts, gNestData->mUsedPlateNums, 
					  gNestData->mPlateIndices, nestRate, lastForwardLineLen, progress);
	printf("\n�Ű��ʣ�%f\n", nestRate);
	autoLayout.calcPartsCenter(gNestData->mNestParts);

	gIsStartNest = false;

	return 0;
}

int StartOptimizeNest(int *progress, int maxIter)
{
	if (!gNestData || gIsStartNest) return -1;

	gIsStartNest = true;

	GAutoLayout &autoLayout = gNestData->mAutoLayout;

	// GPartSort partSort;
	GClusterSort partSort;
	partSort.setLayoutParam(autoLayout.getLayoutFlag(), &gNestData->mParts, &autoLayout.getPartTemplates(),
		&gNestData->mPagePlates, &gNestData->mPalteNums, &gNestData->mPartNums, gNestData->mNestGrid);
	QVector<int> orders = partSort.sort();

	QVector<QList<QVector<int> > >  clusterGroupsResult = partSort.getClusterGroupDatas();

	// GLayoutOptimizer *pOptimizer = new GSAOptimizer();
	GLayoutOptimizer *pOptimizer = new GFireFlyOptimizer();
	gNestData->mpOptimizer = pOptimizer;

	pOptimizer->setClusterGroup(clusterGroupsResult);
	pOptimizer->setClusterGGroups(partSort.getClusterGGroups());	//������ʾ�����������
	pOptimizer->setClusterSGroups(partSort.getClusterSGroups());	//������ʾ����С������
	pOptimizer->setLayoutParam(gNestData->mNestFlag, &gNestData->mPagePlates, &gNestData->mPalteNums, &gNestData->mParts,
		&gNestData->mPartNums, &autoLayout.getPartTemplates(), gNestData->mNestGrid);

	double  layoutRat = 0.0;
	double firstPageHeight = pOptimizer->optimize(maxIter, 99.0, gNestData->mNestParts, gNestData->mUsedPlateNums, gNestData->mPlateIndices, layoutRat, progress);
	printf("��ӡ���յ��Ű���%f,��һҳ�����߶ȣ�%lf\n", layoutRat, firstPageHeight);
	delete pOptimizer;
	gNestData->mpOptimizer = NULL;
	pOptimizer = NULL;
	autoLayout.calcPartsCenter(gNestData->mNestParts);

	gIsStartNest = false;
	return 0;
}

int StartOptimizeNestPage(int iPage, int *progress, int maxIter)
{
	if (!gNestData || gIsStartNest) return -1;
	if (iPage < 0 || iPage >= gNestData->mPlateIndices.count()) return -1;

	gIsStartNest = true;
	// ���ҳ�ָ��ҳĸ��
	//int pageIndex = iPage;
	//QVector<GPart> pages;
	//QVector<int> pageNums;
	//pages << gNestData->mPagePlates[gNestData->mPlateIndices[pageIndex]];
	//pageNums << 1;

	//// �ҳ�ָ��ҳ���
	//QVector<GPart *> lastPartPtrs;
	//QVector<GPart> lastParts;
	//QVector<int> partNums;
	//for (int i = 0; i < gNestData->mNestParts.count(); i++)
	//{
	//	GPart *pPart = &(gNestData->mNestParts[i]);
	//	if (pPart->mPageIndex == pageIndex)
	//	{
	//		pPart = &(gNestData->mParts[i]);
	//		lastPartPtrs << pPart;
	//		lastParts << *pPart;
	//		partNums << 1;
	//	}
	//}

	//// Ԥ����
	//GAutoLayout autoLayout;
	//autoLayout.setLayoutFlag(gNestData->mNestFlag);
	//autoLayout.setLayoutParam(pages, pageNums, gNestData->mNestGrid, lastParts, partNums);
	//autoLayout.preprocess();

	//GClusterSort partSort;
	//partSort.setLayoutParam(autoLayout.getLayoutFlag(), &lastParts, &autoLayout.getPartTemplates(),
	//	&pages, &pageNums, &partNums, gNestData->mNestGrid);
	//partSort.sort();

	//QVector<QList<QVector<int> > >  clusterGroupsResult = partSort.getClusterGroupDatas();
	////GLayoutOptimizer *pOptimizer = new GSAOptimizer();
	//GLayoutOptimizer *pOptimizer = new GFireFlyOptimizer();
	////GLayoutOptimizer *pOptimizer = new UgroupFireFly();

	//pOptimizer->setClusterGroup(clusterGroupsResult);
	//pOptimizer->setLayoutParam(gNestData->mNestFlag, &pages, &pageNums, &lastParts,
	//	&partNums, &autoLayout.getPartTemplates(), gNestData->mNestGrid);

	//double  layoutRate = 0.0;
	//QVector<GPart> lastNestParts;
	//QVector<int> usedPlateNums;
	//QVector<int> plateIndices;
	//pOptimizer->optimize(maxIter, 99.0, lastNestParts, usedPlateNums, plateIndices, layoutRate, progress);
	//printf("���һҳ���Ű���%f\n", layoutRate);
	//delete pOptimizer;

	//// �����һҳ���������д��ԭʼ�����
	//autoLayout.calcPartsCenter(lastNestParts);
	//for (int i = 0; i < lastNestParts.count(); i++)
	//{
	//	*lastPartPtrs[i] = lastNestParts[i];
	//	lastPartPtrs[i]->mPageIndex = pageIndex;
	//}

	gIsStartNest = false;
	return 0;
}

void ForceEndNest()
{
	if (!gNestData || !gIsStartNest) return;

	if (gNestData->mpOptimizer)
	{
		gNestData->mpOptimizer->abort();
	}
	else
	{
		gNestData->mAutoLayout.abort();
	}

	//gNestData->mAutoLayout.clear();
}

bool NestFinished()
{
	return !gIsStartNest;
}

int GetNestPlateNums(int *nestPlateNums)
{
	if (!gNestData || gIsStartNest) return -1;

	for (int i = 0; i < gNestData->mUsedPlateNums.count(); i++)
	{
		nestPlateNums[i] = gNestData->mUsedPlateNums[i];
	}
	return 0;
}

int GetNestPlateTotalNum()
{
	if (!gNestData || gIsStartNest) return -1;

	return gNestData->mPlateIndices.count();
}

int GetNestPlateIndices(int *nestPlateIndices)
{
	if (!gNestData || gIsStartNest) return -1;

	for (int i = 0; i < gNestData->mPlateIndices.count(); i++)
	{
		nestPlateIndices[i] = gNestData->mPlateIndices[i];
	}
	return 0;
}

int GetPartNestedData(int partID, int nIndex, int *plateIndex, double *tsMat, int *sortIndex, int *clusterMaxIndex, int *clustersmallIndex)
{
	if (!gNestData || gIsStartNest) return -1;

	int partIndex = gNestData->mPartIndexHash.value(QPair<int, int>(partID, nIndex), -1);
	if (partIndex < 0 || partIndex >= gNestData->mNestParts.count()) return -1;

	GPart &part = gNestData->mNestParts[partIndex];
	*plateIndex = part.mPageIndex;
	*clusterMaxIndex = gNestData->mNestParts[partIndex].mGroupIndex;
	*clustersmallIndex = gNestData->mNestParts[partIndex].mSubGroupIndex;

	QMatrix &mat = part.mTransMat;
	tsMat[0] = mat.m11();	tsMat[1] = mat.m21();	tsMat[2] = mat.dx();
	tsMat[3] = mat.m12();	tsMat[4] = mat.m22();	tsMat[5] = mat.dy();

	if (sortIndex)
	{
		*sortIndex = part.mSortIndex;
	}

	return 0;
}

int GetPartCenter(int partID, double *px, double *py)
{
	if (!gNestData || gIsStartNest) return -1;

	int partIndex = gNestData->mPartIndexHash.value(QPair<int, int>(partID, 0), -1);
	if (partIndex < 0 || partIndex >= gNestData->mNestParts.count()) return -1;

	GPart &part = gNestData->mNestParts[partIndex];
	*px = part.mCenter.x();
	*py = part.mCenter.y();

	return 0;
}