#include "sendplanwidget.h"
#include "ui_sendplanwidget.h"

SendPlanWidget::SendPlanWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendPlanWidget)
{
    setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    ui->setupUi(this);
}

SendPlanWidget::~SendPlanWidget()
{
    delete ui;
}

void SendPlanWidget::InitTableView()
{
    mCTableView = new TableView();
    ui->frame->layout()->addWidget(mCTableView);
    mCTableView->mTableViewInfo.Init();
    mCTableView->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(32);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("车次");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("类型");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(72);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("方向");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("到达股道");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("到达时刻");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("去向");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("出发股道");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("出发时间");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("属性");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(160);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("注意事项");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 24);
    mCTableView->IintTableColAndRow(16, 11);

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

void SendPlanWidget::InitTableView2()
{
    mCTableView2 = new TableView();
    ui->frame_2->layout()->addWidget(mCTableView2);
    mCTableView2->mTableViewInfo.Init();
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(32);
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("受令单位");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(72);
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("状态");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableView2->SetTableHeadHideV(false);
    mCTableView2->SetTableHeadHeight(0, 24);
    mCTableView2->SetTableHeadHeight(1, 24);
    mCTableView2->IintTableColAndRow(16, 3);

    mCTableView2->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    mCTableView2->SetTableHeadsetStyleSheet(0,
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

    mCTableView2->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");


    mCTableView2->SetTableHeadsetStyleSheet(2,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:vartical {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
    mCTableView2->CreatTableView();
}

void SendPlanWidget::SendPlanUpdata()
{
    QStringList strList;
    if(!m_pMyStation)
        return;
    for (int i = 0; i < m_pMyStation->m_ArrayTrafficLog.size(); i++)
    {
        if(m_pMyStation->m_ArrayTrafficLog.at(i)->m_bDelete || m_pMyStation->m_ArrayTrafficLog.at(i)->m_bDeleteFlag)
        {
            strList << QString("%1").arg(i + 1) << m_pMyStation->m_ArrayTrafficLog.at(i)->m_strTrainNum
                    << "删除" << "删除" << "删除" << "删除" << "删除" << "删除" << "删除" << "删除" << "";
            continue;
        }
        strList << QString("%1").arg(i + 1)
                << m_pMyStation->m_ArrayTrafficLog.at(i)->m_strTrainNum

                << ((m_pMyStation->m_ArrayTrafficLog.at(i)->m_bDelete ||
                     m_pMyStation->m_ArrayTrafficLog.at(i)->m_bDeleteFlag) ?
                     "删除" : "添加")

                << (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xbb ?
                        "" : m_pMyStation->m_ArrayTrafficLog.at(i)->m_strFromAdjtStation)
                << (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xbb ?
                    "" : m_pMyStation->m_ArrayTrafficLog.at(i)->m_strRecvTrainTrack)
                //<< (m_pMyStation->m_ArrayTrafficLog.at(i)->m_strRecvTrainTrack == "未知" ? "" :
                //    m_pMyStation->m_ArrayTrafficLog.at(i)->m_strRecvTrainTrack)

                << (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xbb ? "始发" :
                   (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xdd ? "通过" :
                   (m_pMyStation->m_ArrayTrafficLog.at(i)->m_timRealReachStation.isValid() ?
                    m_pMyStation->m_ArrayTrafficLog.at(i)->m_timRealReachStation.toString("hh:mm") :
                    m_pMyStation->m_ArrayTrafficLog.at(i)->m_timProvReachStation.toString("hh:mm"))))

                << (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xcc ?
                    "" : m_pMyStation->m_ArrayTrafficLog.at(i)->m_strToAdjtStation)
                //<< (m_pMyStation->m_ArrayTrafficLog.at(i)->m_strDepartTrainTrack == "未知" ?
                //    "" : m_pMyStation->m_ArrayTrafficLog.at(i)->m_strDepartTrainTrack)
                << (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xcc ?
                    "" : m_pMyStation->m_ArrayTrafficLog.at(i)->m_strDepartTrainTrack)
                << (m_pMyStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg == 0xcc ? "终到" :
                   (m_pMyStation->m_ArrayTrafficLog.at(i)->m_timRealDepaTrain.isValid() ?
                    m_pMyStation->m_ArrayTrafficLog.at(i)->m_timRealDepaTrain.toString("hh:mm") :
                    m_pMyStation->m_ArrayTrafficLog.at(i)->m_timProvDepaTrain.toString("hh:mm")))

                << QString(m_pMyStation->m_ArrayTrafficLog.at(i)->m_nLevelCX ? "/超限": "") +
                   QString(m_pMyStation->m_ArrayTrafficLog.at(i)->m_bElectric ? "/电力": "") +
                   QString(m_pMyStation->m_ArrayTrafficLog.at(i)->m_bImportant ? "/重点": "") +
                   QString(m_pMyStation->m_ArrayTrafficLog.at(i)->m_bBLKY ? "/客运": "") +
                   QString(m_pMyStation->m_ArrayTrafficLog.at(i)->m_bArmy ? "/军运": "")
                << m_pMyStation->m_ArrayTrafficLog.at(i)->m_strNotes;
    }
    mCTableView->SetTableData(strList);

    if(m_pMyStation)
        mCTableView2->SetTableData(QStringList() << QString("1") << m_pMyStation->getStationName() << "");
}

void SendPlanWidget::on_pushButton_2_clicked()
{
    emit sendPlan();
    this->close();
}

void SendPlanWidget::on_pushButton_clicked()
{
    this->close();
}
