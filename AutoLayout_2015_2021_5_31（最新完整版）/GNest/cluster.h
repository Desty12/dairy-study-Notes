#pragma once
#include<QVector>

class Cluster
{
protected:

	// ��������й�һ��
	// clusterDatas���ԭʼ��������
	// normalizationResult��һ��֮�������
	void normalize(const QVector<double>	&clusterDatas, QVector<double>	&normalizationResult);

	// ��ȡk���������ĵĳ�ʼֵ
	// k��ʾk����������
	// dataClasses��ʾ��normalizationResult��һ��������ȥ��֮�������
	// orignalCenters���k����������
	bool initCenters(int k, const QVector<double>	&dataClasses, QVector<double>	&orignalCenters);

	// ���������kmeans����
	// k��ʾ�������
	// double��ʾ��������
	// normalizationResult��ʾ��һ��֮�������
	// dataClasses ��ʾȥ��֮����������
	// clusterResult ��������������
	// clusterCenters ��������
	bool kmeans(int nk, int niter, const QVector<double> &dataClasses, const QVector<double> &normalAreas, 
				 QList< QVector<int> > &clusterResult, QVector<double> &clusterCenters);

	// �Ծ�������������
	// normalAreas�����һ���������
	// orignalCenters��������
	// clusterResults������
	double evaluateResult(const QVector<double>	&normalAreas, const QVector<double>	&orignalCenters, QList< QVector<int> >	 &clusterResults, int K);

private:
	QVector<double>						mClusterDatas;				// ��������������

public:
	Cluster();
	~Cluster();

	// ���ݾ���
	void dataCluster(QList<QVector<int> >	&clusterGroups);

	// ���������뺯������Ϊģ�庯��
	void setClusterDatas(const QVector<double> &datas);
};

