#include "trainrouteorderwidget.h"
#include "ui_trainrouteorderwidget.h"
#include <QMenu>
#include <QMessageBox>
#pragma execution_character_set("utf-8")

TrainRouteOrderWidget::TrainRouteOrderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainRouteOrderWidget)
{
    ui->setupUi(this);
    mCTableViewTrain = new TableView();

    InitTableViewTrain();
    mCTableViewTrain->SetTableHeadHideV(false);
    mCTableViewTrain->SetTableHeadHideH(true);
    mCTableViewTrain->CreatTableView(false);
    ui->groupBox->layout()->addWidget(mCTableViewTrain);
}

TrainRouteOrderWidget::~TrainRouteOrderWidget()
{
    delete ui;
}

void TrainRouteOrderWidget::InitTableViewTrain()
{
    mCTableViewTrain->mTableViewInfo.Init();

    //初始化表头
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(32);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("车次");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("股道");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(72);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("自触");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(48);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("方向");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(108);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("开始");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("计划");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(64);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("状态");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("进路描述");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(108);
    mCTableViewTrain->SetSectionText("序", 32);
    mCTableViewTrain->SetTableHeadHeight(0, 24);
    mCTableViewTrain->SetTableHeadHeight(1, 24);


    //初始化第一行为空行
    QStringList strList;
    strList<<"1"<<""<< ""<< ""<< ""<< ""<< ""<< ""<< "";
    mCTableViewTrain->IintTableColAndRow(1, 9);
    mCTableViewTrain->SetTableData(strList);
    mCTableViewTrain->SetRowItemsBKColor(0, QColor(240,240,240), 0, 1);
    mCTableViewTrain->SetRowItemsBKColor(0, QColor(250,250,250), 1);

    //设置单元格不可编辑
    mCTableViewTrain->SetEditTriggers(QAbstractItemView::NoEditTriggers);

    SetTableStyleSheet();
}

void TrainRouteOrderWidget::SetTableStyleSheet()
{
    mCTableViewTrain->SetTableHeadsetStyleSheet(0, QString(
        "QTableView{"
            "outline:none;"
            "gridline-color: rgb(200, 200, 200);"
//            "font-size:%1pt;"
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
        "QTableView::item:selected {"
            "outline:none;"
            "background:rgb(5, 125, 255);"
        "}").arg(nFontSize));

    mCTableViewTrain->SetTableHeadsetStyleSheet(1, QString(
        "QHeaderView{"
            "font-size:%1pt;"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: %1pt;"
            "font-weight: normal;"
        "}").arg(nFontSize));


//    mCTableViewTrain->SetTableHeadsetStyleSheet(2, QString(
//        "QHeaderView{"
//            "font-size: %1pt;"
//            "background:rgb(240, 240, 240);"
//        "}"
//        "QHeaderView::section:vartical {"
//            "background:rgb(240, 240, 240);"
//            "font-size: %1pt;"
//            "font-weight: normal;"
//        "}").arg(nFontSize));
}

void TrainRouteOrderWidget::InitTableViewMenu(MyStation* pStation)
{
    pMyStation = pStation;
    connect(mCTableViewTrain,&TableView::itemLBtnClicked,this, &TrainRouteOrderWidget::TableViewLeftMenuInit);
    connect(mCTableViewTrain,&TableView::itemRBtnClicked,this, &TrainRouteOrderWidget::TableViewRightMenuInit);
}

void TrainRouteOrderWidget::TableViewLeftMenuInit(const QModelIndex &index)
{
    if(index.column() == 0)
    {
        mCTableViewTrain->SelectRow(index.row());
    }
    if(pMyStation->m_nFCZKMode || (pMyStation->m_nRoutePermission != 1) ||
            (pMyStation->m_ArrayRouteOrder.size() == 0) || (pMyStation->m_nModeState != 1))
        return;

    TrainRouteOrder* pRouteOrder = getRouteOrderByRowIndex(index.row());

    if(index.column() == 2)
    {
        if(pRouteOrder->father_id != 0)
            return;
        QMenu* pMenu = new QMenu();
        QAction* myAc1 = new QAction("变更股道", mCTableViewTrain);
        myAc1->setEnabled(false);
        pMenu->addAction(myAc1);
        pMenu->addSeparator();
        for(int i = 0; i < pMyStation->DevArray.size(); i++)
        {
            if(pMyStation->DevArray[i]->getDevType() == Dev_GD)
            {
                CGD* pGD=(CGD*)(pMyStation->DevArray[i]);
                if(pGD->pCenter.x() < 0 || pGD->pCenter.y() < 0)
                {
                    continue;
                }
                if(pGD->m_nGDType == GD_QD)
                {
                    QAction* pAction = new QAction(pGD->m_strName, mCTableViewTrain);
                    pAction->setCheckable(true);
                    pMenu->addAction(pAction);
                    if(pGD->m_strName == index.data())
                    {
                       pAction->setChecked(true);
                       pAction->setEnabled(false);
                    }
                    connect(pAction, &QAction::triggered, [=](){
                        //QTableWidgetItem* item = mCTableViewTrain->Item(index.row(), index.column());
                        QVector<TrainRouteOrder> vecTempRouteOrder = pMyStation->getRouteOrderByTrainNum(pRouteOrder->m_strTrainNum, pRouteOrder->m_btBeginOrEndFlg);
                        bool bTouch = true;
                        bool bOperate = false;
                        for(int i = 0; i < vecTempRouteOrder.size(); i++)
                        {
                            if(vecTempRouteOrder.at(i).m_bZHJL)
                            {
                                for(int j = 0; j < vecTempRouteOrder.at(i).m_vecSonTrainRouteOrder.size(); j++)
                                {
                                    bTouch &= (vecTempRouteOrder.at(i).m_vecSonTrainRouteOrder.at(j)->m_nOldRouteState != 0 && vecTempRouteOrder.at(i).m_vecSonTrainRouteOrder.at(j)->m_nOldRouteState != 5);
                                    bOperate |= (vecTempRouteOrder.at(i).m_vecSonTrainRouteOrder.at(j)->m_nOldRouteState != 0 && vecTempRouteOrder.at(i).m_vecSonTrainRouteOrder.at(j)->m_nOldRouteState != 5);
                                }
                            }
                            else bOperate |= (vecTempRouteOrder.at(i).m_nOldRouteState != 0 && vecTempRouteOrder.at(i).m_nOldRouteState != 5);
                        }
                        if(bOperate)
                        {
                            QMessageBox::information(this, tr("STPC"), QString("进路状态不允许修改股道!").arg(pGD->m_strName), tr("确定"),  "", 0);
                            return;
                        }

                        emit sendRouteOrderMsg(MSGTYPE_TRACKINFO, pRouteOrder->route_id, pGD->m_nCode);
                        //item->setText(pAction->text());
                        //if(pRouteOrder->m_bZHJL)
                        //{
                        //    for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                        //    {
                        //        emit sendRouteOrderMsg(MSGTYPE_TRACKINFO, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, pGD->m_nCode);
                        //    }
                        //    emit sendRouteOrderMsg(MSGTYPE_TRACKINFO, pRouteOrder->route_id, pGD->m_nCode);
                        //}
                        //else emit sendRouteOrderMsg(MSGTYPE_TRACKINFO, pRouteOrder->route_id, pGD->m_nCode);
                        emit SendTextAnalysisSignal(95, QCursor::pos());
                    });
                }
            }
        }
        pMenu->exec(QCursor::pos());
    }
    else if(index.column() == 3)
    {
        if(pRouteOrder->m_bZHJL == 1)
            return;
        QMenu* pMenu = new QMenu();
        QAction* myAc1 = new QAction("触发类型");
        myAc1->setEnabled(false);
        myAc1->setCheckable(true);
        pMenu->addAction(myAc1);
        pMenu->addSeparator();

        QAction* myAc2 = new QAction("设置自触");
        myAc2->setCheckable(true);
        pMenu->addAction(myAc2);
        connect(myAc2, &QAction::triggered, [=](){
             emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->route_id, 0xaa);
             emit SendTextAnalysisSignal(97, QCursor::pos());
        });

        QAction* myAc3 = new QAction("取消自触");
        myAc3->setCheckable(true);
        pMenu->addAction(myAc3);
        connect(myAc3, &QAction::triggered, [=](){
             emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->route_id, 0xbb);
             emit SendTextAnalysisSignal(124, QCursor::pos());
        });

        if(pRouteOrder->m_nAutoTouch)
        {
            myAc2->setChecked(true);
            myAc2->setEnabled(false);
        }
        else
        {
            myAc3->setChecked(true);
            myAc3->setEnabled(false);
        }
        pMenu->exec(QCursor::pos());
    }
    else if(index.column() == 8)
    {
        if(pRouteOrder->m_bZHJL == 1)
            return;
        QMenu* pMenu = new QMenu();
        for(int i = 0; i < pRouteOrder->strArrayBTJL.size(); i++)
        {
            QString str = pRouteOrder->strArrayBTJL.at(i);
            str.replace(',', '-');
            QAction* pAction = new QAction(str, mCTableViewTrain);
            pAction->setCheckable(true);
            pMenu->addAction(pAction);
            if(str == index.data())
            {
               pAction->setChecked(true);
               pAction->setEnabled(false);
            }
            connect(pAction, &QAction::triggered, [=](){
                 emit SendTextAnalysisSignal(127, QCursor::pos());
                 QTableWidgetItem* item = mCTableViewTrain->Item(index.row(), index.column());
                 //item->setText(pAction->text());
                 QString str = pRouteOrder->strArrayBTJL.at(i);
                 emit sendRouteOrderMsg(MSGTYPE_ROUTEDESCRIP, pRouteOrder->route_id, str.length(), str);
            });
        }
        pMenu->exec(QCursor::pos());
    }
}

void TrainRouteOrderWidget::TableViewRightMenuInit(const QModelIndex &index)
{
    if(pMyStation->m_nFCZKMode || (pMyStation->m_nRoutePermission != 1) ||
            (pMyStation->m_ArrayRouteOrder.size() == 0) || (pMyStation->m_nModeState != 1))
        return;
    TrainRouteOrder* pRouteOrder = getRouteOrderByRowIndex(index.row());

    QMenu* pMenu = new QMenu();
    QAction* myAc1 = new QAction("人工触发", mCTableViewTrain);
    pMenu->addAction(myAc1);
    connect(myAc1, &QAction::triggered, [=](){
         emit SendTextAnalysisSignal(96, QCursor::pos());
         QString routeDescrip;
         if(pRouteOrder->m_bZHJL != 1)
             routeDescrip = pRouteOrder->m_strRouteDescrip;
         else
         {
             for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
             {
                routeDescrip += pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_strRouteDescrip;
                routeDescrip += ",";
             }
             routeDescrip = routeDescrip.left(routeDescrip.length() - 1);
         }
         QString tips = QString("确定要触发'股道:%1','%2'次列车,'%3','%4'进路?")
                 .arg(pRouteOrder->m_strTrack)
                 .arg(pRouteOrder->m_strTrainNum)
                 .arg(routeDescrip)
                 .arg((pRouteOrder->m_btRecvOrDepart==0)?"接车":"发车");
         int ret = QMessageBox::question(nullptr,"信息对话框",tips,tr("是"),tr("否"),"",0);
         if(ret==0)
         {
             emit SendTextAnalysisSignal(77, QCursor::pos());
             if(pRouteOrder->m_bZHJL != 1)
                emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder->route_id, 0xaa);
             for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
             {
                emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, 0xaa);
             }
         }
         else emit SendTextAnalysisSignal(78, QCursor::pos());

    });

    QAction* myAc3 = new QAction("明细", mCTableViewTrain);
    pMenu->addAction(myAc3);
    connect(myAc3, &QAction::triggered, [=](){
        emit SendTextAnalysisSignal(125, QCursor::pos());
        pRouteOrderDetail = new RouteOrderDetail(this);
        connect(pRouteOrderDetail, &RouteOrderDetail::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
            emit SendTextAnalysisSignal(index, pt, dev);
        });
        pRouteOrderDetail->RouteOrderDataInit(pMyStation, pRouteOrder);
        pRouteOrderDetail->show();
    });
    QAction* myAc4 = new QAction("站场显示", mCTableViewTrain);
    pMenu->addAction(myAc4);
    myAc4->setEnabled(false);

    pMenu->addSeparator();
    QAction* myAc2 = new QAction("删除", mCTableViewTrain);
    pMenu->addAction(myAc2);

    connect(myAc2, &QAction::triggered, [=](){
         emit SendTextAnalysisSignal(126, QCursor::pos());
         if(!pRouteOrder)
             return;
         QString tips = QString("删除第%1条列车进路('股道:%2','%3'次列车,'%4'进路)?")
                 .arg(pRouteOrder->m_nRowIndex + 1)
                 .arg(pRouteOrder->m_strTrack)
                 .arg(pRouteOrder->m_strTrainNum)
                 .arg((pRouteOrder->m_btRecvOrDepart==0)?"接车":"发车");
         int ret = QMessageBox::question(nullptr,"信息对话框",tips,tr("是"),tr("否"),"",0);
         if(ret==0)
         {
             emit SendTextAnalysisSignal(77, QCursor::pos());
             emit sendRouteOrderMsg(MSGTYPE_DELETE, pRouteOrder->route_id, 0xaa);
             for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
             {
                 emit sendRouteOrderMsg(MSGTYPE_DELETE, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, 0xaa);
             }
             pRouteOrder->m_vecSonTrainRouteOrder.clear();
             pMyStation->m_ArrayRouteOrder.removeOne(pRouteOrder);
         }
         else emit SendTextAnalysisSignal(78, QCursor::pos());
    });
    pMenu->exec(QCursor::pos());
}

void TrainRouteOrderWidget::RouteOrderUpData()
{
    QStringList strList;
    int row = 0;
    if(pMyStation->m_ArrayRouteOrder.size() > 0)
    {
        RouteOrderSort(pMyStation->m_ArrayRouteOrder.begin(), pMyStation->m_ArrayRouteOrder.end() - 1);
    }

    for(int i = 0; i < pMyStation->m_ArrayRouteOrder.size(); i++)
    {
        RouteOrderTableDataUpData(pMyStation->m_ArrayRouteOrder.at(i), strList, row);
    }
    mCTableViewTrain->SetTableData(strList);


    for(int i = 0; i < pMyStation->m_ArrayRouteOrder.size(); i++)
    {
        RouteOrderTableColorUpData(pMyStation->m_ArrayRouteOrder.at(i));
    }


    //没有进路序列时显示空行
    if(pMyStation->m_ArrayRouteOrder.size() == 0)
    {
        QStringList strList;
        strList<<"1"<<""<< ""<< ""<< ""<< ""<< ""<< ""<< "";
        mCTableViewTrain->SetTableData(strList);
        mCTableViewTrain->SetRowItemsBKColor(0, QColor(240,240,240), 0, 1);
        mCTableViewTrain->SetRowItemsBKColor(0, QColor(250,250,250), 1);
    }

    //权限判断
    bool flag = pMyStation->m_nRoutePermission != 1 || pMyStation->m_nFCZKMode || pMyStation->m_nModeState != 1;
    PermissionChange(flag);

}

void TrainRouteOrderWidget::RouteOrderTableDataUpData(TrainRouteOrder* order, QStringList& strList, int& row)
{
    if(order->m_bDeleteFlag)
        return;
    strList << QString("%1").arg(row + 1);
    strList << order->m_strTrainNum;
    strList << order->m_strTrack << "";
    strList << order->m_strDirection;
    strList << order->m_timBegin.toString("hh:mm");
    strList << order->m_timPlanned.toString("hh:mm");
    if(order->m_bZHJL == 1)
    {
        strList << "";
        QString strTemp;
        for(int i = 0; i < order->m_vecSonTrainRouteOrder.size(); i++)
        {
            strTemp += order->m_vecSonTrainRouteOrder.at(i)->m_strRouteDescrip;
            if(i < order->m_vecSonTrainRouteOrder.size() - 1)
                strTemp += ",";
        }
        strList << strTemp;
    }
    else
    {
        strList << order->m_strRouteState;
        strList << order->m_strRouteDescrip;
    }



    order->m_nRowIndex = row;
    row++;
    for(int i = 0; i < order->m_vecSonTrainRouteOrder.size(); i++)
    {
        RouteOrderTableDataUpData(order->m_vecSonTrainRouteOrder.at(i), strList, row);
    }
}

void TrainRouteOrderWidget::RouteOrderTableColorUpData(TrainRouteOrder* order)
{
    QColor bkColor;
    QFont font("宋体", nFontSize, QFont::Weight::Bold, true);
    if(order->m_bDeleteFlag)
        return;

    if(order->m_bZHJL == 0)
    {
        if(order->m_nAutoTouch)
            mCTableViewTrain->SetItemCheck(order->m_nRowIndex, 3, Qt::Checked);
        else mCTableViewTrain->SetItemCheck(order->m_nRowIndex, 3, Qt::Unchecked);
    }

    //设置背景色和文字颜色
    switch (order->m_nOldRouteState)
    {
    case 0: bkColor = Qt::yellow; break;
    case 1: {
        bkColor = Qt::green;
        mCTableViewTrain->SetRowItemsTextFont(order->m_nRowIndex, font, 1);
    } break;
    case 2: bkColor = Qt::green; break;
    case 3: {
        bkColor = Qt::green;
        mCTableViewTrain->SetRowItemsTextFont(order->m_nRowIndex, font, 1);
    } break;
    case 4: bkColor = Qt::gray; break;
    case 5: bkColor = Qt::yellow; break;
    }

    QColor textColor;
    if(order->m_nLHFlg == 1)
        textColor = Qt::red;
    else textColor = Qt::black;

    if(order->m_btBeginOrEndFlg == 0x00)
    {
        bkColor = Qt::gray;
        textColor = Qt::black;
    }

    if(order->m_bZHJL == 1)
        mCTableViewTrain->SetRowItemsColor(order->m_nRowIndex, QColor(240, 0, 0), Qt::black, 0, 1);
    else mCTableViewTrain->SetRowItemsColor(order->m_nRowIndex, QColor(240, 240, 240), Qt::black, 0, 1);
    if(order->father_id != 0)
        mCTableViewTrain->SetRowItemsColor(order->m_nRowIndex, QColor(255, 190, 210), Qt::black, 0, 1);

    mCTableViewTrain->SetRowItemsColor(order->m_nRowIndex, bkColor, textColor, 1);
    for(int i = 0; i < order->m_vecSonTrainRouteOrder.size(); i++)
    {
        RouteOrderTableColorUpData(order->m_vecSonTrainRouteOrder.at(i));
    }
}

void TrainRouteOrderWidget::FontReSize(int fontSize, int height, double diploid)
{
    nFontSize = fontSize;
    SetTableStyleSheet();

    mCTableViewTrain->SetTableHeadHeight(1, height);
    mCTableViewTrain->SetTableHeadWidth(0, diploid);
    QFont font("宋体", nFontSize, QFont::Weight::Bold, true);
    mCTableViewTrain->FontReSize(nFontSize);
}

void TrainRouteOrderWidget::ClearData()
{
    mCTableViewTrain->Clear();
}

void TrainRouteOrderWidget::PermissionChange(bool bOnlyRead)
{
    if(bOnlyRead)
    {
        ui->groupBox->setTitle("列车进路序列 只读(不可修改)");
    }
    else
    {
        ui->groupBox->setTitle("列车进路序列 可修改");
    }
}

void TrainRouteOrderWidget::RouteOrderSort(QVector<TrainRouteOrder*>::iterator low, QVector<TrainRouteOrder*>::iterator high)
{
    QVector<TrainRouteOrder*>::iterator order1 = low;
    QVector<TrainRouteOrder*>::iterator order2 = high;
    if(high <= low)
    {
        return;
    }
    TrainRouteOrder* key = *low;
    while(1)
    {
        /*从左向右找比key大的值*/
        while (RouteOrderCompare(*order1, key))
        {
            order1++;
            if (order1 == high){
                break;
            }
        }

        /*从右向左找比key小的值*/

        while (RouteOrderCompare(key, *order2))
        {
            order2--;
            if (order2 == low){
                break;
            }
        }

        if (order1 >= order2) break;
        /*交换i,j对应的值*/
        TrainRouteOrder* temp = *order1;
        *order1 = *order2;
        *order2 = temp;
    }
    /*中枢值与j对应值交换*/
    *low = *order2;
    *order2 = key;
    RouteOrderSort(low, order2 - 1);
    RouteOrderSort(order2 + 1, high);
}

bool TrainRouteOrderWidget::RouteOrderCompare(TrainRouteOrder* order1, TrainRouteOrder* order2)
{
    if(order1->m_strTrainNum == order2->m_strTrainNum)
    {
        if(order1->m_btRecvOrDepart == order2->m_btRecvOrDepart)
        {
            return order1->m_timBegin.toTime_t() <= order2->m_timBegin.toTime_t();
        }
        else return order1->m_btRecvOrDepart == 0;
    }
    else return order1->m_timBegin.toTime_t() <= order2->m_timBegin.toTime_t();
}

TrainRouteOrder* TrainRouteOrderWidget::getRouteOrderByRowIndex(int row)
{
    TrainRouteOrder* routeOrder = nullptr;
    for(int i=0; i<pMyStation->m_ArrayRouteOrder.size(); i++)
    {
        if(row == pMyStation->m_ArrayRouteOrder[i]->m_nRowIndex)
        {
            routeOrder = pMyStation->m_ArrayRouteOrder[i];
            break;
        }
        if(pMyStation->m_ArrayRouteOrder[i]->m_bZHJL == 1)
        {
            for(int j=0; j<pMyStation->m_ArrayRouteOrder.at(i)->m_vecSonTrainRouteOrder.size(); j++)
            {
                if(row == pMyStation->m_ArrayRouteOrder.at(i)->m_vecSonTrainRouteOrder[j]->m_nRowIndex)
                {
                    routeOrder = pMyStation->m_ArrayRouteOrder.at(i)->m_vecSonTrainRouteOrder[j];
                    break;
                }
            }
        }
    }
    return routeOrder;
}
