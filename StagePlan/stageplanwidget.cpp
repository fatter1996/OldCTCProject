#include "stageplanwidget.h"
#include "ui_stageplanwidget.h"
#pragma execution_character_set("utf-8")

StagePlanWidget::StagePlanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StagePlanWidget)
{
    setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    ui->setupUi(this);

}

StagePlanWidget::~StagePlanWidget()
{
    delete ui;
}
void StagePlanWidget::InitTableView()
{
    mCTableView= new TableView();
    ui->widget->layout()->addWidget(mCTableView);
    mCTableView->mTableViewInfo.Init();
    mCTableView->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(32);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("到达车次");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("出发车次");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("调度台");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("类型");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("到达股道");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("到达时间");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("出发股道");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("出发时间");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("签收");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(48);
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(13, 10);

    mCTableView->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    mCTableView->SetTableHeadsetStyleSheet(0,
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

    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");


    mCTableView->SetTableHeadsetStyleSheet(2,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:vartical {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
    mCTableView->CreatTableView();
}

void StagePlanWidget::StagePlanUpdata()
{
    QStringList strList;
    if(!m_pMyStation)
        return;
    for (int i = 0; i < m_pMyStation->m_ArrayStagePlan.size(); i++)
    {
        strList << QString("%1").arg(i + 1)
                << m_pMyStation->m_ArrayStagePlan.at(i)->m_strReachTrainNum
                << m_pMyStation->m_ArrayStagePlan.at(i)->m_strDepartTrainNum
                << m_pMyStation->m_ArrayStagePlan.at(i)->m_strDispatchDesk
                << (m_pMyStation->m_ArrayStagePlan.at(i)->m_btStagePlanKind==0x11?"增加":"删除")
                << (m_pMyStation->m_ArrayStagePlan.at(i)->m_btBeginOrEndFlg == 0xbb ?
                        "" : m_pMyStation->m_ArrayStagePlan.at(i)->m_strRecvTrainTrack)
                << m_pMyStation->m_ArrayStagePlan.at(i)->m_timProvReachStation.toString("hh:mm")
                << (m_pMyStation->m_ArrayStagePlan.at(i)->m_btBeginOrEndFlg == 0xcc ?
                        "" : m_pMyStation->m_ArrayStagePlan.at(i)->m_strDepartTrainTrack)
                << m_pMyStation->m_ArrayStagePlan.at(i)->m_timProvDepaTrain.toString("hh:mm")
                << (m_pMyStation->m_ArrayStagePlan.at(i)->m_nStateSignPlan>0?"已签收":"未签收");
    }
    mCTableView->SetTableData(strList);
    if(m_pMyStation->m_ArrayStagePlan.size() > 0)
    {
        ui->label_4->setText(m_pMyStation->m_ArrayStagePlan.last()->m_timJHRcv.toString("yyyy年MM月dd日 hh时mm分"));
    }
}

void StagePlanWidget::on_pushButton_3_clicked()
{
    emit SendTextAnalysisSignal(123, QCursor::pos());
    this->close();
}

void StagePlanWidget::on_pushButton_clicked()
{
    emit SendTextAnalysisSignal(122, QCursor::pos());
    emit signFor();
}
