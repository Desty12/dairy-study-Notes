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

	double			mMatWidth;		// ���Ͽ��
	double			mMatHeight;		// ���ϸ߶�
	double			mMatGridDelta;	// ����������
	bool			mIsOOBBRot;		// �Ƿ�������ת
	quint32			mRotFlag;
	int				mMaxIterCount;	// ����������

	QVector< GPart > mParts;		// ����α�
	QVector< int > mPartNums;		// �������
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
