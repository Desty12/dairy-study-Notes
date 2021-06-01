#ifndef GMATH_H
#define GMATH_H

#include <math.h>

#define PI 3.1415926535
#define RADIAN_TO_ANGLE 57.29577958
#define ANGLE_TO_RADIAN 0.01745329
#define GMATH_ZERO		1.0E-8
#define IS_ZERO(x)		(fabs(x) < GMATH_ZERO)

// 求数组中最小元素的下标
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


// 求数组中最大元素的下标
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

// 限定数值范围
template <class T>
T gLimitRange(T v, T min, T max)
{
	if (v < min) v = min;
	else if (v > max) v = max;
	return v;
}

// 四舍五入
int gRound( double x );

// 取整
int gTrunc( double x );

// 平方
double gSqr( double x );

// 2D距离
double gDistance2D(double x0, double y0, double x1, double y1); 

// 2D旋转
void gRotate(double sinAF, double cosAF, double sx, double sy, double &dx, double &dy);

// 根据sin和cos值确定角度
double gGetAngle(double sinAF, double cosAF);

// 求3阶行列式的值
double gDeterm3(double a1, double a2, double a3, double b1, double b2, double b3,
				double c1, double c2, double c3);

// 线性最小二乘拟合
bool gFittingLine(double *xArray, double *yArray, int firstIndex, int lastIndex,
				  double &a, double &b);

// 求逆矩阵
bool gInverseMatrix(double *m, int n);

// 选主元三角分解法
void gSelSlove(double **matrix, double *result, int dim);	

// 追赶法求解三对角矩阵
void gTridagSlove( double *aArray, double *bArray,
					double *cArray, double *dArray,
					int count, double *rArray );

// 追赶法求解扩展三对角方程 (方程中a1与cn存在, 适合闭合三切矢方程) 
void gTridagExSlove(double aArray[], double bArray[],
				   double cArray[], double dArray[],
				   int count, double rArray[]); 

// 解一元二次方程
int gSolveTwoeTimeEqu(double a[3], double r[2]);

// 解一元三次方程
int gSolveThreeTimeEqu(double a[4], double r[3]);

// 牛顿迭代法解一元三次方程
double gNewtonSolveThreeTimeEqu(double a[4], double r, 
							  double errLimit = 0.01, int itCount = 32);

// 小数转换为度, 分, 秒格式
void gDecimal2DMS(double dec, int &degree, int &minute, double &second);

// 度, 分, 秒转换为小数
void gDMS2Decimal(int degree, int minute, double second, double &dec);


#endif
