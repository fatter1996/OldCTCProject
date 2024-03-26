#include "mydoc.h"
#include <QtDebug>
#include <QSettings>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QLayout>
#include "GlobalHeaders/GlobalFuntion.h"
#include "StationInterface/StationInterfaceKSK/stationinterfaceksk.h"

//设置中文
#pragma execution_character_set("utf-8")

//初始化主窗口信息
void MyDoc::InitMainWnd(QMainWindow *widget)
{
    //系统窗口类型-1卡斯柯，2卡斯柯集控，3铁科，4通号...
    pMainWindow = widget;
    switch (wndType) {
    case 1: pStationView = StationInterfaceKSK::CreatWnd((QWidget*)pMainWindow); break;
    case 2: break;
    case 3: break;
    case 4: break;
    default: break;
    }
    //pStationView->setFixedSize(pMainWindow->centralWidget()->size().width(), pMainWindow->centralWidget()->size().height() - 52);
    pStationView->setIsShowRoute(bRouteOrderShow);
    pStationView->setIsShowSignRouteSyn(bSignRouteSyn);
    MyStation* pMyStation = getCurrMyStationInStaArray();
    if(!pMyStation)
        return;
    pStationView->InitStationWnd(pMyStation);//getCurrMyStationInStaArray()
    pStationView->InitMenuMain();
    pStationView->InitStationViewToolBar();
    pStationView->InitTrafficLogToolBar();
    pStationView->InitSignForToolBar();

    switch (pMyStation->nShowView) {
    case 1: { pStationView->TurnToStationWnd(); pStationView->ShowStationCtrl(); } break;
    case 2: { pStationView->TurnToTrafficLog(); pStationView->ShowTrafficLog(); } break;
    case 3: { pStationView->TurnToMultiSWnd(); pStationView->ShowMultStation(); } break;
    default: { pStationView->TurnToStationWnd(); pStationView->ShowStationCtrl(); } break;
    }

    //pStationView->ShowStationCtrl();
    ((MainWindow*)pMainWindow)->addWidget(pStationView);
    connect(pStationView->GetStationCtrlDisp(),SIGNAL(SendDataToSecerSignal(int,int,int,int)),this,SLOT(SendStaOperDataSlot(int,int,int,int)));
    connect(pStationView->GetStationCtrlDisp(),&StationCtrlDisp::SendDataRouteToSecerSignal,this,&MyDoc::SendDataRouteToSecerSlot);
    connect(pStationView->GetStationCtrlDisp(),&StationCtrlDisp::sendRouteOrderMsg,this,&MyDoc::SendRouteOrder);
    //调度命令模板
    if(pStationView->GetDDOrderManageWidget()!=nullptr)
    {
        pStationView->GetDDOrderManageWidget()->mVectDisOrderTemplate=mVectDisOrderTemplate;
        pStationView->GetDDOrderManageWidget()->InitDisTemplate();
    }

    connect(pStationView->GetQJlogic(), &QJlogic::SendLogicCheck, this, &MyDoc::SendIntervalLogicCheck);
    connect(this, &MyDoc::RecvLogicCheck, pStationView->GetQJlogic(), &QJlogic::onRecvLogicCheck);

    InitAgainstWrongWnd();
    //阶段计划更新
    connect(this, SIGNAL(StagePlanUpData(bool)), pStationView, SLOT(onStagePlanUpData(bool)));
    //行车日志更新
    connect(this, SIGNAL(TrafficLogUpData()), pStationView, SLOT(onTrafficLogUpData()));
    //进路序列更新
    connect(this, SIGNAL(TrainRouteOrderUpData()), pStationView, SLOT(onTrainRouteOrderUpData()));
    //调度命令签收
    connect(this, SIGNAL(DDOrderManageUpData()), pStationView, SLOT(onDDOrderManageUpData()));
    //connect(this,SIGNAL(DDOrderSignSignal()),this,SLOT(DDOrderSignSolt()));
    //阶段计划签收
    connect(pStationView->GetStagePlanWidget(), &StagePlanWidget::signFor, [=](){
        SendSignFor();
        pStationView->stopStagePlanFlash();
    });

    connect(pStationView->GetSendPlanWidget(), &SendPlanWidget::sendPlan, [=](){
        SendSendPlan();
        pStationView->stopSendPlanFlash();
    });
    //站场重置
    connect(this, SIGNAL(ResetStationSignal()), pStationView, SLOT(ResetStationSlot()));
    //进路序列更新
    RouteOrderWidget* pRouteOrder = pStationView->GetRouteOrderWidget();
    pRouteOrder->AddRouteOrderData(getCurrMyStationInStaArray());
    pRouteOrder->GetTrainRouteOrderWidget()->InitTableViewMenu(getCurrMyStationInStaArray());

    connect(pRouteOrder->GetTrainRouteOrderWidget(),SIGNAL(sendRouteOrderMsg(int, int, int, QString)),this,SLOT(SendRouteOrder(int, int, int, QString)));

    //行车日志
    connect(pStationView->GetTrafficLogManage(),SIGNAL(sendTrafficLogMsg(int, int, QDateTime)),this, SLOT(SendTrafficLog(int, int, QDateTime)));
    connect(pStationView->GetTrafficLogManage(),SIGNAL(updataTrafficLog(int, TrafficLog)),this, SLOT(SendTrafficLogUpdata(int, TrafficLog)));
    connect(pStationView->GetTrafficLogManage(),SIGNAL(updataTrafficLogFlow(int, int, bool)),this, SLOT(SendTrafficLogFlowUpdata(int, int, bool)));
    connect(pStationView->GetTrafficLogManage(),SIGNAL(updataNotes(TrafficLog)),this, SLOT(onUpdataNotes(TrafficLog)));

    //用户登录
    connect(pStationView,SIGNAL(SendUserLongInExitSignal(int,QString,QString)),this,SLOT(SendUserLongInExitSlot(int,QString,QString)));
    //用户登录报警 void UserAlarm(int n);
    connect(this, SIGNAL(UserAlarmSignal(int)), pStationView, SLOT(UserAlarm(int)),Qt::QueuedConnection);
    //系统报警信息
    connect(this,SIGNAL(RecvServerAlarm(QByteArray,int)),pStationView/*->GetStationCtrlDisp()*/,SLOT(SendServerAlarm(QByteArray,int)),Qt::QueuedConnection);

    //调度命令签收
    connect(pStationView->GetDDOrderSignWidget(),SIGNAL(DispatchOrderStationSignSignal(int,bool)),this,SLOT(DispatchOrderStationSignSolt(int,bool)));
    connect(pStationView->GetDDOrderManageWidget(),SIGNAL(DispatchOrderStationSignSignal(int,bool)),this,SLOT(DispatchOrderStationSignSolt(int,bool)));
    //调度命令 转发至机车
    connect(pStationView->GetDDOrderManageWidget(),SIGNAL(ForWardDDOrderToTrain(DispatchOrderStation)),this,SLOT(ForWardDDOrderToTrainSolt(DispatchOrderStation)));
    //调度命令 发送至机车 SendOrderLocomotive
    connect(pStationView->GetDDOrderManageWidget(),SIGNAL(SendOrderLocomotive(DispatchOrderLocomotive*)),this,SLOT(SendOrderLocomotiveSolt(DispatchOrderLocomotive*)));
    //调度命令 发送至调度台 DispatchOrderDispatcher
    connect(pStationView->GetDDOrderManageWidget(),SIGNAL(SendOrderDispatcher(DispatchOrderDispatcher*)),this,SLOT(SendOrderDispatcherSolt(DispatchOrderDispatcher*)));
    //发送自动签收信息
    connect(pStationView, SIGNAL(SendAutoSignStateSignal(bool)), this, SLOT(SendAutoSignStateSolt(bool)));

    connect(pStationView->GetTextAnalysis(), SIGNAL(TextFramePacket(TextFrame)), this, SLOT(SendTextAnalysis(TextFrame)));

    //TextFramePacket
   // connect(pStationView->GetStationCtrlDisp(),&StationCtrlDisp::SysClosed,pMainWindow,&QMainWindow::close);
}

void MyDoc::InitAgainstWrongWnd()
{
    pAgainstWrongWidget = new AgainstWrongWidget(pStationView);
    connect(pAgainstWrongWidget, SIGNAL(sendAgainstWrongMsg(int, int)),this,SLOT(SendAgainstWrong(int, int)));

}

void MyDoc::InitMultiStationDisp()
{
    for(int i = 0; i < vectMultiStation.size(); i++)
    {
        pStationView->AddStationInfo(vectMultiStation.at(i), i);
    }
}
void MyDoc::SendStaOperDataSlot(int nDataType,int nFunType,int ndevcode,int nDevCode2)
{
    if(nDataType==0)//发送功能按钮数据给后台服务
    {
        SendFunButtonDataToCTCService(nFunType,ndevcode,nDevCode2);
    }
    else if(nDataType==1)//向服务端发送当前按下的信号机按钮
    {
        SendXHJPressType(nFunType,ndevcode);
    }
    else if(nDataType==2) //向服务端发送设备按下的消息
    {
         SendDevPressInfo();
    }
    else if(nDataType==3)
    {
        SendStationCtrModeChangeSlot(nFunType,ndevcode,nDevCode2);
    }
    else if(nDataType==4) //向服务端发送清除按钮按下的消息
    {
         SendClearBtnInfo();
    }
}
void MyDoc::SendDataRouteToSecerSlot(int nDtatType,int nFunType,int ndevcode[],QString str)
{
    if(nDtatType==0)
    {
        SendFunButtonDataToCTCService(nFunType,ndevcode,str);
    }

}



