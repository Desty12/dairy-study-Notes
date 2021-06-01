#ifndef GMATH_H
#define GMATH_H

#include <math.h>

#define PI 3.1415926535
#define RADIAN_TO_ANGLE 57.29577958
#define ANGLE_TO_RADIAN 0.01745329
#define GMATH_ZERO		1.0E-8
#define IS_ZERO(x)		(fabs(x) < GMATH_ZERO)

// ����������СԪ�ص��±�
template <class TData> int gGetMin(const TData *data, int begin, int end)
{
	int index = begin;
	TData min = data[index];
	for(int i=begin+1; i<=end; i++)
	{
		if(data[i] < min)
		{
			min = data[i];
			index = i;
		}
	}
	return index;
}


// �����������Ԫ�ص��±�
template <class TData> int gGetMax(const TData *data, int begin, int end)
{
	int index = begin;
	TData max = data[index];
	for(int i=begin+1; i<=end; i++)
	{
		if(data[i] > max)
		{
			max = data[i];
			index = i;
		}
	}
	return index;
}

// �޶���ֵ��Χ
template <class T>
T gLimitRange(T v, T min, T max)
{
	if (v < min) v = min;
	else if (v > max) v = max;
	return v;
}

// ��������
int gRound( double x );

// ȡ��
int gTrunc( double x );

// ƽ��
double gSqr( double x );

// 2D����
double gDistance2D(double x0, double y0, double x1, double y1); 

// 2D��ת
void gRotate(double sinAF, double cosAF, double sx, double sy, double &dx, double &dy);

// ����sin��cosֵȷ���Ƕ�
double gGetAngle(double sinAF, double cosAF);

// ��3������ʽ��ֵ
double gDeterm3(double a1, double a2, double a3, double b1, double b2, double b3,
				double c1, double c2, double c3);

// ������С�������
bool gFittingLine(double *xArray, double *yArray, int firstIndex, int lastIndex,
				  double &a, double &b);

// �������
bool gInverseMatrix(double *m, int n);

// ѡ��Ԫ���Ƿֽⷨ
void gSelSlove(double **matrix, double *result, int dim);	

// ׷�Ϸ�������ԽǾ���
void gTridagSlove( double *aArray, double *bArray,
					double *cArray, double *dArray,
					int count, double *rArray );

// ׷�Ϸ������չ���ԽǷ��� (������a1��cn����, �ʺϱպ�����ʸ����) 
void gTridagExSlove(double aArray[], double bArray[],
				   double cArray[], double dArray[],
				   int count, double rArray[]); 

// ��һԪ���η���
int gSolveTwoeTimeEqu(double a[3], double r[2]);

// ��һԪ���η���
int gSolveThreeTimeEqu(double a[4], double r[3]);

// ţ�ٵ�������һԪ���η���
double gNewtonSolveThreeTimeEqu(double a[4], double r, 
							  double errLimit = 0.01, int itCount = 32);

// С��ת��Ϊ��, ��, ���ʽ
void gDecimal2DMS(double dec, int &degree, int &minute, double &second);

// ��, ��, ��ת��ΪС��
void gDMS2Decimal(int degree, int minute, double second, double &dec);


#endif
