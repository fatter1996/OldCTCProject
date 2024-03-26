#include "hookorder.h"
#include "ui_hookorder.h"
#pragma execution_character_set("utf-8")
HookOrder::HookOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HookOrder)
{
    ui->setupUi(this);
    InitTableViewHook();
    mCTableViewHook->SetTableHeadHideV(true);
    mCTableViewHook->SetTableHeadHideH(true);
    mCTableViewHook->CreatTableView();
    ui->groupBox->layout()->addWidget(mCTableViewHook);
}

HookOrder::~HookOrder()
{
    delete ui;
}

void HookOrder::InitTableViewHook()
{
    mCTableViewHook= new TableView();
    mCTableViewHook->mTableViewInfo.Init();

    mCTableViewHook->mTableViewInfo.Vect_HeaderData.append("股道");
    mCTableViewHook->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableViewHook->mTableViewInfo.Vect_HeaderData.append("操作");
    mCTableViewHook->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableViewHook->mTableViewInfo.Vect_HeaderData.append("状态");
    mCTableViewHook->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableViewHook->mTableViewInfo.Vect_HeaderData.append("记事");
    mCTableViewHook->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableViewHook->mTableViewInfo.Vect_HeaderData.append("作业时间");
    mCTableViewHook->mTableViewInfo.Vect_HeaderDataWidth.append(60);

    mCTableViewHook->SetSectionText("钩计划", 48);
    mCTableViewHook->SetTableHeadHeight(0, 24);
    mCTableViewHook->SetTableHeadHeight(1, 24);
    mCTableViewHook->IintTableColAndRow(1, 6);
    SetTableStyleSheet();
}

void HookOrder::SetTableStyleSheet()
{
    mCTableViewHook->SetTableHeadsetStyleSheet(0, QString(
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

    mCTableViewHook->SetTableHeadsetStyleSheet(1, QString(
        "QHeaderView{"
            "font-size:%1pt;"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: %1pt;"
            "font-weight: normal;"
        "}").arg(nFontSize));


    mCTableViewHook->SetTableHeadsetStyleSheet(2, QString(
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


void HookOrder::FontReSize(int fontSize, int height, double diploid)
{
    nFontSize = fontSize;
    mCTableViewHook->SetTableHeadHeight(1, height);
    mCTableViewHook->SetTableHeadWidth(0, diploid);
    SetTableStyleSheet();
}

