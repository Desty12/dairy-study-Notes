#pragma once

#include "gpartsort.h"

class GClusterSort : public GPartSort
{
public:
	virtual QVector<int> sort();

	// ���Ż�������, �������б����˷�����Ϣ
	void sort(QVector<QList<QVector<int> > > &orignalSequence);
};
