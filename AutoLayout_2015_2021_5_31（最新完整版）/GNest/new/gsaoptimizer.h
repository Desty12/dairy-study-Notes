#ifndef SAMULATED_ANNEALING_H
#define SAMULATED_ANNEALING_H

#include "firefly.h"
#include "glayoutoptimizer.h"

class GSAOptimizer : public GLayoutOptimizer 
{
private:
	// ����ģ���˻���Ҫ�Ĳ���
	double			maxTemperature;				// �˻�����¶�
	double          minTemperature;				// �˻�����¶�
	int             saPerIters;					// ÿ���¶�һ��ʱ��Ҫ������������
	float		    mControlA;  				// �¶��˻�ϵ��						

												// ��Ҫ���ڲ���
	QVector<int>	layoutOrders;			// ���⴫����������Ⱥ�˳��

public:

	GSAOptimizer();
	~GSAOptimizer();

	// ִ��ģ���˻��㷨
	int optimize(int maxIter,			// ����������
		double maxLayoutRate,					// ����Ű���
		QVector< GPart > &nestParts,			// �������
		QVector<int> &usedPageNums,				// ���ʹ������
		QVector<int> &plateIndices,				// �������
		double &layoutRate,						// �Ű���
		int *pProgress);

	// ��Ҫ���ڲ���
	const QVector<int>	& getLayoutSequence() const;

};
#endif