#include "shuntrouteorder.h"
#include "ui_shuntrouteorder.h"
#pragma execution_character_set("utf-8")

ShuntRouteOrder::ShuntRouteOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShuntRouteOrder)
{
    ui->setupUi(this);
    InitTableViewShunt();
    mCTableViewShunt->SetTableHeadHideV(true);
    mCTableViewShunt->SetTableHeadHideH(true);
    mCTableViewShunt->CreatTableView();
    ui->groupBox->layout()->addWidget(mCTableViewShunt);
}

ShuntRouteOrder::~ShuntRouteOrder()
{
    delete ui;
}

void ShuntRouteOrder::InitTableViewShunt()
{
    mCTableViewShunt= new TableView(this);
    mCTableViewShunt->mTableViewInfo.Init();
    mCTableViewShunt->mTableViewInfo.Vect_HeaderData.append("勾序");
    mCTableViewShunt->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableViewShunt->mTableViewInfo.Vect_HeaderData.append("进路及方向");
    mCTableViewShunt->mTableViewInfo.Vect_HeaderDataWidth.append(96);
    mCTableViewShunt->mTableViewInfo.Vect_HeaderData.append("自触");
    mCTableViewShunt->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableViewShunt->mTableViewInfo.Vect_HeaderData.append("状态");
    mCTableViewShunt->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableViewShunt->mTableViewInfo.Vect_HeaderData.append("触发时间");
    mCTableViewShunt->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableViewShunt->mTableViewInfo.Vect_HeaderData.append("占用时间");
    mCTableViewShunt->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableViewShunt->SetSectionText("顺序", 48);
    mCTableViewShunt->SetTableHeadHeight(0, 24);
    mCTableViewShunt->SetTableHeadHeight(1, 24);
    mCTableViewShunt->IintTableColAndRow(1, 6);
    SetTableStyleSheet();

}

void ShuntRouteOrder::SetTableStyleSheet()
{
    mCTableViewShunt->SetTableHeadsetStyleSheet(0, QString(
        "QTableView{"
            "outline:none;"
            "gridline-color: rgb(200, 200, 200);"
            "font-size:%1pt;"
        "}"
        "QHeaderView::section {"
            "background:rgb(240, 240, 240);"
        "}"
        "QTableView QTableCornerButton::section {"
            "background:rgb(240, 240, 240);"
            "font-size:%1pt;"
            "font-family:微软雅黑;"
            "border-top:2px outset rgb(240, 240, 240);"
            "border-left:2px outset rgb(240, 240, 240);"
            "border-bottom:2px outset rgb(80, 80, 80);"
            "border-right:2px outset rgb(80, 80, 80);"
        "}"
        "QTableView QTableCornerButton::section:Pressed {"
            "background:rgb(240, 240, 240);"
            "border-bottom:2px outset rgb(240, 240, 240);"
            "border-right:2px outset rgb(240, 240, 240);"
            "border-top:2px outset rgb(80, 80, 80);"
            "border-left:2px outset rgb(80, 80, 80);"
        "}"
        "QTableView::item{"
            "border:none;"
            "outline:none;"
            "background:rgb(250, 250, 250);"
        "}"
        "QTableView::item:selected {"
            "outline:none;"
            "background:rgb(5, 125, 255);"
        "}").arg(nFontSize));

    mCTableViewShunt->SetTableHeadsetStyleSheet(1, QString(
        "QHeaderView{"
            "font-size:%1pt;"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: %1pt;"
            "font-weight: normal;"
        "}").arg(nFontSize));


    mCTableViewShunt->SetTableHeadsetStyleSheet(2, QString(
        "QHeaderView{"
            "font-size: %1pt;"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:vartical {"
            "background:rgb(240, 240, 240);"
            "font-size: %1pt;"
            "font-weight: normal;"
        "}").arg(nFontSize));
}

void ShuntRouteOrder::FontReSize(int fontSize, int height, double diploid)
{
    nFontSize = fontSize;
    mCTableViewShunt->SetTableHeadHeight(1, height);
    mCTableViewShunt->SetTableHeadWidth(0, diploid);
    SetTableStyleSheet();
}
