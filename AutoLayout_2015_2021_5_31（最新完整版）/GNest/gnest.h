#ifndef GNEST_H
#define GNEST_H

#include "gnest_global.h"


#ifdef __cplusplus
extern "C" {
#endif

	/// <summary>
	/// 初始化函数，清除所有的板材和零件数据
	/// 运行其它函数前应调用该函数
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int InitNest();


	/// <summary>
	/// 销毁函数，清除所有的板材和零件数据
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int DestroyNest();


	/// <summary>
	///  设置板材数据，调用这个函数会自动清除上个板材的数据
	/// </summary>
	/// <param name="nCount">板材外轮廓数据点数</param>
	/// <param name="data">板材外轮廓数据，每个点按(X,Y)占2个数据</param>
	/// <returns>返回0，设置成功，-1，设置失败</returns>
	GNEST_EXPORT int AddNewPlates(int plateID, int NestNum, int nCount, double *data);


	/// <summary>
	/// 给当前板材添加内孔，可以多次调用，多次添加内孔数据
	/// </summary>
	/// <param name="nCount">内孔轮廓的点的个数</param>
	/// <param name="data">内孔的轮廓数据，每个点按(X,Y)占2个数据</param>
	/// <returns>返回0，添加成功，-1，添加失败</returns>
	GNEST_EXPORT int AddInnerHoleToPlate(int plateID, int nCount, double *data);

	/// <summary>
	/// 清除所有母版
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int ClearPlates();

	/// <summary>
	/// 清除所有的零件数据
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int ClearParts();


	/// <summary>
	/// 添加新零件
	/// </summary>
	/// <param name="partID">零件ID</param>
	/// <param name="NestNum">指定改零件可以套料多少个，小于等于0时,不限制个数</param>
	/// <param name="nCount">零件外轮廓数据点数</param>
	/// <param name="data">零件外轮廓数据，每个点按(X,Y)占2个数据</param>
	/// <returns>返回0，添加成功，-1，添加失败</returns>
	GNEST_EXPORT int AddNewSpareParts(int partID, int NestNum, int nCount, double *data);


	/// <summary>
	/// 根据零件ID，添加内孔的数据，每个零件可以允许多次调用该函数，添加多个内孔
	/// </summary>
	/// <param name="partID">零件ID</param>
	/// <param name="nCount">内孔轮廓的点的个数</param>
	/// <param name="data">内孔的轮廓数据，每个点按(X,Y)占2个数据</param>
	/// <returns>返回0，添加成功，-1，添加失败</returns>
	GNEST_EXPORT int AddInnerHoleByPartNo(int partID, int nCount, double *data);

	/// <summary>
	/// 根据零件ID，设置零件排样允许的旋转角度
	/// </summary>
	/// <param name="partID">零件ID</param>
	/// <param name="flag">旋转掩码 0000: 0°: 0001:90; 0010:180; 0100:270;</param>
	/// <returns>返回0，设置成功，-1，设置失败</returns>
	GNEST_EXPORT int SetRotAngleByPartNo(int partID, char flag);


	/// <summary>
	/// 根据零件ID表设置零件优先级
	/// </summary>
	/// <returns>返回0，设置成功，-1，设置失败</returns>
	GNEST_EXPORT int SetPartPriority(int partID, int priority);


	/// <summary>
	/// 设置套料方向
	/// </summary>
	/// <param name="dat">0：自动 ，1：X; 2：Y</param>
	GNEST_EXPORT int SetNestPriorityDir(int dat);

	/// <summary>
	/// 设置套料间距
	/// </summary>
	/// <param name="spaceGrid">网格间距</param>
	GNEST_EXPORT int SetNestSpacing(double spaceGrid);


	/// <summary>
	/// 是否零件允许主轴旋转
	/// </summary>
	/// <param name="IsEnbale">0,不允许，1：允许，默认允许</param>
	GNEST_EXPORT int SetPartEnablePolarRotation(bool IsEnbale);


	/// <summary>
	/// 设置排样旋转角度 0000: 0°: 0001:90; 0010:180; 0100:270;可多选组合（0-7）
	/// </summary>
	/// <param name="flag">旋转角度选择</param>
	GNEST_EXPORT int SetSparePartsAngle(char flag);

	/// <summary>
	/// 预处理, 在启动套料或优化套料前, 必须执行预处理
	/// </summary>
	GNEST_EXPORT int PreProcess();


	//这里是使用指针获取还是用回调函数，需要后面实际使用的时候再去确定那种好用
	/// <summary>
	/// 开始套料
	/// </summary>
	/// <param name="progress">套料进度,需要传入内存地址</param>
	GNEST_EXPORT int StartNest(int *progress);

	/// <summary>
	/// 开始优化套料
	/// </summary>
	/// <param name="progress">套料进度,需要传入内存地址</param>
	GNEST_EXPORT int StartOptimizeNest(int *progress, int maxIter);


	/// <summary>
	/// 优化套料最后一页
	/// </summary>
	/// <param name="progress">套料进度,需要传入内存地址</param>
	GNEST_EXPORT int StartOptimizeNestPage(int iPage, int *progress, int maxIter);

	/// <summary>
	/// 强制结束套料
	/// </summary>
	GNEST_EXPORT void ForceEndNest();


	/// <summary>
	/// 检查套料是否已经完成
	/// </summary>
	GNEST_EXPORT bool NestFinished();


	/// <summary>
	/// 返回套料所需的板材数量
	/// </summary>
	GNEST_EXPORT int GetNestPlateNums(int *nestPlateNums);


	/// <summary>
	/// 返回套料所需的总板材数量
	/// </summary>
	GNEST_EXPORT int GetNestPlateTotalNum();

	/// <summary>
	/// 返回套料母版序列
	/// </summary>
	GNEST_EXPORT int GetNestPlateIndices(int *nestPlateIndices);

	/// <summary>
	/// 根据零件ID和该零件完成的序号获取该零件套料的数据
	/// </summary>
	/// <param name="partID">零件ID</param>
	/// <param name="nIndex">零件的序号</param>
	/// <param name="plateIndex">返回该零件在板材的序号</param>
	/// <param name="matrixData">移动旋转矩阵数据，长度为6的数组</param>
	GNEST_EXPORT int GetPartNestedData(int partID, int nIndex, int *plateIndex, double *tsMat, int *sortIndex=0,int *clusterMaxIndex=0, int *clusterIndex=0);


	/// <summary>
	/// 根据零件ID获取零件中心坐标(相对原始零件坐标)
	/// </summary>
	/// <param name="partID">零件ID</param>
	/// <param name="px">x坐标指针</param>
	/// <param name="py">y坐标指针</param>
	GNEST_EXPORT int GetPartCenter(int partID, double *px, double *py);

#ifdef __cplusplus
}
#endif

#endif
