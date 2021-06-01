#ifndef TSIMPLIFYCURVEDLG_H
#define TSIMPLIFYCURVEDLG_H

#include <QDialog>
#include "ui_tsimplifycurvedlg.h"
#include "gautolayout.h"

class TSimplifyCurveDlg : public QDialog
{
	Q_OBJECT

public:
	TSimplifyCurveDlg(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
	~TSimplifyCurveDlg();

private:
	Ui::TSimplifyCurveDlgClass ui;
	QPolygonF		mPoints;
	QVector<int>	mSegmentIndices;
	bool			mIsClosed;

	double			mMatWidth;		// 材料宽度
	double			mMatHeight;		// 材料高度
	double			mMatGridDelta;	// 材料网格间距
	bool			mIsOOBBRot;		// 是否任意旋转
	quint32			mRotFlag;
	int				mMaxIterCount;	// 最大迭代次数

	QVector< GPart > mParts;		// 多边形表
	QVector< int > mPartNums;		// 零件数量
	int				mPageNum;
	int				mPageIndex;

private slots:
	void on_mLoadBtn_clicked();

	void on_mSettingBtn_clicked();

	void on_mLayoutBtn_clicked();

	void on_mPrevPageBtn_clicked();

	void on_mNextPageBtn_clicked();
};

#endif // TSIMPLIFYCURVEDLG_H
