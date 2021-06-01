/********************************************************************************
** Form generated from reading UI file 'tsettingdlg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TSETTINGDLG_H
#define UI_TSETTINGDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TSettingDlg
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QCheckBox *mOOBBRotCBox;
    QLabel *label_2;
    QLabel *label_3;
    QCheckBox *mRot90CBox;
    QCheckBox *mRot180CBox;
    QCheckBox *mRot270CBox;
    QDoubleSpinBox *mWidthSBox;
    QDoubleSpinBox *mDeltaSBox;
    QLabel *label;
    QDoubleSpinBox *mHeightSBox;
    QLabel *label_4;
    QSpinBox *mMaxIterSBox;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *mOkBtn;
    QPushButton *mCancelBtn;

    void setupUi(QDialog *TSettingDlg)
    {
        if (TSettingDlg->objectName().isEmpty())
            TSettingDlg->setObjectName(QStringLiteral("TSettingDlg"));
        TSettingDlg->resize(346, 177);
        verticalLayout = new QVBoxLayout(TSettingDlg);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        mOOBBRotCBox = new QCheckBox(TSettingDlg);
        mOOBBRotCBox->setObjectName(QStringLiteral("mOOBBRotCBox"));

        gridLayout->addWidget(mOOBBRotCBox, 3, 0, 1, 1);

        label_2 = new QLabel(TSettingDlg);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_3 = new QLabel(TSettingDlg);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        mRot90CBox = new QCheckBox(TSettingDlg);
        mRot90CBox->setObjectName(QStringLiteral("mRot90CBox"));

        gridLayout->addWidget(mRot90CBox, 3, 1, 1, 1);

        mRot180CBox = new QCheckBox(TSettingDlg);
        mRot180CBox->setObjectName(QStringLiteral("mRot180CBox"));

        gridLayout->addWidget(mRot180CBox, 3, 2, 1, 1);

        mRot270CBox = new QCheckBox(TSettingDlg);
        mRot270CBox->setObjectName(QStringLiteral("mRot270CBox"));

        gridLayout->addWidget(mRot270CBox, 3, 3, 1, 1);

        mWidthSBox = new QDoubleSpinBox(TSettingDlg);
        mWidthSBox->setObjectName(QStringLiteral("mWidthSBox"));
        mWidthSBox->setDecimals(1);
        mWidthSBox->setMinimum(1);
        mWidthSBox->setMaximum(100000);
        mWidthSBox->setValue(3000);

        gridLayout->addWidget(mWidthSBox, 0, 1, 1, 3);

        mDeltaSBox = new QDoubleSpinBox(TSettingDlg);
        mDeltaSBox->setObjectName(QStringLiteral("mDeltaSBox"));
        mDeltaSBox->setDecimals(3);
        mDeltaSBox->setMinimum(0.1);
        mDeltaSBox->setMaximum(100000);
        mDeltaSBox->setValue(5);

        gridLayout->addWidget(mDeltaSBox, 2, 1, 1, 3);

        label = new QLabel(TSettingDlg);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        mHeightSBox = new QDoubleSpinBox(TSettingDlg);
        mHeightSBox->setObjectName(QStringLiteral("mHeightSBox"));
        mHeightSBox->setDecimals(1);
        mHeightSBox->setMinimum(1);
        mHeightSBox->setMaximum(100000);
        mHeightSBox->setValue(2000);

        gridLayout->addWidget(mHeightSBox, 1, 1, 1, 3);

        label_4 = new QLabel(TSettingDlg);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        mMaxIterSBox = new QSpinBox(TSettingDlg);
        mMaxIterSBox->setObjectName(QStringLiteral("mMaxIterSBox"));
        mMaxIterSBox->setMinimum(0);
        mMaxIterSBox->setMaximum(100000);
        mMaxIterSBox->setValue(1000);

        gridLayout->addWidget(mMaxIterSBox, 4, 1, 1, 3);


        verticalLayout->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 7, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        mOkBtn = new QPushButton(TSettingDlg);
        mOkBtn->setObjectName(QStringLiteral("mOkBtn"));

        horizontalLayout->addWidget(mOkBtn);

        mCancelBtn = new QPushButton(TSettingDlg);
        mCancelBtn->setObjectName(QStringLiteral("mCancelBtn"));

        horizontalLayout->addWidget(mCancelBtn);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(mWidthSBox, mHeightSBox);
        QWidget::setTabOrder(mHeightSBox, mDeltaSBox);
        QWidget::setTabOrder(mDeltaSBox, mOkBtn);
        QWidget::setTabOrder(mOkBtn, mCancelBtn);

        retranslateUi(TSettingDlg);

        QMetaObject::connectSlotsByName(TSettingDlg);
    } // setupUi

    void retranslateUi(QDialog *TSettingDlg)
    {
        TSettingDlg->setWindowTitle(QApplication::translate("TSettingDlg", "\346\257\215\346\235\220\350\256\276\347\275\256", Q_NULLPTR));
        mOOBBRotCBox->setText(QApplication::translate("TSettingDlg", "\344\270\273\350\275\264\346\227\213\350\275\254", Q_NULLPTR));
        label_2->setText(QApplication::translate("TSettingDlg", "\346\257\215\346\235\220\351\253\230\345\272\246", Q_NULLPTR));
        label_3->setText(QApplication::translate("TSettingDlg", "\346\216\222\347\211\210\347\275\221\346\240\274\351\227\264\350\267\235", Q_NULLPTR));
        mRot90CBox->setText(QApplication::translate("TSettingDlg", "90", Q_NULLPTR));
        mRot180CBox->setText(QApplication::translate("TSettingDlg", "180", Q_NULLPTR));
        mRot270CBox->setText(QApplication::translate("TSettingDlg", "270", Q_NULLPTR));
        label->setText(QApplication::translate("TSettingDlg", "\346\257\215\346\235\220\345\256\275\345\272\246", Q_NULLPTR));
        label_4->setText(QApplication::translate("TSettingDlg", "\350\277\255\344\273\243\344\274\230\345\214\226\346\254\241\346\225\260", Q_NULLPTR));
        mOkBtn->setText(QApplication::translate("TSettingDlg", "\347\241\256\345\256\232(&O)", Q_NULLPTR));
        mCancelBtn->setText(QApplication::translate("TSettingDlg", "\345\217\226\346\266\210(&C)", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TSettingDlg: public Ui_TSettingDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TSETTINGDLG_H
