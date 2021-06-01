#include <QPainter>
#include <QPaintEvent>
#include "gcurveview.h"

GCurveView::GCurveView( QWidget * parent /*= 0*/, Qt::WindowFlags f /*= 0*/ )
{
	mMargins.setLeft(10);
	mMargins.setRight(10);
	mMargins.setTop(10);
	mMargins.setBottom(10);

	mZoomScale = 1;
	mOffsetX = mOffsetY = 0;
	mMinOffsetX = mMinOffsetY = 0;
	mIsDrag = false;
	mPageIndex = 0;
}

GCurveView::~GCurveView()
{

}

void GCurveView::refresh()
{
	int w = (width()-mMargins.left()-mMargins.right());
	int h = (height()-mMargins.top()-mMargins.bottom());
	double sx = mDataRange.width() / w;
	double sy = mDataRange.height() / h;

	if (sx <= sy)
	{
		int offsetX =  qRound((w - mDataRange.width() / sy) / 2);
		mViewRect.setRect(mMargins.left()+offsetX, mMargins.top(),
			w - 2*offsetX, h);
	}
	else
	{
		int offsetY = qRound((h - mDataRange.height() / sx) / 2);
		mViewRect.setRect(mMargins.left(), mMargins.top()+offsetY,
			w, h - 2*offsetY);
	}
}


void GCurveView::setPolygons(const QVector< GPart > &polygons)
{
	mPolygons = polygons;
	//update();
}

void GCurveView::setHoles(const QVector< QPolygonF > &holes)
{
	mHoles = holes;
	//update();
}

void GCurveView::setPlates(const QVector< GPart > &plates)
{
	mPlates = plates;
	//update();
}

void GCurveView::setPlateIndices(const QVector< int > &plateIndices)
{
	mPlateIndices = plateIndices;
	//update();
}

void GCurveView::setCurrentPage(int pageIndex)
{
	mPageIndex = pageIndex;
}

void GCurveView::setMainboardRange(double w, double h)
{
	mDataRange.setRect(0, 0, w, h);

	refresh();
	update();
}

int GCurveView::lpXTodpX(double lx)
{
	return mViewRect.left() + 
		   qRound((lx - mDataRange.left()) * mViewRect.width() * mZoomScale / mDataRange.width()) +
		   mOffsetX;
}

double GCurveView::dpXTolpX(int x)
{
	return mDataRange.left() + 
		   (x - mViewRect.left() - mOffsetX) * mDataRange.width() / (mViewRect.width() * mZoomScale);
}

int GCurveView::lpYTodpY(double ly)
{
	return mViewRect.top() + 
		   qRound((mDataRange.bottom() - ly) * mViewRect.height() * mZoomScale / mDataRange.height()) +
		   mOffsetY;
}

double GCurveView::dpYTolpY(int y)
{
	return mDataRange.bottom() - (y - mViewRect.top() - mOffsetY) * mDataRange.height() / (mViewRect.height() * mZoomScale);
}

double basicValue(int i, int n, double t)
{
	if (i == 0)
	{
		return pow((1-t), n);
	}
	else if (i == n)
	{
		return pow(t, n);
	}
	else
	{
		int a,b;
		a = b = 1;
		for (int k = 1; k <= n-i; k++)
		{
			b *= k;
		}
		for (int k = n; k > i; k--)
		{
			a *= k;
		}
		int cni = a/b;
		return cni * pow(t, i) * pow((1-t), (n-i));
	}

}

void GCurveView::drawPlates(QPainter *p, int i)
{
	const GPart &part = mPlates[i];
	if (part.count() <= 0) return;

	int x, y, x1, y1, n;
	n = part.count();
	p->setPen(QPen(Qt::red, 2));

	x = lpXTodpX(part[0].x());
	y = lpYTodpY(part[0].y());
	for (int i = 1; i <= n; i++)
	{
		x1 = lpXTodpX(part[i%n].x());
		y1 = lpYTodpY(part[i%n].y());
		p->drawLine(x, y, x1, y1);
		x = x1;
		y = y1;
	}

	for (int i = 0; i<part.mHoles.count(); i++)
	{
		const QPolygonF &poly = part.mHoles[i];
		n = poly.count();

		x = lpXTodpX(poly[0].x());
		y = lpYTodpY(poly[0].y());
		for (int j = 1; j <= n; j++)
		{
			x1 = lpXTodpX(poly[j%n].x());
			y1 = lpYTodpY(poly[j%n].y());
			p->drawLine(x, y, x1, y1);
			x = x1;
			y = y1;
		}
	}
}

void GCurveView::drawPolygon(QPainter *p, int iPoly)
{
	const GPart &part = mPolygons[iPoly];
	if (part.count() <= 0) return;
	int x, y, x1, y1, n;
	n = part.count();
	p->setPen(Qt::black);

	QPolygonF drawPolygons;
	QPointF p1;
	x = lpXTodpX(part[0].x());
	y = lpYTodpY(part[0].y());
	p1.setX(x);
	p1.setY(y);

	drawPolygons.append(p1);
	for (int i = 1; i <= n; i++)
	{
		x1 = lpXTodpX(part[i%n].x());
		y1 = lpYTodpY(part[i%n].y());
		QPointF p2;
		p2.setX(x1);
		p2.setY(y1);
		drawPolygons.append(p2);

		p->drawLine(x, y, x1, y1);
		x = x1;
		y = y1;
	}

	QBrush brush;   //画刷。填充几何图形的调色板，由颜色和填充风格组成
	brush.setColor(QColor(0, 255, 0, 120));
	//brush.setColor(Qt::gray);
	brush.setStyle(Qt::SolidPattern);

	QPainterPath myPath;
	myPath.addPolygon(drawPolygons);
	//p->setBrush(brush);
	p->drawPath(myPath);
	p->fillPath(myPath, brush);


	QPointF center = part.mCenter;
	x = lpXTodpX(center.x());
	y = lpYTodpY(center.y());

	x1 = lpXTodpX(center.x());
	y1 = lpYTodpY(center.y()) - 12;

	int x2 = lpXTodpX(center.x());
	int y2 = lpYTodpY(center.y()) + 12;

	//p->drawText(x, y, QString::number(iPoly));

	p->drawText(x, y, QString::number(part.mSortIndex));
	//p->setPen(QPen(Qt::red));
	//p->drawEllipse(QPoint(x, y), 1, 1);

	//// 绘制零件分大类后的零件号
	p->setPen(QPen(Qt::color1));
	p->setFont(QFont("Times New Roma",8,1));
	p->drawText(x1, y1, QString::number(part.mGroupIndex));
	//p->drawEllipse(QPoint(x1, y1), 1, 1);

	//// 绘制零件分小类后的零件号
	//p->setPen(QPen(Qt::color1));
	//p->setFont(QFont("Times New Roma", 6, 1));
	p->drawText(x2, y2, QString::number(part.mSubGroupIndex));
	//p->drawEllipse(QPoint(x2, y2), 1, 1);

	for (int i = 0; i < part.mHoles.count(); i++)
	{
		const QPolygonF &poly = part.mHoles[i];
		n = poly.count();
		//p->setPen(Qt::red);
		QPolygonF  partHolesPaint;
		QPointF p1;
		x = lpXTodpX(poly[0].x());
		y = lpYTodpY(poly[0].y());
		p1.setX(x);
		p1.setY(y);
		partHolesPaint.append(p1);

		for (int j = 1; j <= n; j++)
		{
			x1 = lpXTodpX(poly[j%n].x());
			y1 = lpYTodpY(poly[j%n].y());
			QPointF p2;
			p2.setX(x1);
			p2.setY(y1);
			partHolesPaint.append(p2);
			//p->drawLine(x, y, x1, y1);
			x = x1;
			y = y1;
		}

		QBrush brush;   //画刷。填充几何图形的调色板，由颜色和填充风格组成
		brush.setColor(Qt::white);
		brush.setStyle(Qt::SolidPattern);
		QPainterPath myPath;
		myPath.addPolygon(partHolesPaint);
		p->drawPath(myPath);
		p->fillPath(myPath, brush);
	}
}

void GCurveView::drawHoles(QPainter *p, int iPoly)
{
	const QPolygonF &poly = mHoles[iPoly];
	if (poly.count() <= 0) return;

	int x, y, x1, y1, n;
	n = poly.count();
	//p->setPen(Qt::red);

	QPolygonF drawPolygons;
	QPointF p1;
	x = lpXTodpX(poly[0].x());
	y = lpYTodpY(poly[0].y());
	p1.setX(x);
	p1.setY(y);
	drawPolygons.append(p1);
	for (int i = 1; i <= n; i++)
	{
		x1 = lpXTodpX(poly[i%n].x());
		y1 = lpYTodpY(poly[i%n].y());
		QPointF p2;
		p2.setX(x1);
		p2.setY(y1);
		drawPolygons.append(p2);

		//p->drawLine(x, y, x1, y1);
		x = x1;
		y = y1;
	}

	QBrush brush;
	brush.setColor(Qt::white);
	QPainterPath myPath1;
	myPath1.addPolygon(drawPolygons);
	p->drawPath(myPath1);
	p->fillPath(myPath1, brush);

}

void GCurveView::paintEvent( QPaintEvent * event )
{
	QPainter p(this);
	p.fillRect(event->rect(), Qt::white);

	p.setPen(QPen(Qt::red, 1));
	p.drawText(5, 20, QString::number(mPageIndex));
	for (int i=0; i<mPolygons.count(); i++)
	{
		if (mPolygons[i].mPageIndex == mPageIndex)
		{
			drawPolygon(&p, i);
		}
	}

	if (mPlateIndices.count() > 0 && mPlates.count() > 0 && mPageIndex >= 0)
	{
		drawPlates(&p, mPlateIndices[mPageIndex]);
	}
	else
	{
		p.setPen(QPen(Qt::red, 2));
		int x0 = lpXTodpX(mDataRange.left());
		int x1 = lpXTodpX(mDataRange.right());
		int y0 = lpYTodpY(mDataRange.top());
		int y1 = lpYTodpY(mDataRange.bottom());
		p.drawRect(x0, y0, x1 - x0, y1 - y0);
	}
}

void GCurveView::resizeEvent( QResizeEvent * event )
{
	refresh();
}

void GCurveView::wheelEvent(QWheelEvent * event)
{
	// 获取到用户缩放时鼠标的位置对应的逻辑值
	int mouseX = event->pos().x();
	int mouseY = event->pos().y();

	// 当滚动区不在绘图区不进行缩放
	/*if (mouseX < mViewRect.left()||mouseX > mViewRect.right() ||
		mouseY < mViewRect.top()||mouseY > mViewRect.bottom())
	{
		return;
	}*/


	double lxm = dpXTolpX(mouseX);
	double lym = dpYTolpY(mouseY);

	if (event->delta() > 0)
	{
		mZoomScale *= 1.1;
	}
	else
	{
		mZoomScale /= 1.1;
		if (mZoomScale < 1)
		{
			mZoomScale = 1;
		}
	}

	int w = qRound(mZoomScale * mViewRect.width());
	int h = qRound(mZoomScale * mViewRect.height());
	int dxc = lpXTodpX(lxm);
	int dyc = lpYTodpY(lym);

	mOffsetX += mouseX - dxc;
	mOffsetY += mouseY - dyc;

	mMinOffsetX = mViewRect.width() - w;
	if (mOffsetX < mMinOffsetX) mOffsetX = mMinOffsetX;
	if (mOffsetX > 0) mOffsetX = 0;


	mMinOffsetY = mViewRect.height() - h;
	if (mOffsetY < mMinOffsetY) mOffsetY = mMinOffsetY;
	if (mOffsetY > 0) mOffsetY = 0;

	// 重新绘制
	update();

}

void GCurveView::mouseMoveEvent(QMouseEvent * event)
{
	if (mIsDrag)
	{
		int dx = event->pos().x() - mMousePos.x();
		int dy = event->pos().y() - mMousePos.y();
		mMousePos = event->pos();
		mOffsetX += dx;
		mOffsetY += dy;

		if (mOffsetX < mMinOffsetX) mOffsetX = mMinOffsetX;
		if (mOffsetX > 0) mOffsetX = 0;

		if (mOffsetY < mMinOffsetY) mOffsetY = mMinOffsetY;
		if (mOffsetY > 0) mOffsetY = 0;

		update();
	}
}

void GCurveView::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton)
	{
		mIsDrag = true;
		mMousePos = event->pos();
	}
}

void GCurveView::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton)
	{
		mIsDrag = false;
	}
}
