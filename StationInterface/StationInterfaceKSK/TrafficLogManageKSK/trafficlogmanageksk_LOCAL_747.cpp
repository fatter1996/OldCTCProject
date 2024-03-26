#include "trafficlogmanageksk.h"
#include "ui_trafficlogmanageksk.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")

TrafficLogManageKSK::TrafficLogManageKSK(MyStation* pMyStation, QWidget *parent) :
    TrafficLogManage(parent),
    ui(new Ui::TrafficLogManageKSK)
{
    ui->setupUi(this);
    pCurStation = pMyStation;
    Init();
    InitFromListHead();
    pColumnDisplaySet = new ColumnDisplaySetWidget(this);
    connect(pColumnDisplaySet, &ColumnDisplaySetWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    connect(pColumnDisplaySet, &ColumnDisplaySetWidget::ShowStateChange, [=](){
        QTableWidget* tableHHead = mQFormList->getTableWidgetFormType(0x11);
        QTableWidget* tableData = mQFormList->getTableWidgetFormType(0x33);

        pColumnDisplaySet->SetShowState(tableHHead);
        pColumnDisplaySet->SetShowState(tableData);

        mQFormList->headerHorIndexReSet();
    });
    ui->dateLabel->setText(QDateTime::currentDateTime().toString("yyyy年 MM月dd日 dddd"));

    pColumnDisplaySet->InitMyTree(pColTreeRootNode);
    //邻站预告
    connect(ui->previewBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(152, QCursor::pos());
        AdjacentStationPreview();
    });
    //同意
    connect(ui->agreeBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(153, QCursor::pos());
        AgreeAdjacentStations();
    });
    //到达
    connect(ui->arrivalBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(154, QCursor::pos());
        TrainArrival();
    });
    //出发
    connect(ui->departureBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(155, QCursor::pos());
        TrainDeparture();
    });
    //通过
    connect(ui->passBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(156, QCursor::pos());
        TrainPassThrough();
    });
    //邻站
    connect(ui->adjacentBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(157, QCursor::pos());
        AdjacentStation();
    });
    //取消接车
    connect(ui->cancelArriveBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(158, QCursor::pos());
        CancelReceiving();
    });
    //取消闭塞
    connect(ui->cancelBlockBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(159, QCursor::pos());
        CancelBlock();
    });
    //取消发车
    connect(ui->cancelDepartBtn, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(160, QCursor::pos());
        CancelDepart();
    });

    connect(this, &TrafficLogManageKSK::startFlicker, [=](TrafficLog* pTrafficLog)
    {
        pTrafficLog->m_nTimerId = startTimer(200);
    });

    connect(this, &TrafficLogManageKSK::stopFlicker, [=](TrafficLog* pTrafficLog, int index)
    {
        killTimer(pTrafficLog->m_nTimerId);
        pTrafficLog->m_nTimerId = -1;
        pTrafficLog->m_bFlickerFlag = false;
        mQFormList->setCellBackgroundColor(0x22, index, 0, QColor(240, 240, 240));
    });
}

TrafficLogManageKSK::~TrafficLogManageKSK()
{
    delete ui;
}

void TrafficLogManageKSK::Init()
{
    mQFormList = new QFormList(ui->widget_11);
    mQFormList->setGeometry(0,0,ui->widget_11->width(),ui->widget_11->height());
    QHBoxLayout *layout=new QHBoxLayout();
    layout->setDirection(QBoxLayout::LeftToRight);
    layout->addWidget(mQFormList);
    ui->widget_11->setLayout(layout);
    ui->label_9->setText(QString("中间站行车日志[%1]").arg(pCurStation->m_strStationName));
    InitTrafficLogMenu();
    InitTimeMenu();
    QTableWidget* tableVHead = mQFormList->getTableWidgetFormType(0x22);
    QTableWidget* tableData = mQFormList->getTableWidgetFormType(0x33);

    connect(tableVHead, &QTableWidget::itemClicked, [=](QTableWidgetItem* item){
        //tableData->selectRow(item->row());
        if(lastSelectedRow != -1)
            mQFormList->setRowBackgroundColor(0x33, lastSelectedRow, QColor(240, 240, 240));
        mQFormList->setRowBackgroundColor(0x33, item->row(), QColor(5, 125, 255));
        lastSelectedRow = item->row();
        for(int i = 0; i < verHeraldItemInfo.size(); i++)
        {
            for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
            {
                if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
                {
                    mQFormList->setCellBackgroundColor(0x33, j, verHeraldItemInfo.at(i).heraldCol, verHeraldItemInfo.at(i).color);
                }
            }
        }


        for(int i = 0; i < tableData->selectedItems().size(); i++)
        {
            tableData->selectedItems().at(i)->setSelected(false);
        }

        if(pCurStation && item->row() < pCurStation->m_ArrayTrafficLog.size())
        {
            if(item->isSelected() && selectedTrainNo == item->text() && selectedTrainNoIndex == item->row())
            {
                TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
                if(log->m_bDeleteFlag)
                    deleteFlagAction->setChecked(true);
                else deleteFlagAction->setChecked(false);

                if(log->m_bAllowGDNotMatch)
                    allowGDNotMatch->setChecked(true);
                else allowGDNotMatch->setChecked(false);

                if(log->m_bAllowCRKNotMatch)
                    allowCRKNotMatch->setChecked(true);
                else allowCRKNotMatch->setChecked(false);
                pTraffclogMenu->exec(QCursor::pos());
            }
            else
            {
                selectedTrainNo = item->text();
                selectedTrainNoIndex = item->row();
            }
        }
        else
        {
            selectedTrainNo.clear();
            selectedTrainNoIndex = -1;
        }
    });
    connect(tableData, &QTableWidget::itemClicked, [=](QTableWidgetItem* item){
        selectedTrainNo.clear();
        selectedTrainNoIndex = -1;
        if(lastSelectedRow != -1)
            mQFormList->setRowBackgroundColor(0x33, lastSelectedRow, QColor(240, 240, 240));
        for(int i = 0; i < verHeraldItemInfo.size(); i++)
        {
            for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
            {
                if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
                {
                    mQFormList->setCellBackgroundColor(0x33, j, verHeraldItemInfo.at(i).heraldCol, verHeraldItemInfo.at(i).color);
                }
            }
        }

        if(item->background() != QColor(240, 240, 240))
        {
            mQFormList->setCellBackgroundColor(0x22, item->row(), item->column(), QColor(240, 240, 240));
        }
        QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->selectedItems();
        for(int i = 0; i < items.size(); i++)
        {
            items.at(i)->setSelected(false);
        }
    });
    connect(tableData, &QTableWidget::cellDoubleClicked, [=](int row, int col){
        QTableWidgetItem *item = tableData->item(row, col);
        if(item)
        {
//            qDebug() << "cellDoubleClicked";

            if(pCurStation )
            {
                selectedTrainNo.clear();
                selectedTrainNoIndex = -1;
                QStringList itemStrList;
                if(item->column() == 3 || item->column() == 24) //股道
                {
                    for(int i = 0; i < pCurStation->DevArray.size(); i++)
                    {
                        CGD* pGD=(CGD*)(pCurStation->DevArray[i]);
                        if(pGD->m_nGDType == GD_QD)
                        {
                            if(pGD->pCenter.x() < 0 || pGD->pCenter.y() < 0)
                            {
                                continue;
                            }
                            itemStrList.append(pGD->m_strName);
                        }
                    }
                }
                else if(item->column() == 2 || item->column() == 25) //信号机
                {
                    for(int i = 0; i < pCurStation->DevArray.size(); i++)
                    {
                        if(pCurStation->DevArray[i]->getDevType() == Dev_XH)
                        {
                            CXHD* pXHD=(CXHD*)(pCurStation->DevArray[i]);
                            if(pXHD->pCenter.x() < 0 || pXHD->pCenter.y() < 0)
                            {
                                continue;
                            }
                            if(pXHD->getXHDType() == JZ_XHJ)
                            {
                                itemStrList.append(pXHD->m_strName);
                            }
                        }
                    }
                }
                else if(item->column() == 45) //超时原因
                {
                    itemStrList << " " << "机务" << "车务" << "电务" << "工务" << "其他";
                }
                else if(item->column() >= 48 && item->column() <= 63) //任务
                {
                    switch (item->column()) {
                    case 48: itemStrList << " " << "列检"; break;
                    case 49: itemStrList << " " << "交令"; break;
                    case 50: itemStrList << " " << "交票"; break;
                    case 51: itemStrList << " " << "列尾"; break;
                    case 52: itemStrList << " " << "机车"; break;
                    case 53: itemStrList << " " << "货检"; break;
                    case 54: itemStrList << " " << "乘降"; break;
                    case 55: itemStrList << " " << "上水"; break;
                    case 56: itemStrList << " " << "摘挂"; break;
                    case 57: itemStrList << " " << "换乘"; break;
                    case 58: itemStrList << " " << "装卸"; break;
                    case 59: itemStrList << " " << "吸污"; break;
                    case 60: itemStrList << " " << "道口"; break;
                    case 61: itemStrList << " " << "车号"; break;
                    case 62: itemStrList << " " << "站务"; break;
                    case 63: itemStrList << " " << "综控"; break;
                    default: break;
                    }
                }
                else if(item->column() == 64) //超限
                {
                    itemStrList << " " << "超级" << "一级" << "二级" << "三级";
                }
                else return;
                mQFormList->AddComboBox(0x33, item->row(), item->column(), itemStrList, item->text());
            }
        }
    });

    tableData->setContextMenuPolicy (Qt::CustomContextMenu);
    connect(tableData, &QTableWidget::customContextMenuRequested, [=](QPoint pt)
    {
        QTableWidgetItem* item = tableData->itemAt(pt);
        if(item && (item->column() == 7 || item->column() == 28))
        {
//            qDebug() << item->row() << item->column();
            pTimeMenuItem = item;
            pTimeMenu->exec(QCursor::pos());
        }
    });

    //表格单元格数据发生更改
    connect(mQFormList, &QFormList::tableWidgetDataChanged, [=](int type, QTableWidgetItem* item){
        if(type == 0x22)
        {
            if(item->row() < pCurStation->m_ArrayTrafficLog.size())
            {
                TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[item->row()];
                if(!traffic)
                    return;
                traffic->m_bNewTrafficLog = false;
            }
        }

        if(type == 0x33)
        {

            if(item->row() < pCurStation->m_ArrayTrafficLog.size())
            {
                TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[item->row()];
                if(traffic == nullptr)
                {
                    return;
                }
                if(traffic->m_bNewTrafficLog)
                {
                    if(item->column() == nCol - 1)
                    {
                        traffic->m_bNewTrafficLog = false;
                    }
                    return;
                }
                mQFormList->getTableWidgetFormType(0x33)->blockSignals(true);
                if(item && item->row() < pCurStation->m_ArrayTrafficLog.size())
                {
                    TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[item->row()];
                    if(traffic == nullptr)
                    {
                        return;
                    }

                    if(item->column() == 3 || item->column() == 24) //股道防错办
                    {
                        CGD* pGD = nullptr;
                        for(int i = 0; i < pCurStation->vectGDAttr.size(); i++)
                        {
                            pGD = (CGD*)(pCurStation->vectGDAttr[i]);
                            if(pGD->m_strName == item->text())
                                break;
                        }
                        if(!pGD) return;

                        if(pGD->jfcAttr == traffic->m_nLHFlg) //客货股道属性不符
                        {
                            //QMessageBox::information(this, tr("STPC"), QString("客货股道属性不符!\r\n不能选择股道%1!").arg(pGD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                            QString str = ((item->column() == 3)?traffic->m_strRecvTrainTrack : traffic->m_strDepartTrainTrack);
                            item->setText(str);
                            mQFormList->getTableWidgetFormType(0x33)->blockSignals(false);
                            return;
                        }

                        if(pGD->overLimit < traffic->m_nLevelCX) //超限属性不符
                        {
                            //QMessageBox::information(this, tr("STPC"), QString("股道%1不能接超限车!\r\n不能选择股道%1!").arg(pGD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                            QString str = ((item->column() == 3)?traffic->m_strRecvTrainTrack : traffic->m_strDepartTrainTrack);
                            item->setText(str);
                            mQFormList->getTableWidgetFormType(0x33)->blockSignals(false);
                            return;
                        }

                        //计划已触发，不能修改股道
                        QVector<TrainRouteOrder> vecTempRouteOrder = pCurStation->getRouteOrderByTrainNum(traffic->m_strTrainNum, traffic->m_btBeginOrEndFlg);
                        for(int i = 0; i < vecTempRouteOrder.size(); i++)
                        {
                            if(vecTempRouteOrder.at(i).m_nOldRouteState != 0 && vecTempRouteOrder.at(i).m_nOldRouteState != 5)
                            {
                                QString str = ((item->column() == 3)?traffic->m_strRecvTrainTrack : traffic->m_strDepartTrainTrack);
                                item->setText(str);
                                mQFormList->getTableWidgetFormType(0x33)->blockSignals(false);
                                return;
                            }
                        }
                    }
                    if(item->column() == 2 || item->column() == 25) //接发车口防错办
                    {
                        CXHD* pXHD = nullptr;
                        for(int i = 0; i < pCurStation->vectGatewayAttr.size(); i++)
                        {
                            pXHD = (CXHD*)(pCurStation->vectGatewayAttr[i]);
                            if(pXHD->m_strName == item->text())
                                break;
                        }
                        if(!pXHD)
                        {
                            return;
                        }

                        if(pXHD->direct != 0
                                && (traffic->m_bReachTrainNumSX && pXHD->direct == 1)
                                && (!traffic->m_bReachTrainNumSX && pXHD->direct == 4)
                                && (traffic->m_bDepartTrainNumSX && pXHD->direct == 3)
                                && (!traffic->m_bDepartTrainNumSX && pXHD->direct == 2))
                        {
                            //QMessageBox::information(this, tr("STPC"), QString("接发车口不符!\r\n不能选择信号机%1!").arg(pXHD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                            QString str = (item->column() == 2) ?  traffic->m_strXHD_JZk: traffic->m_strXHD_CZk;
                            item->setText(str);
                            mQFormList->getTableWidgetFormType(0x33)->blockSignals(false);
                            return;
                        }
                    }
                }

                selectedTrainNo = mQFormList->getTableWidgetFormType(0x22)->item(item->row(), 0)->text();
                selectedTrainNoIndex = item->row();
//                qDebug() << item->row() << item->column() << item->text();
                if(!ItemDataChange(item->column(), item->text(), traffic))
                {
                    mQFormList->getTableWidgetFormType(0x33)->blockSignals(false);
                    return;
                }

                QDateTime date = QDateTime::currentDateTime();
                QString strTime = item->text();
                strTime.replace(".", ":");
                strTime.replace(",", ":");
                strTime.replace("。", ":");
                strTime.replace("，", ":");
                strTime.replace("：", ":");
                QTime time = QTime::fromString(strTime, "hh:mm");
                if(time.isValid())
                    date.setTime(time);

                switch (item->column()) {
                case 2: traffic->m_strXHD_JZk = item->text(); break;
                case 3: {
                    traffic->m_strRecvTrainTrack = item->text();
                    traffic->m_strDepartTrainTrack = item->text();
                    mQFormList->getTableWidgetFormType(0x33)->item(item->row(), 24)->setText(item->text());
                    }; break;
                case 4: traffic->m_timAgrFromAdjtStaDepaTrain = date; break;
                case 5: traffic->m_timFromAdjtStaDepaTrain = date; break;
                case 7: traffic->m_timRealReachStation = date; break;
                case 24: traffic->m_strDepartTrainTrack = item->text(); break;
                case 25: traffic->m_strXHD_CZk = item->text(); break;
                case 26: traffic->m_timToAdjtStaAgrDepaTrain = date; break;
                case 28: traffic->m_timRealDepaTrain = date; break;
                case 29: traffic->m_timtoAdjtStation = date; break;
                case 46: (item->text() == "电力") ? (traffic->m_bElectric = 0x11) : (traffic->m_bElectric = 0); break;
                case 47: traffic->m_strNotes = item->text(); break;
                case 48: (item->text() == "列检") ? (traffic->m_btLJStatus = true) : (traffic->m_btLJStatus = false); break;
                case 49: (item->text() == "交令") ? (traffic->m_btJALStatus = true) : (traffic->m_btJALStatus = false); break;
                case 50: (item->text() == "交票") ? (traffic->m_btJPStatus = true) : (traffic->m_btJPStatus = false); break;
                case 51: (item->text() == "列尾") ? (traffic->m_btLWStatus = true) : (traffic->m_btLWStatus = false); break;
                case 52: (item->text() == "机车") ? (traffic->m_btJCStatus = true) : (traffic->m_btJCStatus = false); break;
                case 53: (item->text() == "货检") ? (traffic->m_btHJStatus = true) : (traffic->m_btHJStatus = false); break;
                case 54: (item->text() == "乘降") ? (traffic->m_btCJStatus = true) : (traffic->m_btCJStatus = false); break;
                case 55: (item->text() == "上水") ? (traffic->m_btSSStatus = true) : (traffic->m_btSSStatus = false); break;
                case 56: (item->text() == "摘挂") ? (traffic->m_btZGStatus = true) : (traffic->m_btZGStatus = false); break;
                case 57: (item->text() == "换乘") ? (traffic->m_btHCStatus = true) : (traffic->m_btHCStatus = false); break;
                case 58: (item->text() == "装卸") ? (traffic->m_btZXStatus = true) : (traffic->m_btZXStatus = false); break;
                case 59: (item->text() == "吸污") ? (traffic->m_btXWStatus = true) : (traffic->m_btXWStatus = false); break;
                case 60: (item->text() == "道口") ? (traffic->m_btDKStatus = true) : (traffic->m_btDKStatus = false); break;
                case 61: (item->text() == "车号") ? (traffic->m_btCHStatus = true) : (traffic->m_btCHStatus = false); break;
                case 62: (item->text() == "站务") ? (traffic->m_btZWStatus = true) : (traffic->m_btZWStatus = false); break;
                case 63: (item->text() == "综控") ? (traffic->m_btZKStatus = true) : (traffic->m_btZKStatus = false); break;
                case 64: {
                    if(item->text() == "一级")
                        traffic->m_nLevelCX = 1;
                    else if(item->text() == "二级")
                        traffic->m_nLevelCX = 2;
                    else if(item->text() == "三级")
                        traffic->m_nLevelCX = 3;
                    else if(item->text() == "超级")
                        traffic->m_nLevelCX = 4;
                    else traffic->m_nLevelCX = 0;
                } break;
                case 65: (item->text() == "军运") ? (traffic->m_bArmy = true) : (traffic->m_bArmy = false); break;
                case 66: (item->text() == "重点") ? (traffic->m_bImportant = true) : (traffic->m_bImportant = false); break;                   
                default: return;
                }

                switch (item->column()) {
                case 4: AgreeAdjacentStations(date); break;
                case 5: AdjacentStationDepart(date); break;
                case 7: TrainArrival(date); break;
                case 26: AdjacentStation(date); break;
                case 28: TrainDeparture(date); break;
                case 29: AdjacentStationArrival(date); break;
                case 47: emit updataNotes(*traffic); break;
                case 48: emit updataTrafficLogFlow(traffic->log_id, 0x02, traffic->m_btLJStatus); break; //列检
                case 49: emit updataTrafficLogFlow(traffic->log_id, 0x01, traffic->m_btJALStatus); break; //交令
                case 50: emit updataTrafficLogFlow(traffic->log_id, 0x05, traffic->m_btJPStatus); break; //交票
                case 51: emit updataTrafficLogFlow(traffic->log_id, 0x08, traffic->m_btLWStatus); break; //列尾
                case 52: emit updataTrafficLogFlow(traffic->log_id, 0x0c, traffic->m_btJCStatus); break; //机车
                case 53: emit updataTrafficLogFlow(traffic->log_id, 0x09, traffic->m_btHJStatus); break; //货检
                case 54: emit updataTrafficLogFlow(traffic->log_id, 0x06, traffic->m_btCJStatus); break; //乘降
                case 55: emit updataTrafficLogFlow(traffic->log_id, 0x03, traffic->m_btSSStatus); break; //上水
                case 56: emit updataTrafficLogFlow(traffic->log_id, 0x07, traffic->m_btZGStatus); break; //摘挂
                case 57: emit updataTrafficLogFlow(traffic->log_id, 0x0a, traffic->m_btHCStatus); break; //换乘
                case 58: emit updataTrafficLogFlow(traffic->log_id, 0x0b, traffic->m_btZXStatus); break; //装卸
                case 59: emit updataTrafficLogFlow(traffic->log_id, 0x04, traffic->m_btXWStatus); break; //吸污
                case 60: emit updataTrafficLogFlow(traffic->log_id, 0x0d, traffic->m_btDKStatus); break; //道口
                case 61: emit updataTrafficLogFlow(traffic->log_id, 0x0e, traffic->m_btCHStatus); break; //车号
                case 62: emit updataTrafficLogFlow(traffic->log_id, 0x10, traffic->m_btZWStatus); break; //站务
                case 63: emit updataTrafficLogFlow(traffic->log_id, 0x0f, traffic->m_btZKStatus); break; //综控
                default:
                    emit updataTrafficLog(0x00, *traffic); break;
                }

                traffic->m_bModify = true;
            }
            else item->setText("");

            mQFormList->getTableWidgetFormType(0x33)->blockSignals(false);
        }

    });
    //表格单元格comboBox数据发生更改
    connect(mQFormList, &QFormList::comboBoxIndexChanged, [=](QString text, int row, int col){
        QTableWidgetItem* item = mQFormList->getTableWidgetFormType(0x33)->item(row, col);
        if(item && item->row() < pCurStation->m_ArrayTrafficLog.size())
        {
            TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[item->row()];
            if(traffic == nullptr)
                return;

            if(item->column() == 3 || item->column() == 24) //股道防错办
            {
                CGD* pGD = nullptr;
                for(int i = 0; i < pCurStation->vectGDAttr.size(); i++)
                {
                    pGD = (CGD*)(pCurStation->vectGDAttr[i]);
                    if(pGD->m_strName == text)
                        break;
                }
                if(!pGD) return;

                if(pGD->jfcAttr == traffic->m_nLHFlg) //客货股道属性不符
                {
                    QMessageBox::information(this, tr("STPC"), QString("客货股道属性不符!\r\n不能选择股道%1!").arg(pGD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                    return;
                }

                if(pGD->overLimit < traffic->m_nLevelCX) //超限属性不符
                {
                    QMessageBox::information(this, tr("STPC"), QString("股道%1不能接超限车!\r\n不能选择股道%1!").arg(pGD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                    return;
                }

                //计划已触发，不能修改股道
                QVector<TrainRouteOrder> vecTempRouteOrder = pCurStation->getRouteOrderByTrainNum(traffic->m_strTrainNum, traffic->m_btBeginOrEndFlg);
                for(int i = 0; i < vecTempRouteOrder.size(); i++)
                {
                    if(vecTempRouteOrder.at(i).m_nOldRouteState != 0 && vecTempRouteOrder.at(i).m_nOldRouteState != 5)
                    {
                        QMessageBox::information(this, tr("STPC"), QString("进路状态不允许修改到达股道!").arg(pGD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                        return;
                    }
                }

            }
            if(item->column() == 2 || item->column() == 25) //接发车口防错办
            {
                CXHD* pXHD = nullptr;
                for(int i = 0; i < pCurStation->vectGatewayAttr.size(); i++)
                {
                    pXHD = (CXHD*)(pCurStation->vectGatewayAttr[i]);
                    if(pXHD->m_strName == text)
                        break;
                }
                if(!pXHD) return;

                if(pXHD->direct != 0
                        && (traffic->m_bReachTrainNumSX && pXHD->direct == 1)
                        && (!traffic->m_bReachTrainNumSX && pXHD->direct == 4)
                        && (traffic->m_bDepartTrainNumSX && pXHD->direct == 3)
                        && (!traffic->m_bDepartTrainNumSX && pXHD->direct == 2))
                {
                    QMessageBox::information(this, tr("STPC"), QString("接发车口不符!\r\n不能选择信号机%1!").arg(pXHD->m_strName), tr("确定"), /*tr("取消"),*/ "", 0);
                    return;
                }
            }
        }
    });
}

void TrafficLogManageKSK::InitTrafficLogMenu()
{
    pTraffclogMenu = new QMenu(this);
    QAction *Action1 = new QAction(tr("上报到达点"), pTraffclogMenu);
    connect(Action1,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(161, QCursor::pos());
        ManualPointReportingWidgert* pManualPointReporting = new ManualPointReportingWidgert(this);
        connect(pManualPointReporting, &ManualPointReportingWidgert::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        pManualPointReporting->Init(pCurStation, log, 1);
        connect(pManualPointReporting,&ManualPointReportingWidgert::reportingPoints,[=](QDateTime date1, QDateTime date2){
            TrainArrival(date1);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        });
        pManualPointReporting->show();
    });
    pTraffclogMenu->addAction(Action1);

    QAction *Action2 = new QAction(tr("上报出发点"), pTraffclogMenu);
    connect(Action2,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(162, QCursor::pos());
        ManualPointReportingWidgert* pManualPointReporting = new ManualPointReportingWidgert(this);
        connect(pManualPointReporting, &ManualPointReportingWidgert::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        pManualPointReporting->Init(pCurStation, log, 2);
        connect(pManualPointReporting,&ManualPointReportingWidgert::reportingPoints,[=](QDateTime date1, QDateTime date2){
            TrainDeparture(date2);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        });
        pManualPointReporting->show();
    });
    pTraffclogMenu->addAction(Action2);

    QAction *Action3 = new QAction(tr("上报通过点"), pTraffclogMenu);
    connect(Action3,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(163, QCursor::pos());
        ManualPointReportingWidgert* pManualPointReporting = new ManualPointReportingWidgert(this);
        connect(pManualPointReporting, &ManualPointReportingWidgert::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        pManualPointReporting->Init(pCurStation, log, 3);
        connect(pManualPointReporting,&ManualPointReportingWidgert::reportingPoints,[=](QDateTime date1, QDateTime date2){
            if(date1 == date2)
            {
                TrainPassThrough(date1);
                if(m_bShowSignRouteSyn)
                    TrafficLogUpData();
            }
            else
            {
                TrainArrival(date1);
                TrainDeparture(date2);
                if(m_bShowSignRouteSyn)
                    TrafficLogUpData();
            }
        });
        pManualPointReporting->show();
    });
    pTraffclogMenu->addAction(Action3);

    QAction *Action4 = new QAction(tr("为始发车"), pTraffclogMenu);
    connect(Action4,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(164, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        log->m_btBeginOrEndFlg = JFC_TYPE_SF;
        emit updataTrafficLog(0x00, *log);
        if(m_bShowSignRouteSyn)
            TrafficLogUpData();
    });
    pTraffclogMenu->addAction(Action4);

    QAction *Action5 = new QAction(tr("为终到车"), pTraffclogMenu);
    connect(Action5,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(165, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        log->m_btBeginOrEndFlg = JFC_TYPE_ZD;
        emit updataTrafficLog(0x00, *log);
        if(m_bShowSignRouteSyn)
            TrafficLogUpData();
    });
    pTraffclogMenu->addAction(Action5);

    pTraffclogMenu->addSeparator();
    QAction *Action6 = new QAction(tr("上报速报信息"), pTraffclogMenu);
    pTraffclogMenu->addAction(Action6);
    connect(Action6,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(166, QCursor::pos());
        TrainMarshal* pTrainMarshal = new TrainMarshal;
        pTrainMarshal->show();
    });

    QAction *Action7 = new QAction(tr("修改车次号"), pTraffclogMenu);
    connect(Action7,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(167, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        TrainNunberChangeWidget* pTrainNunberChange = new TrainNunberChangeWidget(this);
        connect(pTrainNunberChange, &TrainNunberChangeWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        pTrainNunberChange->Init(pCurStation, log);
        connect(pTrainNunberChange,&TrainNunberChangeWidget::trainNunberChange,[=](QString arrival, QString departure, bool bReserve, bool bFinalArrival){
            //emit trainNunberChange(arrival, departure, bReserve, bFinalArrival);
            TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
            log->m_strReachTrainNumOld = log->m_strReachTrainNum;
            log->m_strReachTrainNum = arrival;
            log->m_strDepartTrainNumOld = log->m_strDepartTrainNum;
            log->m_strDepartTrainNum = departure;
            log->m_strTrainNum = log->m_strReachTrainNum!=""?log->m_strReachTrainNum:log->m_strDepartTrainNum;
            if(bReserve)
            {

            }
            if(bFinalArrival)
            {
                log->m_btBeginOrEndFlg = JFC_TYPE_ZD;
            }
            emit updataTrafficLog(0x00, *log);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        });

        pTrainNunberChange->show();
    });
    pTraffclogMenu->addAction(Action7);

    QAction *Action8 = new QAction(tr("修改相关邻站"), pTraffclogMenu);
    connect(Action8,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(168, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        AdjacentStationChange* pAdjacentStationChange = new AdjacentStationChange(this);
        connect(pAdjacentStationChange, &AdjacentStationChange::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        pAdjacentStationChange->Init(pCurStation, log);
        connect(pAdjacentStationChange, &AdjacentStationChange::AdjStationChange, [=](QString FromAdjtStation, QString ToAdjtStation){
            TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
            log->m_strFromAdjtStation = FromAdjtStation;
            log->m_strToAdjtStation = ToAdjtStation;
            emit updataTrafficLog(0x00, *log);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        });
        pAdjacentStationChange->show();
    });
    //pTraffclogMenu->addAction(Action8);

    QAction *Action9 = new QAction(tr("删除"), pTraffclogMenu);
    connect(Action9,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(169, QCursor::pos());
        TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        //emit updataTrafficLog(0x01, plog);
        int ret = QMessageBox::information(this, tr("警告"), tr("确定删除该列车的行车信息?"), tr("确定"), tr("取消"), "", 0);
        if(ret == 0)
        {
            emit SendTextAnalysisSignal(77, QCursor::pos());
            emit sendTrafficLogMsg(plog->log_id, 0x1c, QDateTime::currentDateTime());
            plog->m_bDelete = true;
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        }
        else emit SendTextAnalysisSignal(78, QCursor::pos());
    });
    pTraffclogMenu->addAction(Action9);

    pTraffclogMenu->addSeparator();
    QAction *Action10 = new QAction(tr("清除闪烁"), pTraffclogMenu);    
    connect(Action10,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(170, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        emit stopFlicker(log, selectedTrainNoIndex);
    });
    pTraffclogMenu->addAction(Action10);

    QAction *Action11 = new QAction(tr("全体信息"), pTraffclogMenu);   
    connect(Action11,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(171, QCursor::pos());
    });
    pTraffclogMenu->addAction(Action11);

    pTraffclogMenu->addSeparator();
    QAction *Action12 = new QAction(tr("修改列车计划"), pTraffclogMenu);    
    connect(Action12,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(172, QCursor::pos());
        NewTraffclogWidget* pNewTraffclog = new NewTraffclogWidget(this);
        connect(pNewTraffclog, &NewTraffclogWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        pNewTraffclog->setWindowTitle(QString("修改列车计划"));
        pNewTraffclog->Init(pCurStation, true);
        pNewTraffclog->InitTraffclog(pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex));
        connect(pNewTraffclog,&NewTraffclogWidget::addNewTraffclog,[=](TrafficLog pTrafficLog){
            emit updataTrafficLog(0x00, pTrafficLog);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        });
        pNewTraffclog->show();
    });
    pTraffclogMenu->addAction(Action12);

    QAction *Action13 = new QAction(tr("设置/取消重点列车"), pTraffclogMenu);
    //Action13->setCheckable(true);
    connect(Action13,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(173, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        if(log->m_bImportant == 0)
        {
            log->m_bImportant = 1;
        }
        else
        {
            log->m_bImportant = 0;
        }
        emit updataTrafficLog(0x00, *log);
        if(m_bShowSignRouteSyn)
            TrafficLogUpData();
    });
    pTraffclogMenu->addAction(Action13);

    deleteFlagAction = new QAction(tr("设置删除标记"), pTraffclogMenu);
    deleteFlagAction->setCheckable(true);
    connect(deleteFlagAction,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(174, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        int ret = QMessageBox::information(this, tr("警告"), tr("确定添加/取消 删除标记?"), tr("确定"), tr("取消"), "", 0);
        if(ret == 0)
        {
            emit SendTextAnalysisSignal(77, QCursor::pos());
            if(deleteFlagAction->isChecked())
            {
                log->m_bDeleteFlag = true;
            }
            else
            {
                log->m_bDeleteFlag = false;
            }
            emit updataTrafficLog(0x00, *log);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        }
        else emit SendTextAnalysisSignal(78, QCursor::pos());
    });
    pTraffclogMenu->addAction(deleteFlagAction);

    allowGDNotMatch = new QAction(tr("允许股道与基本径路不一致"), pTraffclogMenu);
    allowGDNotMatch->setCheckable(true);
    connect(allowGDNotMatch,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(175, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        if(allowGDNotMatch->isChecked())
        {
            log->m_bAllowGDNotMatch = 1;
            allowGDNotMatch->setChecked(true);
        }
        else
        {
            log->m_bAllowGDNotMatch = 0;
            allowGDNotMatch->setChecked(false);
        }
        emit updataTrafficLog(0x00, *log);
        if(m_bShowSignRouteSyn)
            TrafficLogUpData();
    });
    pTraffclogMenu->addAction(allowGDNotMatch);

    allowCRKNotMatch = new QAction(tr("允许出入口与基本径路不一致"), pTraffclogMenu);
    allowCRKNotMatch->setCheckable(true);
    connect(allowCRKNotMatch,&QAction::triggered,[=](){
        emit SendTextAnalysisSignal(176, QCursor::pos());
        TrafficLog* log = pCurStation->m_ArrayTrafficLog.at(selectedTrainNoIndex);
        if(allowCRKNotMatch->isChecked())
        {
            log->m_bAllowCRKNotMatch = 1;
            allowCRKNotMatch->setChecked(true);
        }
        else
        {
            log->m_bAllowCRKNotMatch = 0;
            allowCRKNotMatch->setChecked(false);
            if(m_bShowSignRouteSyn)
                TrafficLogUpData();
        }
        emit updataTrafficLog(0x00, *log);
    });
    pTraffclogMenu->addAction(allowCRKNotMatch);
}

void TrafficLogManageKSK::InitTimeMenu()
{
    pTimeMenu = new QMenu(this);
    QAction *Action1 = new QAction(tr("计划+100分钟"), pTimeMenu);
    connect(Action1, &QAction::triggered, [=](){
        AddReachTime(100);
    });
    pTimeMenu->addAction(Action1);
    QAction *Action2 = new QAction(tr("计划+50分钟"), pTimeMenu);
    connect(Action2, &QAction::triggered, [=](){
        AddReachTime(50);
    });
    pTimeMenu->addAction(Action2);
    QAction *Action3 = new QAction(tr("计划+20分钟"), pTimeMenu);
    connect(Action3, &QAction::triggered, [=](){
        AddReachTime(20);
    });
    pTimeMenu->addAction(Action3);
    QAction *Action4 = new QAction(tr("计划+10分钟"), pTimeMenu);
    connect(Action4, &QAction::triggered, [=](){
        AddReachTime(10);
    });
    pTimeMenu->addAction(Action4);
    QAction *Action5 = new QAction(tr("计划+5分钟"), pTimeMenu);
    connect(Action5, &QAction::triggered, [=](){
        AddReachTime(5);
    });
    pTimeMenu->addAction(Action5);
    QAction *Action6 = new QAction(tr("计划+2分钟"), pTimeMenu);
    connect(Action6, &QAction::triggered, [=](){
        AddReachTime(2);
    });
    pTimeMenu->addAction(Action6);
    QAction *Action7 = new QAction(tr("计划+1分钟"), pTimeMenu);
    connect(Action7, &QAction::triggered, [=](){
        AddReachTime(50);
    });
    pTimeMenu->addAction(Action7);

    pTimeMenu->addSeparator();

    QAction *Action8 = new QAction(tr("计划-1分钟"), pTimeMenu);
    connect(Action8, &QAction::triggered, [=](){
        AddReachTime(-1);
    });
    pTimeMenu->addAction(Action8);
    QAction *Action9 = new QAction(tr("计划-2分钟"), pTimeMenu);
    connect(Action9, &QAction::triggered, [=](){
        AddReachTime(-2);
    });
    pTimeMenu->addAction(Action9);
    QAction *Action10 = new QAction(tr("计划-5分钟"), pTimeMenu);
    connect(Action10, &QAction::triggered, [=](){
        AddReachTime(-5);
    });
    pTimeMenu->addAction(Action10);
    QAction *Action11 = new QAction(tr("计划-10分钟"), pTimeMenu);
    connect(Action11, &QAction::triggered, [=](){
        AddReachTime(-10);
    });
    pTimeMenu->addAction(Action11);
    QAction *Action12 = new QAction(tr("计划-20分钟"), pTimeMenu);
    connect(Action12, &QAction::triggered, [=](){
        AddReachTime(-20);
    });
    pTimeMenu->addAction(Action12);
    QAction *Action13 = new QAction(tr("计划-50分钟"), pTimeMenu);
    connect(Action13, &QAction::triggered, [=](){
        AddReachTime(-50);
    });
    pTimeMenu->addAction(Action13);
    QAction *Action14 = new QAction(tr("计划-100分钟"), pTimeMenu);
    connect(Action14, &QAction::triggered, [=](){
        AddReachTime(-100);
    });
    pTimeMenu->addAction(Action14);
}

void TrafficLogManageKSK::InitFromListHead()
{
    InitFromListHeadH();
    InitFromListHeadV();
    mQFormList->dataTableClear();
    for(int i = 0; i < nCol; i++)
    {
        if(i == 0 || i == 1 || i == 2 || i == 3 || i == 6
                || i == 22 || i == 23 || i == 24 || i == 25 || i == 27
                || i == 24 || (i >= 48 && i <= 64))
        {
            mQFormList->setColReadOnly(i);
        }
    }

}

void TrafficLogManageKSK::InitFromListHeadH()
{
    mHFormListHead.Init();
    mHFormListHead.ntype=0x11;
    int nrow=5;
    for (int i=0;i<nrow;i++)
    {
        if(i==nrow-2)
            mHFormListHead.v_Hightl.append(50);
        else
            mHFormListHead.v_Hightl.append(30);
    }

    for(int i=0;i<nCol;i++)
    {
        mHFormListHead.v_Width.append(64);
        for (int j=0;j<nrow;j++)
        {
            mHFormListHead.v_itemName.append(QString::number(i+1));
            mHFormListHead.v_ItemPos.append({j,i});
        }
    }

     //QVector<QVector<int>>v_Span---------横(纵)向表头单元格合并信息数组容器，
    //例如：v_Span[i]={1, 20, 2, 4}表示从（1，20）位置为左上角起始位置，覆盖两行四列合并

    {
        mHFormListHead.v_itemName[0]="到达";
        mHFormListHead.v_Span.append({0,0,1,22});
        {
            mHFormListHead.v_itemName[1]="列车车次";
            mHFormListHead.v_Span.append({1,0,2,2});
            {
                mHFormListHead.v_itemName[0*5+3]="下行";
                mHFormListHead.v_itemName[1*5+3]="上行";
            }

            mHFormListHead.v_itemName[2*5+1]="接车口";
            mHFormListHead.v_Span.append({1,2,3,1});

            mHFormListHead.v_itemName[3*5+1]="接车股道";
            mHFormListHead.v_Span.append({1,3,3,1});

            mHFormListHead.v_itemName[4*5+1]="时分";
            mHFormListHead.v_Span.append({1,4,1,4});
            {
                mHFormListHead.v_itemName[4*5+2]="同意邻站发车";
                mHFormListHead.v_Span.append({2,4,2,1});

                mHFormListHead.v_itemName[5*5+2]="邻站出发";
                mHFormListHead.v_Span.append({2,5,2,1});

                mHFormListHead.v_itemName[6*5+2]="本站到达";
                mHFormListHead.v_Span.append({2,6,1,2});
                {
                    mHFormListHead.v_itemName[6*5+3]="规定";
                    mHFormListHead.v_itemName[7*5+3]="实际";
                }
            }

            mHFormListHead.v_itemName[8*5+1]="摘车辆数";
            mHFormListHead.v_Span.append({1,8,1,4});
            {
                mHFormListHead.v_itemName[8*5+2]="运用车";
                mHFormListHead.v_Span.append({2,8,1,2});
                {
                    mHFormListHead.v_itemName[8*5+3]="重车";
                    mHFormListHead.v_itemName[9*5+3]="空车";
                }

                mHFormListHead.v_itemName[10*5+2]="非运用车";
                mHFormListHead.v_Span.append({2,10,2,1});

                mHFormListHead.v_itemName[11*5+2]="守车";
                mHFormListHead.v_Span.append({2,11,2,1});
            }

            mHFormListHead.v_itemName[12*5+1]="占用区间凭证号码";
            mHFormListHead.v_Span.append({1,12,3,1});

            mHFormListHead.v_itemName[13*5+1]="机车型号";
            mHFormListHead.v_Span.append({1,13,3,1});

            mHFormListHead.v_itemName[14*5+1]="列车编组";
            mHFormListHead.v_Span.append({1,14,1,3});
            {
                mHFormListHead.v_itemName[14*5+2]="车数";
                mHFormListHead.v_Span.append({2,14,2,1});

                mHFormListHead.v_itemName[15*5+2]="换长";
                mHFormListHead.v_Span.append({2,15,2,1});

                mHFormListHead.v_itemName[16*5+2]="总重";
                mHFormListHead.v_Span.append({2,16,2,1});
            }

            mHFormListHead.v_itemName[17*5+1]="电话记录号码";
            mHFormListHead.v_Span.append({1,17,1,5});
            {
                mHFormListHead.v_itemName[17*5+2]="承认闭塞";
                mHFormListHead.v_Span.append({2,17,2,1});
                mHFormListHead.v_itemName[18*5+2]="列车到达补机返回";
                mHFormListHead.v_Span.append({2,18,2,1});
                mHFormListHead.v_itemName[19*5+2]="取消闭塞";
                mHFormListHead.v_Span.append({2,19,2,1});
                mHFormListHead.v_itemName[20*5+2]="出站跟踪调车";
                mHFormListHead.v_Span.append({2,20,2,1});
                mHFormListHead.v_itemName[21*5+2]="出站跟踪调车完毕";
                mHFormListHead.v_Span.append({2,21,2,1});
            }
        }

        mHFormListHead.v_itemName[22*5]="出发";
        mHFormListHead.v_Span.append({0,22,1,22});
        {
            mHFormListHead.v_itemName[22*5+1]="列车车次";
            mHFormListHead.v_Span.append({1,22,2,2});
            {
                mHFormListHead.v_itemName[22*5+3]="下行";
                mHFormListHead.v_itemName[23*5+3]="上行";
            }

            mHFormListHead.v_itemName[24*5+1]="发车股道";
            mHFormListHead.v_Span.append({1,24,3,1});

            mHFormListHead.v_itemName[25*5+1]="发车口";
            mHFormListHead.v_Span.append({1,25,3,1});

            mHFormListHead.v_itemName[26*5+1]="时分";
            mHFormListHead.v_Span.append({1,26,1,4});
            {
                mHFormListHead.v_itemName[26*5+2]="邻站同意发车";
                mHFormListHead.v_Span.append({2,26,2,1});

                mHFormListHead.v_itemName[27*5+2]="本站出发";
                mHFormListHead.v_Span.append({2,27,1,2});
                {
                    mHFormListHead.v_itemName[27*5+3]="规定";
                    mHFormListHead.v_itemName[28*5+3]="实际";
                }

                mHFormListHead.v_itemName[29*5+2]="邻站到达";
                mHFormListHead.v_Span.append({2,29,2,1});
            }

            mHFormListHead.v_itemName[30*5+1]="摘车辆数";
            mHFormListHead.v_Span.append({1,30,1,4});
            {
                mHFormListHead.v_itemName[30*5+2]="运用车";
                mHFormListHead.v_Span.append({2,30,1,2});
                {
                    mHFormListHead.v_itemName[30*5+3]="重车";
                    mHFormListHead.v_itemName[31*5+3]="空车";
                }

                mHFormListHead.v_itemName[32*5+2]="非运用车";
                mHFormListHead.v_Span.append({2,32,2,1});

                mHFormListHead.v_itemName[33*5+2]="守车";
                mHFormListHead.v_Span.append({2,33,2,1});
            }

            mHFormListHead.v_itemName[34*5+1]="占用区间凭证号码";
            mHFormListHead.v_Span.append({1,34,3,1});

            mHFormListHead.v_itemName[35*5+1]="机车型号";
            mHFormListHead.v_Span.append({1,35,3,1});

            mHFormListHead.v_itemName[36*5+1]="列车编组";
            mHFormListHead.v_Span.append({1,36,1,3});
            {
                mHFormListHead.v_itemName[36*5+2]="车数";
                mHFormListHead.v_Span.append({2,36,2,1});

                mHFormListHead.v_itemName[37*5+2]="换长";
                mHFormListHead.v_Span.append({2,37,2,1});

                mHFormListHead.v_itemName[38*5+2]="总重";
                mHFormListHead.v_Span.append({2,38,2,1});
            }

            mHFormListHead.v_itemName[39*5+1]="电话记录号码";
            mHFormListHead.v_Span.append({1,39,1,5});
            {
                mHFormListHead.v_itemName[39*5+2]="承认闭塞";
                mHFormListHead.v_Span.append({2,39,2,1});

                mHFormListHead.v_itemName[40*5+2]="列车到达补机返回";
                mHFormListHead.v_Span.append({2,40,2,1});

                mHFormListHead.v_itemName[41*5+2]="取消闭塞";
                mHFormListHead.v_Span.append({2,41,2,1});

                mHFormListHead.v_itemName[42*5+2]="出站跟踪调车";
                mHFormListHead.v_Span.append({2,42,2,1});

                mHFormListHead.v_itemName[43*5+2]="出站跟踪调车完毕";
                mHFormListHead.v_Span.append({2,43,2,1});
            }
        }

        mHFormListHead.v_itemName[44*5]="本务机车调车时分";
        mHFormListHead.v_Span.append({0,44,4,1});

        mHFormListHead.v_itemName[45*5]="列车停站超过规定时间原因";
        mHFormListHead.v_Span.append({0,45,4,1});

        mHFormListHead.v_itemName[46*5]="计划属性";
        mHFormListHead.v_Span.append({0,46,4,1});

        mHFormListHead.v_itemName[47*5]="记事";
        mHFormListHead.v_Span.append({0,47,4,1});

        mHFormListHead.v_itemName[48*5]="任务";
        mHFormListHead.v_Span.append({0,48,1,16});
        {
            mHFormListHead.v_itemName[48*5+1]="列检";
            mHFormListHead.v_Span.append({1,48,3,1});
            mHFormListHead.v_itemName[49*5+1]="交令";
            mHFormListHead.v_Span.append({1,49,3,1});
            mHFormListHead.v_itemName[50*5+1]="交票";
            mHFormListHead.v_Span.append({1,50,3,1});
            mHFormListHead.v_itemName[51*5+1]="列尾";
            mHFormListHead.v_Span.append({1,51,3,1});
            mHFormListHead.v_itemName[52*5+1]="机车";
            mHFormListHead.v_Span.append({1,52,3,1});
            mHFormListHead.v_itemName[53*5+1]="货检";
            mHFormListHead.v_Span.append({1,53,3,1});
            mHFormListHead.v_itemName[54*5+1]="乘降";
            mHFormListHead.v_Span.append({1,54,3,1});
            mHFormListHead.v_itemName[55*5+1]="上水";
            mHFormListHead.v_Span.append({1,55,3,1});
            mHFormListHead.v_itemName[56*5+1]="摘挂";
            mHFormListHead.v_Span.append({1,56,3,1});
            mHFormListHead.v_itemName[57*5+1]="换乘";
            mHFormListHead.v_Span.append({1,57,3,1});
            mHFormListHead.v_itemName[58*5+1]="装卸";
            mHFormListHead.v_Span.append({1,58,3,1});
            mHFormListHead.v_itemName[59*5+1]="吸污";
            mHFormListHead.v_Span.append({1,59,3,1});
            mHFormListHead.v_itemName[60*5+1]="道口";
            mHFormListHead.v_Span.append({1,60,3,1});
            mHFormListHead.v_itemName[61*5+1]="车号";
            mHFormListHead.v_Span.append({1,61,3,1});
            mHFormListHead.v_itemName[62*5+1]="站务";
            mHFormListHead.v_Span.append({1,62,3,1});
            mHFormListHead.v_itemName[63*5+1]="综控";
            mHFormListHead.v_Span.append({1,63,3,1});
        }

        mHFormListHead.v_itemName[64*5]="超限";
        mHFormListHead.v_Span.append({0,64,4,1});

        mHFormListHead.v_itemName[65*5]="军运";
        mHFormListHead.v_Span.append({0,65,4,1});

        mHFormListHead.v_itemName[66*5]="重点";
        mHFormListHead.v_Span.append({0,66,4,1});
    }

    mQFormList->init_HeaderTab(mHFormListHead.ntype,nrow,nCol,true,mHFormListHead.v_Hightl,mHFormListHead.v_Width,mHFormListHead.v_Span,
                               mHFormListHead.v_ItemPos,mHFormListHead.v_itemName,true,false);
    QString strSheet="background-color: rgb(240, 240, 240);";
    mQFormList->setTabStyleSheet(mHFormListHead.ntype,strSheet);
    mQFormList->setAdaptiveWidth(2);
    mQFormList->setAdaptiveWidth(3);
    mQFormList->setAdaptiveWidth(4);
    mQFormList->setAdaptiveWidth(5);
    mQFormList->setAdaptiveWidth(6);
    mQFormList->setAdaptiveWidth(7);
    mQFormList->setAdaptiveWidth(24);
    mQFormList->setAdaptiveWidth(25);
    mQFormList->setAdaptiveWidth(26);
    mQFormList->setAdaptiveWidth(27);
    mQFormList->setAdaptiveWidth(28);
    mQFormList->setAdaptiveWidth(29);
    mQFormList->setAdaptiveWidth(45);
    mQFormList->setAdaptiveWidth(46);
    mQFormList->setAdaptiveWidth(47);
    mQFormList->setAdaptiveWidth(64);
    mQFormList->getTableWidgetFormType(0x11)->horizontalHeader()->setMinimumSectionSize(64);
    mQFormList->getTableWidgetFormType(0x33)->horizontalHeader()->setMinimumSectionSize(64);
}

void TrafficLogManageKSK::InitFromListHeadV()
{
    mVFormListHead.Init();
    mVFormListHead.ntype = 0x22;
    int nrow=50;
    int ncol=1;
    for (int i=0;i<nrow;i++)
    {
        mVFormListHead.v_Hightl.append(24);
    }
    for(int i=0;i<ncol;i++)
    {
        mVFormListHead.v_Width.append(108);
        for (int j=0;j<nrow;j++)
        {
            mVFormListHead.v_itemName.append(QString::number(j + 1));
            mVFormListHead.v_ItemPos.append({j,i});
        }
    }
    mQFormList->init_HeaderTab(mVFormListHead.ntype,nrow,ncol,true,mVFormListHead.v_Hightl,mVFormListHead.v_Width,mVFormListHead.v_Span,
                               mVFormListHead.v_ItemPos,mVFormListHead.v_itemName,false,false);
    QString strSheet="background-color: rgb(240, 240, 240);";
    mQFormList->setTabStyleSheet(mVFormListHead.ntype,strSheet);
    for (int i=0;i<nrow;i++)
    {
        mQFormList->getTableWidgetFormType(0x22)->item(i, 0)->setData(Qt::TextColorRole, QColor(80,80,80));
    }
}

void TrafficLogManageKSK::TrafficLogUpData()
{
    QVector<QVector<int>> itemPos;
    QVector<QString> itemName;
    mQFormList->dataTableClear();
    mQFormList->headerVerReSet(mVFormListHead.v_ItemPos, mVFormListHead.v_itemName);
    //verHeraldItemInfo.clear();
    mQFormList->setAllItemsBackGColor(0x33, QColor(240, 240, 240));
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
        {
            if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
            {
                mQFormList->setCellBackgroundColor(0x33, j, verHeraldItemInfo.at(i).heraldCol, verHeraldItemInfo.at(i).color);
            }
        }
    }

    for (int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
    {
        TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[i];
        int col = mQFormList->getDataIndexByText(0x22, 0, traffic->m_strTrainNum);
        if(col < 0 || col >= pCurStation->m_ArrayTrafficLog.size())
            traffic->m_bNewTrafficLog = true;
        else traffic->m_bNewTrafficLog = false;
    }

    int index = 0;
    for (int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
    {
        TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[i];
        if(traffic->m_bDelete)
            continue;
        //traffic->m_bNewTrafficLog = true;
        if(traffic->m_btBeginOrEndFlg == JFC_TYPE_SF)
        {
            if(showType == 1 && !traffic->m_bDepartTrainNumSX)//显示上行
                continue;

            if(showType == 2 && traffic->m_bDepartTrainNumSX)//显示下行
                continue;
        }
        else if(traffic->m_btBeginOrEndFlg == JFC_TYPE_ZD)
        {
            if(showType == 1 && !traffic->m_bReachTrainNumSX)//显示上行
                continue;

            if(showType == 2 && traffic->m_bReachTrainNumSX)//显示下行
                continue;
        }
        else
        {
            if(showType == 1 && (!traffic->m_bReachTrainNumSX && !traffic->m_bDepartTrainNumSX))//显示上行
                continue;

            if(showType == 2 && (traffic->m_bReachTrainNumSX && traffic->m_bDepartTrainNumSX))//显示下行
                continue;
        }

        if(traffic->m_btBeginOrEndFlg == JFC_TYPE_SF)//始发
        {
            AddItemData(&itemPos, &itemName, index, 7, "始发");
            if(!traffic->m_bDepartTrainNumSX)
                AddItemData(&itemPos, &itemName, index, 22, traffic->m_strDepartTrainNum);
            else AddItemData(&itemPos, &itemName, index, 23, traffic->m_strDepartTrainNum);
            AddItemData(&itemPos, &itemName, index, 24, traffic->m_strDepartTrainTrack);
            AddItemData(&itemPos, &itemName, index, 25, traffic->m_strXHD_CZk);

            AddItemData(&itemPos, &itemName, index, 26, traffic->m_timToAdjtStaAgrDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 27, traffic->m_timChartDepaTrain.toString("hh:mm"));
            if(traffic->m_timRealDepaTrain.isNull())
                //AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
                AddItemData(&itemPos, &itemName, index, 28, traffic->m_timProvDepaTrain.toString("hh:mm"));
            else AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 29, traffic->m_timtoAdjtStation.toString("hh:mm"));
        }
        else if(traffic->m_btBeginOrEndFlg == JFC_TYPE_ZD)//终到
        {
            if(!traffic->m_bReachTrainNumSX)
                AddItemData(&itemPos, &itemName, index, 0, traffic->m_strReachTrainNum);
            else AddItemData(&itemPos, &itemName, index, 1, traffic->m_strReachTrainNum);

            AddItemData(&itemPos, &itemName, index, 2, traffic->m_strXHD_JZk);
            AddItemData(&itemPos, &itemName, index, 3, traffic->m_strRecvTrainTrack);
            AddItemData(&itemPos, &itemName, index, 4, traffic->m_timAgrFromAdjtStaDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 5, traffic->m_timFromAdjtStaDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 6, traffic->m_timChartReachStation.toString("hh:mm"));
            if(traffic->m_timRealReachStation.isNull())
                AddItemData(&itemPos, &itemName, index, 7, traffic->m_timProvReachStation.toString("hh:mm"));
            else AddItemData(&itemPos, &itemName, index, 7, traffic->m_timRealReachStation.toString("hh:mm"));
            //AddItemData(&itemPos, &itemName, index, 7, traffic->m_timRealReachStation.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 28, "终到");
//            qDebug() << traffic->m_timRealReachStation.toString(Qt::ISODateWithMs) << traffic->m_timRealReachStation.isNull();
        }
        else if(traffic->m_btBeginOrEndFlg == JFC_TYPE_TG)//通过
        {
            if(!traffic->m_bReachTrainNumSX)
                AddItemData(&itemPos, &itemName, index, 0, traffic->m_strReachTrainNum);
            else AddItemData(&itemPos, &itemName, index, 1, traffic->m_strReachTrainNum);

            AddItemData(&itemPos, &itemName, index, 2, traffic->m_strXHD_JZk);
            AddItemData(&itemPos, &itemName, index, 3, traffic->m_strRecvTrainTrack);
            AddItemData(&itemPos, &itemName, index, 4, traffic->m_timAgrFromAdjtStaDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 5, traffic->m_timFromAdjtStaDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 6, traffic->m_timChartReachStation.toString("hh:mm"));
            //AddItemData(&itemPos, &itemName, index, 7, traffic->m_timRealReachStation.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 7, "通过");
            //if(traffic->m_timRealReachStation.isNull())
            //    AddItemData(&itemPos, &itemName, index, 7, traffic->m_timProvReachStation.toString("hh:mm"));
            //else AddItemData(&itemPos, &itemName, index, 7, "通过");
            if(!traffic->m_bDepartTrainNumSX)
                AddItemData(&itemPos, &itemName, index, 22, traffic->m_strDepartTrainNum);
            else AddItemData(&itemPos, &itemName, index, 23, traffic->m_strDepartTrainNum);
            AddItemData(&itemPos, &itemName, index, 24, traffic->m_strDepartTrainTrack);
            AddItemData(&itemPos, &itemName, index, 25, traffic->m_strXHD_CZk);
            AddItemData(&itemPos, &itemName, index, 26, traffic->m_timToAdjtStaAgrDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 27, traffic->m_timChartDepaTrain.toString("hh:mm"));
            //AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
            if(traffic->m_timRealDepaTrain.isNull())
                //AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
                AddItemData(&itemPos, &itemName, index, 28, traffic->m_timProvDepaTrain.toString("hh:mm"));
            else AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 29, traffic->m_timtoAdjtStation.toString("hh:mm"));
        }
        else
        {
            if(!traffic->m_bReachTrainNumSX)
                AddItemData(&itemPos, &itemName, index, 0, traffic->m_strReachTrainNum);
            else AddItemData(&itemPos, &itemName, index, 1, traffic->m_strReachTrainNum);
            AddItemData(&itemPos, &itemName, index, 2, traffic->m_strXHD_JZk);
            AddItemData(&itemPos, &itemName, index, 3, traffic->m_strRecvTrainTrack);
            AddItemData(&itemPos, &itemName, index, 4, traffic->m_timAgrFromAdjtStaDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 5, traffic->m_timFromAdjtStaDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 6, traffic->m_timChartReachStation.toString("hh:mm"));
            if(traffic->m_timRealReachStation.isNull())
                //AddItemData(&itemPos, &itemName, index, 7, traffic->m_timRealReachStation.toString("hh:mm"));
                AddItemData(&itemPos, &itemName, index, 7, traffic->m_timProvReachStation.toString("hh:mm"));
            else AddItemData(&itemPos, &itemName, index, 7, traffic->m_timRealReachStation.toString("hh:mm"));

            if(!traffic->m_bDepartTrainNumSX)
                AddItemData(&itemPos, &itemName, index, 22, traffic->m_strDepartTrainNum);
            else AddItemData(&itemPos, &itemName, index, 23, traffic->m_strDepartTrainNum);
            AddItemData(&itemPos, &itemName, index, 24, traffic->m_strDepartTrainTrack);
            AddItemData(&itemPos, &itemName, index, 25, traffic->m_strXHD_CZk);
            AddItemData(&itemPos, &itemName, index, 26, traffic->m_timToAdjtStaAgrDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 27, traffic->m_timChartDepaTrain.toString("hh:mm"));
            if(traffic->m_timRealDepaTrain.isNull())
                //AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
                AddItemData(&itemPos, &itemName, index, 28, traffic->m_timProvDepaTrain.toString("hh:mm"));
            else AddItemData(&itemPos, &itemName, index, 28, traffic->m_timRealDepaTrain.toString("hh:mm"));
            AddItemData(&itemPos, &itemName, index, 29, traffic->m_timtoAdjtStation.toString("hh:mm"));
        }

        if(traffic->m_bElectric)
            AddItemData(&itemPos, &itemName, index, 46, "电力");

        AddItemData(&itemPos, &itemName, index, 47, traffic->m_strNotes);

        if(traffic->m_btLJStatus)
            AddItemData(&itemPos, &itemName, index, 48, "列检");
        if(traffic->m_btJALStatus)
            AddItemData(&itemPos, &itemName, index, 49, "交令");
        if(traffic->m_btJPStatus)
            AddItemData(&itemPos, &itemName, index, 50, "交票");
        if(traffic->m_btLWStatus)
            AddItemData(&itemPos, &itemName, index, 51, "列尾");
        if(traffic->m_btJCStatus)
            AddItemData(&itemPos, &itemName, index, 52, "机车");
        if(traffic->m_btHJStatus)
            AddItemData(&itemPos, &itemName, index, 53, "货检");
        if(traffic->m_btCJStatus)
            AddItemData(&itemPos, &itemName, index, 54, "乘降");
        if(traffic->m_btSSStatus)
            AddItemData(&itemPos, &itemName, index, 55, "上水");
        if(traffic->m_btZGStatus)
            AddItemData(&itemPos, &itemName, index, 56, "摘挂");
        if(traffic->m_btHCStatus)
            AddItemData(&itemPos, &itemName, index, 57, "换乘");
        if(traffic->m_btZXStatus)
            AddItemData(&itemPos, &itemName, index, 58, "装卸");
        if(traffic->m_btXWStatus)
            AddItemData(&itemPos, &itemName, index, 59, "吸污");
        if(traffic->m_btDKStatus)
            AddItemData(&itemPos, &itemName, index, 60, "道口");
        if(traffic->m_btCHStatus)
            AddItemData(&itemPos, &itemName, index, 61, "车号");
        if(traffic->m_btZWStatus)
            AddItemData(&itemPos, &itemName, index, 62, "站务");
        if(traffic->m_btZKStatus)
            AddItemData(&itemPos, &itemName, index, 63, "综控");

        if(traffic->m_nLevelCX == 0)
            AddItemData(&itemPos, &itemName, index, 64, " ");
        else if(traffic->m_nLevelCX == 1)
            AddItemData(&itemPos, &itemName, index, 64, "一级");
        else if(traffic->m_nLevelCX == 2)
            AddItemData(&itemPos, &itemName, index, 64, "二级");
        else if(traffic->m_nLevelCX == 3)
            AddItemData(&itemPos, &itemName, index, 64, "三级");
        else if(traffic->m_nLevelCX == 4)
            AddItemData(&itemPos, &itemName, index, 64, "超级");

        if(traffic->m_bArmy)
            AddItemData(&itemPos, &itemName, index, 65, "军运");
        if(traffic->m_bImportant)
            AddItemData(&itemPos, &itemName, index, 66, "重点");


        index++;
    }
    mQFormList->upData_Info(0x33, itemPos, itemName);


    QString strSheet="background-color: rgb(240, 240, 240);";
    mQFormList->setTabStyleSheet(mVFormListHead.ntype,strSheet);

    index = 0;
    for (int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
    {
        TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[i];
        if(traffic->m_bDelete)
            continue;
        if(traffic->m_btBeginOrEndFlg == JFC_TYPE_SF)
        {
            if(showType == 1 && !traffic->m_bDepartTrainNumSX)//显示上行
            {
                continue;
            }

            if(showType == 2 && traffic->m_bDepartTrainNumSX)//显示下行
            {
                continue;
            }
        }
        else if(traffic->m_btBeginOrEndFlg == JFC_TYPE_ZD)
        {
            if(showType == 1 && !traffic->m_bReachTrainNumSX)//显示上行
            {
                continue;
            }

            if(showType == 2 && traffic->m_bReachTrainNumSX)//显示下行
            {
                continue;
            }
        }
        else
        {
            if(showType == 1 && (!traffic->m_bReachTrainNumSX && !traffic->m_bDepartTrainNumSX))//显示上行
            {
                continue;
            }

            if(showType == 2 && (traffic->m_bReachTrainNumSX && traffic->m_bDepartTrainNumSX))//显示下行
            {
                continue;
            }
        }

        QString strTrainNum = traffic->m_strTrainNum;
        if(traffic->m_bImportant)
            strTrainNum += "(重)";
        if(traffic->m_bDeleteFlag)
            strTrainNum += "(删)";

        mQFormList->upData_Info(0x22, index, 0, strTrainNum);
        mQFormList->getTableWidgetFormType(0x22)->item(index, 0)->setData(Qt::TextColorRole, QColor(0,0,0));

        if(traffic->m_timAgrFromAdjtStaDepaTrain.isNull() && traffic->m_timToAdjtStaAgrDepaTrain.isNull()
                && traffic->m_timRealReachStation.isNull() && traffic->m_timRealDepaTrain.isNull()
                && traffic->m_timFromAdjtStaDepaTrain.isNull() && traffic->m_timtoAdjtStation.isNull()
                && !isSetBkColor(index))
        {
            if(traffic->m_nLHFlg == 1)
                mQFormList->SetRowTextColor(index, PURPLE);
            else mQFormList->SetRowTextColor(index, SkyBlue);
        }
        else
        {
            if(traffic->m_nLHFlg == 1)
                mQFormList->SetRowTextColor(index, Qt::red);
            else mQFormList->SetRowTextColor(index, Qt::blue);
        }

        if(traffic->m_timRealReachStation.isNull() && traffic->m_btBeginOrEndFlg != JFC_TYPE_SF)
            mQFormList->SetItemTextColor(index, 7, Qt::black);

        if(traffic->m_timRealDepaTrain.isNull() && traffic->m_btBeginOrEndFlg != JFC_TYPE_ZD)
            mQFormList->SetItemTextColor(index, 28, Qt::black);
        index++;
    }
}

void TrafficLogManageKSK::AddItemData(QVector<QVector<int>>* itemPos, QVector<QString>* itemName, int row, int col, QString str)
{
    QVector<int> pos;
    pos.append(row);
    pos.append(col);
    itemPos->append(pos);
    itemName->append(str);
}

void TrafficLogManageKSK::SetTableHeadVisable(bool b)
{
    ui->frame_10->setHidden(!b);
}
//预告
void TrafficLogManageKSK::AdjacentStationPreview()
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }
    HeraldItemInfo info;

    info.heraldCol = 26;
    //标红
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    if(plog->m_btBeginOrEndFlg == JFC_TYPE_ZD)
        return;
    info.logId = plog->log_id;
    if(plog->m_nLHFlg == 1)
        info.color = Qt::red;
    else info.color = Qt::blue;
    info.colorTimer = startTimer(5000);
    mQFormList->setCellBackgroundColor(0x33, items[0]->row(), info.heraldCol, info.color);
    if(!plog->m_timToAdjtStaAgrDepaTrain.isNull())
    {
        mQFormList->setCellBackgroundColor(0x33, items[0]->row(), info.heraldCol, QColor(240, 240, 240));
    }
    verHeraldItemInfo.append(info);

    //与邻站信息显示
    ui->textBrowser_2->append(QString("%1,%2,要牌预告-%3")
                              .arg(pCurStation->m_strStationName)
                              .arg(plog->m_strTrainNum)
                              .arg(QDateTime::currentDateTime().toString("hh:mm")));
    ui->textBrowser_2->append(QString("%1,同意%2次发车-%3")
                              .arg(pCurStation->m_strStationName)
                              .arg(plog->m_strTrainNum)
                              .arg(QDateTime::currentDateTime().toString("hh:mm")));
    //发送命令
    emit sendTrafficLogMsg(plog->log_id, 0x01, QDateTime::currentDateTime());
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}
//同意
void TrafficLogManageKSK::AgreeAdjacentStations(QDateTime dateTime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    //取消标红
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        if(plog->log_id == verHeraldItemInfo.at(i).logId && verHeraldItemInfo[i].heraldCol == 4)
        {
            mQFormList->setCellBackgroundColor(0x33, items[0]->row(), verHeraldItemInfo[i].heraldCol, QColor(240, 240, 240));
            verHeraldItemInfo.remove(i);
        }
    }
    //与邻站信息显示

    ui->textBrowser_2->append(QString("%1,同意%2次接车-%3")
                              .arg(pCurStation->m_strStationName)
                              .arg(plog->m_strTrainNum)
                              .arg(QDateTime::currentDateTime().toString("hh:mm")));
    //发送命令
    if(dateTime.isNull())
        emit sendTrafficLogMsg(plog->log_id, 0x02, QDateTime::currentDateTime());
    else emit sendTrafficLogMsg(plog->log_id, 0x02, dateTime);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}
//到达
void TrafficLogManageKSK::TrainArrival(QDateTime datetime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
//        qDebug() << selectedTrainNo << selectedTrainNoIndex;
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //显示时间
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    QDateTime date;
    if(datetime.isNull())
        date = QDateTime::currentDateTime();
    else date = datetime;

    //与邻站信息显示
    ui->textBrowser_2->append(QString("%1,%2次到达-%3")
                              .arg(pCurStation->m_strStationName)
                              .arg(plog->m_strTrainNum)
                              .arg(date.toString("hh:mm")));
    //发送命令
    emit sendTrafficLogMsg(plog->log_id, 0x03, date);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}
//出发
void TrafficLogManageKSK::TrainDeparture(QDateTime datetime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //显示时间
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    QDateTime date;
    if(datetime.isNull())
        date = QDateTime::currentDateTime();
    else date = datetime;

    //文字颜色改为红色
    for(int i = 0; i < nCol; i++)
    {
        mQFormList->SetItemTextColor(items[0]->row(), i, Qt::red);
    }
    //与邻站信息显示
    ui->textBrowser_2->append(QString("%1,%2次出发-%3")
                              .arg(pCurStation->m_strStationName)
                              .arg(plog->m_strTrainNum)
                              .arg(date.toString("hh:mm")));
    //发送命令
    emit sendTrafficLogMsg(plog->log_id, 0x04, date);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}
//通过
void TrafficLogManageKSK::TrainPassThrough(QDateTime datetime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //显示时间
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    if(plog->m_btBeginOrEndFlg == JFC_TYPE_SF) //始发
    {
        return TrainDeparture(datetime);
    }
    else if(plog->m_btBeginOrEndFlg == JFC_TYPE_ZD) //终到
    {
        return TrainArrival(datetime);
    }
    QDateTime date;
    if(datetime.isNull())
        date = QDateTime::currentDateTime();
    else date = datetime;
    //文字颜色改为红色
    for(int i = 0; i < nCol; i++)
    {
        mQFormList->SetItemTextColor(items[0]->row(), i, Qt::red);
    }
    //与邻站信息显示
    ui->textBrowser_2->append(QString("%1,%2次通过-%3")
                              .arg(pCurStation->m_strStationName)
                              .arg(plog->m_strTrainNum)
                              .arg(date.toString("hh:mm")));
    //发送命令
    emit sendTrafficLogMsg(plog->log_id, 0x05, date);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}
//邻站
void TrafficLogManageKSK::AdjacentStation(QDateTime dateTime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //取消标红
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        HeraldItemInfo info = verHeraldItemInfo.at(i);
        for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
        {
            if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
            {
                mQFormList->setCellBackgroundColor(0x33, j, info.heraldCol, QColor(240, 240, 240));
                verHeraldItemInfo.remove(i);
            }
        }
    }

    //发送命令
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    if(dateTime.isNull())
        emit sendTrafficLogMsg(plog->log_id, 0x06, QDateTime::currentDateTime());
    else emit sendTrafficLogMsg(plog->log_id, 0x06, dateTime);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}

void TrafficLogManageKSK::CancelReceiving()
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }
    //发送命令
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    emit sendTrafficLogMsg(plog->log_id, 0x07, QDateTime::currentDateTime());
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}

void TrafficLogManageKSK::CancelBlock()
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //取消标红
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        HeraldItemInfo info = verHeraldItemInfo.at(i);
        for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
        {
            if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId && info.heraldCol == 26)
            {
                if(lastSelectedRow == j)
                    mQFormList->setCellBackgroundColor(0x33, j, info.heraldCol, QColor(5, 125, 255));
                else mQFormList->setCellBackgroundColor(0x33, j, info.heraldCol, QColor(240, 240, 240));
                verHeraldItemInfo.remove(i);
            }
        }
    }
    //发送命令
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    emit sendTrafficLogMsg(plog->log_id, 0x08, QDateTime::currentDateTime());
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}

void TrafficLogManageKSK::CancelDepart()
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }
    //发送命令
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    emit sendTrafficLogMsg(plog->log_id, 0x09, QDateTime::currentDateTime());
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}

void TrafficLogManageKSK::AdjacentStationDepart(QDateTime dateTime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //发送命令
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    if(dateTime.isNull())
        emit sendTrafficLogMsg(plog->log_id, 0x0C, QDateTime::currentDateTime());
    else emit sendTrafficLogMsg(plog->log_id, 0x0C, dateTime);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}
//邻站到达
void TrafficLogManageKSK::AdjacentStationArrival(QDateTime dateTime)
{
    QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly);
    if(items.isEmpty())
    {
        QMessageBox::information(nullptr,"CTC-STPC","请选定车次");
        return;
    }

    //发送命令
    TrafficLog* plog = pCurStation->m_ArrayTrafficLog.at(items[0]->row());
    if(dateTime.isNull())
        emit sendTrafficLogMsg(plog->log_id, 0x0D, QDateTime::currentDateTime());
    else emit sendTrafficLogMsg(plog->log_id, 0x0D, dateTime);
    selectedTrainNo.clear();
    selectedTrainNoIndex = -1;
}

void TrafficLogManageKSK::AddNewTrafficLog()
{
    NewTraffclogWidget* pNewTraffclog = new NewTraffclogWidget(this);
    connect(pNewTraffclog, &NewTraffclogWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    pNewTraffclog->setWindowTitle(QString("新增列车信息"));
    connect(pNewTraffclog,&NewTraffclogWidget::addNewTraffclog,[=](TrafficLog pTrafficLog){
        emit updataTrafficLog(0x00, pTrafficLog);
    });
    pNewTraffclog->Init(pCurStation);
    pNewTraffclog->show();
}

void TrafficLogManageKSK::ColumnDisplaySet()
{
    pColumnDisplaySet->UpDataState();
    pColumnDisplaySet->show();
}

void TrafficLogManageKSK::SearchTrainNo()
{
    SearchTrainNoWidget* pSearchTrainNo = new SearchTrainNoWidget(this);
    connect(pSearchTrainNo, &SearchTrainNoWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    connect(pSearchTrainNo,&SearchTrainNoWidget::searchTrainNum,[=](QString trainNum){
        QList<QTableWidgetItem*> items = mQFormList->getTableWidgetFormType(0x22)->findItems(trainNum, Qt::MatchExactly);
        if(items.size() > 0)
        {
            mQFormList->getTableWidgetFormType(0x33)->selectRow(items[0]->row());
            mQFormList->getTableWidgetFormType(0x22)->selectRow(items[0]->row());
            selectedTrainNo = items[0]->text();
            selectedTrainNoIndex = items[0]->row();
        }
        pSearchTrainNo->close();
    });

    pSearchTrainNo->show();
}

bool TrafficLogManageKSK::isSelectedRow()
{
    return mQFormList->getTableWidgetFormType(0x22)->findItems(selectedTrainNo, Qt::MatchExactly).isEmpty();
}

void TrafficLogManageKSK::AddDispatchDeskMsg(QString msg)
{
    ui->textBrowser->append(msg);
}

void TrafficLogManageKSK::ShowMeetGiveWayPlanWidget()
{
    MeetGiveWayPlan* pMeetGiveWayPlan = new MeetGiveWayPlan(this);
    connect(pMeetGiveWayPlan, &MeetGiveWayPlan::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    pMeetGiveWayPlan->show();
}

void TrafficLogManageKSK::timerEvent(QTimerEvent *event)
{
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        if(event->timerId() == verHeraldItemInfo[i].colorTimer)
        {
            killTimer(verHeraldItemInfo[i].colorTimer);
            for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
            {
                if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
                {
                    mQFormList->setCellBackgroundColor(0x33, j, verHeraldItemInfo.at(i).heraldCol, QColor(240, 240, 240));
                }
            }
            verHeraldItemInfo.remove(i);
        }
    }

    for(int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
    {
        TrafficLog* pTrafficLog = pCurStation->m_ArrayTrafficLog.at(i);
        if(!pTrafficLog) return;
        if(event->timerId() == pTrafficLog->m_nTimerId)
        {
            int index = mQFormList->getDataIndexByText(0x22, 0, pTrafficLog->m_strTrainNum);
            if(pTrafficLog->m_bFlickerFlag)
            {
                mQFormList->setCellBackgroundColor(0x22, index, 0, Qt::red);
            }
            else
            {
                mQFormList->setCellBackgroundColor(0x22, index, 0, QColor(240, 240, 240));
            }
        }
    }
}

void TrafficLogManageKSK::ColorClear()
{
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
        {
            if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
            {
                mQFormList->setCellBackgroundColor(0x33, j, verHeraldItemInfo[i].heraldCol, QColor(240, 240, 240));
            }
        }
    }
    verHeraldItemInfo.clear();
}

bool TrafficLogManageKSK::isSetBkColor(int row)
{
    for(int i = 0; i < verHeraldItemInfo.size(); i++)
    {
        for(int j = 0; j < pCurStation->m_ArrayTrafficLog.size(); j++)
        {
            if(pCurStation->m_ArrayTrafficLog.at(j)->log_id == verHeraldItemInfo.at(i).logId)
            {
                if(row == j) return true;

            }
        }
    }
    return false;
}

void TrafficLogManageKSK::TextClear()
{
    ui->textBrowser->clear();
    ui->textBrowser_2->clear();
}

void TrafficLogManageKSK::AdjacentStationReportingPoint(int type, int logId)
{
    if(type == 1)
    {
        HeraldItemInfo info;
        for(int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
        {
            if(logId == pCurStation->m_ArrayTrafficLog.at(i)->log_id)
            {
                if(pCurStation->m_ArrayTrafficLog.at(i)->m_btBeginOrEndFlg != JFC_TYPE_SF)
                {
                    info.logId = logId;
                    if(pCurStation->m_ArrayTrafficLog.at(i)->m_nLHFlg == 1)
                        info.color = Qt::red;
                    else info.color = Qt::blue;
                    //info.colorTimer = startTimer(5000);
                    info.heraldCol = 4;
                    verHeraldItemInfo.append(info);
                    SetCellbkColor(logId, 4, true);
                }
                break;
            }
        }
    }
    else if(type == 2)
    {
        //取消标红
        for(int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
        {
            if(logId == pCurStation->m_ArrayTrafficLog.at(i)->log_id)
            {
                for(int j = 0; j < verHeraldItemInfo.size(); j++)
                {
                    HeraldItemInfo info = verHeraldItemInfo.at(j);
                    if(info.logId == logId && info.heraldCol == 26)
                    {
                        mQFormList->setCellBackgroundColor(0x33, i, info.heraldCol, QColor(240, 240, 240));
                        verHeraldItemInfo.remove(j);
                    }
                }
                SetCellbkColor(logId, 26, false);
            }
        }
    }
    else if(type == 3)
    {
        //取消标红
        for(int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
        {
            if(logId == pCurStation->m_ArrayTrafficLog.at(i)->log_id)
            {
                for(int j = 0; j < verHeraldItemInfo.size(); j++)
                {
                    HeraldItemInfo info = verHeraldItemInfo.at(j);
                    if(info.logId == logId && info.heraldCol == 4)
                    {
                        mQFormList->setCellBackgroundColor(0x33, i, info.heraldCol, QColor(240, 240, 240));
                        verHeraldItemInfo.remove(j);
                    }
                    SetCellbkColor(logId, 4, false);
                }
            }
        }

    }
}

void TrafficLogManageKSK::SetCellbkColor(int logId, int col, bool isMark)
{
    int row = 0;
    QColor color;
    for(int i = 0; i < pCurStation->m_ArrayTrafficLog.size(); i++)
    {
        if(logId == pCurStation->m_ArrayTrafficLog.at(i)->log_id)
        {
            row = i;
            if(!isMark)
                color = QColor(240, 240, 240);
            else if(pCurStation->m_ArrayTrafficLog.at(i)->m_nLHFlg == 1)
                color = Qt::red;
            else color = Qt::blue;
            break;
        }
    }

    mQFormList->setCellBackgroundColor(0x33, row, col, color);
}

void TrafficLogManageKSK::SetDutyInfo(QVector<UserInfo> vecUser)
{
    if(vecUser[0].nstate == 1)
    {
        if(vecUser[1].nstate == 2)
        {
            ui->dutyLabel1->setText(vecUser[0].strUserName);
            ui->dutyTimeLabel1->setText("06:00~");
            ui->dutyLabel2->setText("");
            ui->dutyTimeLabel2->setText("");
        }
        else
        {
            ui->dutyLabel1->setText(vecUser[0].strUserName);
            ui->dutyTimeLabel1->setText(QString("06:00~%1").arg(QDateTime::currentDateTime().toString("hh:MM")));
            ui->dutyLabel2->setText(vecUser[1].strUserName);
            ui->dutyTimeLabel2->setText(QString("%1~").arg(QDateTime::currentDateTime().toString("hh:MM")));
        }
    }
    else
    {
        if(vecUser[1].nstate == 2)
        {
            ui->dutyLabel1->setText(QString("%1(销)").arg(vecUser[0].strUserName));
            ui->dutyTimeLabel1->setText(QString("06:00~%1").arg(QDateTime::currentDateTime().toString("hh:MM")));
            ui->dutyLabel2->setText(QString("%1(销)").arg(vecUser[1].strUserName));
            ui->dutyTimeLabel2->setText(QString("%1~").arg(QDateTime::currentDateTime().toString("hh:MM")));
        }
        else
        {
            ui->dutyLabel1->setText(QString("%1(销)").arg(vecUser[0].strUserName));
            ui->dutyTimeLabel1->setText(QString("06:00~%1").arg(QDateTime::currentDateTime().toString("hh:MM")));
            ui->dutyLabel2->setText(vecUser[1].strUserName);
            ui->dutyTimeLabel2->setText(QString("%1~").arg(QDateTime::currentDateTime().toString("hh:MM")));
        }
    }
}

void TrafficLogManageKSK::AddReachTime(int minute)
{
    if(!pTimeMenuItem)
        return;
    if(pTimeMenuItem->row() >= pCurStation->m_ArrayTrafficLog.size())
        return;

    TrafficLog* traffic = pCurStation->m_ArrayTrafficLog[pTimeMenuItem->row()];
    if(!traffic)
        return;
    if(pTimeMenuItem->column() == 7)
    {
        traffic->m_timProvReachStation = traffic->m_timProvReachStation.addSecs(minute * 60);
        if(traffic->m_btBeginOrEndFlg == 0xaa || traffic->m_btBeginOrEndFlg == 0xdd)
            traffic->m_timProvDepaTrain = traffic->m_timProvDepaTrain.addSecs(minute * 60);
        if(traffic->m_btBeginOrEndFlg == 0xbb)
            return;
    }

    else if(pTimeMenuItem->column() == 28)
    {
        QDateTime tempTime;
        tempTime = traffic->m_timProvDepaTrain.addSecs(minute * 60);
        if(traffic->m_btBeginOrEndFlg == 0xdd)
            traffic->m_btBeginOrEndFlg = 0xaa;

        if(traffic->m_btBeginOrEndFlg == 0xcc)
            return;

        if(tempTime < traffic->m_timProvReachStation)
        {
            QMessageBox::information(this, tr("STPC"), QString("出发时间不能小于到达时间!"), tr("确定"));
            return;
        }
        traffic->m_timProvDepaTrain = tempTime;

    }
    if(traffic->m_timProvReachStation == traffic->m_timProvDepaTrain)
        traffic->m_btBeginOrEndFlg = 0xdd;

    emit updataTrafficLog(0x00, *traffic);
    if(m_bShowSignRouteSyn)
        TrafficLogUpData();
}

bool TrafficLogManageKSK::ItemDataChange(int col, QString itemData, TrafficLog* pTrafficLog)
{
    bool isSame = false;

    QDateTime date = QDateTime::currentDateTime();
    QString strTime = itemData;
    strTime.replace(".", ":");
    strTime.replace(",", ":");
    strTime.replace("。", ":");
    strTime.replace("，", ":");
    strTime.replace("：", ":");
    QTime time = QTime::fromString(itemData, "hh:mm");
    if(time.isValid())
        date.setTime(time);
    itemData.replace(" ", "");
    switch (col) {
    case 2: isSame = pTrafficLog->m_strXHD_JZk == itemData; break;
    case 3: isSame = pTrafficLog->m_strRecvTrainTrack == itemData; break;
    case 4: isSame = pTrafficLog->m_timAgrFromAdjtStaDepaTrain == date; break;
    case 5: isSame = pTrafficLog->m_timFromAdjtStaDepaTrain == date; break;
    case 7: isSame = pTrafficLog->m_timRealReachStation == date; break;
    case 24: isSame = pTrafficLog->m_strDepartTrainTrack == itemData; break;
    case 25: isSame = pTrafficLog->m_strXHD_CZk == itemData; break;
    case 26: isSame = pTrafficLog->m_timToAdjtStaAgrDepaTrain == date; break;
    case 28: isSame = pTrafficLog->m_timRealDepaTrain == date; break;
    case 29: isSame = pTrafficLog->m_timtoAdjtStation == date; break;
    case 46: isSame = (itemData == "电力" &&  pTrafficLog->m_bElectric == 0x11) || (itemData == "" &&  pTrafficLog->m_bElectric == 0); break;
    case 47: isSame = pTrafficLog->m_strNotes == itemData; break;
    case 48: isSame = (itemData == "列检" && pTrafficLog->m_btLJStatus) || (itemData == "" &&  !pTrafficLog->m_btLJStatus); break;
    case 49: isSame = (itemData == "交令" && pTrafficLog->m_btJALStatus) || (itemData == "" &&  !pTrafficLog->m_btJALStatus); break;
    case 50: isSame = (itemData == "交票" && pTrafficLog->m_btJPStatus) || (itemData == "" &&  !pTrafficLog->m_btJPStatus); break;
    case 51: isSame = (itemData == "列尾" && pTrafficLog->m_btLWStatus) || (itemData == "" &&  !pTrafficLog->m_btLWStatus); break;
    case 52: isSame = (itemData == "机车" && pTrafficLog->m_btJCStatus) || (itemData == "" &&  !pTrafficLog->m_btJCStatus); break;
    case 53: isSame = (itemData == "货检" && pTrafficLog->m_btHJStatus) || (itemData == "" &&  !pTrafficLog->m_btHJStatus); break;
    case 54: isSame = (itemData == "乘降" && pTrafficLog->m_btCJStatus) || (itemData == "" &&  !pTrafficLog->m_btCJStatus); break;
    case 55: isSame = (itemData == "上水" && pTrafficLog->m_btSSStatus) || (itemData == "" &&  !pTrafficLog->m_btSSStatus); break;
    case 56: isSame = (itemData == "摘挂" && pTrafficLog->m_btZGStatus) || (itemData == "" &&  !pTrafficLog->m_btZGStatus); break;
    case 57: isSame = (itemData == "换乘" && pTrafficLog->m_btHCStatus) || (itemData == "" &&  !pTrafficLog->m_btHCStatus); break;
    case 58: isSame = (itemData == "装卸" && pTrafficLog->m_btZXStatus) || (itemData == "" &&  !pTrafficLog->m_btZXStatus); break;
    case 59: isSame = (itemData == "吸污" && pTrafficLog->m_btXWStatus) || (itemData == "" &&  !pTrafficLog->m_btXWStatus); break;
    case 60: isSame = (itemData == "道口" && pTrafficLog->m_btDKStatus) || (itemData == "" &&  !pTrafficLog->m_btDKStatus); break;
    case 61: isSame = (itemData == "车号" && pTrafficLog->m_btCHStatus) || (itemData == "" &&  !pTrafficLog->m_btCHStatus); break;
    case 62: isSame = (itemData == "站务" && pTrafficLog->m_btZWStatus) || (itemData == "" &&  !pTrafficLog->m_btZWStatus); break;
    case 63: isSame = (itemData == "综控" && pTrafficLog->m_btZKStatus) || (itemData == "" &&  !pTrafficLog->m_btZKStatus); break;
    case 64: isSame = (itemData == "一级" && pTrafficLog->m_nLevelCX == 1) ||
            (itemData == "二级" && pTrafficLog->m_nLevelCX == 2) || (itemData == "三级" && pTrafficLog->m_nLevelCX == 3) ||
            (itemData == "超级" && pTrafficLog->m_nLevelCX == 4) || (itemData == "" && pTrafficLog->m_nLevelCX == 0); break;
    case 65: isSame = (itemData == "军运" && pTrafficLog->m_bArmy) || (itemData == "" &&  !pTrafficLog->m_bArmy); break;
    case 66: isSame = (itemData == "重点" && pTrafficLog->m_bImportant) || (itemData == "" &&  !pTrafficLog->m_bImportant); break;

    }
    return !isSame;
}
