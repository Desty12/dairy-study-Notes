/********************************************************************************
** Form generated from reading UI file 'tsimplifycurvedlg.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TSIMPLIFYCURVEDLG_H
#define UI_TSIMPLIFYCURVEDLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include "gcurveview.h"

QT_BEGIN_NAMESPACE

class Ui_TSimplifyCurveDlgClass
{
public:
    QVBoxLayout *verticalLayout;
    GCurveView *mCurveView;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *mSettingBtn;
    QPushButton *mLoadBtn;
    QPushButton *mLayoutBtn;
    QPushButton *mPrevPageBtn;
    QPushButton *mNextPageBtn;

    void setupUi(QDialog *TSimplifyCurveDlgClass)
    {
        if (TSimplifyCurveDlgClass->objectName().isEmpty())
            TSimplifyCurveDlgClass->setObjectName(QStringLiteral("TSimplifyCurveDlgClass"));
        TSimplifyCurveDlgClass->resize(1141, 818);
        verticalLayout = new QVBoxLayout(TSimplifyCurveDlgClass);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mCurveView = new GCurveView(TSimplifyCurveDlgClass);
        mCurveView->setObjectName(QStringLiteral("mCurveView"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(mCurveView->sizePolicy().hasHeightForWidth());
        mCurveView->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(mCurveView);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        mSettingBtn = new QPushButton(TSimplifyCurveDlgClass);
        mSettingBtn->setObjectName(QStringLiteral("mSettingBtn"));

        horizontalLayout->addWidget(mSettingBtn);

        mLoadBtn = new QPushButton(TSimplifyCurveDlgClass);
        mLoadBtn->setObjectName(QStringLiteral("mLoadBtn"));

        horizontalLayout->addWidget(mLoadBtn);

        mLayoutBtn = new QPushButton(TSimplifyCurveDlgClass);
        mLayoutBtn->setObjectName(QStringLiteral("mLayoutBtn"));

        horizontalLayout->addWidget(mLayoutBtn);

        mPrevPageBtn = new QPushButton(TSimplifyCurveDlgClass);
        mPrevPageBtn->setObjectName(QStringLiteral("mPrevPageBtn"));

        horizontalLayout->addWidget(mPrevPageBtn);

        mNextPageBtn = new QPushButton(TSimplifyCurveDlgClass);
        mNextPageBtn->setObjectName(QStringLiteral("mNextPageBtn"));

        horizontalLayout->addWidget(mNextPageBtn);


        verticalLayout->addLayout(horizontalLayout);

        QWidget::setTabOrder(mLoadBtn, mLayoutBtn);
        QWidget::setTabOrder(mLayoutBtn, mPrevPageBtn);
        QWidget::setTabOrder(mPrevPageBtn, mNextPageBtn);
        QWidget::setTabOrder(mNextPageBtn, mSettingBtn);

        retranslateUi(TSimplifyCurveDlgClass);

        QMetaObject::connectSlotsByName(TSimplifyCurveDlgClass);
    } // setupUi

    void retranslateUi(QDialog *TSimplifyCurveDlgClass)
    {
        TSimplifyCurveDlgClass->setWindowTitle(QApplication::translate("TSimplifyCurveDlgClass", "TSimplifyCurveDlg", Q_NULLPTR));
        mSettingBtn->setText(QApplication::translate("TSimplifyCurveDlgClass", "Setting", Q_NULLPTR));
        mLoadBtn->setText(QApplication::translate("TSimplifyCurveDlgClass", "Load", Q_NULLPTR));
        mLayoutBtn->setText(QApplication::translate("TSimplifyCurveDlgClass", "Layout", Q_NULLPTR));
        mPrevPageBtn->setText(QApplication::translate("TSimplifyCurveDlgClass", "<<", Q_NULLPTR));
        mNextPageBtn->setText(QApplication::translate("TSimplifyCurveDlgClass", ">>", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TSimplifyCurveDlgClass: public Ui_TSimplifyCurveDlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TSIMPLIFYCURVEDLG_H
