#ifndef GCURVE_VIEW_H
#define GCURVE_VIEW_H

#include <QWidget>
#include <QPolygonF>
#include <QRect>
#include <QRectF>

#include "gautolayout.h"

class GCurveView : public QWidget
{
protected:
	QRect			mMargins;
	QRect			mViewRect;
	QRectF			mDataRange;
	QRectF			mDataViewRect;
	double			mZoomScale;
	int				mOffsetX, mOffsetY;
	int				mMinOffsetX, mMinOffsetY;
	QPoint			mMousePos;				// ¼ÇÂ¼Êó±êÍÏ×§Î»ÖÃ
	bool			mIsDrag;				// ÊÇ·ñÍÏ×§

	QVector< GPart >			mPolygons;
	QVector< GPart >			mPlates;
	QVector< int >				mPlateIndices;
	QVector< QPolygonF >		mHoles;
	int				mPageIndex;

protected:
	void refresh();

	int lpXTodpX(double lx);
	int lpYTodpY(double ly);

	double dpXTolpX(int x);
	double dpYTolpY(int y);

	void drawPolygon(QPainter *p, int i);

	void drawHoles(QPainter *p, int i);

	void drawPlates(QPainter *p, int i);

	virtual void paintEvent(QPaintEvent * event);

	virtual void resizeEvent(QResizeEvent * event);

	virtual void mouseMoveEvent(QMouseEvent * event);

	virtual void mousePressEvent(QMouseEvent * event);

	virtual void mouseReleaseEvent(QMouseEvent * event);

	virtual void wheelEvent(QWheelEvent * event);

public:
	GCurveView(QWidget * parent = 0, Qt::WindowFlags f = 0);
	~GCurveView();

	void setPolygons(const QVector< GPart > &polygons);

	void setHoles(const QVector< QPolygonF > &holes);

	void setPlates(const QVector< GPart > &plates);

	void setPlateIndices(const QVector< int > &plateIndices);

	void setMainboardRange(double w, double h);

	void setCurrentPage(int pageIndex);
};

#endif
