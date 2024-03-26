#include "stationinterfaceksk.h"
#include "ui_stationinterfaceksk.h"
#pragma execution_character_set("utf-8")
#include <QtDebug>
#include <QPushButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QLabel>
#include <QSettings>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QLayout>
#include <QDateTime>
#include <QProcess>
StationInterfaceKSK::StationInterfaceKSK(StationInterface *parent) :
    StationInterface(parent),
    ui(new Ui::StationInterfaceKSK)
{
    ui->setupUi(this);

    ui->centralwidget->setMouseTracking(true);
    ui->stationWidget->setMouseTracking(true);

    ui->dockWidget->installEventFilter(this);
    pRouteOrderWidget = ui->routeOrderDockWinget;

    mSystemalarm=new SystemAlarm(this);
    mSystemalarm->hide();
    connect(mSystemalarm,&SystemAlarm::WidgetCloseSignal,[=](){
       // emit SysAlarm();
        MenuInfo*  menuInfo = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 0, 2);
        if(!mSystemalarm->isVisible())
        {
            menuInfo->pAction->setChecked(true);
        }
        else
        {
            mSystemalarm->hide();
            menuInfo->pAction->setChecked(false);
        }
    });


    QList<QDockWidget*> dockList;
    QList<int> widthList;
    dockList.append(ui->dockWidget);
    widthList.append(120);
    resizeDocks(dockList, widthList, Qt::Vertical);

    connect(ui->dockWidget,&QDockWidget::visibilityChanged,[=](){
        MenuInfo*  menuInfo = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 0, 3);
        if(ui->dockWidget->isVisible())
            menuInfo->pAction->setChecked(true);
        else menuInfo->pAction->setChecked(false);
    });

    mTextShowAttribute= new TextShowAttribute(this);
    connect(mTextShowAttribute, &TextShowAttribute::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });
    mTextShowAttribute->hide();

    pDDOrderSignWidget=new DDOrderSign();
    connect(pDDOrderSignWidget, &DDOrderSign::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    pStagePlanWidget = new StagePlanWidget();
    connect(pStagePlanWidget, &StagePlanWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });
    pStagePlanWidget->InitTableView();

    pSendPlanWidget = new SendPlanWidget();
    //connect(pSendPlanWidget, &pSendPlanWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
    //    pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    //});
    pSendPlanWidget->InitTableView();
    pSendPlanWidget->InitTableView2();

    m_trainmarshal=new TrainMarshal();
    m_stationstorage=new StationStorage();
    m_ccxnameedit=new ccxnameedit();
    m_QJlogic=new QJlogic(this);

    m_yt46=new yt46();
    m_tsrs=new tsrs();

    m_option=new option();

    mPassWordEntryDlg_NEW = new Dialog_PassWord(this);
    //mPassWordEntryDlg_NEW->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(mPassWordEntryDlg_NEW, &Dialog_PassWord::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    connect(m_option, &option::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    mAboutWidget = new AboutWidget(/*this*/);
    connect(mAboutWidget, &AboutWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    mStageChronicle =new StageChronicle();
    connect(mStageChronicle, &StageChronicle::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    mlogin =new LonInWidget();
    connect(mlogin, &LonInWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    muserexit=new UserExitWidget();
    connect(muserexit, &UserExitWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
    });

    //用户登录窗口信号槽绑定
    connect(mlogin,SIGNAL(LogInSignal(QString)),this,SLOT(LogInSolt(QString)));
    //用户退出窗口信号槽绑定
    connect(muserexit,SIGNAL(UserExitSignal(bool)),this,SLOT(UserExitSolt(bool)));
}

StationInterfaceKSK::~StationInterfaceKSK()
{
    delete ui;
}

StationInterface* StationInterfaceKSK::CreatWnd(QWidget* parent)
{
    return new StationInterfaceKSK((StationInterface *)parent);
}

StationCtrlDisp* StationInterfaceKSK::CreateStationCtrlDisp(QWidget *parent, MyStation* pMyStation)
{
    pStationCtrl = new StationCtrlDispKSK(parent);
    pStationCtrl->InitStationInfo(pMyStation);
    pStationCtrl->StationMenuAuxiliary();
    ui->stationWidget->layout()->addWidget(pStationCtrl);
    pStationCtrl->hide();
    pStationCtrl->InitViewSize(pStationCtrl->pCurrStation->StaConfigInfo.initialSize);
    pStationCtrl->InitSet();
    return pStationCtrl;
}

void StationInterfaceKSK::InitStationCtrlDisp()
{
    if(isShowRoutepreWnd)
        ui->dockWidget->show();
    else ui->dockWidget->hide();

    //文字属性窗口信号槽绑定
    connect(mTextShowAttribute,SIGNAL(SignalOk(QString,bool)),this,SLOT(TextShowSolt(QString,bool)));
    //文字属性窗口信号槽绑定
    connect(pStationCtrl,SIGNAL(ShowTextShowAttributeSignal()),this,SLOT(ShowTextShowAttribute()));
    //用户登录信息
    mUserInfoVect=pStationCtrl->pCurrStation->mUserInfoVectSTA;
    connect(pStationCtrl->pCurrStation,SIGNAL(UserUpDate(int,QString,int)),this,SLOT(UserUpDateSolt(int,QString,int)));
    //调度命令签收 拒绝信号    emit DDOrderSignSignal();
    connect(pDDOrderSignWidget,SIGNAL(DDOrderSignSignal()),this,SLOT(DDOrderSignSolt()));
    connect(pDDOrderManageWidget,SIGNAL(DDOrderSignSignal()),this,SLOT(DDOrderSignSolt()));

    //调度命令跟新 updateDisOrderSignal();
    connect(pStationCtrl->pCurrStation,SIGNAL(updateDisOrderSignal()),this,SLOT(updateDisOrderSolt()));
     connect(pStationCtrl->pCurrStation,SIGNAL(RecSeverDDMLSign()),this,SLOT(DDOrderSignSolt()));

     //阶段计划签收
     connect(pStationCtrl->pCurrStation,SIGNAL(RecSeverStagePlanSign()),this,SLOT(StagePlanSignSolt()));
    //自动签收
    connect(m_option,SIGNAL(AutoSignState(bool)),this,SLOT(AutoSignStateSolt(bool)));
//    //菜单退出
//    connect(this,&StationInterfaceKSK::MenuCloseSignal,this,&StationInterface::UserExitSolt);
    connect(pStationCtrl, &StationCtrlDispKSK::StaMenuTriggered, this, [=](int type, int flag)
    {
        switch (type) {
        case 1: ShowTextShowAttribute(); break;
        case 2: TextShowSolt("绝缘节", flag);; break;
        case 3: onStationViewScale(true, flag); break;
        case 4: onTrainNumWndScale(flag); break;
        case 5: onTrainNumWndShowStateChenge(flag); break;
        }
    });

    m_QJlogic->Init(CurrStation());
}

MultiStationDisp* StationInterfaceKSK::CreateMultiStationDisp(QWidget *parent, MyStation* pMyStation)
{
    pMultiStation = new MultiStationDispKSK(parent);

    ui->stationWidget->layout()->addWidget(pMultiStation);
    pMultiStation->hide();
    return pMultiStation;
}

TrafficLogManage* StationInterfaceKSK::CreateTrafficLogManage(QWidget *parent, MyStation* pMyStation)
{
    pTrafficLogManage = new TrafficLogManageKSK(pMyStation, parent);
    pTrafficLogManage->SetCurStation(pMyStation, isShowSignRouteSyn);
    ui->stationWidget->layout()->addWidget(pTrafficLogManage);
    pTrafficLogManage->hide();
    pTrainDiagram = CreateTrainDiagramWidget(parent, pMyStation);
    return pTrafficLogManage;
}

TrainDiagramWidget* StationInterfaceKSK::CreateTrainDiagramWidget(QWidget *parent, MyStation* pMyStation)
{
    pTrainDiagram = new TrainDiagramWidget(parent);
    pTrainDiagram->Init(pMyStation);
    ui->stationWidget->layout()->addWidget(pTrainDiagram);
    pTrainDiagram->hide();
    return pTrainDiagram;
}

DDOrderManage* StationInterfaceKSK::CreateDDOrderManage(QWidget *parent, MyStation* pMyStation)
{
    pDDOrderManageWidget=new DDOrderManage(this);
    pDDOrderManageWidget->hide();
    return pDDOrderManageWidget;
}

//初始化主菜单-卡斯柯
void StationInterfaceKSK::InitMenuMain()
{
    pMenuBar = new QMenuBar(this);
    MenuBarInfo* menuBarInfo1 = new MenuBarInfo;
    menuBarInfo1->pMenu = pMenuBar->addMenu(tr("登录(L)"));
    {
        QAction* pAction1 =  menuBarInfo1->addNewAction(this, 1, 0, tr("用户登录"));
        connect(pAction1, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                QStringList list;
                for (int i=0;i<mUserInfoVect.size();i++)
                {
                    list<<mUserInfoVect[i].strUserName;
                }

                pTextAnalysis->sendTextAnalysisMsg(46, mapFromGlobal(QCursor::pos()));
                ShowUserLogInExit(0,"","",list);
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        QAction* pAction2 =  menuBarInfo1->addNewAction(this, 1, 1, tr("用户注销"));
        connect(pAction2, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(47, mapFromGlobal(QCursor::pos()));
                ShowUserLogInExit(1,GetUserLogin(),GetUserLoginpassword(GetUserLogin()));
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        QAction* pAction3 =  menuBarInfo1->addNewAction(this, 1, 2, tr("退出"));
        connect(pAction3, &QAction::triggered, [=](){
            pTextAnalysis->sendTextAnalysisMsg(48, mapFromGlobal(QCursor::pos()));
            ShowUserLogInExit(2);
        });
    }
    m_vecMenuBarInfo.append(menuBarInfo1);

    MenuBarInfo* menuBarInfo2 = new MenuBarInfo;
    menuBarInfo2->pMenu = pMenuBar->addMenu(tr("显示(S)"));
    {
        menuBarInfo2->addNewAction(this, 1, 0, tr("工具栏"));
        {
            MenuInfo* menuInfo = menuBarInfo2->vecSubMenuInfo[0];
            QAction* pAction1 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 0, tr("标准按钮"));
            connect(pAction1, &QAction::triggered, [=](){
                pTextAnalysis->sendTextAnalysisMsg(187, mapFromGlobal(QCursor::pos()));
                if(pStationCtrl->pCurrStation->commToServer<10)
                {
                    if(pStationViewToolBar->isHidden())
                        pStationViewToolBar->show();
                    else pStationViewToolBar->hide();
                }
                else
                {
                    QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                    pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                }
            });

            QAction* pAction2 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 1, tr("签收栏"));
            connect(pAction2, &QAction::triggered, [=](){
                if(pStationCtrl->pCurrStation->commToServer<10)
                {
                    pTextAnalysis->sendTextAnalysisMsg(49, mapFromGlobal(QCursor::pos()));
                    if(pSignForToolBar->isHidden())
                        pSignForToolBar->show();
                    else pSignForToolBar->hide();
                }
                else
                {
                    QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                    pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                }
            });

            QAction* pAction3 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 2, tr("显示系统信息窗口"), true, true);
            connect(pAction3, &QAction::triggered, [=](){
                if(pStationCtrl->pCurrStation->commToServer<10)
                {
                    pTextAnalysis->sendTextAnalysisMsg(50, mapFromGlobal(QCursor::pos()));
                    ShowSystemAlarmWidget(pAction3->isChecked());
                }
                else
                {
                    pAction3->setChecked(!pAction3->isChecked());
                    QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                    pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                }
            });

            QAction* pAction4 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 3, tr("显示进路序列管理窗口"), true, true, isShowRoutepreWnd);
            connect(pAction4, &QAction::triggered, [=](){
                if(pStationCtrl->pCurrStation->commToServer<10)
                {
                    pTextAnalysis->sendTextAnalysisMsg(51, mapFromGlobal(QCursor::pos()));
                    if(pAction4->isChecked())
                        ui->dockWidget->show();
                    else ui->dockWidget->hide();
                }
                else
                {
                    pAction4->setChecked(!pAction4->isChecked());
                    QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                    pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                }
            });
        }

        menuBarInfo2->addNewAction(this, 1, 1, tr("站场图"));
        {
            MenuInfo* menuInfo = menuBarInfo2->vecSubMenuInfo[1];
            //站场界面
            {
                QAction* pAction1 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 0, tr("放大站场图"), true, true);
                QAction* pAction2 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 1, tr("缩小站场图"), true, true);
                QAction* pAction3 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 2, tr("还原站场图"), false);

                connect(pAction1, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        onStationViewScale(pAction1->isChecked(), 1);
                    }
                    else
                    {
                        pAction1->setChecked(!pAction1->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                connect(pAction2, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        onStationViewScale(pAction2->isChecked(), 2);
                    }
                    else
                    {
                        pAction2->setChecked(!pAction2->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                connect(pAction3, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        onStationViewScale(pStationCtrl->isStationViewReSize || pMultiStation->isStationViewReSize, 0);
                    }
                    else
                    {
                        pAction3->setChecked(!pAction3->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });
            }
            menuBarInfo2->addSeparator();
            //车次窗
            {
                QAction* pAction1 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 3, tr("放大车次窗"));
                QAction* pAction2 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 4, tr("缩小车次窗"));
                QAction* pAction3 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 5, tr("还原车次窗"), false);

                connect(pAction1, &QAction::triggered, [=](){

                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(191, mapFromGlobal(QCursor::pos()));
                        onTrainNumWndScale(1);
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                connect(pAction2, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(192, mapFromGlobal(QCursor::pos()));
                        onTrainNumWndScale(2);
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                connect(pAction3, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(193, mapFromGlobal(QCursor::pos()));
                        onTrainNumWndScale(0);
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });
            }
            menuBarInfo2->addSeparator();
            //显示内容
            {
                QAction* pAction1 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 6, tr("文字显示"));
                connect(pAction1, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(194, mapFromGlobal(QCursor::pos()));
                        ShowTextShowAttribute();
                    }
                    else
                    {
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                QAction* pAction2 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 7, tr("绝缘节显示"), true, true, pStationCtrl->bShow_Jyj);
                connect(pAction2, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(195, mapFromGlobal(QCursor::pos()));
                        TextShowSolt("绝缘节", pAction2->isChecked());
                    }
                    else
                    {
                        pAction2->setChecked(!pAction2->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                QAction* pAction3 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 8, tr("列车按钮显示"), true, true, pStationCtrl->bShow_LCAN);
                connect(pAction3, &QAction::triggered, [=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(196, mapFromGlobal(QCursor::pos()));
                        SetTrainBtnShow(pAction3->isChecked());
                    }
                    else
                    {
                        pAction3->setChecked(!pAction3->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                QAction* pAction4 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 9, tr("调车按钮显示"), true, true, pStationCtrl->bShow_DCAN);
                connect(pAction4,&QAction::triggered,[=](){
                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(197, mapFromGlobal(QCursor::pos()));
                        SetShuntingBtnShow(pAction4->isChecked());
                    }
                    else
                    {
                        pAction4->setChecked(!pAction4->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }
                });

                QAction* pAction5 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 10, tr("进路序列窗显示"), true, true, pStationCtrl->bShow_DCAN);
                connect(pAction5,&QAction::triggered,[=](){

                    if(pStationCtrl->pCurrStation->commToServer<10)
                    {
                        pTextAnalysis->sendTextAnalysisMsg(198, mapFromGlobal(QCursor::pos()));
                        onRoutePreWndShowStateChenge(pAction5->isChecked());
                    }
                    else
                    {
                        pAction5->setChecked(!pAction5->isChecked());
                        QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                        pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    }

                    pTextAnalysis->sendTextAnalysisMsg(198, mapFromGlobal(QCursor::pos()));
                    onRoutePreWndShowStateChenge(pAction5->isChecked());
                });

                menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 11, tr("显示状态条"), false, true, true);
                menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 12, tr("调车进路显示"), false, true, true);
                menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 13, tr("道岔位置"), false, true, true);
            }
        }
        menuBarInfo2->addNewAction(this, 1, 2, tr("行车日志"));
        {
            MenuInfo* menuInfo = menuBarInfo2->vecSubMenuInfo[2];

            QAction* pAction1 = menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 0, tr("显示表头"), true, true, true);
            connect(pAction1, &QAction::triggered, [=](){
                if(pStationCtrl->pCurrStation->commToServer<10)
                {
                    pTextAnalysis->sendTextAnalysisMsg(199, mapFromGlobal(QCursor::pos()));
                    pTrafficLogManage->SetTableHeadVisable(pAction1->isChecked());
                }
                else
                {
                    pAction1->setChecked(!pAction1->isChecked());
                    QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                    pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                }
            });

            menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 1, tr("显示邻站"), false);
            menuInfo->addSeparator();
            menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 2, tr("扩大"), false);
            menuInfo->addNewSubAction(menuBarInfo2->pMenu, 2, 2, tr("缩小"), false);
        }

    }
    m_vecMenuBarInfo.append(menuBarInfo2);

    MenuBarInfo* menuBarInfo3 = new MenuBarInfo;
    menuBarInfo3->pMenu = pMenuBar->addMenu(tr("工具(U)"));
    {
        QAction* pAction1 = menuBarInfo3->addNewAction(this, 1, 0, tr("调度命令管理"));
        connect(pAction1,&QAction::triggered,[=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(53, mapFromGlobal(QCursor::pos()));
                pDDOrderManageWidget->SetUserStaName(GetUserLogin(), CurrStation()->getStationName());
                pDDOrderManageWidget->SetOrderData(CurrStation()->m_ArrayDisOrderRecv, CurrStation()->m_ArrayDisOrderDisp, CurrStation()->m_ArrayDisOrderLocomot);
                pDDOrderManageWidget->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }

        });

        menuBarInfo3->addNewAction(this, 1, 1, tr("甩挂作业"), false);

        QAction* pAction2 = menuBarInfo3->addNewAction(this, 1, 2, tr("列车编组(速报表)"));
        connect(pAction2, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(54, mapFromGlobal(QCursor::pos()));
                m_trainmarshal->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        QAction* pAction3 = menuBarInfo3->addNewAction(this, 1, 3, tr("站存车(现存车)"));
        connect(pAction3, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(55, mapFromGlobal(QCursor::pos()));
                m_stationstorage->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        menuBarInfo3->addNewAction(this, 1, 4, tr("列控命令"), false);

        QAction* pAction4 = menuBarInfo3->addNewAction(this, 1, 5, tr("车站直接限速"));
        connect(pAction4, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(56, mapFromGlobal(QCursor::pos()));
                m_tsrs->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        QAction* pAction5 = menuBarInfo3->addNewAction(this, 1, 6, tr("运统46"));
        connect(pAction5, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(57, mapFromGlobal(QCursor::pos()));
                m_yt46->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        menuBarInfo3->addNewAction(this, 1, 7, tr("站场图回放"), false);

        QAction* pAction6 = menuBarInfo3->addNewAction(this, 1, 8, tr("列控区间占用逻辑检查"));
        connect(pAction6, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {

                mPassWordEntryDlg_NEW->strDescrib = "";
                mPassWordEntryDlg_NEW->strTittle = "请输入密码";
                mPassWordEntryDlg_NEW->InitPassWord();
                mPassWordEntryDlg_NEW->exec();
                if(mPassWordEntryDlg_NEW->onOK == true)
                {
                    m_QJlogic->Clear();
                    pTextAnalysis->sendTextAnalysisMsg(58, mapFromGlobal(QCursor::pos()));
                    m_QJlogic->show();
                }
                mPassWordEntryDlg_NEW->strTittle = "铅封按钮,请输入密码";
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        menuBarInfo3->addSeparator();
        menuBarInfo3->addNewAction(this, 1, 9, tr("用户管理"), false);

        QAction* pAction7 = menuBarInfo3->addNewAction(this, 1, 10, tr("选项"));
        connect(pAction7,&QAction::triggered,[=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(59, mapFromGlobal(QCursor::pos()));
                m_option->Init(0, CurrStation()->AutoSignStage);
                m_option->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        menuBarInfo3->addSeparator();
        menuBarInfo3->addNewAction(this, 1, 11, tr("确报查询"), false);
    }
    m_vecMenuBarInfo.append(menuBarInfo3);

    MenuBarInfo* menuBarInfo4 = new MenuBarInfo;
    menuBarInfo4->pMenu = pMenuBar->addMenu(tr("调试(D)"));
    {
        menuBarInfo4->addNewAction(this, 1, 0, tr("刷新站场图"), false);
        menuBarInfo4->addNewAction(this, 1, 1, tr("测试调度命令"), false);
        menuBarInfo4->addNewAction(this, 1, 2, tr("发送XML信息"), false);
        menuBarInfo4->addSeparator();
        QAction* pAction = menuBarInfo4->addNewAction(this, 1, 3, tr("语音静音"));
        connect(pAction, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(52, mapFromGlobal(QCursor::pos()));
                if(CurrStation()->tts->state()==QTextToSpeech::Speaking)
                {
                    CurrStation()->tts->pause();
                }
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

    }
    m_vecMenuBarInfo.append(menuBarInfo4);

    MenuBarInfo* menuBarInfo5 = new MenuBarInfo;
    menuBarInfo5->pMenu = pMenuBar->addMenu(tr("窗口(W)"));
    {
        menuBarInfo5->addNewAction(this, 1, 0, tr("层叠窗口"), false);
        menuBarInfo5->addNewAction(this, 1, 1, tr("显示调整"), false);
        menuBarInfo5->addSeparator();
        QAction* pAction = menuBarInfo5->addNewAction(this, 1, 2, tr("调车作业单"), false);
        connect(pAction, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(60, mapFromGlobal(QCursor::pos()));
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });

        menuBarInfo5->addSeparator();
        menuBarInfo5->addNewAction(this, 1, 3, tr("站场平面"), false);
        menuBarInfo5->addNewAction(this, 1, 4, tr("行车日志"), false);
        menuBarInfo5->addNewAction(this, 1, 5, tr("规章制度"), false);
        menuBarInfo5->addNewAction(this, 1, 6, tr("信号员控制面板"), false);
    }
    m_vecMenuBarInfo.append(menuBarInfo5);

    MenuBarInfo* menuBarInfo6 = new MenuBarInfo;
    menuBarInfo6->pMenu = pMenuBar->addMenu(tr("帮助(H)"));
    {
        QAction* pAction = menuBarInfo6->addNewAction(this, 1, 0, tr("关于CTC-STPC"));
        connect(pAction, &QAction::triggered, [=](){
            if(pStationCtrl->pCurrStation->commToServer<10)
            {
                pTextAnalysis->sendTextAnalysisMsg(200, mapFromGlobal(QCursor::pos()));
                mAboutWidget->show();
            }
            else
            {
                QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            }
        });
    }
    m_vecMenuBarInfo.append(menuBarInfo6);

    setMenuBar(pMenuBar);
}
//初始化工具栏-卡斯柯-主工具栏
void StationInterfaceKSK::InitStationViewToolBar()
{
    pStationViewToolBar = new QToolBar(this);

    //单站画面/站间透明
    QPushButton* btn1 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/1.png", "站间透明", true);
    QPushButton* btn2 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/2.png", "单站画面", true, true);
    connect(btn1, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(28, mapFromGlobal(QCursor::pos()));
            TurnToMultiSWnd();
            ShowMultStation();
        }
        else
        {
            btn1->setChecked(!btn1->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    connect(btn2, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(29, mapFromGlobal(QCursor::pos()));
            TurnToStationWnd();
            ShowStationCtrl();
        }
        else
        {
            btn2->setChecked(!btn2->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    //显示文字属性，点击后弹出如下所示的对话框打勾的选项表
    //示此设备的文字标注在站场图上显示。
    QPushButton* btn3 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/3.png", "文字");
    connect(btn3, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(30, mapFromGlobal(QCursor::pos()));
            ShowTextShowAttribute();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    pStationViewToolBar->addSeparator();
    //绝缘节显示/隐藏按钮：点击此按钮可以在站场图上显示或隐藏绝缘节。
    QPushButton* btn4 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/4.png", "绝缘节", true, true);
    connect(btn4, &QPushButton::clicked, [=](){
        pTextAnalysis->sendTextAnalysisMsg(31, mapFromGlobal(QCursor::pos()));
        TextShowSolt("绝缘节", btn4->isChecked());
    });

    //列车（通过）按钮显示/隐藏：点击此按钮可以在站场图上显示或隐藏列车（通过）按钮，列车按钮和通过按钮是方形按钮，
    QPushButton* btn5 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/5.png", "列车按钮", true, true);
    connect(btn5, &QPushButton::clicked, [=](){
        pTextAnalysis->sendTextAnalysisMsg(32, mapFromGlobal(QCursor::pos()));
        SetTrainBtnShow(btn5->isChecked());
    });

    //调车按钮显示/隐藏：点击此按钮可以在站场图上显示或隐藏调车按钮，调车按钮是圆形按钮
    QPushButton* btn6 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/6.png", "调车按钮", true, true);
    connect(btn6, &QPushButton::clicked, [=](){
        pTextAnalysis->sendTextAnalysisMsg(33, mapFromGlobal(QCursor::pos()));
        SetShuntingBtnShow(btn6->isChecked());
    });

    pStationViewToolBar->addSeparator();
    /*车次窗位置显示/隐藏：点击此按钮后站场图上出现可进行车
    次窗操作的线框，在线框中点击鼠标右键，可以出现车次号操作的
    菜单*/
    QPushButton* btn7 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/7.png", "车次", true, false);
    connect(btn7, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(34, mapFromGlobal(QCursor::pos()));
            onTrainNumWndShowStateChenge(btn7->isChecked());
        }
        else
        {
            btn7->setChecked(!btn7->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    /*进路窗显示/隐藏：当进路窗显示时，站场图上显示进路窗，
如下图所示：*/
    QPushButton* btn8 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/8.png", "进路窗", true, true);
    connect(btn8, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(35, mapFromGlobal(QCursor::pos()));
            onRoutePreWndShowStateChenge(btn8->isChecked());
        }
        else
        {
            btn8->setChecked(!btn8->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    pStationViewToolBar->addSeparator();
    /*文字标注：点击此按钮后，将鼠标点击到站场图的任何位置都会
出现一个白色框，在此框中则可以输入需要标注的文字，输入后按
回车键即可，需要删除时点击右键选择“删除”即可
*/
    QPushButton* btn9 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/9.png", "标注");
    connect(btn9, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(36, mapFromGlobal(QCursor::pos()));
            pStationCtrl->bTextAnnotation = true;
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    /*终端按钮闪烁：点击此按钮后，在办理进路时，点击始端按
钮后可供选择的终端按钮则会闪烁*/
    QPushButton* btn10 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/10.png", "终端按钮闪烁", true, true);
    connect(btn10, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            if(btn10->isEnabled())
            {
                pTextAnalysis->sendTextAnalysisMsg(37, mapFromGlobal(QCursor::pos()));
                pStationCtrl->bLCANFlash = btn10->isChecked();
                pStationCtrl->pCurrStation->SetXHJANShow(2, pStationCtrl->bLCANFlash);
            }
        }
        else
        {
            btn10->setChecked(!btn10->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    pStationViewToolBar->addSeparator();
    /*⑪⑫⑬站场图缩放操作按钮：第一个为放大；第二个为缩小；
第三个为还原，其中放大和缩小操作如下（以放大为例）：用鼠标左
键选中放大，此时此按钮变为凹下状态 ，将鼠标移至站场图上，
当鼠标形状为 时点击鼠标左键，站场图也就随之放大了。如果这
时要恢复鼠标为正常状态，只需将鼠标移至 点击使其呈凸出状态
即可。*/
    //放大站场图
    QPushButton* btn11 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/11.png", "放大", true, false);
    connect(btn11, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(38, mapFromGlobal(QCursor::pos()));
            onStationViewScale(btn11->isChecked(), 1);
        }
        else
        {
            btn11->setChecked(!btn11->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    //缩小站场图
    QPushButton* btn12 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/12.png", "缩小", true, false);
    connect(btn12, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(39, mapFromGlobal(QCursor::pos()));
            onStationViewScale(btn12->isChecked(), 2);
        }
        else
        {
            btn12->setChecked(!btn12->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    //还原
    QPushButton* btn13 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/19.png", "初始大小", false, false, "://Res/toolbar1/13.png");
    btn13->setEnabled(false);
    connect(btn13, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(40, mapFromGlobal(QCursor::pos()));
            onStationViewScale(pStationCtrl->isStationViewReSize || pMultiStation->isStationViewReSize, 0);
        }
        else
        {
            btn13->setChecked(!btn13->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    pStationViewToolBar->addSeparator();
    /*⑭⑮⑯车次号缩放操作按钮：第一个为放大；第二个为缩小；
第三个为还原，此操作针对站场图上所有的车次号。*/
    QPushButton* btn14 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/14.png", "车次窗放大");
    connect(btn14, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(41, mapFromGlobal(QCursor::pos()));
            onTrainNumWndScale(1);
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    QPushButton* btn15 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/15.png", "车次窗缩小");
    connect(btn15, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(42, mapFromGlobal(QCursor::pos()));
            onTrainNumWndScale(2);
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    QPushButton* btn16 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/16.png", "车次窗还原");
    btn16->setEnabled(false);
    connect(btn16, &QPushButton::clicked, [=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(43, mapFromGlobal(QCursor::pos()));
            onTrainNumWndScale(0);
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    pStationViewToolBar->addSeparator();
    /*查找车次号按钮：点击此菜单则出现如下对话框，其中罗列
了当前站场上的所有车次。用鼠标选中所要查找的车次后，点击确
认就可以找到此车次所在的画面。*/
    QPushButton* btn17 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/17.png", "查找车次窗");
    connect(btn17,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(44, mapFromGlobal(QCursor::pos()));
            pStationCtrl->ShowSeekTrain();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });


    /*行车日志切换按钮，点击此按钮后可在行车日志界面与站场图
界面进行切换*/
    QPushButton* btn18 = addToolBarBtn(pStationViewToolBar, m_vecStationViewToolBarBtn, "://Res/toolbar1/18.png", "返回到行车日志");
    connect(btn18,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(45, mapFromGlobal(QCursor::pos()));

            TurnToTrafficLog();
            if(nWndFlag2 == 1)
                ShowTrafficLog();
            else if(nWndFlag2 == 2)
                showTrainDiagramWidget();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });

    addToolBar(pStationViewToolBar);
    //添加换行
    addToolBarBreak(Qt::TopToolBarArea);
}
//初始化工具栏-卡斯柯-行车日志工具栏
void StationInterfaceKSK::InitTrafficLogToolBar()
{
    pTrafficLogToolBar = new QToolBar(this);
    //pTrafficLogToolBar->setStyleSheet("QPushButton{border-radius:0px; background-color: rgb(240, 240, 240);} "
    //                       "QPushButton:hover{background-color:rgba(85,170,235,120);border-radius: 3px;border: 1px solid rgba(85, 170, 255,160);}"
    //                       "QPushButton:pressed{background-color:rgba(25,160,245,120);border-radius: 3px;border: 1px solid rgba(25,160,245,160);}");

    QPushButton* btn1 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/打印.png", "打印");
    connect(btn1,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(135, mapFromGlobal(QCursor::pos()));
    });

    QPushButton* btn2 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/预览.png", "打印预览");
    connect(btn2,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(136, mapFromGlobal(QCursor::pos()));
    });

    pTrafficLogToolBar->addSeparator();

    QPushButton* btn3 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/图标转换.png", "图表转换", true);
    connect(btn3,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(137, mapFromGlobal(QCursor::pos()));

            if(nWndFlag2 == 1)
            {
                btn3->setChecked(true);
                showTrainDiagramWidget();
            }
            else
            {
                btn3->setChecked(false);
                ShowTrafficLog();
            }
        }
        else
        {
            btn3->setChecked(!btn3->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton* btn4 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/添加列车.png", "增加新车");
    connect(btn4,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(138, mapFromGlobal(QCursor::pos()));
            pTrafficLogManage->AddNewTrafficLog();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton* btn5 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/添加用户.png", "用户管理");
    connect(btn5,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(139, mapFromGlobal(QCursor::pos()));
    });

    QPushButton* btn6 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/历史运统.png", "历史报表查阅");
    connect(btn6,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(140, mapFromGlobal(QCursor::pos()));
    });

    QPushButton* btn7 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/视图.png", "视图");
    connect(btn7,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(141, mapFromGlobal(QCursor::pos()));
            pTrafficLogManage->ColumnDisplaySet();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton* btn8 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/显示下行车.png", "下行车次", true);
    QPushButton* btn9 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/显示上行车.png", "上行车次", true);
    QPushButton* btn10 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/显示上下行车.png", "上下行车次", true, true);

    connect(btn8,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(142, mapFromGlobal(QCursor::pos()));
            btn8->setChecked(true);
            btn9->setChecked(false);
            btn10->setChecked(false);
            pTrafficLogManage->setShowType(2);
            pTrafficLogManage->TrafficLogUpData();
        }
        else
        {
            btn8->setChecked(!btn8->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });
    connect(btn9,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(143, mapFromGlobal(QCursor::pos()));
            btn8->setChecked(false);
            btn9->setChecked(true);
            btn10->setChecked(false);
            pTrafficLogManage->setShowType(1);
            pTrafficLogManage->TrafficLogUpData();
        }
        else
        {
            btn9->setChecked(!btn9->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });
    connect(btn10,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(144, mapFromGlobal(QCursor::pos()));
            btn8->setChecked(false);
            btn9->setChecked(false);
            btn10->setChecked(true);
            pTrafficLogManage->setShowType(0);
            pTrafficLogManage->TrafficLogUpData();
        }
        else
        {
            btn10->setChecked(!btn10->isChecked());
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton* btn11 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/同意临站发车.png", "同意邻站发车");
    connect(btn11,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(145, mapFromGlobal(QCursor::pos()));
            pTrafficLogManage->AgreeAdjacentStations();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton* btn12 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/临站同意发车.png", "邻站同意发车");
    connect(btn12,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(146, mapFromGlobal(QCursor::pos()));
            pTrafficLogManage->AdjacentStation();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });


    QPushButton *btn13 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/会让计划.png", "会让计划");
    connect(btn13,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(147, mapFromGlobal(QCursor::pos()));
            pTrafficLogManage->ShowMeetGiveWayPlanWidget();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton *btn14 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/查找.png", "查找车次");
    connect(btn14,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(148, mapFromGlobal(QCursor::pos()));
            pTrafficLogManage->SearchTrainNo();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    pTrafficLogToolBar->addSeparator();

    QPushButton *btn15 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/站场图切换.png", "返回到站场图");
    connect(btn15,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(149, mapFromGlobal(QCursor::pos()));
            if(nWndFlag == 1)
                ShowStationCtrl();
            else ShowMultStation();
            pStationViewToolBar->show();
            pTrafficLogToolBar->hide();
            if(isShowRoutepreWnd)
                ui->dockWidget->show();
            else ui->dockWidget->hide();

            QAction* pLableCommServer = pSignForToolBar->findChild<QAction*>("lableCommServer");
            pLableCommServer->setVisible(false);
            QAction* pLableCommSynchronousSta = pSignForToolBar->findChild<QAction*>("lableCommSynchronousSta");
            pLableCommSynchronousSta->setVisible(true);
            QAction* pSendPlan = pSignForToolBar->findChild<QAction*>("sendPlan");
            pSendPlan->setVisible(false);
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
        }
    });

    QPushButton *btn16 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/调车作业单.png", "调车作业单");
    connect(btn16,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(150, mapFromGlobal(QCursor::pos()));
        //SetDiploid_CCC(0);
    });

    QPushButton *btn17 = addToolBarBtn(pTrafficLogToolBar, m_vecTrafficLogToolBarBtn, "://Res/toolbar2/帮助.png", "关于");
    connect(btn17,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(151, mapFromGlobal(QCursor::pos()));
    });

    addToolBar(pTrafficLogToolBar);
    //添加换行
    addToolBarBreak(Qt::TopToolBarArea);
    pTrafficLogToolBar->hide();
}
//初始化工具栏-卡斯柯-签收工具栏
void StationInterfaceKSK::InitSignForToolBar()
{
    pSignForToolBar = new QToolBar(this);
    pSignForToolBar->setStyleSheet("QPushButton{background-color: rgb(210, 210, 210);}"
                           "QLabel{background-color: rgb(240, 240, 240);}");

    QPushButton *m_btn1=new QPushButton(pSignForToolBar);
    m_btn1->setText("调度命令");
    m_btn1->setObjectName("DDOrderManage");
    m_btn1->setFixedSize(96, 28);
    connect(m_btn1,&QPushButton::clicked,[=](){
        pTextAnalysis->sendTextAnalysisMsg(24, mapFromGlobal(QCursor::pos()));
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            if(pStationCtrl->pCurrStation->GetDisOrderRecvByShow()!=nullptr)
            {
                //DDOrderManageFlash=false;
                m_btn1->setStyleSheet("");
                pDDOrderSignWidget->mDispatchOrderStation=pStationCtrl->pCurrStation->GetDisOrderRecvByShow();
                if(pDDOrderSignWidget->mDispatchOrderStation->nStateDisOrder==0x03
                        ||pDDOrderSignWidget->mDispatchOrderStation->nStateDisOrder==0x04)
                {
                    DDOrderManageFlash=false;
                    QMessageBox::warning(this, tr("STPC"), "没有收到调度命令",tr("确定"),/*tr("取消"),*/"",0);
                    pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                    return;
                }
                else
                {
                    DDOrderManageFlash=true;
                }
                pDDOrderSignWidget->Init(GetUserLogin());
                pDDOrderSignWidget->DDOrderUpDate();
                pDDOrderSignWidget->show();
            }
            else
            {
                QMessageBox::warning(this, tr("STPC"), "没有收到调度命令",tr("确定"),/*tr("取消"),*/"",0);
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
                return;
            }
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }

    });
    pSignForToolBar->addWidget(m_btn1);

    QLabel *m_lable = new QLabel(pSignForToolBar);
    m_lable->setFixedSize(160, 28);
    //m_lable->setStyleSheet("background-color: rgb(240, 240, 240);");
    m_lable->setFrameShape (QFrame::WinPanel);
    m_lable->setFrameShadow(QFrame::Sunken);
    m_lable->setEnabled(false);
    pSignForToolBar->addWidget(m_lable);

    if(pStationCtrl->pCurrStation->strZXBPath != "")
    {
        //m_btn2->hide();
        QPushButton *m_btn2=new QPushButton(pSignForToolBar);
        m_btn2->setText("占线板(F9)");
        m_btn2->setObjectName("BusyBoard");
        m_btn2->setFixedSize(96, 28);
        connect(m_btn2,&QPushButton::clicked,[=](){
            // 判断进程是否存在
            if(CurrStation()->strZXBPath == "")
                return ;
            QProcess tasklist;
            QString fileName = CurrStation()->strZXBPath.mid(CurrStation()->strZXBPath.lastIndexOf('\\') + 1);
            tasklist.start("tasklist",
                QStringList() << "/NH"
                << "/FO" << "CSV"
                << "/FI" << QString("IMAGENAME eq %1").arg(fileName));
            tasklist.waitForFinished();

            QString strOutput = tasklist.readAllStandardOutput();
            //如果进程不存在,则启动进程
            if (!strOutput.startsWith(QString("\"%1").arg(fileName)))
            {
                QProcess pLreleaseQM;
                pLreleaseQM.startDetached(CurrStation()->strZXBPath, QStringList(), CurrStation()->strZXBPath.left(CurrStation()->strZXBPath.lastIndexOf('\\') + 1));
                //pLreleaseQM.kill();
            }
        });
        pSignForToolBar->addWidget(m_btn2);
    }

    QPushButton *m_btn3=new QPushButton(pSignForToolBar);
    m_btn3->setText("阶段计划");
    m_btn3->setObjectName("stagePlan");
    m_btn3->setFixedSize(96, 28);
    //m_btn3->setStyleSheet("background-color: rgb(210, 210, 210);");
    connect(m_btn3,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(25, mapFromGlobal(QCursor::pos()));
            pStagePlanWidget->InitStation(CurrStation());
            pStagePlanWidget->StagePlanUpdata();
            pStagePlanWidget->show();
        }
        else
        {
            QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0);
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    QPushButton *m_btn4=new QPushButton(pSignForToolBar);
    m_btn4->setText("发送计划");
    m_btn4->setObjectName("SendPlan");
    m_btn4->setFixedSize(96, 28);
    m_btn4->setVisible(false);
    connect(m_btn4,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer < 10)
        {
            pTextAnalysis->sendTextAnalysisMsg(26, mapFromGlobal(QCursor::pos()));
            pSendPlanWidget->InitStation(CurrStation());
            pSendPlanWidget->SendPlanUpdata();
            pSendPlanWidget->show();

        }
        else
        {
            if(QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0) == 0)
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    //m_btn4->setStyleSheet("background-color: rgb(210, 210, 210);");

    QPushButton *m_btn5=new QPushButton(pSignForToolBar);
    m_btn5->setText("阶段记事");
    m_btn5->setFixedSize(96, 28);
    //m_btn5->setStyleSheet("background-color: rgb(210, 210, 210);");
    //mStageChronicle
    connect(m_btn5,&QPushButton::clicked,[=](){
        if(pStationCtrl->pCurrStation->commToServer<10)
        {
            pTextAnalysis->sendTextAnalysisMsg(27, mapFromGlobal(QCursor::pos()));
            mStageChronicle->show();
        }
        else
        {
            if(QMessageBox::critical(this, tr("错误"), tr(COMMUNICATION_TIPS),tr("确定"),"",0) == 0)
                pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
        }
    });
    QLabel *m_lableDateTime=new QLabel(pSignForToolBar);
    m_lableDateTime->setObjectName("currDateTimeLabel");
    m_lableDateTime->setFrameShape (QFrame::WinPanel);
    m_lableDateTime->setFrameShadow(QFrame::Sunken);
    m_lableDateTime->setFixedSize(160, 28);
    m_lableDateTime->setStyleSheet("font: 75 11pt 微软雅黑;"
                                   /*"font-style:Bold;"*/);
    m_lableDateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    m_lableDateTime->setAlignment(Qt::AlignCenter);

    QLabel *m_lableCommSynchronousSta=new QLabel(pSignForToolBar);
    m_lableCommSynchronousSta->setText("与同步站机通信正常");
    m_lableCommSynchronousSta->setFixedSize(120, 28);
    //m_lableCommSynchronousSta->setStyleSheet("background-color: rgb(240, 240, 240);");
    m_lableCommSynchronousSta->setFrameShape (QFrame::WinPanel);
    m_lableCommSynchronousSta->setFrameShadow(QFrame::Sunken);
    m_lableCommSynchronousSta->setAlignment(Qt::AlignCenter);

    QLabel *m_lableCommCenter=new QLabel(pSignForToolBar);
    m_lableCommCenter->setText("与中心通信中断");
    m_lableCommCenter->setFixedSize(120, 28);
    //m_lableCommCenter->setStyleSheet("background-color: rgb(240, 240, 240);");
    m_lableCommCenter->setFrameShape (QFrame::WinPanel);
    m_lableCommCenter->setFrameShadow(QFrame::Sunken);
    m_lableCommCenter->setAlignment(Qt::AlignCenter);

    QLabel *m_lableCommServer=new QLabel(pSignForToolBar);
    m_lableCommServer->setText("与车站服务器通信正常");
    m_lableCommServer->setFixedSize(140, 28);
    //m_lableCommServer->setStyleSheet("background-color: rgb(240, 240, 240);");
    m_lableCommServer->setFrameShape (QFrame::WinPanel);
    m_lableCommServer->setFrameShadow(QFrame::Sunken);
    m_lableCommServer->setAlignment(Qt::AlignCenter);


    pSignForToolBar->addWidget(m_btn3);

    QAction* m_SendPlan = pSignForToolBar->addWidget(m_btn4);//发送计划
    m_SendPlan->setObjectName("sendPlan");
//    m_SendPlan->setVisible(false);

    pSignForToolBar->addWidget(m_btn5);
    pSignForToolBar->addWidget(m_lableDateTime);

    QAction* m_ActionCommSynchronousSta = pSignForToolBar->addWidget(m_lableCommSynchronousSta);
    m_ActionCommSynchronousSta->setObjectName("lableCommSynchronousSta");
    m_ActionCommSynchronousSta->setVisible(true);

    pSignForToolBar->addWidget(m_lableCommCenter);

    QAction* m_ActionCommServer = pSignForToolBar->addWidget(m_lableCommServer);
    m_ActionCommServer->setObjectName("lableCommServer");
    m_ActionCommServer->setVisible(false);



    addToolBar(pSignForToolBar);
}

QPushButton* StationInterfaceKSK::addToolBarBtn(QToolBar* parent, QVector<QPushButton*>& vecBarBtn, QString iconFile, QString toolTip, bool checkable, bool checked, QString disableFile)
{
    if(disableFile == "")
        disableFile = iconFile;
    QPushButton *btn = new QPushButton(parent);
    //QIcon icon;
    //icon.addFile(iconFile);
    //btn->setIcon(icon);
    btn->setStyleSheet(QString("QPushButton{"
                               "background-image:url(%1);"
                               "background-origin: content;"
                               "background-position: center;"
                               "background-repeat: no-repeat;"
                               "}"
                               "QPushButton:disabled{"
                               "background-image:url(%2);"
                               "background-origin: content;"
                               "background-position: center;"
                               "background-repeat: no-repeat;"
                               "}").arg(iconFile).arg(disableFile));
    btn->setCheckable(true);
    btn->setToolTip(toolTip);
    btn->setFixedSize(32,32);
    btn->setIconSize(QSize(24,24));
    btn->setFlat(true);
    btn->setCheckable(checkable);
    if(checkable)
        btn->setChecked(checked);
    vecBarBtn.append(btn);
    parent->addWidget(btn);
    return btn;
}

void StationInterfaceKSK::TurnToStationWnd()
{
    m_vecStationViewToolBarBtn[0]->setChecked(false);
    m_vecStationViewToolBarBtn[1]->setChecked(true);
    //绝缘节
    m_vecStationViewToolBarBtn[3]->setChecked(pStationCtrl->bShow_Jyj);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 7)->pAction->setChecked(pStationCtrl->bShow_Jyj);
    //列车/调车按钮
    m_vecStationViewToolBarBtn[4]->setEnabled(true);
    m_vecStationViewToolBarBtn[5]->setEnabled(true);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 8)->pAction->setEnabled(true);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 9)->pAction->setEnabled(true);
    //车次窗
    m_vecStationViewToolBarBtn[6]->setChecked(pStationCtrl->bShowCheCi);
    //进路预告窗
    m_vecStationViewToolBarBtn[7]->setChecked(pStationCtrl->bRoutePreWndShowState);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 10)->pAction->setChecked(pStationCtrl->bRoutePreWndShowState);
    //终端闪烁
    m_vecStationViewToolBarBtn[9]->setEnabled(true);
    //站场放大缩小
    m_vecStationViewToolBarBtn[10]->setChecked(pStationCtrl->nEnlargeNarrow == 1);
    m_vecStationViewToolBarBtn[11]->setChecked(pStationCtrl->nEnlargeNarrow == 2);
    m_vecStationViewToolBarBtn[12]->setEnabled(pStationCtrl->isStationViewReSize);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 0)->pAction->setChecked(pStationCtrl->nEnlargeNarrow == 1);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 1)->pAction->setChecked(pStationCtrl->nEnlargeNarrow == 2);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 2)->pAction->setEnabled(pStationCtrl->isStationViewReSize);
    //车次窗放大缩小
    m_vecStationViewToolBarBtn[15]->setEnabled(pStationCtrl->isTrainNumWndReSize);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 5)->pAction->setEnabled(pStationCtrl->isTrainNumWndReSize);
}

void StationInterfaceKSK::TurnToMultiSWnd()
{
    m_vecStationViewToolBarBtn[0]->setChecked(true);
    m_vecStationViewToolBarBtn[1]->setChecked(false);
    //绝缘节
    m_vecStationViewToolBarBtn[3]->setChecked(pMultiStation->bShowJYJ);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 7)->pAction->setChecked(pMultiStation->bShowJYJ);
    //列车/调车按钮
    m_vecStationViewToolBarBtn[4]->setEnabled(false);
    m_vecStationViewToolBarBtn[5]->setEnabled(false);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 8)->pAction->setEnabled(false);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 9)->pAction->setEnabled(false);
    //车次窗
    m_vecStationViewToolBarBtn[6]->setChecked(pMultiStation->bShowCheCi);
    //进路预告窗
    m_vecStationViewToolBarBtn[7]->setChecked(pMultiStation->bRoutePreWndShowState);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 10)->pAction->setChecked(pMultiStation->bRoutePreWndShowState);
    //终端闪烁
    m_vecStationViewToolBarBtn[9]->setEnabled(false);
    //站场放大缩小
    m_vecStationViewToolBarBtn[10]->setChecked(pMultiStation->nEnlargeNarrow == 1);
    m_vecStationViewToolBarBtn[11]->setChecked(pMultiStation->nEnlargeNarrow == 2);
    m_vecStationViewToolBarBtn[12]->setEnabled(pMultiStation->isStationViewReSize);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 0)->pAction->setChecked(pMultiStation->nEnlargeNarrow == 1);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 1)->pAction->setChecked(pMultiStation->nEnlargeNarrow == 2);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 2)->pAction->setEnabled(pMultiStation->isStationViewReSize);
    //车次窗放大缩小
    m_vecStationViewToolBarBtn[15]->setEnabled(pMultiStation->isTrainNumWndReSize);
    m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 5)->pAction->setEnabled(pMultiStation->isTrainNumWndReSize);
}

void StationInterfaceKSK::TurnToTrafficLog()
{
    ui->dockWidget->hide();
    pStationViewToolBar->hide();
    pTrafficLogToolBar->show();
    QAction* pLableCommServer = pSignForToolBar->findChild<QAction*>("lableCommServer");
    pLableCommServer->setVisible(true);
    QAction* pLableCommSynchronousSta = pSignForToolBar->findChild<QAction*>("lableCommSynchronousSta");
    pLableCommSynchronousSta->setVisible(false);
    QAction* pSendPlan = pSignForToolBar->findChild<QAction*>("sendPlan");
    pSendPlan->setVisible(isShowSignRouteSyn);
    pSignForToolBar->update();
}

void StationInterfaceKSK::SignForBtnFlash()
{
    if(pSignForToolBar)
    {
        //阶段计划按钮闪烁
        QPushButton* stagePlanBtn = pSignForToolBar->findChild<QPushButton*>("stagePlan");
        if(stagePlanBtn)
        {
            if(StagePlanFlash)
            {
                if(isRed)
                    stagePlanBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
                else stagePlanBtn->setStyleSheet("background-color: rgb(227, 23, 13);");
                isRed = !isRed;
            }
            else stagePlanBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
        }

        //调度命令按钮闪烁
        QPushButton* DDOrderManageBtn = pSignForToolBar->findChild<QPushButton*>("DDOrderManage");
        if(DDOrderManageBtn)
        {
            if(DDOrderManageFlash)
            {
                if(isRedDDOrderManage)
                    DDOrderManageBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
                else DDOrderManageBtn->setStyleSheet("background-color: rgb(227, 23, 13);");
                isRedDDOrderManage = !isRedDDOrderManage;
            }
            else DDOrderManageBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
        }

        //发送计划按钮闪烁
        QPushButton* SendPlanBtn = pSignForToolBar->findChild<QPushButton*>("SendPlan");
        if(SendPlanBtn)
        {
            if(SendPlanFlash)
            {
                if(isRedSendPlanFlash)
                    SendPlanBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
                else SendPlanBtn->setStyleSheet("background-color: rgb(227, 23, 13);");
                isRedSendPlanFlash = !isRedSendPlanFlash;
            }
            else SendPlanBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
        }
    }
}

void StationInterfaceKSK::ShowStationCtrl()
{
    pStationCtrl->show();
    pMultiStation->hide();
    pTrafficLogManage->hide();
    pTrainDiagram->hide();
    nWndFlag = 1;
}

void StationInterfaceKSK::ShowMultStation()
{    
    pStationCtrl->hide();
    pMultiStation->show();
    pTrafficLogManage->hide();
    pTrainDiagram->hide();
    nWndFlag = 2;
}
void StationInterfaceKSK::ShowTrafficLog()
{
    pStationCtrl->hide();
    pMultiStation->hide();
    pTrafficLogManage->show();
    pTrainDiagram->hide();
    nWndFlag2 = 1;
}

void StationInterfaceKSK::showTrainDiagramWidget()
{
    pStationCtrl->hide();
    pMultiStation->hide();
    pTrafficLogManage->hide();
    pTrainDiagram->show();
    nWndFlag2 = 2;
}

void StationInterfaceKSK::mousePressEvent(QMouseEvent * event)
{
    return StationInterface::mousePressEvent(event);
}

void StationInterfaceKSK::ShowMousePoint()
{
    QPoint coursePoint= QCursor::pos();
    QPoint mcoursePoint= ui->stationWidget->mapFromGlobal(coursePoint);
    if(nWndFlag==1)//单站
    {
       ShowPoint(pStationCtrl->mcoursePoint);//label显示内容
    }
    else if(nWndFlag==2)//多站
    {
        ShowPoint(mcoursePoint);//label显示内容
    }
}

void StationInterfaceKSK::ShowCurrDateTime()
{
    QLabel* m_lableDateTime = this->findChild<QLabel*>("currDateTimeLabel");
    if(m_lableDateTime)
        m_lableDateTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
}

void StationInterfaceKSK::RouteOrderUpdata()
{
    pRouteOrderWidget->RouteOrderTableUpData();
}

void StationInterfaceKSK::onStationViewScale(bool isScale, int enlargeNarrow)
{
    QAction* action1 = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 0)->pAction;
    QAction* action2 = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 1)->pAction;
    QAction* action3 = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 2)->pAction;
    if(isScale)
    {
        nEnlargeNarrow = enlargeNarrow;
        QCursor cursor;
        QPixmap pixmap;
        if(nEnlargeNarrow == 1)
        {
            pixmap.load("://Res/mouseEnlarge.png");
            cursor = QCursor(pixmap);
        }
        else if(nEnlargeNarrow == 2)
        {
            pixmap.load("://Res/mouseNarrow.png");
            cursor = QCursor(pixmap);
        }
        else if(nEnlargeNarrow == 0)
        {
            cursor = Qt::ArrowCursor;
            if(nWndFlag == 1)
                pStationCtrl->StationViewResize(nEnlargeNarrow);
            else if(nWndFlag == 2)
                pMultiStation->StationViewResize(nEnlargeNarrow);
        }

        if(nWndFlag == 1)
        {
            pStationCtrl->mOldCursor = cursor;
            pStationCtrl->nEnlargeNarrow = enlargeNarrow;
        }
        else if(nWndFlag == 2)
        {
            pMultiStation->setCursor(cursor);
            pMultiStation->nEnlargeNarrow = enlargeNarrow;
        }

        //状态同步
        if(nEnlargeNarrow == 1)
        {
            action1->setChecked(true);
            action2->setChecked(false);

            m_vecStationViewToolBarBtn[10]->setChecked(true);
            m_vecStationViewToolBarBtn[11]->setChecked(false);
        }
        else if(nEnlargeNarrow == 2)
        {
            action1->setChecked(false);
            action2->setChecked(true);

            m_vecStationViewToolBarBtn[10]->setChecked(false);
            m_vecStationViewToolBarBtn[11]->setChecked(true);
        }
        else if(nEnlargeNarrow == 0)
        {
            action1->setChecked(false);
            action2->setChecked(false);
            action3->setEnabled(false);

            m_vecStationViewToolBarBtn[10]->setChecked(false);
            m_vecStationViewToolBarBtn[11]->setChecked(false);
            m_vecStationViewToolBarBtn[12]->setEnabled(false);
        }
        if(nEnlargeNarrow == 3)
        {
            action3->setEnabled(true);
            m_vecStationViewToolBarBtn[12]->setEnabled(true);
            pStationCtrl->SetStationAllView(true);
        }
    }
    else
    {
        nEnlargeNarrow = 0;
        action1->setChecked(false);
        action2->setChecked(false);
        m_vecStationViewToolBarBtn[10]->setChecked(false);
        m_vecStationViewToolBarBtn[11]->setChecked(false);

        if(nWndFlag == 1)
        {
            pStationCtrl->mOldCursor = Qt::ArrowCursor;
        }
        else if(nWndFlag == 2)
        {
            pMultiStation->setCursor(Qt::ArrowCursor);
        }
    }
}

void StationInterfaceKSK::onTrainNumWndScale(int nEnlargeNarrow)
{
    QAction* action = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 5)->pAction;
    if(nWndFlag == 1)
    {
        pStationCtrl->TrainNOViewResize(nEnlargeNarrow);
        m_vecStationViewToolBarBtn[15]->setEnabled(pStationCtrl->isTrainNumWndReSize);
        action->setEnabled(pStationCtrl->isTrainNumWndReSize);
    }
    else if(nWndFlag == 2)
    {
        pMultiStation->TrainNOViewResize(nEnlargeNarrow);
        m_vecStationViewToolBarBtn[15]->setEnabled(pMultiStation->isTrainNumWndReSize);
        action->setEnabled(pMultiStation->isTrainNumWndReSize);
    }
}

void StationInterfaceKSK::onRoutePreWndShowStateChenge(bool b)
{
    if(nWndFlag == 1)
        pStationCtrl->RoutePreWndShowStateChenge(b);
    else if(nWndFlag == 2)
        pMultiStation->RoutePreWndShowStateChenge(b);

    QAction* action = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 10)->pAction;

    m_vecStationViewToolBarBtn[7]->setChecked(b);
    action->setChecked(b);

}

void StationInterfaceKSK::onStagePlanUpData(bool isSync)
{
    if(!isSync)
    {
        if(CurrStation()->AutoSignStage == false)
        {
            StagePlanFlash=true;
        }
        else
        {
            if(isShowSignRouteSyn)
                SendPlanFlash=true;
        }
        pTrafficLogManage->AddDispatchDeskMsg(QString("值班员:下达新阶段计划-%1").arg(QDateTime::currentDateTime().toString("hh:mm")));
        pTrafficLogManage->AddDispatchDeskMsg(QString("%1:值班员,我签收新下达的阶段计划-%2").arg(CurrStation()->m_strStationName).arg(QDateTime::currentDateTime().toString("hh:mm")));  
    }
    if(pStagePlanWidget)
    {
        pStagePlanWidget->StagePlanUpdata();
    }
}

void StationInterfaceKSK::onTrafficLogUpData()
{
    StationInterfaceKSK::pTrafficLogManage->TrafficLogUpData();
}

void StationInterfaceKSK::onTrainRouteOrderUpData()
{
    pRouteOrderWidget->RouteOrderTableUpData();
}

void StationInterfaceKSK::ResetStationSlot()
{
    StagePlanFlash=false;
    SendPlanFlash=false;
    DDOrderManageFlash=false;
    QPushButton* DDOrderManageBtn = pSignForToolBar->findChild<QPushButton*>("DDOrderManage");
    DDOrderManageBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
    mVectTableDate.clear();
    mSystemalarm->InitData(mVectTableDate);
    pTrafficLogManage->ColorClear();
    pTrafficLogManage->TextClear();
    pStationCtrl->OperateOrderClean();
}

void StationInterfaceKSK::SetTrainBtnShow(bool b)
{
    if(pStationCtrl!=nullptr)
    {
        pStationCtrl->bShow_LCAN = b;
        CurrStation()->SetXHJANShow(0, pStationCtrl->bShow_LCAN);
    }

    QAction* action = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 8)->pAction;
    if(m_vecStationViewToolBarBtn[4]->isEnabled() && action->isEnabled())
    {
        m_vecStationViewToolBarBtn[4]->setChecked(b);
        action->setChecked(b);
    }
}

void StationInterfaceKSK::SetShuntingBtnShow(bool b)
{
    if(pStationCtrl!=nullptr)
    {
        pStationCtrl->bShow_DCAN = b;
        CurrStation()->SetXHJANShow(1, pStationCtrl->bShow_DCAN);
    }

    QAction* action = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 9)->pAction;
    if(m_vecStationViewToolBarBtn[5]->isEnabled() && action->isEnabled())
    {
        m_vecStationViewToolBarBtn[5]->setChecked(b);
        action->setChecked(b);
    }
}

void StationInterfaceKSK::onTrainNumWndShowStateChenge(bool b)
{
    if(nWndFlag == 1)
        pStationCtrl->TrainNumWndShowStateChenge(b);
    else if(nWndFlag == 2)
        pMultiStation->TrainNumWndShowStateChenge(b);

    m_vecStationViewToolBarBtn[6]->setChecked(b);
}

void StationInterfaceKSK::StationViewResize()
{
    if(nWndFlag == 1)
    {
        pStationCtrl->StationViewResize(nEnlargeNarrow);
    }
    else if(nWndFlag == 2)
    {
        pMultiStation->StationViewResize(nEnlargeNarrow);
    }

    QAction* action = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 2)->pAction;
    if(nEnlargeNarrow == 1)
    {
        action->setEnabled(true);
        m_vecStationViewToolBarBtn[12]->setEnabled(true);
        pTextAnalysis->sendTextAnalysisMsg(79, mapFromGlobal(QCursor::pos()));
    }
    else if(nEnlargeNarrow == 2)
    {
        action->setEnabled(true);
        m_vecStationViewToolBarBtn[12]->setEnabled(true);
        pTextAnalysis->sendTextAnalysisMsg(80, mapFromGlobal(QCursor::pos()));
    }
}

void StationInterfaceKSK::ShowSystemAlarmWidget(bool b)
{
    if(b)
    {
        mSystemalarm->InitData(mVectTableDate);
        mSystemalarm->show();
    }
    else
    {
          mSystemalarm->hide();
    }
}
void StationInterfaceKSK::UpDataSysTitle(QMainWindow *parent)
{
    if(parent!=nullptr)
    {
        QString strstate="";
        if(mUserInfoVect[curUserIndex].nstate==0x01)
        {
            strstate=" 登录";
        }
        else if(mUserInfoVect[curUserIndex].nstate==0x02)
        {
            strstate=" 注销";
        }
        parent->setWindowTitle(SYSTITLE+mUserInfoVect[curUserIndex].strUserName+strstate);
        pTrafficLogManage->SetDutyInfo(mUserInfoVect);
    }
}
QString StationInterfaceKSK::GetUserLogin()
{
    return mUserInfoVect[curUserIndex].strUserName;
}
QString StationInterfaceKSK::GetUserLoginpassword(QString strname)
{
    for (int i=mUserInfoVect.size()-1;i>=0;i--)
    {
        if(mUserInfoVect[i].strUserName==strname)
        {
            return mUserInfoVect[i].strPassWord;
        }
    }
    return "";
}
//void StationInterfaceKSK::ClosedSYS(QMainWindow *parent)
//{
//    parent->close();
//}

void StationInterfaceKSK::TextShowSolt(QString name,bool b)
{
    if(nWndFlag == 1)
    {
        pStationCtrl->TextShowSolt(name, b);
    }
    else if(nWndFlag == 2)
    {
        pMultiStation->TextShowSolt(name, b);
    }
    mTextShowAttribute->ChangeStatus(nWndFlag, name, b);

    if(name == "绝缘节")
    {
        QAction* action = m_vecMenuBarInfo.at(1)->getSubActionByIndex(2, 1, 7)->pAction;
        action->setChecked(b);
        m_vecStationViewToolBarBtn[3]->setChecked(b);
    }
}

void StationInterfaceKSK::ShowTextShowAttribute()
{
    mTextShowAttribute->setJSQShowFlag = CurrStation()->StaConfigInfo.bCanChgVisleJSQ;
    mTextShowAttribute->DealFunction();
    mTextShowAttribute->ShowWidgetByType(nWndFlag);
}

void StationInterfaceKSK::SetUserInfo(QString name,QString password)
{
    UserInfo mUserInfo;
    mUserInfo.Init();
    mUserInfo.strUserName=name;
    mUserInfo.strPassWord=password;
    mUserInfoVect.append(mUserInfo);
}

void StationInterfaceKSK::UserAlarm(int n)
{
    if(n==0x02)
    {
        QMessageBox::critical(this, tr("STPC"), "密码错误",tr("确定"),tr("取消"),"",0);
    }
    else if(n==0x03)
    {
        QMessageBox::critical(this, tr("STPC"), "用户名错误",tr("确定"),tr("取消"),"",0);
    }
}

void StationInterfaceKSK::UserUpDateSolt(int n,QString name,int nstate)
{
    for(int i=0;i<mUserInfoVect.size();i++)
    {
         if(mUserInfoVect[i].strUserName==name)
         {
             mUserInfoVect[i].nindex=n;
             mUserInfoVect[i].nstate=nstate;
         }
    }
}

void StationInterfaceKSK::SetServerAlarm(QByteArray recvArray, int recvlen)
{
    TableData mTableData;
    bool blevel1=false;
    if((int)recvArray[9]==0x40)
    {
        //系统消息框内容
        if((int)recvArray[10]==0x01)
        {
            mTableData.Init();
            //信息级别
            if((int)recvArray[11]==0x01)
            {
                mTableData.strLevel="错误";
                blevel1=true;
            }
            else if((int)recvArray[11]==0x02)
            {
                mTableData.strLevel="警告";
            }
            else if((int)recvArray[11]==0x03)
            {
                mTableData.strLevel="信息";
            }
            else if((int)recvArray[11]==0x04)
            {
                mTableData.strLevel="防错办返回信息";
            }
            //信息类型
            if((int)recvArray[12]==0x01)
            {
                mTableData.strType="系统";
            }
            else if((int)recvArray[12]==0x02)
            {
                mTableData.strType="行车";
            }
            //信息时间 7
            {
                int year;
                int a =((int)recvArray[13])&0xff;
                int b=((int)recvArray[14])<<8;
                year=a+b;
                int month=(int)recvArray[15];
                int data=(int)recvArray[16];
                int hour=(int)recvArray[17];
                int minute=(int)recvArray[18];
                int second=(int)recvArray[19];
                mTableData.strData=/*QString::number(year)+"-"+*/QString::number(month)+"-"
                        +QString::number(data)+" "+QString::number(hour)+":"
                        +QString::number(minute)+":"+QString::number(second);
                QDateTime datetime=QDateTime::fromString(mTableData.strData,"M-d h:m:s");
                //qDebug()<<datetime<<datetime.toString("MM-dd hh:mm:ss");
                mTableData.strData=datetime.toString("MM-dd hh:mm:ss");

            }
            //信息描述长度 1
            int nlen1=0;
            nlen1=(int)recvArray[20];
            //信息描述内容
            QByteArray Data;
            Data=recvArray.mid(21,nlen1);
            mTableData.strDescribe = QString::fromLocal8Bit(Data);
            //信息来源长度 1
            int nlen2=0;
            nlen2=(int)recvArray[20+nlen1+1];
            //信息来源内容
            Data=recvArray.mid(20+nlen1+2,nlen2);
            mTableData.strSource = QString::fromLocal8Bit(Data);
            mVectTableDate.append(mTableData);
            //AddData();
            mSystemalarm->InitData(mVectTableDate);
        }
        else if((int)recvArray[10]==0x02)
        {
            //类型
            int ntype =(int)recvArray[11];
            //信息描述长度 1
            int nlen1=0;
            nlen1=(int)recvArray[12];
            //信息描述内容
            QByteArray Data;
            Data=recvArray.mid(13,nlen1);
            QString strDescribe = QString::fromLocal8Bit(Data);
            if(ntype==0x01)
            {
                QMessageBox::information(pStationCtrl,tr("STPC"),strDescribe,tr("确定"),tr("取消"),"",0);
                //pStationCtrl->ShowSySMessage(strDescribe);

            }
            else if(ntype==0x02)
            {
                QMessageBox::question(this, tr("STPC"), strDescribe,tr("确定"),tr("取消"),"",0);
            }
            else if(ntype==0x03)
            {
                //QMessageBox::information(this, tr("STPC"), strDescribe,tr("确定"),tr("取消"),"",0);
                 QMessageBox::warning(this, tr("STPC"), strDescribe,tr("确定"),tr("取消"),"",0);
                //pStationCtrl->ShowSySMessage(strDescribe);
            }
            else if(ntype==0x04)
            {
                QMessageBox::critical(this, tr("STPC"), strDescribe,tr("确定"),tr("取消"),"",0);
            }
            //qDebug()<<"------------------"<<ntype<<strDescribe;
        }
        //一级报警信息弹出弹窗
        if(blevel1)
        {
            ShowSystemAlarmWidget(true);
        }
    }
}

void StationInterfaceKSK::SendServerAlarm(QByteArray recvArray, int recvlen)
{
    SetServerAlarm(recvArray,recvlen);
}
//更新调度命令
void StationInterfaceKSK::onDDOrderManageUpData()
{
    DDOrderManageFlash=true;
}
void StationInterfaceKSK::DDOrderSignSolt()
{
    DDOrderManageFlash=false;
    QPushButton* DDOrderManageBtn = pSignForToolBar->findChild<QPushButton*>("DDOrderManage");
    DDOrderManageBtn->setStyleSheet("background-color: rgb(210, 210, 210);");
}
void StationInterfaceKSK::StagePlanSignSolt()
{
    //stopStagePlanFlash();
    stopSendPlanFlash();
}
void StationInterfaceKSK::updateDisOrderSolt()
{
    if(pDDOrderManageWidget!=nullptr)
    {
        pDDOrderManageWidget->SetOrderData(pStationCtrl->pCurrStation->m_ArrayDisOrderRecv,pStationCtrl->pCurrStation->m_ArrayDisOrderDisp,pStationCtrl->pCurrStation->m_ArrayDisOrderLocomot);
    }
}
//自动签收
void StationInterfaceKSK::AutoSignStateSolt(bool bsign)
{
    // //发送自动签收
    emit SendAutoSignStateSignal(bsign);
}

//用户登录
void StationInterfaceKSK::LogInSolt(QString mUserName)
{
    for(int i = 0; i < mUserInfoVect.size(); i++)
    {
        if(mUserInfoVect.at(i).strUserName == mUserName)
        {
            curUserIndex = i;
            emit SendUserLongInExitSignal(0, mUserInfoVect.at(i).strUserName, mUserInfoVect.at(i).strPassWord);
            return;
        }
    }

}
//用户退出
void StationInterfaceKSK::UserExitSolt(bool b)
{
    exit(0);
}

void StationInterfaceKSK::ShowUserLogInExit(int ntype,QString strname,QString strpassword,QStringList strlist)
{
    if(ntype==0)//用户登录
    {
        mlogin->Init(strlist);
        mlogin->show();
    }
    else if(ntype==1)//用户注销
    {
        QString qMsg = QString("您确实要注销么？");
        int ret = QMessageBox::information(this,tr("STPC"),qMsg,tr("确定"),tr("取消"),"",0);
        if(ret==0)
        {
            pTextAnalysis->sendTextAnalysisMsg(77, mapFromGlobal(QCursor::pos()));
            emit SendUserLongInExitSignal(1,strname,strpassword);
        }
        else pTextAnalysis->sendTextAnalysisMsg(78, mapFromGlobal(QCursor::pos()));
    }
    else if(ntype==2)//退出
    {
        muserexit->show();
        //emit SysClosed(true);
    }
}

