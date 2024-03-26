#include "qftj.h"
#include "ui_qftj.h"
#include <QAction>
#include <QStringListModel>
#include <QStandardItemModel>
#pragma execution_character_set("utf-8")
QFTJ::QFTJ(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QFTJ)
{
    ui->setupUi(this);
    //this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    //setWindowModality(Qt::ApplicationModal);
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    mCTableViewQFTJ= new TableView(ui->widget_2);
    mCTableViewQFTJ->SetScrollBarV(true);
    mCTableViewQFTJ->mTableViewInfo.Init();
    mCTableViewQFTJ->mTableViewInfo.Vect_HeaderData.append("计数器");
    mCTableViewQFTJ->mTableViewInfo.Vect_HeaderDataWidth.append(150);
    mCTableViewQFTJ->mTableViewInfo.Vect_HeaderData.append("当前值");
    mCTableViewQFTJ->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableViewQFTJ->SetTableHeadHideV(false);
    mCTableViewQFTJ->IintTableColAndRow(15,2);
    mCTableViewQFTJ->SetTableHeadHeight(0,8);
    mCTableViewQFTJ->SetTableHeadHeight(1,19);
    mCTableViewQFTJ->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    mCTableViewQFTJ->SetTableHeadsetStyleSheet(0,"QHeaderView::section {"
                                                "color: black;padding-left: 4px;border: 1px solid #6c6c6c;}");

    mCTableViewQFTJ->CreatTableView();
    mCTableViewQFTJ->SetTableDefaultAlignment(1);
}

QFTJ::~QFTJ()
{
    delete ui;
}
void QFTJ::InitShow(QString strStationName,QVector<QianFeng> QianFengVect)
{
    ui->comboBox->clear();
    ui->comboBox->addItem(strStationName);
    QStringList strList;
    for (int i=0;i<QianFengVect.size();i++)
    {
        strList<<QianFengVect[i].StrName<<QString::number(QianFengVect[i].Counter);
    }
    mCTableViewQFTJ->SetTableData(strList,Qt::AlignLeft);
}
