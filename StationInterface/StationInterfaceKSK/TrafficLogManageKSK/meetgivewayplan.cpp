#include "meetgivewayplan.h"
#include "ui_meetgivewayplan.h"
#pragma execution_character_set("utf-8")

MeetGiveWayPlan::MeetGiveWayPlan(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MeetGiveWayPlan)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);

    InitTableView();
    pMeetGiveWayTable->SetTableHeadHideV(true);
    pMeetGiveWayTable->SetTableHeadHideH(true);
    pMeetGiveWayTable->CreatTableView();
    ui->widget->layout()->addWidget(pMeetGiveWayTable);
}

MeetGiveWayPlan::~MeetGiveWayPlan()
{
    delete ui;
}

void MeetGiveWayPlan::InitTableView()
{
    pMeetGiveWayTable= new TableView();
    pMeetGiveWayTable->mTableViewInfo.Init();

    pMeetGiveWayTable->mTableViewInfo.Vect_HeaderData.append("车次");
    pMeetGiveWayTable->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    pMeetGiveWayTable->mTableViewInfo.Vect_HeaderData.append("让车");
    pMeetGiveWayTable->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    pMeetGiveWayTable->mTableViewInfo.Vect_HeaderData.append("会车");
    pMeetGiveWayTable->mTableViewInfo.Vect_HeaderDataWidth.append(120);

    pMeetGiveWayTable->SetSectionText(" ", 0);
    pMeetGiveWayTable->SetTableHeadHeight(0, 24);
    pMeetGiveWayTable->SetTableHeadHeight(1, 24);
    pMeetGiveWayTable->SetTableHeadHideV(false);
    //pMeetGiveWayTable->IintTableColAndRow(0, 3);
    SetTableStyleSheet();
}

void MeetGiveWayPlan::SetTableStyleSheet()
{
    pMeetGiveWayTable->SetTableHeadsetStyleSheet(0,
        "QTableView{"
            "outline:none;"
            "gridline-color: rgb(200, 200, 200);"
        "}"
        "QHeaderView::section {"
            "background:rgb(240, 240, 240);"
        "}"
        "QTableView QTableCornerButton::section {"
            "background:rgb(240, 240, 240);"
            "font-size:9pt;"
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
        "}");

    pMeetGiveWayTable->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");


    pMeetGiveWayTable->SetTableHeadsetStyleSheet(2,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:vartical {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}

void MeetGiveWayPlan::on_pushButton_clicked()
{
    emit SendTextAnalysisSignal(123, QCursor::pos());
    this->close();
}
