#include "tsettingdlg.h"
#include "gautolayout.h"

TSettingDlg::TSettingDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

TSettingDlg::~TSettingDlg()
{

}

double TSettingDlg::getWidth()
{
	return ui.mWidthSBox->value();
}

double TSettingDlg::getHeight()
{
	return ui.mHeightSBox->value();
}

double TSettingDlg::getDelta()
{
	return ui.mDeltaSBox->value();
}

bool TSettingDlg::isOOBBRot()
{
	return ui.mOOBBRotCBox->isChecked();
}

quint32 TSettingDlg::getRotFlag()
{
	quint32 flag = 0;

	if (ui.mRot90CBox->checkState() == Qt::Checked) flag |= LAYOUT_OPT_ROTATE90;
	if (ui.mRot180CBox->checkState() == Qt::Checked) flag |= LAYOUT_OPT_ROTATE180;
	if (ui.mRot270CBox->checkState() == Qt::Checked) flag |= LAYOUT_OPT_ROTATE270;

	return flag;
}

int TSettingDlg::getMaxIterCount()
{
	return ui.mMaxIterSBox->value();
}

void TSettingDlg::setWidth(double w)
{
	ui.mWidthSBox->setValue(w);
}

void TSettingDlg::setHeight(double h)
{
	ui.mHeightSBox->setValue(h);
}

void TSettingDlg::setDelta(double delta)
{
	ui.mDeltaSBox->setValue(delta);
}

void TSettingDlg::setOOBBRot(bool isRot)
{
	ui.mOOBBRotCBox->setChecked(isRot);
}

void TSettingDlg::setRotFlag(quint32 flag)
{
	if (flag & LAYOUT_OPT_ROTATE90) ui.mRot90CBox->setCheckState(Qt::Checked);
	else ui.mRot90CBox->setCheckState(Qt::Unchecked);

	if (flag & LAYOUT_OPT_ROTATE180) ui.mRot180CBox->setCheckState(Qt::Checked);
	else ui.mRot180CBox->setCheckState(Qt::Unchecked);

	if (flag & LAYOUT_OPT_ROTATE270) ui.mRot270CBox->setCheckState(Qt::Checked);
	else ui.mRot270CBox->setCheckState(Qt::Unchecked);
}

void TSettingDlg::setMaxIterCount(int iterCount)
{
	ui.mMaxIterSBox->setValue(iterCount);
}

void TSettingDlg::on_mOkBtn_clicked()
{
	accept();
}

void TSettingDlg::on_mCancelBtn_clicked()
{
	reject();
}
