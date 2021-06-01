#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QPolygonF>
#include "tsimplifycurvedlg.h"
#include "gmath.h"
#include "tsettingdlg.h"
#include "gautolayout.h"
#include "gnest.h"
#include <QTime>
#include <time.h>
#include <QProgressDialog>

TSimplifyCurveDlg::TSimplifyCurveDlg(QWidget *parent, Qt::WindowFlags flags)
	: QDialog(parent, flags | Qt::Window)
{
	ui.setupUi(this);

	srand(time(NULL));

	// 	mMatWidth = 3000;
	// 	mMatHeight = 2000;
	// 	mMatGridDelta = 20;

	mMatWidth = 1220;
	mMatHeight = 2440;
	mMatGridDelta = 10;
	mIsOOBBRot = false;
	mRotFlag = 0x01;
	mMaxIterCount = 0;

	mPageNum = 1;
	mPageIndex = 0;
	ui.mCurveView->setMainboardRange(mMatWidth, mMatHeight);
}

TSimplifyCurveDlg::~TSimplifyCurveDlg()
{
}

QString getLine(QTextStream &stream)
{
	QString line;
	while (!stream.atEnd())
	{
		line = stream.readLine().trimmed();
		if (!line.isEmpty()) return line;
	}
	return "";
}

bool loadParts(const QString &fname, GPart &part, int &partNum)
{
	QFile file(fname);
	if (!file.open(QFile::ReadOnly)) return false;
	QTextStream stream(&file);

	partNum = 1;

	int n;
	stream >> n;
	if (n <= 0 || n > 1000000)
	{
		file.close();
		return false;
	}

	double x, y, x1, y1;
	for (int i = 0; i<n; i++)
	{
		stream >> x >> y;
		if (i > 0)
		{
			double d = sqrt((x1 - x)*(x1 - x) + (y1 - y)*(y1 - y));
			//printf("d=%f\n", d);
			if (d < 1e-3) continue;
		}
		part << QPointF(x, y);
		x1 = x;		y1 = y;
	}

	while (!stream.atEnd())
	{
		QString str = getLine(stream);

		if (str.toLower() == "in")
		{
			int nh;
			stream >> nh;
			for (int i = 0; i < nh; i++)
			{
				stream >> n;
				part.mHoles << QPolygonF();
				QPolygonF &hole = part.mHoles.last();
				for (int j = 0; j < n; j++)
				{
					stream >> x >> y;
					if (j > 0)
					{
						double d = sqrt((x1 - x)*(x1 - x) + (y1 - y)*(y1 - y));
						//printf("d=%f\n", d);
						if (d < 1e-3) continue;
					}
					hole << QPointF(x, y);
					x1 = x;		y1 = y;
				}
			}
		}
		else if (str.toLower() == "count" || str.toLower() == "nestnum")
		{
			stream >> partNum;
			if (partNum < 1) partNum = 1;
		}
		else if (str.toLower() == "priority" || str.toLower() == "prior")
		{
			stream >> part.mPriority;
		}
		else if (str.toLower() == "rotalflag")
		{
			stream >> part.mLayoutFlag;
		}
	}

	file.close();
	return true;
}

double distance2D(double x0, double y0, double x1, double y1)
{
	return sqrt((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
}

void increasePoints(QVector<QPointF> &points, double step)
{
	size_t j = 1;
	step *= 1.5;
	j = points.size() - 1;
	while (j >= 1)
	{
		double d = distance2D(points[j].x(), points[j].y(), points[j - 1].x(), points[j - 1].y());
		if (d > step)
		{
			points.insert(j, QPointF(0.5*(points[j].x() + points[j - 1].x()), 0.5*(points[j].y() + points[j - 1].y())));
			j++;
		}
		else j--;
	}
}

void TSimplifyCurveDlg::on_mLoadBtn_clicked()
{
	QStringList fnames = QFileDialog::getOpenFileNames(this, tr("Open File"),
		"", tr("Text File (*.txt);;All Files (*.*)"));

	if (fnames.isEmpty()) return;

	mParts.clear();
	mPartNums.clear();

	for (int i = 0; i<fnames.count(); i++)
	{
		mParts.append(GPart());
		GPart &poly = mParts.last();
		int num;
		if (!loadParts(fnames[i], poly, num))
		{
			QMessageBox::warning(this, windowTitle(), tr("Load File Failed!"));
		}
		mPartNums << num;

		printf("文件: %s, 点数: %d\n", fnames[i].toLocal8Bit().data(), poly.count());
	}

	ui.mCurveView->setPolygons(QVector<GPart>());
	ui.mCurveView->update();
}

void TSimplifyCurveDlg::on_mSettingBtn_clicked()
{
	TSettingDlg setDlg(this);
	setDlg.setWidth(mMatWidth);
	setDlg.setHeight(mMatHeight);
	setDlg.setDelta(mMatGridDelta);
	setDlg.setOOBBRot(mIsOOBBRot);
	setDlg.setRotFlag(mRotFlag);
	setDlg.setMaxIterCount(mMaxIterCount);
	if (setDlg.exec() == QDialog::Accepted)
	{
		mMatWidth = setDlg.getWidth();
		mMatHeight = setDlg.getHeight();
		mMatGridDelta = setDlg.getDelta();
		mIsOOBBRot = setDlg.isOOBBRot();
		mRotFlag = setDlg.getRotFlag();
		mMaxIterCount = setDlg.getMaxIterCount();
		ui.mCurveView->setMainboardRange(mMatWidth, mMatHeight);
	}
}

double * poly2Array(const QPolygonF &poly)
{
	double *data = new double[poly.count() * 2];
	for (int i = 0, j = 0; i < poly.count(); i++)
	{
		data[j++] = poly[i].x();
		data[j++] = poly[i].y();
	}
	return data;
}

#include <QThread>
class NestThread : public QThread
{
private:
	int		mMaxIterCount;
	int		mProgress;
	int		mNestType;
	int		mPageIndex;

public:

	NestThread(QObject * parent = 0) : QThread(parent)
	{
		mMaxIterCount = 0;
		mProgress = 0;
		mNestType = 0;
		mPageIndex = -1;
	}

	void setMaxIterCount(int iterCount)
	{
		mMaxIterCount = iterCount;
	}

	void setNestType(int typ)
	{
		mNestType = typ;
	}

	void setPageIndex(int ipage)
	{
		mPageIndex = ipage;
	}

	virtual void run()
	{
		mProgress = 0;
		if (mMaxIterCount <= 0)
			StartNest(&mProgress);
		else if (mNestType == 1)
		{
			StartOptimizeNest(&mProgress, mMaxIterCount);
		}
		else if (mNestType == 2)
		{
			StartOptimizeNestPage(mPageIndex, &mProgress, mMaxIterCount);
		}
	}

	void forceEndNest()
	{
		ForceEndNest();
	}

	int getProgress()
	{
		return mProgress;
	}
};

void TSimplifyCurveDlg::on_mLayoutBtn_clicked()
{
	GAutoLayout autoLayout;
	QVector<int> polygonPages;
	QVector< GPart > layoutPolygons;
	QVector< GPart > plates;
	QTime timer;
	timer.start();
	GPart pagePoly;
	QVector<int> priorityOrder;

	double *data = NULL;

	// 初始化自动排样
	InitNest();

	// 设置板材多边形
	/*pagePoly << QPointF(0, 0) << QPointF(0, mMatHeight)
	//<< QPointF(mMatWidth, mMatHeight) << QPointF(mMatWidth, 0);
	<< QPointF(mMatWidth, mMatHeight) << QPointF(mMatWidth, mMatHeight / 2) << QPointF(mMatWidth / 2, mMatHeight / 2)
	<< QPointF(mMatWidth / 2, 0);
	plates << pagePoly;
	data = poly2Array(pagePoly);
	AddNewPlates(1, 2, pagePoly.count(), data);
	delete []data;
	data = NULL;*/

	pagePoly.clear();
	pagePoly << QPointF(0, 0) << QPointF(0, mMatHeight)
		<< QPointF(mMatWidth, mMatHeight) << QPointF(mMatWidth, 0);
	plates << pagePoly;
	data = poly2Array(pagePoly);
	AddNewPlates(2, 1000, pagePoly.count(), data);
	delete[]data;
	data = NULL;

	QRectF pageBox = pagePoly.boundingRect();
	int pageIndex;

	// 设置排样参数
	SetNestSpacing(mMatGridDelta);
	SetPartEnablePolarRotation(mIsOOBBRot);
	SetSparePartsAngle(mRotFlag);
	SetNestPriorityDir(1);
	// 设置零件多边形
	int partNum = 1;
	for (int i = 0; i < mParts.count(); i++)
	{
		data = poly2Array(mParts[i]);
		AddNewSpareParts(i, mPartNums[i], mParts[i].count(), data);
		delete[]data;
		data = NULL;

		SetPartPriority(i, mParts[i].mPriority);
		SetRotAngleByPartNo(i, mParts[i].mLayoutFlag);
		for (int j = 0; j < mParts[i].mHoles.count(); j++)
		{
			data = poly2Array(mParts[i].mHoles[j]);
			AddInnerHoleByPartNo(i, mParts[i].mHoles[j].count(), data);
			delete[]data;
			data = NULL;
		}

		/*QRectF box = mParts[i].boundingRect();
		if (box.width() > pageBox.width() || box.height() > pageBox.height())
		{
		SetRotAngleByPartNo(i, 0x01);
		}*/
	}

	// 启动排样线程
	QProgressDialog progress("Nest...", "Abort", 0, 100, this);
	progress.setWindowModality(Qt::WindowModal);
	NestThread nestThread;
	nestThread.setMaxIterCount(mMaxIterCount);
	nestThread.setNestType(1);
	nestThread.setPageIndex(GetNestPlateTotalNum() - 1);

	PreProcess();

	nestThread.start();
	// 检查排样是否完成, 用户是否终止
	while (!nestThread.isFinished())
	{
		qApp->processEvents();
		// 获取排样进度
		progress.setValue(nestThread.getProgress());
		if (progress.wasCanceled())
		{
			nestThread.forceEndNest();
		}
		::_sleep(10);
		//::Sleep(10);
	}
	printf("Layout time: %d\n", timer.elapsed());

	// 启动最后一页排样
	progress.setLabelText("Nest Last Page ...");
	progress.show();
	NestThread lastNestThread;
	lastNestThread.setNestType(2);
	lastNestThread.setMaxIterCount(mMaxIterCount);
	pageIndex = GetNestPlateTotalNum() - 2;
	lastNestThread.setPageIndex(pageIndex);
	lastNestThread.start();
	// 检查排样是否完成, 用户是否终止
	while (!lastNestThread.isFinished())
	{
		qApp->processEvents();
		// 获取排样进度
		progress.setValue(nestThread.getProgress());
		if (progress.wasCanceled())
		{
			lastNestThread.forceEndNest();
		}
		::_sleep(10);
		//::Sleep(10);
	}
	printf("%d Page: Layout time: %d\n", pageIndex, timer.elapsed());

	// 获取板材数量
	//GetNestPlateNums(&mPageNum);
	mPageNum = GetNestPlateTotalNum();
	QVector<int> plateIndices(mPageNum);
	GetNestPlateIndices(plateIndices.data());
	// 获取零件参数
	QVector< GPart > parts;
	double tsMat[6], cx, cy;
	for (int i = 0; i < mParts.count(); i++)
	{
		for (int j = 0; j < mPartNums[i]; j++)
		{
			int sortIndex;
			int clusterSmallIndex;
			int clusterMaxIndex;
			GetPartNestedData(i, j, &pageIndex, tsMat, &sortIndex, &clusterMaxIndex, &clusterSmallIndex);
			GetPartCenter(i, &cx, &cy);
			parts << GPart();
			GPart &part = parts.last();
			part.mSortIndex = sortIndex;
			part.mGroupIndex = clusterMaxIndex;
			part.mSubGroupIndex = clusterSmallIndex;
			// 获取零件所在板材下标
			part.mPageIndex = pageIndex;

			// 获取零件的零件聚类类别号

			if (part.mPageIndex < 0)
			{
				// 如果板材下标 < 0说明该零件没有排样成功
				parts.removeLast();
				printf("%d, cann't layout\n", i);
				continue;
			}
			// 对零件多边形顶点进行映射
			QMatrix mat(tsMat[0], tsMat[3], tsMat[1], tsMat[4], tsMat[2], tsMat[5]);
			// 坐标映射公式:
			// x = x*tsMat[0] + y*tsMat[1] + tsMat[2]
			// y = x*tsMat[3] + y*tsMat[4] + tsMat[5]
			(QPolygonF &)part = mat.map(mParts[i]);
			part.mCenter = QPointF(cx, cy);//mat.map(QPointF(cx, cy));
			for (int k = 0; k < mParts[i].mHoles.count(); k++)
			{
				part.mHoles << mat.map(mParts[i].mHoles[k]);
			}
		}
	}

	// 销毁自动排样
	DestroyNest();


	ui.mCurveView->setPlates(plates);
	ui.mCurveView->setPlateIndices(plateIndices);
	ui.mCurveView->setPolygons(parts);
	mPageIndex = 0;
	ui.mCurveView->setCurrentPage(0);
	ui.mCurveView->update();
}

void TSimplifyCurveDlg::on_mPrevPageBtn_clicked()
{
	mPageIndex--;
	if (mPageIndex < 0) mPageIndex = mPageNum - 1;
	ui.mCurveView->setCurrentPage(mPageIndex);
	ui.mCurveView->update();
}

void TSimplifyCurveDlg::on_mNextPageBtn_clicked()
{
	mPageIndex++;
	if (mPageIndex >= mPageNum) mPageIndex = 0;
	ui.mCurveView->setCurrentPage(mPageIndex);
	ui.mCurveView->update();
}