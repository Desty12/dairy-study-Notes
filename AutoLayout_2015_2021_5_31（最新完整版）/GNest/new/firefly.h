#ifndef FIRT_FLY_H
#define FIRT_FLY_H

#include "gautolayout.h"
#include "glayoutoptimizer.h"

class FireFly : public GLayoutOptimizer 
{
public:
	struct FireWorm
	{
		QVector<QList<QVector<int> > >		sequenceGroups;		//  ����������
		double								layoutRating;		//	�Ű���

		FireWorm();
		~FireWorm();
	};

	FireFly();
	~FireFly();

	// ��Ҫ���ڲ���
	const QVector<int>	& getLayoutSequence() const;

	// ����ִ��ө����㷨
	int optimize(int maxIter,					// ����������
		double maxLayoutRate,					// ����Ű���
		QVector< GPart > &nestParts,			// �������
		QVector<int> &usedPageNums,				// ���ʹ������
		QVector<int> &plateIndices,				// �������
		double &layoutRate,						// �Ű���
		int *pProgress);


protected:

	// ��ʼ��ө�����Ⱥ
	void InitFireFlies();

	// ����ө���i��ө���j֮��ĺ�������
	int calDistance_i_j(const QVector<QList<QVector<int> > >  &seQuenceI, const QVector<QList<QVector<int> > > &sequenceJ, int &allDistances);

	// ����ө���i��ө���j֮���������
	double calAttraction_i_j(int rI_J, int allDistance);

	// �����������и���֮��Ĳ����н�
	// sequenceΪ����������У�orignalSequenceΪδ����֮ǰ���������ڶԱ�
	//void dealInfeasibleSequence(QVector<QList<QVector<int> > > &sequence, const QVector<QList<QVector<int> > > &orignalSequence);
	void dealInfeasibleSequence(QList<QVector<int> > &sequence, const QList<QVector<int> > &orignalSequence);

	// ����ө���i��ө���j�ƶ������У�λ�ø���
	void updateLocationItoJ(QVector<QList<QVector<int> > > &sequenceI, const QVector<QList<QVector<int> > > &sequenceJ, double attraction);

	// ����ө���i�����ө���j�����ӫ������
	// double  calMutualLight_i_j(int distance, double MaxLight);

	// ��ʼ������ө�����Ⱥ����
	void	initWorstFireWormSequence(QVector<QList<QVector<int> > > &sequences, const QVector<QList<QVector<int> > > &bestSeq);

private:
	// ��Ҫ���ڲ���
	QVector<int>			   mLayoutOrders;			// ���⴫����������Ⱥ�˳��
	//long long				   mDifSeqNums;				// ��¼������ܲ����Ĳ�ͬ

	QVector< FireWorm >		   mFireFlySerises;			// ���ө�����Ⱥ��ʼ��֮�������						 		 
	int                        mFireFlyNumbers;			//ө�����Ⱥ����
	double                     r;				   	    // ����ϵ��
	double                     B0;					    // ���������
	int                        m;						// ������ֻө���֮��ĺ����������ֵ
	float                      p;					    // ���벻���н⴦��ϵ��p
	int                        MaxIterm;				// ө�������������
	double                     I0;						// ���ӫ������
};
#endif