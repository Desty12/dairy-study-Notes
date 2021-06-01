#pragma once
#ifndef UGROYP_FIRE_FLY
#define UGROYP_FIRE_FLY

#include "gautolayout.h"
#include "glayoutoptimizer.h"

class UgroupFireFly : public GLayoutOptimizer
{
public:
	struct UgroupFireWorm
	{
		QVector<int>						sequenceGroups;		//  ����������
		double								layoutRating;		//	�Ű���

		UgroupFireWorm();
		~UgroupFireWorm();
	};

	UgroupFireFly();
	~UgroupFireFly();



	// ��Ҫ���ڲ���
	const QVector<int>	& getLayoutSequence() const;

	// ����ִ��ө����㷨
	double optimize(int maxIter,					// ����������
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
	double calDistance_i_j(const QVector<int>  &seQuenceI, const QVector<int> &sequenceJ);

	// ����ө���i��ө���j֮���������
	double calAttraction_i_j(double rI_J);

	// �����������и���֮��Ĳ����н�
	// sequenceΪ����������У�orignalSequenceΪδ����֮ǰ���������ڶԱ�
	//void dealInfeasibleSequence(QVector<QList<QVector<int> > > &sequence, const QVector<QList<QVector<int> > > &orignalSequence);
	void dealInfeasibleSequence(QVector<int> &sequence, const QVector<int> &orignalSequence);

	// ����ө���i��ө���j�ƶ������У�λ�ø���
	void updateLocationItoJ(QVector<int> &sequenceI, const QVector<int> &sequenceJ, double attraction);

	// ��ʼ������ө�����Ⱥ����
	void	initWorstFireWormSequence(QVector<int> &sequences, const QVector<int> &bestSeq);

private:
	// ��Ҫ���ڲ���
	QVector<int>			   mLayoutOrders;			// ���⴫����������Ⱥ�˳��

	QHash<QVector<int>, SaveLayoutDatas>				mSaveLayoutDatas;
	QVector< UgroupFireWorm >	   mFireFlySerises;			// ���ө�����Ⱥ��ʼ��֮�������						 		 
	int                        mFireFlyNumbers;			//ө�����Ⱥ����
	double                     r;				   	    // ����ϵ��
	double                     B0;					    // ���������
	int                        m;						// ������ֻө���֮��ĺ����������ֵ
	float                      p;					    // ���벻���н⴦��ϵ��p
	int                        MaxIterm;				// ө�������������
	double                     I0;						// ���ӫ������
};
#endif