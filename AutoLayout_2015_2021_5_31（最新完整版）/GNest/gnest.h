#ifndef GNEST_H
#define GNEST_H

#include "gnest_global.h"


#ifdef __cplusplus
extern "C" {
#endif

	/// <summary>
	/// ��ʼ��������������еİ�ĺ��������
	/// ������������ǰӦ���øú���
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int InitNest();


	/// <summary>
	/// ���ٺ�����������еİ�ĺ��������
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int DestroyNest();


	/// <summary>
	///  ���ð�����ݣ���������������Զ�����ϸ���ĵ�����
	/// </summary>
	/// <param name="nCount">������������ݵ���</param>
	/// <param name="data">������������ݣ�ÿ���㰴(X,Y)ռ2������</param>
	/// <returns>����0�����óɹ���-1������ʧ��</returns>
	GNEST_EXPORT int AddNewPlates(int plateID, int NestNum, int nCount, double *data);


	/// <summary>
	/// ����ǰ�������ڿף����Զ�ε��ã��������ڿ�����
	/// </summary>
	/// <param name="nCount">�ڿ������ĵ�ĸ���</param>
	/// <param name="data">�ڿ׵��������ݣ�ÿ���㰴(X,Y)ռ2������</param>
	/// <returns>����0����ӳɹ���-1�����ʧ��</returns>
	GNEST_EXPORT int AddInnerHoleToPlate(int plateID, int nCount, double *data);

	/// <summary>
	/// �������ĸ��
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int ClearPlates();

	/// <summary>
	/// ������е��������
	/// </summary>
	/// <returns></returns>
	GNEST_EXPORT int ClearParts();


	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="partID">���ID</param>
	/// <param name="NestNum">ָ��������������϶��ٸ���С�ڵ���0ʱ,�����Ƹ���</param>
	/// <param name="nCount">������������ݵ���</param>
	/// <param name="data">������������ݣ�ÿ���㰴(X,Y)ռ2������</param>
	/// <returns>����0����ӳɹ���-1�����ʧ��</returns>
	GNEST_EXPORT int AddNewSpareParts(int partID, int NestNum, int nCount, double *data);


	/// <summary>
	/// �������ID������ڿ׵����ݣ�ÿ��������������ε��øú�������Ӷ���ڿ�
	/// </summary>
	/// <param name="partID">���ID</param>
	/// <param name="nCount">�ڿ������ĵ�ĸ���</param>
	/// <param name="data">�ڿ׵��������ݣ�ÿ���㰴(X,Y)ռ2������</param>
	/// <returns>����0����ӳɹ���-1�����ʧ��</returns>
	GNEST_EXPORT int AddInnerHoleByPartNo(int partID, int nCount, double *data);

	/// <summary>
	/// �������ID��������������������ת�Ƕ�
	/// </summary>
	/// <param name="partID">���ID</param>
	/// <param name="flag">��ת���� 0000: 0��: 0001:90; 0010:180; 0100:270;</param>
	/// <returns>����0�����óɹ���-1������ʧ��</returns>
	GNEST_EXPORT int SetRotAngleByPartNo(int partID, char flag);


	/// <summary>
	/// �������ID������������ȼ�
	/// </summary>
	/// <returns>����0�����óɹ���-1������ʧ��</returns>
	GNEST_EXPORT int SetPartPriority(int partID, int priority);


	/// <summary>
	/// �������Ϸ���
	/// </summary>
	/// <param name="dat">0���Զ� ��1��X; 2��Y</param>
	GNEST_EXPORT int SetNestPriorityDir(int dat);

	/// <summary>
	/// �������ϼ��
	/// </summary>
	/// <param name="spaceGrid">������</param>
	GNEST_EXPORT int SetNestSpacing(double spaceGrid);


	/// <summary>
	/// �Ƿ��������������ת
	/// </summary>
	/// <param name="IsEnbale">0,������1������Ĭ������</param>
	GNEST_EXPORT int SetPartEnablePolarRotation(bool IsEnbale);


	/// <summary>
	/// ����������ת�Ƕ� 0000: 0��: 0001:90; 0010:180; 0100:270;�ɶ�ѡ��ϣ�0-7��
	/// </summary>
	/// <param name="flag">��ת�Ƕ�ѡ��</param>
	GNEST_EXPORT int SetSparePartsAngle(char flag);

	/// <summary>
	/// Ԥ����, ���������ϻ��Ż�����ǰ, ����ִ��Ԥ����
	/// </summary>
	GNEST_EXPORT int PreProcess();


	//������ʹ��ָ���ȡ�����ûص���������Ҫ����ʵ��ʹ�õ�ʱ����ȥȷ�����ֺ���
	/// <summary>
	/// ��ʼ����
	/// </summary>
	/// <param name="progress">���Ͻ���,��Ҫ�����ڴ��ַ</param>
	GNEST_EXPORT int StartNest(int *progress);

	/// <summary>
	/// ��ʼ�Ż�����
	/// </summary>
	/// <param name="progress">���Ͻ���,��Ҫ�����ڴ��ַ</param>
	GNEST_EXPORT int StartOptimizeNest(int *progress, int maxIter);


	/// <summary>
	/// �Ż��������һҳ
	/// </summary>
	/// <param name="progress">���Ͻ���,��Ҫ�����ڴ��ַ</param>
	GNEST_EXPORT int StartOptimizeNestPage(int iPage, int *progress, int maxIter);

	/// <summary>
	/// ǿ�ƽ�������
	/// </summary>
	GNEST_EXPORT void ForceEndNest();


	/// <summary>
	/// ��������Ƿ��Ѿ����
	/// </summary>
	GNEST_EXPORT bool NestFinished();


	/// <summary>
	/// ������������İ������
	/// </summary>
	GNEST_EXPORT int GetNestPlateNums(int *nestPlateNums);


	/// <summary>
	/// ��������������ܰ������
	/// </summary>
	GNEST_EXPORT int GetNestPlateTotalNum();

	/// <summary>
	/// ��������ĸ������
	/// </summary>
	GNEST_EXPORT int GetNestPlateIndices(int *nestPlateIndices);

	/// <summary>
	/// �������ID�͸������ɵ���Ż�ȡ��������ϵ�����
	/// </summary>
	/// <param name="partID">���ID</param>
	/// <param name="nIndex">��������</param>
	/// <param name="plateIndex">���ظ�����ڰ�ĵ����</param>
	/// <param name="matrixData">�ƶ���ת�������ݣ�����Ϊ6������</param>
	GNEST_EXPORT int GetPartNestedData(int partID, int nIndex, int *plateIndex, double *tsMat, int *sortIndex=0,int *clusterMaxIndex=0, int *clusterIndex=0);


	/// <summary>
	/// �������ID��ȡ�����������(���ԭʼ�������)
	/// </summary>
	/// <param name="partID">���ID</param>
	/// <param name="px">x����ָ��</param>
	/// <param name="py">y����ָ��</param>
	GNEST_EXPORT int GetPartCenter(int partID, double *px, double *py);

#ifdef __cplusplus
}
#endif

#endif
