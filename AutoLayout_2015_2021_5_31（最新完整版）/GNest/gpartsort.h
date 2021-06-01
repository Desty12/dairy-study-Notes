#pragma once

#include <QVector>
#include "gautolayout.h"

class GPartSort
{
protected:
	quint32											mLayoutFlag;
	const QVector< GPart > *						mpParts;
	const QVector< GAutoLayout::PartTemplate * > *	mpPartTemplates;
	const QVector<GPart> *						    mpPagePlates;
	const QVector<int>	*							mpageNums;
	const QVector< int > *						    mpPartNums;
	double											mpGridDelta;
	QVector<QList<QVector<int> > >					mClusterGroups;		// �������յľ��������Ϣ

	QVector<QPair<int, int> >						mClusterBGroups;	//����������ݣ���һ��ֵ��ʾ����±꣬�ڶ���ֵ��ʾ�������
	QVector<QPair<int, int> >						mClusterSGroups;	//����С�����ݣ���һ��ֵ��ʾ����±꣬�ڶ���ֵ��ʾ�������

public:
	GPartSort();
	virtual ~GPartSort();

	void setLayoutParam(quint32 layoutFlag, const QVector< GPart > *pParts,
						const QVector< GAutoLayout::PartTemplate * > *pPartTemplates,
						const QVector<GPart>	*pPagePlates,
						const QVector<int>      *pageNums,
						const QVector< int >	*pPartNums,
						double					gridDeltaX
						);
	
	virtual QVector<int> sort();

	const QVector<QList<QVector<int> > > &getClusterGroupDatas() const;

	const QVector<QPair<int, int> > &getClusterGGroups() const;	// ���������Ϣ���⿪��

	const QVector<QPair<int, int> > &getClusterSGroups() const;	// ����С����Ϣ���⿪��
};

