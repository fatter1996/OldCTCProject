#include "routeorderwidget.h"
#include "ui_routeorderwidget.h"
#include <QPushButton>
#include <QMessageBox>
#pragma execution_character_set("utf-8")
RouteOrderWidget::RouteOrderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RouteOrderWidget)
{
    ui->setupUi(this);
    pTrainRouteOrder = ui->widget;
    //connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(on_pushButton_clicked()));
    Timer_ID_500 = startTimer(500);
}

RouteOrderWidget::~RouteOrderWidget()
{
    delete ui;
}

void RouteOrderWidget::AddRouteOrderData(MyStation* pStation)
{
    pCurStation = pStation;
    curPermission = pCurStation->m_nRoutePermission;
    curFCZKMode = pCurStation->m_nFCZKMode;
    curModeState = pCurStation->m_nModeState;
    ui->comboBox_2->addItem(pCurStation->m_strStationName);
    if(pCurStation->m_nRoutePermission != 1 || pCurStation->m_nFCZKMode || pCurStation->m_nModeState != 1)
    {
        pTrainRouteOrder->PermissionChange(true);
        ui->widget_2->PermissionChange(true);
    }
    else
    {
        pTrainRouteOrder->PermissionChange(false);
        ui->widget_2->PermissionChange(false);
    }
}

void RouteOrderWidget::RouteOrderTableUpData()
{
    pTrainRouteOrder->RouteOrderUpData();
    if(pCurStation->m_nRoutePermission != 1 || pCurStation->m_nFCZKMode || pCurStation->m_nModeState != 1)
    {
        ui->widget_2->PermissionChange(true);
    }
    else
    {
        ui->widget_2->PermissionChange(false);
    }
}

TrainRouteOrderWidget* RouteOrderWidget::GetTrainRouteOrderWidget()
{
    return pTrainRouteOrder;
}

void RouteOrderWidget::on_comboBox_3_activated(int index)
{
    switch (index)
    {
    case 0:
    {
        ui->widget->FontReSize(9, 24);
        ui->widget_2->getHookOrder()->FontReSize(9, 24);
        ui->widget_2->getShuntRouteOrder()->FontReSize(9, 24);
        emit ui->widget->SendTextAnalysisSignal(202, QCursor::pos(), "常规");
        break;
    }
    case 1:
    {
        ui->widget->FontReSize(12, 28);
        ui->widget_2->getHookOrder()->FontReSize(12, 28);
        ui->widget_2->getShuntRouteOrder()->FontReSize(12, 28);
        emit ui->widget->SendTextAnalysisSignal(202, QCursor::pos(), "大");
        break;
    }
    case 2:
    {
        ui->widget->FontReSize(15, 32, 1.2);
        ui->widget_2->getHookOrder()->FontReSize(15, 32, 1.2);
        ui->widget_2->getShuntRouteOrder()->FontReSize(15, 32, 1.2);
        emit ui->widget->SendTextAnalysisSignal(202, QCursor::pos(), "超大");
        break;
    }
    case 3:
    {
        ui->widget->FontReSize(18, 36, 1.4);
        ui->widget_2->getHookOrder()->FontReSize(18, 36, 1.4);
        ui->widget_2->getShuntRouteOrder()->FontReSize(18, 36, 1.4);
        emit ui->widget->SendTextAnalysisSignal(202, QCursor::pos(), "极大");
        break;
    }
    default:
    {
        ui->widget->FontReSize(9, 24);
        ui->widget_2->getHookOrder()->FontReSize(9, 24);
        ui->widget_2->getShuntRouteOrder()->FontReSize(9, 24);
        break;
    }
    }
    pTrainRouteOrder->RouteOrderUpData();
}

void RouteOrderWidget::on_comboBox_2_activated(const QString &arg1)
{
    if(pCurStation && (arg1 == pCurStation->m_strStationName))// && (pCurStation->m_nRoutePermission != 1)
    {
        //申请权限
        emit ui->widget->SendTextAnalysisSignal(202, QCursor::pos(), pCurStation->m_strStationName);
        emit ui->widget->sendRouteOrderMsg(MSGTYPE_PERMISSION, 0, 0xaa);
        ui->widget->ClearData();
        Timer_ID_500_2 = startTimer(500);
    }
}

void RouteOrderWidget::on_pushButton_clicked()
{
    if(pCurStation && (pCurStation->m_nRoutePermission == 1))
    {
        emit ui->widget->SendTextAnalysisSignal(201, QCursor::pos());
        //只读
        int ret = QMessageBox::information(this, tr("STPC"), tr("您是否要释放进路权限?"), tr("确定"), tr("取消"), "", 0);
        if(ret == 0)
        {
            emit ui->widget->SendTextAnalysisSignal(77, QCursor::pos());
            emit ui->widget->sendRouteOrderMsg(MSGTYPE_PERMISSION, 0, 0xbb);
            RouteOrderTableUpData();
        }
        else emit ui->widget->SendTextAnalysisSignal(78, QCursor::pos());
    }
}

void RouteOrderWidget::timerEvent(QTimerEvent* enent)
{
    if(enent->timerId() == Timer_ID_500)
    {
        if(pCurStation)
        {
            if(curPermission != pCurStation->m_nRoutePermission || curFCZKMode != pCurStation->m_nFCZKMode
                    || curModeState != pCurStation->m_nModeState)
            {
//                qDebug() << "RouteOrderTableUpData";
                RouteOrderTableUpData();
                curPermission = pCurStation->m_nRoutePermission;
                curFCZKMode = pCurStation->m_nFCZKMode;
                curModeState = pCurStation->m_nModeState;
            }
        }
    }
    if(enent->timerId() == Timer_ID_500_2)
    {
        RouteOrderTableUpData();
        killTimer(Timer_ID_500_2);
    }
}
