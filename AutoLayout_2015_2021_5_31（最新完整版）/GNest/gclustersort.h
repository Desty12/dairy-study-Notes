#pragma once

#include "gpartsort.h"

class GClusterSort : public GPartSort
{
public:
	virtual QVector<int> sort();

	// 供优化器调用, 传出序列保留了分组信息
	void sort(QVector<QList<QVector<int> > > &orignalSequence);
};
