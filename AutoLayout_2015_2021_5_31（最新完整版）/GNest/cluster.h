#pragma once
#include<QVector>

class Cluster
{
protected:

	// 对面积进行归一化
	// clusterDatas存放原始聚类数据
	// normalizationResult归一化之后的数据
	void normalize(const QVector<double>	&clusterDatas, QVector<double>	&normalizationResult);

	// 获取k个聚类中心的初始值
	// k表示k个聚类中心
	// dataClasses表示在normalizationResult归一化基础上去重之后的数据
	// orignalCenters存放k个聚类中心
	bool initCenters(int k, const QVector<double>	&dataClasses, QVector<double>	&orignalCenters);

	// 对面积进行kmeans聚类
	// k表示聚类个数
	// double表示迭代次数
	// normalizationResult表示归一化之后的数据
	// dataClasses 表示去重之后的面积数据
	// clusterResult 聚类结果的零件序号
	// clusterCenters 聚类中心
	bool kmeans(int nk, int niter, const QVector<double> &dataClasses, const QVector<double> &normalAreas, 
				 QList< QVector<int> > &clusterResult, QVector<double> &clusterCenters);

	// 对聚类结果进行评估
	// normalAreas面积归一化后的数据
	// orignalCenters聚类中心
	// clusterResults聚类结果
	double evaluateResult(const QVector<double>	&normalAreas, const QVector<double>	&orignalCenters, QList< QVector<int> >	 &clusterResults, int K);

private:
	QVector<double>						mClusterDatas;				// 存放零件聚类数据

public:
	Cluster();
	~Cluster();

	// 数据聚类
	void dataCluster(QList<QVector<int> >	&clusterGroups);

	// 将参数传入函数设置为模板函数
	void setClusterDatas(const QVector<double> &datas);
};

