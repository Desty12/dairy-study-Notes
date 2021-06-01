#ifndef GLAYOUT_OPTIMIZER_H
#define GLAYOUT_OPTIMIZER_H

#include <QVector>
#include <QHash>
#include <QVariant>
#include "gautolayout.h"
#include "omp.h"

#define OMP_NUM_THREADS omp_get_num_procs()

class GLayoutOptimizer 
{
protected:

	struct SaveLayoutDatas
	{
		QVector<GPart>	nestParts;
		QVector<int>	usedPageNums;
		QVector<int>	plateIndices;
		double          nestRating;
		int				firstPageNestHeight;

		SaveLayoutDatas()
		{
			nestRating = 0.0;
			firstPageNestHeight = 0;
		}
	};

	const QVector<GPart> *								mpPagePlates;		// ĸ���ָ��
	const QVector<int> *								mpPageNums;			// ĸ��������ָ��
	const QVector<GPart> *								mpParts;			// �����ָ��
	const QVector<int> *								mpPartNums;			// ���������ָ��
	const QVector<GAutoLayout::PartTemplate *> *		mpPartTemplates;	// ���ģ���ָ��
	double												mGridDelta;			// ����������
	QVector<QList<QVector<int> > >						mClusterGroups;		// ���������ŷ���
	quint32												mLayoutFlag;		// ������־
	QHash<QString, QVariant>							mOptParams;			// �Ż�����
	bool												mIsAbort;			// �Ƿ��ж��Ż�����
	QVector<int>										mSortSequences;		// ����ֻ�����������������������

	QHash<QVector<int>, SaveLayoutDatas>				mRecordLayoutSeQ;	// ��¼ȫ���Ű����к��Ű���Ϣ
	QList<QVector<quint32> >							mFeasibleRots;		// ��¼��������Ŀ���ת�Ƕ�
	QHash<QVector<int>, int >							mMarkSeqHashs;		// ��¼��ʼ��Ⱥ��������У�����������ͬ�����³�ʼ����
	QVector<QList<QVector<int> > >						mbestSequences;	    // ����ȫ�����������������

	//////  �Ż���������Ҫ�õ��Ĺ��ߺ��� ///////////////

	// ѡ����Ҫ�������������
	// isHashFindΪtrue��ʾ���ɵ������Ҫ��hash���в��������Ƿ��ظ���Ϊfalse��ʾ����Ҫִ�в���
	void changeGroupsDatas(QVector<QList<QVector<int> > > &aimSequence, int changeNum, quint32 mark, bool isHashFind);

	// ѡ����Ҫ���������С����е���
	void changSmallGroupsData(QVector<int> &tmpList, quint32 mark);

	// �ı�����ڵ�С��ǰ��˳��
	void changeSmallGroupsOrders(QVector<QList<QVector<int> > > &tempSeQ);

	// �������С���ڵ��������
	void randomChangeData(QVector<int> &arr1);

	// ���ѡ�������������ݽ��иı�
	void changeTwoData(QVector<int> &arr);

	// ���������ݽ���΢��
	void fewChangeData(QVector<int> &arr);

	// �������
	void insertVariation(QVector<int>	&arr);

	// ��ת����
	void reverseVariation(QVector<int> &arr);

	// ��������
	void  swapeVariation(QVector<int> &arr);

	// ��Ѱ���㷨��ִ��ө����㷨
	double  layout(QVector<QList<QVector<int> > >  layoutSeq, QVector< GPart > &tmpNestParts, QVector<int> &tmpUsedPageNums, QVector<int> &tmpPlateIndices, int &height);

	// ��������������ת��Ϊһ���������
	void  groupsSeq2Seq(const QVector<QList<QVector<int> > > &sequence, QVector<int> &orders);

	// ���ѡ��һ��С����������������
	void  smallGroupSeqAdjust(QVector<QList<QVector<int> > >  layoutSeq);

public:
	GLayoutOptimizer();
	virtual ~GLayoutOptimizer();

	// ������������
	void setLayoutParam(quint32 layoutFlag,
		const QVector<GPart> *	pPagePlates,
		const QVector<int> *	pPageNums,
		const QVector<GPart> *	pParts,
		const QVector<int> *	pPartNums,
		const QVector< GAutoLayout::PartTemplate * > *pPartTemplates,
		double					gridDelta);

	// ���þ������
	void setClusterGroup(const QVector< QList< QVector<int> > > &	clusterGroups);

	// ����ֻ�����������֮���������У�δ���飩
	void setSortSequences(const QVector<int>	&originSeuqnce);

	// �����Ż�����
	void setOptimizeParam(const QHash<QString, QVariant> &params);

	const QVector<QList<QVector<int> > > & getBestGroupSequences() const;

	// ִ���Ż��㷨
	virtual int optimize(int maxIter,			// ����������
		double maxLayoutRate,					// ����Ű���
		QVector< GPart > &nestParts,			// �������
		QVector<int> &usedPageNums,				// ���ʹ������
		QVector<int> &plateIndices,				// �������
		double &layoutRate,						// �Ű���
		int *pProgress);						// �Ű����

	void abort();

	// ����������������У�չ��Ϊ�����ȼ��ִ��������������У�������������ֻ����������ȼ�������Ϣ��
	void severalGroups2prioGroups(const QVector<QList<QVector<int> > > &groupSeq, QVector<QList<QVector<int> > > &resultSeq);
};
#endif