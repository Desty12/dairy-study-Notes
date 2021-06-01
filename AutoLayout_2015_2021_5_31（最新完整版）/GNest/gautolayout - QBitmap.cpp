#include "gautolayout.h"
#include "gutils.h"
#include "gmath.h"

#include <QImage>
#include <QPainter>

#include <opencv2/opencv.hpp>
#include <vector>
#include <time.h>

#include <QTime>
#include <QDateTime>
#include <QBitmap>

using namespace std;

GPart::GPart()
{
	mPartID = -1;
	mPageIndex = -1;
	mLayoutFlag = -1;
	mSortIndex = 0;
	mPriority = 0;
	mClusterSmallIndex = 0;
	mClusterMaxIndex = 0;
}

void GPart::cloneGeometry(const GPart &other)
{
	(QPolygonF &)*this = (QPolygonF &)other;
	mHoles = other.mHoles;
}

GAutoLayout::PartTemplate::PartTemplate()
{
	//mPageIndex = -1;
	mPartIndex = -1;
	mRatio = 0;
	mMaskArea = 0;
	mPolyArea = 0;
}

GAutoLayout::PartTemplate::~PartTemplate()
{
}

GAutoLayout::PageTemplate::PageTemplate()
{
	mPageMaskIndex = -1;
	mPageIndex = -1;
	mMask = NULL;
	mRemains = 0;
	mLayoutDirect = LEFT_TOP;
	mMaxHeight = 0;
	mMaxWidth = 0;
}


GAutoLayout::PageTemplate::~PageTemplate()
{
	if (mMask) gFree2(mMask);
}

GAutoLayout::EvaluateResult::EvaluateResult()
{
	mRemains = 0;		//剩余空间
	mForwardLine = 0;	//前锋线
	mNeatDegrees = 0;	//齐整度
	mIsLay = false;
	mMaxWidth = 0;
	mMaxHeight = 0;
}

GAutoLayout::EvaluateResult::~EvaluateResult()
{
}

GAutoLayout::PageMask::PageMask()
{
	mPageWidth = 0;
	mPageHeight = 0;
	mPageX0 = mPageY0 = 0;
	mPageNumX = mPageNumY = 0;
	mMask = NULL;
}

GAutoLayout::PageMask::~PageMask()
{
	if (mMask)
	{
		gFree2(mMask);
	}
}

GAutoLayout::GAutoLayout()
{
	mIsSelfPartTemplates = false;
	mGridDeltaX = 1;
	mGridDeltaY = 1;
	mLayoutFlag = LAYOUT_OPT_ROTATE90 | LAYOUT_OPT_ROTATE180 | LAYOUT_OPT_ROTATE270 |
				  LAYOUT_OPT_ORDER_BY_X;
}

GAutoLayout::~GAutoLayout()
{
	clear();
}

quint32 GAutoLayout::getLayoutFlag() const
{
	return mLayoutFlag;
}

void GAutoLayout::setLayoutFlag(quint32 flag)
{
	mLayoutFlag = flag;
}

void GAutoLayout::setLayoutParam(const QVector<GPart> &pagePlates, const QVector<int> &pageNums,
								 double gridDelta, const QVector< GPart > &parts, const QVector< int > &partNums)
{
	mPagePlates = pagePlates;
	mPageNums = pageNums;
	mGridDeltaX = gridDelta;
	mGridDeltaY = gridDelta;
	mParts = parts;
	mPartNums = partNums;
}


const QVector< GAutoLayout::PartTemplate * > & GAutoLayout::getPartTemplates() const
{
	return mPartTemplates;
}

void GAutoLayout::setPartTemplates(const QVector< PartTemplate * > &partTemplates)
{
	if (mIsSelfPartTemplates)
	{
		qDeleteAll(mPartTemplates);
		mPartTemplates.clear();
	}
	mPartTemplates = partTemplates;
	mIsSelfPartTemplates = false;
}

void GAutoLayout::clear()
{
	mParts.clear();
	if (mIsSelfPartTemplates)
	{
		qDeleteAll(mPartTemplates);
	}
	mPartTemplates.clear();
	mIsSelfPartTemplates = false;

	qDeleteAll(mPageTemplates);
	mPageTemplates.clear();
	mPageNums.clear();
	mUsedPageNums.clear();
	mPageMasks.clear();
	mPagePlates.clear();
	mPartNums.clear();
}

void GAutoLayout::transform(GPart &poly, double &w, double &h, QMatrix &matrix)
{
	double alpha = 0, beta = 0;
	QRectF box = poly.boundingRect();
	if (mLayoutFlag & LAYOUT_OPT_OOBB_ROT)
	{
		// 计算多边形OBBox
		vector< cv::Point > points;
		for (int i=0; i<poly.count(); i++)
		{
			points.push_back(cv::Point(poly[i].x(), poly[i].y()));
		}
		cv::RotatedRect obbox = cv::minAreaRect(points);
		alpha = -obbox.angle;
		double r = fmod(alpha, 90);
		if (fabs(r) < 3) alpha = 0;
		//printf("%f ", angle);

		if (alpha != 0)
		{
			box.setRect(0, 0, obbox.size.width, obbox.size.height);
		}
	}
	double angle, sina, cosa;
	angle = alpha + beta;
	if (angle == 0)
	{
		sina = 0;	cosa = 1;
	}
	else if (angle == -90)
	{
		sina = -1;	cosa = 0;
	}
	else
	{
		angle *= ANGLE_TO_RADIAN;
		sina = sin(angle);
		cosa = cos(angle);
	}
	for (int i=0; i<poly.count(); i++)
	{
		double x = poly[i].x();
		double y = poly[i].y();
		gRotate(sina, cosa, x, y, x, y);
		poly[i] = QPointF(x, y);
	}
	for (int j=0; j<poly.mHoles.count(); j++)
	{
		for (int i=0; i<poly.mHoles[j].count(); i++)
		{
			double x = poly.mHoles[j][i].x();
			double y = poly.mHoles[j][i].y();
			gRotate(sina, cosa, x, y, x, y);
			poly.mHoles[j][i] = QPointF(x, y);
		}
	}

	box = poly.boundingRect();
	matrix.setMatrix(cosa, sina, -sina, cosa, -box.x(), -box.y());
	poly.translate(-box.x(), -box.y());

	for (int i=0; i<poly.mHoles.count(); i++)
	{
		poly.mHoles[i].translate(-box.x(), -box.y());
	}
	
	box = poly.boundingRect();
	w = box.width();
	h = box.height();
	
}

// 计算多边形的实际面积
double calcPolyArea(const QPolygonF &poly)
{
	double sum = 0.0, temp;
	int i,n = poly.count();
	for(i=1; i<n; i++)
	{
		temp = poly[i-1].x()*poly[i].y() - poly[i].x()*poly[i-1].y();
		sum += temp;
	}
	temp = poly[n-1].x()*poly[0].y() - poly[0].x()*poly[n-1].y();
	sum = abs(sum);

	return sum/2.0;
}

void calcScanLineCrossPts(const QPolygonF &poly, double xc, double yc, QVector<double> &xms, QVector<double> &yms)
{
	// 遍历多边形的每条边计算水平和垂直扫描线的交点
	for (int i = 0; i < poly.count(); i++)
	{
		const QPointF &pt0 = poly[i];
		const QPointF &pt1 = poly[(i + 1) % poly.count()];

		if (pt0.y() != pt1.y())
		{
			QPointF p0 = pt0;
			QPointF p1 = pt1;
			if (p1.y() < p0.y()) qSwap(p0, p1);
			if (yc >= p0.y() && yc < p1.y())
			{
				double xm = p0.x() + (p1.x() - p0.x())*(yc - p0.y()) / (p1.y() - p0.y());
				xms << xm;
			}
		}
		if (pt0.x() != pt1.x())
		{
			QPointF p0 = pt0;
			QPointF p1 = pt1;
			if (p1.x() < p0.x()) qSwap(p0, p1);
			if (xc >= p0.x() && xc < p1.x())
			{
				double ym = p0.y() + (p1.y() - p0.y()) * (xc - p0.x()) / (p1.x() - p0.x());
				yms << ym;
			}
		}
	}
}

void calcPartCenter(const GPart &part, double &xc, double &yc)
{
	double area = 0;
	xc = yc = 0;
	int np = part.count();
	for (int i = 1; i <= np; i++)
	{
		double x1 = part[i % np].x();
		double y1 = part[i % np].y();
		double x0 = part[i - 1].x();
		double y0 = part[i - 1].y();

		double temp = (y1*x0 - x1*y0) * 0.5;
		area += temp;
		xc += temp * (x0 + x1) / 3.0;
		yc += temp * (y0 + y1) / 3.0;
	}
	xc /= area;
	yc /= area;

	QPointF pc(xc, yc);
	if (part.containsPoint(pc, Qt::OddEvenFill))
	{
		int i;
		for (i = 0; i < part.mHoles.count(); i++)
		{
			if (part.mHoles[i].containsPoint(pc, Qt::OddEvenFill))
			{
				break;
			}
		}
		// 如果重心在外多边形内部, 且不在任何内孔中, 则该重心有效
		if (i >= part.mHoles.count()) return;
	}

	// 寻找合理的重心位置
	QVector< double > xms, yms;
	calcScanLineCrossPts(part, xc, yc, xms, yms);
	for (int i = 0; i < part.mHoles.count(); i++)
	{
		calcScanLineCrossPts(part.mHoles[i], xc, yc, xms, yms);
	}

	// 对交点排序
	qSort(xms);
	qSort(yms);

	double dMin;
	if (yms.count() >= 2)
	{
		dMin = HUGE_VAL;
		double yc1;
		for (int i = 0; i < yms.count(); i += 2)
		{
			double y = 0.5 * (yms[i] + yms[i + 1]);
			double d = fabs(y - yc);
			if (d < dMin)
			{
				dMin = d;
				yc1 = y;
			}
		}
		yc = yc1;
		return;
	}

	if (xms.count() >= 2)
	{
		dMin = HUGE_VAL;
		double xc1;
		for (int i = 0; i < xms.count(); i += 2)
		{
			double x = 0.5 * (xms[i] + xms[i + 1]);
			double d = fabs(x - xc);
			if (d < dMin)
			{
				dMin = d;
				xc1 = x;
			}
		}
		xc = xc1;
		return;
	}
}

void GAutoLayout::createPolyLMask(const QImage &maskImage, LMask &lmask, LMask &lmaskY)
{
	int ny = maskImage.height();
	int nx = maskImage.width();
	lmask.clear();
	QPair<int, int> pair;
	int nnx = ceil(nx / 8.0);
	for (int i = 0; i < ny; i++)
	{ 
		const uchar *line = maskImage.scanLine(i);
		lmask.append(QVector< QPair<int, int> >());
		int j;
		bool bstart = false;
		for (j = 0; j < nnx; j++)
		{
			int k0 = j * 8;
			for (int k = 0; k < 8 && k0 + k < nx; k++)
			{
				//if (line[j * 4] != 255)
				if (line[j] & (0x01 << k))
				{
					if (!bstart)
					{
						pair.first = k0+k;
						bstart = true;
					}
				}
				else
				{
					if (bstart)
					{
						pair.second = k0+k - 1;
						lmask.last() << pair;
						bstart = false;
					}
				}
			}
		}
		if (bstart)
		{
			pair.second = nx - 1;// j - 1;
			lmask.last() << pair;
		}
	}

	//延Y轴
	/*QImage imageY = maskImage;
	QMatrix matrix;
	matrix.rotate(90.0);
	imageY = imageY.transformed(matrix, Qt::FastTransformation);
	imageY = imageY.mirrored(true, false);

	lmaskY.clear();
	QPair<int, int> pairY;
	for (int i = 0; i < nx; i++)
	{
		const uchar *line = imageY.scanLine(i);
		lmaskY.append(QVector< QPair<int, int> >());
		int j;
		bool bstart = false;
		for (j = 0; j < ny; j++)
		{
			if (line[j * 4] != 255)
			{
				if (!bstart)
				{
					pairY.first = j;
					bstart = true;
				}
			}
			else
			{
				if (bstart)
				{
					pairY.second = j - 1;
					lmaskY.last() << pairY;
					bstart = false;
				}
			}
		}
		if (bstart)
		{
			pairY.second = j - 1;
			lmaskY.last() << pairY;
		}
	}*/
}

quint32 gDrawTime = 0;
quint32 gMaskTime = 0;

GAutoLayout::PartTemplate * GAutoLayout::createPolyMask(GPart &poly, double w, double h, const QMatrix &matrix)
{
	int nx, ny;
	PartTemplate *pMask = new PartTemplate();
	nx = ceil(w / mGridDeltaX);
	ny = ceil(h / mGridDeltaY);

	QTime timer;

	timer.start();
	pMask->mMaskArea = nx * ny;
	pMask->mMaskSizeList << QSize(nx, ny);
	double scale = 1; 
	//QImage maskImage(nx*scale, ny*scale, QImage::Format_RGB32);
	//QPixmap maskImage(nx*scale, ny*scale);
	//maskImage.fill(Qt::white);
	QBitmap maskImage(nx*scale, ny*scale);
	if (maskImage.width() <= 1 || maskImage.height() <= 1)
	{
		maskImage.fill(Qt::color1);
	}
	maskImage.fill(Qt::color0);
	QPainter p(&maskImage);
	QPolygonF maskPoly;
	for (int i = 0; i < poly.count(); i++)
	{
		QPointF pt(poly[i].x()*scale / mGridDeltaX, poly[i].y()*scale / mGridDeltaY);
		maskPoly << pt;
	}
	//p.setPen(QPen(Qt::black, 1));
	//p.setBrush(Qt::black);
	p.setPen(Qt::color1);
	p.setBrush(Qt::color1);
	p.drawPolygon(maskPoly);

	QRectF boxf0;
	boxf0 = maskPoly.boundingRect();
	pMask->mAspect = boxf0.width() / boxf0.height();

	p.setRenderHint(QPainter::Antialiasing);
	for (int j = 0; j < poly.mHoles.count(); j++)
	{
		QPolygonF maskHole;
		for (int i = 0; i < poly.mHoles[j].count(); i++)
		{
			QPointF pt(poly.mHoles[j][i].x()*scale / mGridDeltaX, poly.mHoles[j][i].y()*scale / mGridDeltaY);
			maskHole << pt;
		}
		//p.setPen(Qt::white);
		//p.setBrush(Qt::white);
		p.setPen(Qt::color0);
		p.setBrush(Qt::color0);
		p.drawPolygon(maskHole);
	}
	p.setRenderHint(QPainter::Antialiasing, false);
	//QImage maskImage2 = maskImage.scaled(nx, ny, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QImage maskImage2 = maskImage.toImage();// .scaled(nx, ny, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	//maskImage2.save("test_pMask2.bmp");
	gDrawTime += timer.elapsed();

	timer.restart();
	pMask->mMaskPosList.append(QPolygon());
	QPolygon &maskPos = pMask->mMaskPosList.last();
	/*for (int i = 0; i < ny; i++)
	{
		uchar *line = maskImage2.scanLine(i);
		for (int j = 0; j < nx; j++)
		{
			if (line[j * 4] != 255)
			{
				maskPos << QPoint(j, i);
			}
		}
	}*/
	int nnx = ceil(nx / 8.0);
	for (int i = 0; i < ny; i++)
	{
		uchar *line = maskImage2.scanLine(i);
		for (int j = 0; j < nnx; j++)
		{
			int k0 = j * 8;
			for (int k = 0; k < 8 && k0 + k < nx; k++)
			{
				if ((line[j] & (0x01 << k)))
				{
					maskPos << QPoint(k0+k, i);
				}
			}
		}
	}
	pMask->mLMaskList.append(LMask());
	pMask->mLMaskListY.append(LMask());
	createPolyLMask(maskImage2, pMask->mLMaskList.last(), pMask->mLMaskListY.last());
	gMaskTime += timer.elapsed();

	int sum = maskPos.count();
	pMask->mRatio = (double)sum / (nx * ny);
	pMask->mPolygonList.append(poly);
	pMask->mTransMats << matrix;

	/*int ixc, iyc;
	calcPartCenter(maskImage, ixc, iyc);
	double xc, yc;
	xc = (ixc * mGridDeltaX + 0.5 * mGridDeltaX) / scale;
	yc = (iyc * mGridDeltaY + 0.5 * mGridDeltaY) / scale; */
	double xc, yc;
	calcPartCenter(poly, xc, yc);

	QPointF pc = matrix.inverted().map(QPointF(xc, yc));
	poly.mCenter = pc;

	// 旋转90-270度
	double rotates[3][2] = { 1, 0, 0, -1, -1, 0 };
	quint32 rotFlag = LAYOUT_OPT_ROTATE90;
	quint32 layoutFlag = mLayoutFlag;
	if (poly.mLayoutFlag >= 0)
	{
		layoutFlag = poly.mLayoutFlag;
	}
	for (int k = 0; k < 3; k++, rotFlag <<= 1)
	{
		if (!(rotFlag & layoutFlag))
		{
			continue;
		}

		QRectF boxf;
		QRect box;
		double sina = rotates[k][0];
		double cosa = rotates[k][1];

		pMask->mPolygonList.append(GPart());
		GPart &polyT = pMask->mPolygonList.last();
		for (int i = 0; i < poly.count(); i++)
		{
			double x, y;
			gRotate(sina, cosa, poly[i].x(), poly[i].y(), x, y);
			polyT << QPointF(x, y);
		}
		for (int j = 0; j < poly.mHoles.count(); j++)
		{
			polyT.mHoles << QPolygonF();
			for (int i = 0; i < poly.mHoles[j].count(); i++)
			{
				double x, y;
				gRotate(sina, cosa, poly.mHoles[j][i].x(), poly.mHoles[j][i].y(), x, y);
				polyT.mHoles.last() << QPointF(x, y);
			}
		}
		boxf = polyT.boundingRect();
		polyT.translate(-boxf.x(), -boxf.y());
		QMatrix mat = matrix * QMatrix(cosa, sina, -sina, cosa, -boxf.x(), -boxf.y());
		pMask->mTransMats << mat;

		for (int i = 0; i < poly.mHoles.count(); i++)
		{
			polyT.mHoles[i].translate(-boxf.x(), -boxf.y());
		}

		pMask->mMaskPosList.append(QPolygon());
		QPolygon &maskPosT = pMask->mMaskPosList.last();
		for (int i = 0; i < maskPos.count(); i++)
		{
			double x, y;
			gRotate(sina, cosa, maskPos[i].x(), maskPos[i].y(), x, y);
			maskPosT << QPoint(qRound(x), qRound(y));
		}
		box = maskPosT.boundingRect();
		maskPosT.translate(-box.x(), -box.y());
		pMask->mMaskSizeList << QSize(box.width(), box.height());

		pMask->mAspect = boxf.width() / boxf.height();
		if (pMask->mAspect < 1) pMask->mAspect = 1.0 / pMask->mAspect;

		QTransform trans;
		int angle;
		if (k == 0) angle = 90;
		else if (k == 1) angle = 180;
		else angle = 270;

		trans.rotate(angle);
		QImage maskImage3 = maskImage2.transformed(trans);
		pMask->mLMaskList.append(LMask());
		pMask->mLMaskListY.append(LMask());
		createPolyLMask(maskImage3, pMask->mLMaskList.last(), pMask->mLMaskListY.last());
		//maskImage3.save("111.bmp");
	}

	return pMask;
}

double cal2DLength(QPointF p1, QPointF p2)
{
	return sqrt((p2.x() - p1.x())*(p2.x() - p1.x()) + (p2.y() - p1.y())*(p2.y() - p1.y()));
}

int calPolyPerimeter(const GPart &poly)
{
	double length = 0;;
	int result = 0;
	int i;
	int n = poly.count();
	for (i = 1; i <= n; ++i)
	{
		length += cal2DLength(poly[(i - 1) % n], poly[i%n]);
	}
	result = ceil(length);
	return result;
}

void GAutoLayout::preprocess()
{
	gDrawTime = 0;
	gMaskTime = 0;
	QTime timer;
	timer.start();
	for (int i=0; i<mParts.count(); i++)
	{
		GPart &part = mParts[i];

		double w, h;
		QMatrix matrix;
		transform(part, w, h, matrix);

		PartTemplate *pTemplate = createPolyMask(part, w, h, matrix);
		pTemplate->mPolyArea = calcPolyArea(part);
		pTemplate->mPerimeter = calPolyPerimeter(part);
		pTemplate->mPartIndex = i;
		pTemplate->mPriority = part.mPriority;
		mPartTemplates.append(pTemplate);
	}
	mIsSelfPartTemplates = true;

	printf("Total Time=%d\nDraw Time = %d\nMask Time=%d\n", timer.elapsed(), gDrawTime, gMaskTime);

	//printf("\n");
}

void GAutoLayout::preprocessPage()
{
	for (int ip = 0; ip < mPagePlates.count(); ip++)
	{
		GPart &pagePlate = mPagePlates[ip];
		mPageMasks << PageMask();
		PageMask &pageMask = mPageMasks.last();

		QRectF box = pagePlate.boundingRect();
		pagePlate.translate(-box.x(), -box.y());
		pageMask.mPageX0 = box.x();
		pageMask.mPageY0 = box.y();
		pageMask.mPageWidth = box.width();
		pageMask.mPageHeight = box.height();
		pageMask.mPageNumX = int(pageMask.mPageWidth / mGridDeltaX);
		pageMask.mPageNumY = int(pageMask.mPageHeight / mGridDeltaY);

		double scale = 1;
		int nx = pageMask.mPageNumX;
		int ny = pageMask.mPageNumY;
		const QPolygonF &poly = pagePlate;
		//QImage maskImage(nx*scale, ny*scale, QImage::Format_RGB32);
		//maskImage.fill(Qt::white);
		QBitmap maskImage(nx*scale, ny*scale);
		maskImage.fill(Qt::color0);
		QPainter p(&maskImage);
		QPolygonF maskPoly;
		for (int i = 0; i < poly.count(); i++)
		{
			QPointF pt(poly[i].x()*scale / mGridDeltaX, poly[i].y()*scale / mGridDeltaY);
			maskPoly << pt;
		}
		//p.setPen(Qt::black);
		//p.setBrush(Qt::black);
		p.setPen(Qt::color1);
		p.setBrush(Qt::color1);
		p.drawPolygon(maskPoly);
		// 绘制内孔多边形
		for (int i = 0; i < pagePlate.mHoles.count(); i++)
		{
			const QPolygonF &holePoly = pagePlate.mHoles[i];
			QPolygonF holeMaskPoly;
			for (int j = 0; j < holePoly.count(); j++)
			{
				QPointF pt(holePoly[j].x()*scale / mGridDeltaX, holePoly[j].y()*scale / mGridDeltaY);
				holeMaskPoly << pt;
			}
			//p.setPen(QPen(Qt::white, 2));
			//p.setBrush(Qt::white);
			p.setPen(QPen(Qt::color0, 2));
			p.setBrush(Qt::color0);
			p.drawPolygon(holeMaskPoly);
		}
		//QImage maskImage2 = maskImage.scaled(nx, ny, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		QImage maskImage2 = maskImage.toImage();// .scaled(nx, ny, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

		gAlloc2(pageMask.mMask, pageMask.mPageNumY, pageMask.mPageNumX);
		memset(pageMask.mMask[0], 1, sizeof(bool)*pageMask.mPageNumY*pageMask.mPageNumX);

		int nnx = ceil(nx / 8.0);
		for (int i = 0; i < ny; i++)
		{
			uchar *line = maskImage2.scanLine(i);
			for (int j = 0; j < nnx; j++)
			{
				int k0 = j * 8;
				for (int k = 0; k < 8 && k0 + k < nx; k++)
				{
					if (!(line[j] & (0x01 << k)))
					{
						pageMask.mMask[i][k0 + k] = false;
					}
				}
			}
		}
	}
}

void putLMask(int i0, const QVector< QPair<int, int> > &polyLMask, QVector< QPair<int, int> > &pageLMask)
{
	int iseg0, iseg;
	iseg0 = 0;
	for (int i = 0; i < polyLMask.count(); i++)
	{
		int first = polyLMask[i].first + i0;
		int last = polyLMask[i].second + i0;
		// 查找first所在的母板分段 => iseg
		for (iseg = iseg0; iseg < pageLMask.count(); iseg++)
		{
			if (pageLMask[iseg].first <= first && last <= pageLMask[iseg].second) break;
		}
		QPair<int, int> pair0(-1, -1);
		QPair<int, int> &pagePair = pageLMask[iseg];
		if (first > pagePair.first)
		{
			pair0.first = pagePair.first;
			pair0.second = first - 1;
		}
		if (last <= pagePair.second)
		{
			pagePair.first = last + 1;
		}
		if (pagePair.first > pagePair.second)
		{
			if (pair0.first < 0)
			{
				pageLMask.remove(iseg);	// 情况(4)分段恰好被用完
			}
			else
			{
				pagePair = pair0;			// 情况(3)前部有剩余
				iseg0++;
			}
		}
		else
		{
			if (pair0.first >= 0)
			{
				pageLMask.insert(iseg, pair0);		// 情况(1)分段被割成2段
				iseg0++;
			}
			// else		// 情况(2)分段后部有剩余, 此时无需额外操作
		}
	}
}

void GAutoLayout::putPoly(const QPoint &layoutPos, PartTemplate *pPolyTemp, int tempIndex, PageTemplate *pPageTemp, GPart *pPart)
{
	bool **tempMask = pPageTemp->mMask;
	QPolygon &maskPos = pPolyTemp->mMaskPosList[tempIndex];
	PageMask &pageMask = mPageMasks[pPageTemp->mPageMaskIndex];
	int ix = layoutPos.x();
	int iy = layoutPos.y();
	double tx = (ix + 0.0) * mGridDeltaX + pageMask.mPageX0;
	double ty = (iy + 0.0) * mGridDeltaY + pageMask.mPageY0;
	GPart &poly = *pPart;
	poly.cloneGeometry(pPolyTemp->mPolygonList[tempIndex]);
	poly.translate(tx, ty);
	poly.mTransMat = pPolyTemp->mTransMats[tempIndex] * QMatrix(1, 0, 0, 1, tx, ty);
	for (int i = 0; i<poly.mHoles.count(); i++)
	{
		poly.mHoles[i].translate(tx, ty);
	}
	for (int i = 0; i<maskPos.count(); i++)
	{
		QPoint &pos = maskPos[i];
		tempMask[iy + pos.y()][ix + pos.x()] = true;
	}
	
	if (layoutPos.x() + pPolyTemp->mMaskSizeList[tempIndex].width() > pPageTemp->mMaxWidth)
	{
		pPageTemp->mMaxWidth = layoutPos.x() + pPolyTemp->mMaskSizeList[tempIndex].width();
	}
	if (layoutPos.y() + pPolyTemp->mMaskSizeList[tempIndex].height() > pPageTemp->mMaxHeight)
	{
		pPageTemp->mMaxHeight = layoutPos.y() + pPolyTemp->mMaskSizeList[tempIndex].height();
	}
	LMask &pageLMask = pPageTemp->mLMaskX;
	LMask &polyLMask = pPolyTemp->mLMaskList[tempIndex];
	for (int i = 0; i < polyLMask.count(); i++)
	{
		putLMask(ix, polyLMask[i], pageLMask[i + iy]);
	}
	pPageTemp->mRemains -= maskPos.count();
}

void GAutoLayout::addPage(int pageMaskIndex)
{
	PageMask &pageMask = mPageMasks[pageMaskIndex];
	PageTemplate *pPageTemp = new PageTemplate();
	pPageTemp->mPageMaskIndex = pageMaskIndex;
	gAlloc2(pPageTemp->mMask, pageMask.mPageNumY, pageMask.mPageNumX);
	//memset(pPageTemp->mMask[0], 0, sizeof(bool)*mPageNumX*mPageNumY);
	memcpy(pPageTemp->mMask[0], pageMask.mMask[0], sizeof(bool)*pageMask.mPageNumX*pageMask.mPageNumY);
	pPageTemp->mRemains = pageMask.mPageNumX * pageMask.mPageNumY;
	pPageTemp->mPageIndex = mPageTemplates.count();
	mPageTemplates.append(pPageTemp);
	if (mLayoutFlag & LAYOUT_OPT_ORDER_BY_X)
	{
		pPageTemp->mForwardLine.fill(-1, pageMask.mPageNumY);
	}
	else
	{
		pPageTemp->mForwardLine.fill(-1, pageMask.mPageNumX);
	}
	// 生成初始线性模板  分别生成XY方向两个线性模板
	QVector< QPair<int, int> > maskLine;
	maskLine << QPair<int, int>(0, pageMask.mPageNumX);
	for (int i = 0; i < pageMask.mPageNumY; i++)
	{
		pPageTemp->mLMaskX << maskLine;
	}
	QVector<QPair<int, int>>maskLineY;
	maskLineY << QPair<int, int>(0, pageMask.mPageNumY);
	for (int i = 0; i < pageMask.mPageNumX; i++)
	{
		pPageTemp->mLMaskY << maskLineY;
	}
}

class PageLessThan
{
private:
	QVector< GAutoLayout::PageTemplate *> *mpPageMasks;
public:
	PageLessThan(QVector< GAutoLayout::PageTemplate * > *pPageMasks)
	{
		mpPageMasks = pPageMasks;
	}
	bool operator ()(int index0, int index1)
	{
		return mpPageMasks->at(index0)->mRemains > mpPageMasks->at(index1)->mRemains;
	}
};

void GAutoLayout::calcForwardLineParam(int nx, int ny, const QVector<int> &forwardLine, int &remains, int &len)
{
	len = 0;
	if (mLayoutFlag & LAYOUT_OPT_ORDER_BY_X)
	{
		//len = nx - forwardLine[0];
		remains = nx - forwardLine[0] - 1;
		for (int i = 1; i < ny; i++)
		{
			len += abs(forwardLine[i] - forwardLine[i - 1]);
			//if (forwardLine[i] != forwardLine[i-1]) len++;
			remains += nx - forwardLine[i] - 1;
		}
		//len += nx - forwardLine[ny-1];
	}
	else
	{
		remains = ny - forwardLine[0];
		for (int i = 1; i < nx; i++)
		{
			len += abs(forwardLine[i] - forwardLine[i - 1]);
			//if (forwardLine[i] != forwardLine[i-1]) len++;
			remains += ny - forwardLine[i];
		}
	}
}

bool mathLMask(int i0, const QVector< QPair<int, int> > &polyLMask, const QVector< QPair<int, int> > &pageLMask, int &moveLength)
{
	int iseg, iseg0;
	iseg0 = 0;
	for (int i = 0; i < polyLMask.count(); i++)
	{
		int first = polyLMask[i].first + i0;
		int last = polyLMask[i].second + i0;
		for (iseg = iseg0; iseg < pageLMask.count(); iseg++)
		{
			if (pageLMask[iseg].first <= first && last <= pageLMask[iseg].second) break;
			if (last <= pageLMask[iseg].second && pageLMask[iseg].first > first)
			{
				moveLength = pageLMask[iseg].first - first;
				return false;
			}
		}
		iseg0 = iseg;
	}
	return true;
}

void GAutoLayout::tryLayout(PartTemplate *pPolyTemp, int k, PageTemplate *pPageTemp, quint32 layoutOrder,
	 const int &mPageNumX, const int &mPageNumY, EvaluateResult &result)
{
	QPoint pos;
	QVector<int> fline;
	int remains, flen;
	int ix, iy, nx, ny, iseg;
	bool **pageMask = pPageTemp->mMask;
	QPolygon &maskPos = pPolyTemp->mMaskPosList[k];
	LMask &pageLMask = pPageTemp->mLMaskX;
	LMask &polyLMask = pPolyTemp->mLMaskList[k];

	nx = pPolyTemp->mMaskSizeList[k].width();
	ny = pPolyTemp->mMaskSizeList[k].height();

	bool isCollison = true;
	result.mIsLay = false;

	if (layoutOrder & LAYOUT_OPT_ORDER_BY_X)
	{
		for (iy = 0; iy <= mPageNumY - ny; iy++)
		{
			for (iseg = 0; iseg < pageLMask[iy].count(); iseg++)
			{
				const QPair<int, int> &pair = pageLMask[iy][iseg];
				int f0 = polyLMask[0].first().first;			// 零件真正起始点
				int e0 = nx - polyLMask[0].last().second - 1;   // 零件真正结束点 
				int ix0 = pair.first - f0;						// 母板起始排放点
				if (ix0 < 0) ix0 = 0;
				int ix1 = pair.second + e0;
				if (ix1 > mPageNumX - nx) ix1 = mPageNumX - nx;
				ix = ix0;
				while (ix <= ix1)
				{
					isCollison = false;
					for (int i = 0; i < polyLMask.count(); i++)
					{
						int moveLength = 0;
						if (!mathLMask(ix, polyLMask[i], pageLMask[iy + i], moveLength))
						{
							isCollison = true;
							ix += moveLength;
							break;
						}
					}
					if (!isCollison)
					{
						goto TRY_LAYOUT_END;
					}
				}
			} 
		}
	}
	else
	{
		QVector< QPair<int, int> > isegs(mPageNumY - ny + 1);
		for (int i = 0; i < mPageNumY - ny + 1; i++)
		{
			isegs[i].first = 0;
			if (pageLMask[i].count() > 0) isegs[i].second = pageLMask[i][0].first;
			else isegs[i].second = mPageNumX;
			
		}
		
		while (true)
		{
			// 寻找Y方向可用X最小的iy
			iy = -1;
			isCollison = true;
			int minX = mPageNumX;
			for (int i = 0; i <= mPageNumY - ny; i++)
			{
				if (isegs[i].first < pageLMask[i].count() && isegs[i].second < minX)
				{
					iy = i;
					minX = isegs[i].second;
				}
			}
			// 如果没有可用的排样位置, 退出
			if (iy < 0) goto TRY_LAYOUT_END;

			iseg = isegs[iy].first;
			const QPair<int, int> &pair = pageLMask[iy][iseg];
			int f0 = polyLMask[0].first().first;			// 零件真正起始点
			int e0 = nx - polyLMask[0].last().second - 1;   // 零件真正结束点 
			int ix0 = isegs[iy].second - f0;				// 母板起始排放点
			if (ix0 < 0) ix0 = 0;
			int ix1 = pair.second + e0;
			if (ix1 > mPageNumX - nx) ix1 = mPageNumX - nx;
			ix = ix0;
			if (ix <= ix1)
			{
				isCollison = false;
				for (int i = 0; i < polyLMask.count(); i++)
				{
					int moveLength = 0;
					if (!mathLMask(ix, polyLMask[i], pageLMask[iy + i], moveLength))
					{
						isCollison = true;
						ix += moveLength;
						break;
					}
				}
			}
			if (!isCollison)
			{
				goto TRY_LAYOUT_END;
			}
			else
			{
				if (ix > ix1)
				{
					isegs[iy].first++;
					if (isegs[iy].first < pageLMask[iy].count()) 
						isegs[iy].second = pageLMask[iy][isegs[iy].first].first;
					else isegs[iy].second = mPageNumX;
				}
				else isegs[iy].second = ix+f0;
			}
		}
	}

TRY_LAYOUT_END:
	if (!isCollison)
	{
		fline = pPageTemp->mForwardLine;
		for (int i = 0; i < polyLMask.count(); i++)
		{
			if (polyLMask[i].count() <= 0) continue;
			int xend = ix + polyLMask[i].last().second;
			if (fline[i + iy] < xend)
			{
				fline[i + iy] = xend;
			}
		}
		calcForwardLineParam(mPageNumX, mPageNumY, fline, result.mRemains, result.mForwardLine);
		if (ix + pPolyTemp->mMaskSizeList[k].width() > pPageTemp->mMaxWidth)
		{
			result.mMaxWidth = ix + pPolyTemp->mMaskSizeList[k].width();
		}
		else result.mMaxWidth = pPageTemp->mMaxWidth;
		if (iy + pPolyTemp->mMaskSizeList[k].height() > pPageTemp->mMaxHeight)
		{
			result.mMaxHeight = iy + pPolyTemp->mMaskSizeList[k].height();
		}
		else result.mMaxHeight = pPageTemp->mMaxHeight;
		result.mPos = QPoint(ix, iy);
		result.mForLine = fline;
		result.mIsLay = true;
	}
}

bool GAutoLayout::layoutPoly(int iPage, PartTemplate *pPolyMask, GPart *pPart)
{
	PageTemplate *pPageTemp = mPageTemplates[iPage];
	PageMask &pageMask = mPageMasks[pPageTemp->mPageMaskIndex];
	QVector<int> forwardLine;
	QPoint layoutPoint;
	int kMask = -1;
	int maxRemains = 0;
	int minForwardLen = 0;
	bool isLay = false;
	int maxHeight, maxWidth;
	//开启y轴扫描的话改为true
	bool openDirY = false;

	int pageNumX = pageMask.mPageNumX;
	int pageNumY = pageMask.mPageNumY;
	int minArea = pageNumX * pageNumY;

	quint32 pRotFlag = mRotFlags[pPolyMask->mPartIndex];
	quint32 rotFlag;
	// 计算指定旋转角度的掩码
	if (pRotFlag & 0x80000000)
	{
		rotFlag = 0xFFFFFFFF;
	}
	else
	{
		if (pRotFlag == 0) rotFlag = 1;
		else
		{
			quint32 flag = 1;
			for (int k = 0; k < 3; k++, flag <<= 1)
			{
				if (flag & pRotFlag)
				{
					rotFlag = 2 << k;
					break;
				}
			}
		}
	}
	for (int k = 0; k < pPolyMask->mPolygonList.count(); k++)// pPolyMask->mPolygonList.count()
	{
		if (!(rotFlag & (1 << k))) continue;
		EvaluateResult layoutDirX;
		tryLayout(pPolyMask, k, pPageTemp, mLayoutFlag, pageNumX, pageNumY, layoutDirX);
		if (layoutDirX.mIsLay)
		{
			isLay = true;
			int temMaxRemain = 0;
			int temMinForwardLen = 0;
			int temForwordLen = 0;
			QVector<int> temFLine;
			if (openDirY)
			{
				EvaluateResult layoutDirY;
				tryLayout(pPolyMask, k, pPageTemp, LAYOUT_OPT_ORDER_BY_Y, pageNumX, pageNumY, layoutDirY);
				QPoint dirX = layoutDirX.mPos;
				QPoint dirY = layoutDirY.mPos;
				QPoint temLayoutPos;
				int dirXArea = layoutDirX.mMaxWidth * layoutDirX.mMaxHeight;
				int dirYArea = layoutDirY.mMaxWidth * layoutDirY.mMaxHeight;
				int temArea = 0;
				if (dirXArea < dirYArea)
				{
					temForwordLen = layoutDirX.mForwardLine;
					temFLine = layoutDirX.mForLine;
					temArea = dirXArea;
					temLayoutPos = layoutDirX.mPos;
					temMaxRemain = layoutDirX.mRemains;
					temMinForwardLen = layoutDirX.mForwardLine;
				}
				else if (dirXArea > dirYArea)
				{
					temForwordLen = layoutDirY.mForwardLine;
					temFLine = layoutDirY.mForLine;
					temArea = dirYArea;
					temLayoutPos = layoutDirY.mPos;
					temMaxRemain = layoutDirY.mRemains;
					temMinForwardLen = layoutDirY.mForwardLine;
				}
				else if (dirXArea = dirYArea)
				{
					if (layoutDirX.mRemains > layoutDirY.mRemains
						|| (layoutDirX.mRemains == layoutDirY.mRemains && layoutDirX.mForwardLine <= layoutDirY.mForwardLine))
					{
						temForwordLen = layoutDirX.mForwardLine;
						temFLine = layoutDirX.mForLine;
						temArea = dirXArea;
						temLayoutPos = layoutDirX.mPos;
						temMaxRemain = layoutDirX.mRemains;
						temMinForwardLen = layoutDirX.mForwardLine;
					}
					else if (layoutDirX.mRemains < layoutDirY.mRemains
						|| (layoutDirX.mRemains == layoutDirY.mRemains && layoutDirX.mForwardLine > layoutDirY.mForwardLine))
					{
						temForwordLen = layoutDirY.mForwardLine;
						temFLine = layoutDirY.mForLine;
						temArea = dirYArea;
						temLayoutPos = layoutDirY.mPos;
						temMaxRemain = layoutDirY.mRemains;
						temMinForwardLen = layoutDirY.mForwardLine;
					}
				}
				if (temArea < minArea)
				{
					kMask = k;
					minArea = temArea;
					layoutPoint = temLayoutPos;
					forwardLine = temFLine;
					minForwardLen = temForwordLen;
					maxRemains = temMaxRemain;
				}
				else if (temArea == minArea)
				{
					if (temMaxRemain > maxRemains || (temMaxRemain == maxRemains && temMinForwardLen < minForwardLen))
					{
						kMask = k;
						minArea = temArea;
						layoutPoint = temLayoutPos;
						forwardLine = temFLine;
						minForwardLen = temMinForwardLen;
						maxRemains = temMaxRemain;
					}
				}
			}//x单向评估标准
			else
			{
				QPoint dirX = layoutDirX.mPos;
				if (layoutDirX.mRemains > maxRemains || (layoutDirX.mRemains == maxRemains && layoutDirX.mForwardLine < minForwardLen))
				{
					kMask = k;
					//minArea = dirXArea;
					layoutPoint = dirX;
					forwardLine = layoutDirX.mForLine;
					minForwardLen = layoutDirX.mForwardLine;
					maxRemains = layoutDirX.mRemains;
					maxHeight = layoutDirX.mMaxHeight;
					maxWidth = layoutDirX.mMaxWidth;
				}
			}
		}
		else
		{
			if (k == pPolyMask->mPolygonList.count() - 1 && isLay == false) return false;
			else continue;
		}
	}
	if (kMask >= 0)
	{
		putPoly(layoutPoint, pPolyMask, kMask, pPageTemp, pPart);
		pPageTemp->mMaxWidth = maxWidth;
		pPageTemp->mMaxHeight = maxHeight;
		pPart->mPageIndex = iPage;
		pPageTemp->mForwardLine = forwardLine;
		//pPolyMask->mPageIndex = iPage;
		return true;
	}
	return false;
}

float GAutoLayout::layout(const QVector<int> &orders, 
						  const QVector<quint32> &rots, 
						  QVector< GPart > &nestParts,
						  QVector<int> &usedPageNums,
						  QVector<int> &plateIndices,
						  int *pProgress)
{
	QDate now = QDate::currentDate();

	float nestRating = 0;  // 暂存当前零件序列的排版率
	double allPartAreas = 0, allMasterArea = 0;
	if (pProgress) *pProgress = 0;
	mIsAbort = false;
	qDeleteAll(mPageTemplates);
	mPageTemplates.clear();
	mUsedPageNums.clear();
	mPageMasks.clear();
	mRotFlags = rots;
	if (mRotFlags.isEmpty())
	{
		for (int i = 0; i < mParts.count(); i++)
		{
			mRotFlags << 0x80000000;
		}
	}
	mUsedPageNums.fill(0, mPageNums.count());
	
	preprocessPage();
	addPage(0);
	mUsedPageNums[0]++;
	PageLessThan pageLessThan(&mPageTemplates);
	
	
	QVector< QVector< GPart > > layoutParts;
	for (int i = 0; i < mParts.count(); i++)
	{
		layoutParts << QVector< GPart >();
		for (int k = 0; k < mPartNums[i]; k++)
		{
			layoutParts.last() << mParts[i];
		}
	}
	int ipart = 0;
	for (int i = 0; i < orders.count(); i++)
	{
		if (mIsAbort) break;

		if (ipart >= 500) break;
		if (now > QDate(2021, 1, 1) && ipart >= 20)
		{
			break;
		}

		if (pProgress)
		{
			*pProgress = qRound((i + 1) * 100.0 / mPartTemplates.count());
			//printf("progress = %d\n", *pProgress);
		}

		int pidx = orders[i];
		PartTemplate *pPartTemp = mPartTemplates[pidx];
		allPartAreas += pPartTemp->mPolyArea;
		for (int k = 0; k < mPartNums[pidx]; k++)
		{
			//printf("Layout Part %d(%d)\n", mParts[pidx].mPartID, k);
			GPart &part = layoutParts[pidx][k];
			part.mSortIndex = i;
			ipart++;

			// 按照剩余可用面积大小排序页面
			QVector<int> pageIndices;
			for (int j = 0; j < mPageTemplates.count(); j++)
			{
				pageIndices << j;
			}
			qStableSort(pageIndices.begin(), pageIndices.end() - 1, pageLessThan);

			bool ret = false;
			for (int j = 0; j < pageIndices.count(); j++)
			{
				ret = layoutPoly(pageIndices[j], pPartTemp, &part);
				if (ret) break;
			}

			if (ret) continue;

			// 新开页面
			for (int j = 0; j < mPageNums.count(); j++)
			{
				if (mUsedPageNums[j] < mPageNums[j])
				{
					addPage(j);
					mUsedPageNums[j]++;
					ret = layoutPoly(mPageTemplates.count() - 1, pPartTemp, &part);
					//if (!ret)
					//{
					//	// 无法安置当前多边形, 理论上不应当出现该问题
					//	// 该多边形大于母版, 无法安置, 应特殊处理
					//	//printf("cann't layout %d\n", i);
					//}
				}
			}
		}
	}

	nestParts.clear();
	for (int i = 0; i<layoutParts.count(); i++)
	{
		nestParts.append(layoutParts[i]);
	}

	QVector<bool> pageUsedTags(mPageTemplates.count(), false);
	for (int i = 0; i<layoutParts.count(); i++)
	{
		GPart &part = nestParts[i];
		if (part.mPageIndex >= 0)
		{
			pageUsedTags[part.mPageIndex] = true;
		}
	}

	// 去除未使用的母版
	for (int i = pageUsedTags.count() - 1; i >= 0; i--)
	{
		if (!pageUsedTags[i])
		{
			for (int j = 0; j < layoutParts.count(); j++)
			{
				GPart &part = nestParts[i];
				if (part.mPageIndex > i) part.mPageIndex--;
			}
			mUsedPageNums[mPageTemplates[i]->mPageMaskIndex]--;
			delete mPageTemplates[i];
			mPageTemplates.remove(i);
		}
	}

	usedPageNums = mUsedPageNums;
	plateIndices.clear();
	for (int i = 0; i < mPageTemplates.count(); i++)
	{
		plateIndices << mPageTemplates[i]->mPageMaskIndex;

		allMasterArea += mPageMasks[0].mPageHeight*(mPageTemplates[i]->mMaxWidth*mGridDeltaX);
	}
	nestRating = (allPartAreas / allMasterArea) * 100;

	return nestRating;
}

void GAutoLayout::abort()
{
	mIsAbort = true;
}
