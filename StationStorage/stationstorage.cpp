#include "stationstorage.h"
#include "ui_stationstorage.h"
#include <QDebug>
#pragma execution_character_set("utf-8")
StationStorage::StationStorage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationStorage)
{
    ui->setupUi(this);
    Init();

    connect(ui->pushButton_GDEdit, SIGNAL(clicked()), this, SLOT(pushButton1_Clicked()));

}

StationStorage::~StationStorage()
{
    delete ui;
}
void StationStorage::Init()
{
    m_ccxnameedit= new ccxnameedit();
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_7->layout()->addWidget(mCTableView);

    InitTableView2();
    mCTableView2->SetTableHeadHideV(false);
    mCTableView2->SetTableHeadHideH(true);
    mCTableView2->CreatTableView();
    ui->widget_8->layout()->addWidget(mCTableView2);

    InitTableView3();
    mCTableView3->SetTableHeadHideV(false);
    mCTableView3->SetTableHeadHideH(true);
    mCTableView3->CreatTableView();
    ui->widget_9->layout()->addWidget(mCTableView3);
}
void StationStorage::pushButton1_Clicked()
{
    m_ccxnameedit->show();
}
void StationStorage::InitTableView()
{
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();

    mCTableView->mTableViewInfo.Vect_HeaderData.append("股道名称");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("现在存车信息");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("小计");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(0, 3);
    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}
void StationStorage::InitTableView2()
{
    mCTableView2= new TableView();
    mCTableView2->mTableViewInfo.Init();

    mCTableView2->mTableViewInfo.Vect_HeaderData.append("车种");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("空");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("重");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(60);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView2->SetTableHeadHeight(0, 24);
    mCTableView2->SetTableHeadHeight(1, 16);
    mCTableView2->IintTableColAndRow(0, 3);
    mCTableView2->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");


}
void StationStorage::InitTableView3()
{
    mCTableView3= new TableView();
    mCTableView3->mTableViewInfo.Init();

    mCTableView3->mTableViewInfo.Vect_HeaderData.append("车厢类型");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView3->mTableViewInfo.Vect_HeaderData.append("辆数");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView3->mTableViewInfo.Vect_HeaderData.append("说明/重车去向");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableView3->mTableViewInfo.Vect_HeaderData.append("运用标记");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(80);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView3->SetTableHeadHeight(0, 24);
    mCTableView3->SetTableHeadHeight(1, 16);
    mCTableView3->IintTableColAndRow(0, 4);
    mCTableView3->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");

}
