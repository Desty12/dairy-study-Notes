#ifndef TSETTINGDLG_H
#define TSETTINGDLG_H

#include <QDialog>
#include "ui_tsettingdlg.h"

class TSettingDlg : public QDialog
{
	Q_OBJECT

public:
	TSettingDlg(QWidget *parent = 0);
	~TSettingDlg();

	double getWidth();

	double getHeight();

	double getDelta();

	bool isOOBBRot();

	quint32 getRotFlag();

	int getMaxIterCount();

	void setWidth(double w);

	void setHeight(double h);

	void setDelta(double delta);

	void setOOBBRot(bool isRot);

	void setRotFlag(quint32 flag);

	void setMaxIterCount(int iterCount);

private:
	Ui::TSettingDlg ui;

private slots:
	void on_mOkBtn_clicked();

	void on_mCancelBtn_clicked();
};

#endif // TSETTINGDLG_H
