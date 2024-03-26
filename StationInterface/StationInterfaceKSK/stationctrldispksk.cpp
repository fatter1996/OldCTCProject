#include "stationctrldispksk.h"
#include "ui_stationctrldispksk.h"
#include <QScrollBar>
StationCtrlDispKSK::StationCtrlDispKSK(QWidget *parent) :
    StationCtrlDisp(parent),
    ui(new Ui::StationCtrlDispKSK)
{
    ui->setupUi(this);
    installEventFilter(this);
    ui->widget->installEventFilter(this);
    this->setMouseTracking(true);

    ui->scrollArea->setMouseTracking(true);
    ui->widget->setMouseTracking(true);
    nElapsed = 0;
    Timer_ID_500 = startTimer(500);
    pStationFunButtonWidget= ui->widget_2;
    pStationFunButtonWidget->Init(1);
    pStationFunButtonWidget->installEventFilter(this);
    pStationFunButtonWidget->setMouseTracking(true);
    connect(pStationFunButtonWidget, &StationFunButtonWidget::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
//    pStationFunButtonWidget->SetFunButtonEn(20,false);
//    pStationFunButtonWidget->SetFunButtonEn(21,false);


    InitParameter();
    InitClass();
    InitConnect();
    InitMouseRightMenu();
    initialSize = ui->widget->size();
}

StationCtrlDispKSK::~StationCtrlDispKSK()
{
    delete ui;
}
void StationCtrlDispKSK::InitSet()
{
    //坡道解锁
    if(pCurrStation->strPDJSXHJ.size()>0)
    {
        pStationFunButtonWidget->SetFunButtonVisable(RAMP_RELEASE,true);
    }
    else
    {
        pStationFunButtonWidget->SetFunButtonVisable(RAMP_RELEASE,false);
        for(int i=0;i<pCurrStation->mQianFengVect.size();i++)
        {
            if(pCurrStation->mQianFengVect[i].StrName=="坡道解锁")
            {
                pCurrStation->mQianFengVect.remove(i);
            }
        }

    }
    //点灯灭灯
    {
        pStationFunButtonWidget->SetFunButtonVisable(LIGHT_ON,pCurrStation->bDDMDFalg);
        pStationFunButtonWidget->SetFunButtonVisable(LIGHT_OFF,pCurrStation->bDDMDFalg);
    }
    //故障通知
    {
        FUNCBUTTON *pfUNCBUTTON=nullptr;
        bool bGZTZFalg=false;
        for(int j=0;j<pCurrStation->FuncBtnArray.size();j++)
        {
            if(pCurrStation->FuncBtnArray[j]->getDevType() == Dev_FUNCB)
            {
                 pfUNCBUTTON=(FUNCBUTTON*)(pCurrStation->FuncBtnArray[j]);
                 if(pfUNCBUTTON->Button_Name=="故障通知")
                 {
                     bGZTZFalg=true;
                 }
            }
        }
        if(!bGZTZFalg)
        {
            for(int i=0;i<pCurrStation->mQianFengVect.size();i++)
            {
                if(pCurrStation->mQianFengVect[i].StrName=="故障通知")
                {
                    pCurrStation->mQianFengVect.remove(i);
                }
            }
        }
    }
}
void StationCtrlDispKSK::InitParameter()
{
    bShow_Jyj=true;
    bShow_LCAN=true;
    bShow_DCAN=true;
    bShow_RouteList=true;

    bLCANFlash=true;
    bDCANFlash=false;
    bBTANFlash=false;
    nEnlargeNarrow=0;
    nCTCControlMode=0x22;
    nCTCPlanControlMode=0;
    mCBaseDataOld=nullptr;
    mOldCursor=Qt::ArrowCursor;
    nPoorShuntType=-1;
    mFunButtonDevInfo.Init();
    bPassWordResult=false;
}
void StationCtrlDispKSK::InitViewSize(QSize minitialSize)
{
    initialSize=minitialSize;
    ui->widget->setFixedSize(initialSize.width() * pCurrStation->nDiploid, initialSize.height() * pCurrStation->nDiploid);
}
void StationCtrlDispKSK::InitClass()
{
    mPoorShuntDlg = new PoorShuntDlg_Dlg();
    connect(mPoorShuntDlg, &PoorShuntDlg_Dlg::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    mPassWordEntryDlg=new PassWordEntryDlg(this);
    mPassWordEntryDlg->hide();
    mPassWordEntryDlg->setObjectName("");
    mPassWordEntryDlg->installEventFilter(this);
    mPassWordEntryDlg->setMouseTracking(true);
    mPassWordEntryDlg_NEW = new Dialog_PassWord(this);
    connect(mPassWordEntryDlg_NEW, &Dialog_PassWord::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    //FZList = new DLG_FZList(this);
    mKeyBoard=new KeyBoard();
//    mQFTJ = new QFTJ(this);
//    mQFTJ->hide();
    mStateChose = new StateChose();
    connect(mStateChose, &StateChose::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    mModeChange = new ModeChange();
    connect(mModeChange, &ModeChange::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    m_SeekTrain = new SeekTrain();

    mTrainRouteHandle=new trainroutehandle_DLG();
    connect(mTrainRouteHandle, &trainroutehandle_DLG::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    mQFTJ = new QFTJ(this);
    mDCRouteTimeWidget=new DCRouteTimeWidget_Dlg();
    connect(mDCRouteTimeWidget, &DCRouteTimeWidget_Dlg::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
}
void StationCtrlDispKSK::InitConnect()
{
    //connect(mKeyBoard,SIGNAL(recvstrDataSignal(QString,QString)),mPassWordEntryDlg,SLOT(receiveKeyboardDataSlot(QString,QString)));
    //connect(mPassWordEntryDlg,SIGNAL(ClosedSignal()),mKeyBoard,SLOT(RecvClosedSlot()));
    connect(mPassWordEntryDlg,SIGNAL(CheckReturnSignal(bool)),this,SLOT(PassWordResult(bool)));
    //分录不良选择结果信号槽绑定
    connect(mPoorShuntDlg,SIGNAL(resultsignal(int,int,int)),this,SLOT(RecResultPoorShuntSolt(int,int,int)));
    //铅封窗口显示信号槽绑定
    connect(pStationFunButtonWidget,SIGNAL(LeadSwalingShowSignal()),this,SLOT(LeadSwalingShowSlot()));
    //辅助菜单选择结果信号槽绑定
    connect(pStationFunButtonWidget,SIGNAL(MenuAuxiliaryChoseSignal(QString)),this,SLOT(MenuAuxiliaryChoseSlot(QString)));
    //状态选择按下信号槽绑定
    connect(pStationFunButtonWidget,SIGNAL(StateChoseSignal()),this,SLOT(StateChoseSlot()));
    //方式转化按下信号槽绑定
    connect(pStationFunButtonWidget,SIGNAL(ModeChangeSignal(QString)),this,SLOT(ModeChangeSlot(QString)));
    //命令清除 命令按下按下信号槽绑定
    connect(pStationFunButtonWidget,SIGNAL(OrderButtonSignal(int)),this,SLOT(OrderButtonSlot(int)));
    //命令清除 命令按下按下信号槽绑定
    connect(pStationFunButtonWidget->FZList,SIGNAL(OrderFZListSignal(int)),this,SLOT(OrderButtonSlot(int)));
    //股道无电
    connect(pStationFunButtonWidget->FZList,SIGNAL(GDWDSignal()),this,SLOT(GDWDSolt()));
    //接触网定位无电
    connect(pStationFunButtonWidget->FZList,SIGNAL(JCWDWWDSignal()),this,SLOT(JCWDWWDSolt()));
    //接触网反位无电
    connect(pStationFunButtonWidget->FZList,SIGNAL(JCWFWWDSignal()),this,SLOT(JCWFWWDSolt()));
    //尖轨故障
    connect(pStationFunButtonWidget->FZList,SIGNAL(JGGZSignal()),this,SLOT(JGGZSolt()));
    //心轨故障
    connect(pStationFunButtonWidget->FZList,SIGNAL(XGGZSignal()),this,SLOT(XGGZSolt()));
    //破封统计
    connect(pStationFunButtonWidget->FZList,SIGNAL(PFTJSignal()),this,SLOT(PFTJSolt()));

    //状态选择信号槽绑定
    connect(mStateChose,SIGNAL(SendStationCtrModeChoseSignal(int,int,int)),this,SLOT(StateAndModeSendSolt(int,int,int)));
    //方式转换信号槽绑定
    connect(mModeChange,SIGNAL(SendStationCtrModeChangeSignal(int,int,int)),this,SLOT(StateAndModeSendSolt(int,int,int)));


    //进路办理车次输入窗口
    //connect(mTrainRouteHandle,SIGNAL(CheckReturnSignal(QString)),this,SLOT(TrainRouteResult(QString)));
}
void StationCtrlDispKSK::magicTime()
{
    QPainter painter(ui->widget);
    //ui->widget->setFixedSize(initialSize.width() * pCurrStation->nDiploid, initialSize.height() * pCurrStation->nDiploid);
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen; //画笔
    pen.setColor(QColor(0, 0, 0));
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(QColor(255, 0, 0, 0)); //画刷
    painter.setPen(pen); //添加画笔
    painter.setBrush(brush); //添加画刷
    //显示站场
    if(pCurrStation)
    {
        //显示当前车站
        if(pCurrStation->commToServer<20)//10s
        {
            pCurrStation->SetShowJYJ(bShow_Jyj);
            pCurrStation->draw(&painter,nElapsed);
        }
    }
    painter.end();
}


void StationCtrlDispKSK::StationViewResize(int nEnlargeNarrow)
{

    pCurrStation->SetDiploid(nEnlargeNarrow, pCurrStation->nDiploid);
    if(pCurrStation->nDiploid > 1)
        ui->widget->setFixedSize(initialSize.width() * pCurrStation->nDiploid, initialSize.height() * pCurrStation->nDiploid);
    else
       ui->widget->setFixedSize(initialSize);
    isStationViewReSize = !(pCurrStation->nDiploid == 1.0f);
}
void StationCtrlDispKSK::StationMenuAuxiliary()
{
    pStationFunButtonWidget->SetMenuAuxiliary(GetDCBtnJGXG());
}
void StationCtrlDispKSK::SetStationAllView(bool b)
{
    if(b)
    {
        pCurrStation->nDiploid = 1.08f;
        isStationViewReSize = true;
    }
    else
    {
       pCurrStation->nDiploid=1;
    }
    ui->widget->setFixedSize(initialSize.width() * pCurrStation->nDiploid, initialSize.height() * pCurrStation->nDiploid);
}
void StationCtrlDispKSK::TrainNOViewResize(int nEnlargeNarrow)
{
    pCurrStation->SetDiploid(nEnlargeNarrow, pCurrStation->nDiploidCheCi);
    isTrainNumWndReSize = !(pCurrStation->nDiploidCheCi == 1.0f);
}

void StationCtrlDispKSK::TrainNumWndShowStateChenge(bool b)
{
    bShowCheCi = b;
    pCurrStation->SetTrainNumWndShow(bShowCheCi);
}
void StationCtrlDispKSK::RoutePreWndShowStateChenge(bool b)
{
    bRoutePreWndShowState = b;
    pCurrStation->SetRoutePreWndShow(bRoutePreWndShowState);
}
//初始化光标资源
void StationCtrlDispKSK::InitCursor()
{
    QPixmap pixmap;
    // 光标图片 需要注意注意图片的大小 不然会比较难看
    pixmap.load("://Res/trainHand.png");
    QCursor mcursor(pixmap);
    setCursor(mcursor);
}
//重置光标为箭头
void StationCtrlDispKSK::ResetCursor()
{
     setCursor(mOldCursor);
}
void StationCtrlDispKSK::InitMouseRightMenu()
{
    //道岔右键菜单
    {
        menuDC = new QMenu(/*this*/);
        QAction *Action1 = new QAction(tr("定操"), ui->widget);
        QAction *Action2 = new QAction(tr("反操"), ui->widget);
        QAction *Action3 = new QAction(tr("单锁"), ui->widget);
        QAction *Action4 = new QAction(tr("单解"), ui->widget);
        QAction *Action5 = new QAction(tr("封锁/解封"), ui->widget);
        QAction *Action6 = new QAction(tr("区故解"), ui->widget);
        QAction *Action7 = new QAction(tr("岔前 分路不良"), ui->widget);
        QAction *Action8 = new QAction(tr("定位 分路不良"), ui->widget);
        QAction *Action9 = new QAction(tr("反位 分路不良"), ui->widget);
        QAction *Action10 = new QAction(tr("尖轨故障"), ui->widget);
        QAction *Action11 = new QAction(tr("心轨故障"), ui->widget);
        QAction *Action12 = new QAction(tr("接触网定位无电"), ui->widget);
        QAction *Action13 = new QAction(tr("接触网反位无电"), ui->widget);
        QAction *Action14 = new QAction(tr("确认空闲"), ui->widget);
        menuDC->addAction(Action1);
        menuDC->addAction(Action2);
        menuDC->addSeparator();
        menuDC->addAction(Action3);
        menuDC->addAction(Action4);
        menuDC->addSeparator();
        menuDC->addAction(Action5);
        menuDC->addAction(Action6);
        menuDC->addSeparator();
        menuDC->addAction(Action7);
        menuDC->addAction(Action8);
        menuDC->addAction(Action9);
        menuDC->addAction(Action14);
        menuDC->addSeparator();
//        menuDC->addAction(Action10);
//        menuDC->addAction(Action11);
//        menuDC->addSeparator();
        menuDC->addAction(Action12);
        menuDC->addAction(Action13);
        Action1->setCheckable(true);
        Action2->setCheckable(true);
        Action3->setCheckable(true);
        Action4->setCheckable(true);
        Action5->setCheckable(true);
        //Action6->setCheckable(true);
        Action7->setCheckable(true);
        Action8->setCheckable(true);
        Action9->setCheckable(true);
//        Action10->setCheckable(true);
//        Action11->setCheckable(true);
        Action12->setCheckable(true);
        Action13->setCheckable(true);
        Action14->setCheckable(true);
    }
    //信号右键
    {
        menuXH = new QMenu(/*this*/);
        QAction *Action10 = new QAction(tr("办理   通过进路"), ui->widget);
        QAction *Action1 = new QAction(tr("列车   进路办理"), ui->widget);
        QAction *Action2 = new QAction(tr("         取消进路"), ui->widget);
        QAction *Action3 = new QAction(tr("         信号重开"), ui->widget);
        connect(Action3, &QAction::triggered, [=](){
            nXHType = 1;
        });
        QAction *Action4 = new QAction(tr("         封锁/解封"), ui->widget);
        QAction *Action5 = new QAction(tr("         总人解"), ui->widget);
        QAction *Action6 = new QAction(tr("引导"), ui->widget);
        QAction *Action7 = new QAction(tr("点灯/灭灯"), ui->widget);
        QAction *Action8 = new QAction(tr("坡道解锁"), ui->widget);

        QAction *Action9 = new QAction(tr("调车   进路办理"), ui->widget);
        QAction *Action11 = new QAction(tr("         取消进路"), ui->widget);
        QAction *Action12 = new QAction(tr("         信号重开"), ui->widget);
        connect(Action12, &QAction::triggered, [=](){
            nXHType = 2;
        });
        QAction *Action13 = new QAction(tr("         封锁/解封"), ui->widget);
        QAction *Action14 = new QAction(tr("         总人解"), ui->widget);
        QAction *Action15 = new QAction(tr("点灯/灭灯"), ui->widget);
        QAction *Action16 = new QAction(tr("封锁/解封"), ui->widget);

        Action1->setObjectName("列车");
        Action2->setObjectName("列车");
        Action3->setObjectName("列车");
        Action4->setObjectName("列车");
        Action5->setObjectName("列车");

        Action8->setObjectName("坡道解锁");
        Action10->setObjectName("通过");
        Action9->setObjectName("调车");
        Action11->setObjectName("调车");
        Action12->setObjectName("调车");
        Action13->setObjectName("调车");
        Action14->setObjectName("调车");
        //Action15->setObjectName("调车");
        Action6->setObjectName("引导");
        Action3->setEnabled(false);
        Action12->setEnabled(false);
        menuXH->addAction(Action10);
        menuXH->addAction(Action1);
        menuXH->addAction(Action2);
        menuXH->addAction(Action3);
        menuXH->addAction(Action4);
        menuXH->addAction(Action5);

        menuXH->addSeparator();
        menuXH->addAction(Action9);
        menuXH->addAction(Action11);
        menuXH->addAction(Action12);
        menuXH->addAction(Action13);
        menuXH->addAction(Action14);
//        menuXH->addSeparator();
//        menuXH->addAction(Action15);
        menuXH->addSeparator();
        menuXH->addAction(Action6);
        menuXH->addAction(Action7);
        menuXH->addAction(Action8);
        menuXH->addAction(Action16);
        Action13->setCheckable(true);
        Action4->setCheckable(true);
    }
    //股道右键
    {
        menuGD = new QMenu(/*this*/);
        //menuGD->setMouseTracking(true);
        QAction *Action1 = new QAction(tr("封锁/解封"), ui->widget);
        Action1->setObjectName("区间");
        QAction *Action2 = new QAction(tr("区故解"), ui->widget);
        Action2->setObjectName("区间");
        QAction *Action3 = new QAction(tr("分路不良"), ui->widget);
        Action3->setObjectName("区间");
        QAction *Action4 = new QAction(tr("股道无电"), ui->widget);
        QAction *Action5 = new QAction(tr("添加调机号"), ui->widget);
        QAction *Action6 = new QAction(tr("删除调机号"), ui->widget);
        QAction *Action7 = new QAction(tr("修改调机号"), ui->widget);
        QAction *Action8 = new QAction(tr("确认空闲"), ui->widget);
        menuGD->addAction(Action1);
        menuGD->addAction(Action2);
        menuGD->addSeparator();
        menuGD->addAction(Action3);
        menuGD->addAction(Action8);
        menuGD->addSeparator();
        menuGD->addAction(Action4);
//        menuGD->addSeparator();
//        menuGD->addAction(Action5);
//        menuGD->addAction(Action6);
//        menuGD->addAction(Action7);

        Action1->setCheckable(true);
        //Action2->setCheckable(true);
        Action3->setCheckable(true);
        Action4->setCheckable(true);
        Action8->setCheckable(true);

    }
    //站场界面右键
    {
        menuSTA = new QMenu(this);
        QAction *Action1 = new QAction(tr("车站选择"), ui->widget);
        QAction *Action2 = new QAction(tr("车站浏览"), ui->widget);
        QAction *Action3 = new QAction(tr("主工具条"), ui->widget);
        QAction *Action4 = new QAction(tr("CTC按钮"), ui->widget);
        QAction *Action5 = new QAction(tr("文字显示"), ui->widget);
        QAction *Action6 = new QAction(tr("绝缘节"), ui->widget);
        QAction *Action7 = new QAction(tr("还原 站场图"), ui->widget);
        QAction *Action8 = new QAction(tr("放大 站场图"), ui->widget);
        QAction *Action9 = new QAction(tr("缩小 站场图"), ui->widget);
        QAction *Action10 = new QAction(tr("站场图全景"), ui->widget);
        QAction *Action11 = new QAction(tr("还原 车次窗"), ui->widget);
        QAction *Action12 = new QAction(tr("放大 车次窗"), ui->widget);
        QAction *Action13 = new QAction(tr("缩小 车次窗"), ui->widget);
        QAction *Action14 = new QAction(tr("车次窗位置"), ui->widget);
        QAction *Action15 = new QAction(tr("查找车次窗"), ui->widget);
        QAction *Action16 = new QAction(tr("接通光带"), ui->widget);
        QAction *Action17 = new QAction(tr("继续接通光带30s"), ui->widget);
        QAction *Action18 = new QAction(tr("线条宽度"), ui->widget);
        QAction *Action19 = new QAction(tr("信号机大小"), ui->widget);
        QAction *Action20 = new QAction(tr("按钮和表示灯大小"), ui->widget);

        //车站选择选项二级菜单
        QMenu *menuSTAChose = new QMenu();
        QAction *Action1_1 = new QAction("标准站", ui->widget);
        Action1_1->setCheckable(true);
        Action1_1->setChecked(true);
        Action1_1->setObjectName("staname");
        menuSTAChose->addAction(Action1_1);
        Action1->setMenu(menuSTAChose);
        //车站浏览 MENU_STAREAD
        QMenu *menuSTALOOK = new QMenu();
        QAction *Action2_1 = new QAction(tr("首站"), ui->widget);
        QAction *Action2_2 = new QAction(tr("前站"), ui->widget);
        QAction *Action2_3 = new QAction(tr("后站"), ui->widget);
        QAction *Action2_4 = new QAction(tr("尾站"), ui->widget);
        Action2_1->setEnabled(false);
        Action2_2->setEnabled(false);
        Action2_3->setEnabled(false);
        Action2_4->setEnabled(false);
        menuSTALOOK->addAction(Action2_1);
        menuSTALOOK->addAction(Action2_2);
        menuSTALOOK->addAction(Action2_3);
        menuSTALOOK->addAction(Action2_4);
        Action2->setMenu(menuSTALOOK);

        //主工具条
        Action3->setEnabled(false);
        //CTC按钮
        Action4->setCheckable(true);
        Action4->setChecked(true);
        //绝缘节
        Action6->setCheckable(true);
        Action6->setChecked(true);
        //还原 站场图
        Action7->setCheckable(false);
        Action7->setEnabled(false);
        Action8->setCheckable(false);
        Action9->setCheckable(false);
        //站场图全景
        Action10->setCheckable(false);
        //还原 车次窗
        Action11->setCheckable(false);
        Action11->setEnabled(false);
        //车次窗位置
        Action14->setCheckable(true);
        //线条宽度
        Action18->setEnabled(false);
        //信号机大小
        Action19->setEnabled(false);
        //按钮和表示灯大小
        Action20->setEnabled(false);
        menuSTA->addAction(Action1);
        menuSTA->addAction(Action2);
        menuSTA->addSeparator();
        menuSTA->addAction(Action3);
        menuSTA->addAction(Action4);
        menuSTA->addAction(Action5);
        menuSTA->addAction(Action6);
        menuSTA->addSeparator();
        menuSTA->addAction(Action7);
        menuSTA->addAction(Action8);
        menuSTA->addAction(Action9);
        menuSTA->addAction(Action10);
        menuSTA->addSeparator();
        menuSTA->addAction(Action11);
        menuSTA->addAction(Action12);
        menuSTA->addAction(Action13);
        menuSTA->addSeparator();
        menuSTA->addAction(Action14);
        menuSTA->addAction(Action15);
        menuSTA->addSeparator();
        menuSTA->addAction(Action16);
        menuSTA->addAction(Action17);
        menuSTA->addSeparator();
        menuSTA->addAction(Action18);
        menuSTA->addAction(Action19);
        menuSTA->addAction(Action20);
        //CTC按钮
        connect(Action4,&QAction::triggered,[=](){
           if(Action4->isChecked())
           {
               pStationFunButtonWidget->show();
           }
           else
           {
                pStationFunButtonWidget->hide();
           }

        });

        //文字显示
        connect(Action5,&QAction::triggered,[=](){
            emit StaMenuTriggered(1);
        });
        //绝缘节显示
        connect(Action6,&QAction::triggered,[=](){
             emit StaMenuTriggered(2, Action6->isChecked());
        });
        //放大站场图
        connect(Action8,&QAction::triggered,[=](){
            emit StaMenuTriggered(3, 1);
        });
        //缩小站场图
        connect(Action9,&QAction::triggered,[=](){
            emit StaMenuTriggered(3, 2);
        });
        //还原站场图
        connect(Action7,&QAction::triggered,[=](){
            emit StaMenuTriggered(3, 0);
        });

        //站场图全景
        connect(Action10,&QAction::triggered,[=](){
            emit StaMenuTriggered(3, 3);
        });
         //放大车次窗
        connect(Action12,&QAction::triggered,[=](){
            emit StaMenuTriggered(4, 1);
        });
         //缩小车次窗
        connect(Action13,&QAction::triggered,[=](){
            emit StaMenuTriggered(4, 2);
        });
        //还原车次窗
        connect(Action11,&QAction::triggered,[=](){
            emit StaMenuTriggered(4, 0);
        });
        //车次窗位置
        connect(Action14,&QAction::triggered,[=](){
            emit StaMenuTriggered(5, Action14->isChecked());
        });

        //查找车次窗
        connect(Action15,&QAction::triggered,[=](){
             ShowSeekTrain();
        });
        //接通光带
        connect(Action16,&QAction::triggered,[=](){
            nLightband_15=0;
            BLightband_15=true;
             //ShowSeekTrain();
        });
        //继续接通光带30s
        connect(Action17,&QAction::triggered,[=](){
             nLightband_30=0;
             BLightband_30=true;
             //ShowSeekTrain();
        });
    }

    //功能按钮按下右键菜单
    {
        menuFunOrder = new QMenu(this);
        QAction *Action1 = new QAction(tr("命令下达"), ui->widget);
        QAction *Action2 = new QAction(tr("命令清除"), ui->widget);
        menuFunOrder->addAction(Action1);
        menuFunOrder->addAction(Action2);
    }
    //车次操作
    {
        menuCheCi = new QMenu(/*this*/);
        QAction *Action1 = new QAction(tr("加车次号"), menuCheCi);
        QAction *Action2 = new QAction(tr("删车次号"), menuCheCi);
        QAction *Action3 = new QAction(tr("车次号确认"), menuCheCi);
        QAction *Action4 = new QAction(tr("修正车次号"), menuCheCi);
        QAction *Action8 = new QAction(tr("变更车次号"), menuCheCi);
        QAction *Action5 = new QAction(tr("改车次属性"), menuCheCi);
        QAction *Action6 = new QAction(tr("设置车次停稳"), menuCheCi);
        QAction *Action7 = new QAction(tr("设置车次启动"), menuCheCi);
        menuCheCi->addAction(Action1);
        menuCheCi->addAction(Action2);
        menuCheCi->addAction(Action3);
        menuCheCi->addAction(Action4);
        menuCheCi->addAction(Action8);
        menuCheCi->addAction(Action5);
        menuCheCi->addAction(Action6);
        menuCheCi->addAction(Action7);
    }
    //文字标注右键菜单
    {
        //
        menuTextAnnotation = new QMenu(/*this*/);
        QAction *Action1 = new QAction(tr("修改"), menuTextAnnotation);
        QAction *Action2 = new QAction(tr("删除"), menuTextAnnotation);
        QAction *Action3 = new QAction(tr("字体"), menuTextAnnotation);
        QAction *Action4 = new QAction(tr("前景色"), menuTextAnnotation);
        QAction *Action5 = new QAction(tr("背景色"), menuTextAnnotation);
        QAction *Action6 = new QAction(tr("清空所有标记"), menuTextAnnotation);
        menuTextAnnotation->addAction(Action1);
        menuTextAnnotation->addAction(Action2);
        menuTextAnnotation->addAction(Action3);
        menuTextAnnotation->addAction(Action4);
        menuTextAnnotation->addAction(Action5);
        menuTextAnnotation->addAction(Action6);
    }

}
//车次操作
bool StationCtrlDispKSK::CHECIOperate(MyStation* pMyStation)
{
    Train* pTrain;
    TrainNumWnd* pTrainNumWnd;
    QSettings* settings = new QSettings("Data/Global.ini", QSettings::IniFormat);
    // 指定为中文
    settings->setIniCodec("UTF-8");

    //读取配置-通信信息
    bool bTrainWidgetRBtnMenu = settings->value("CONFIG/TrainWidgetRBtnMenu").toBool();
    //车次操作
    for(int i=0; i<pMyStation->m_ArrayTrainNumWnd.size(); i++)
    {
        pTrainNumWnd = pMyStation->m_ArrayTrainNumWnd[i];
        if(pTrainNumWnd->m_RectWnd.contains(mcoursePoint)/*&&(pTrainNumWnd->m_bVisible||pTrainNumWnd->m_strCheCi!="")*/)
        {
            QAction *selectAct=nullptr;
            QVector<QAction*> vecAction;
            if(pTrainNumWnd->m_strCheCi!="")
            {
                pTrain = pMyStation->GetTrainByCheCi(pTrainNumWnd->m_strCheCi);
                //设置“加车次号”禁止使用
                SetMenuState(menuCheCi,"加车次号",false,false);

                menuCheCi->addSeparator();


                if(bTrainWidgetRBtnMenu)
                {
                    TrainRouteOrder* pRouteOrderJC = nullptr;
                    TrainRouteOrder* pRouteOrderFC = nullptr;
                    for(int j = 0; j < pMyStation->m_ArrayRouteOrder.size(); j++)
                    {
                        if(pMyStation->m_ArrayRouteOrder.at(j)->m_strTrainNum == pTrainNumWnd->m_strCheCi && pMyStation->m_ArrayRouteOrder.at(j)->father_id == 0)
                        {
                            if(pMyStation->m_ArrayRouteOrder.at(j)->m_btRecvOrDepart == 0)
                                pRouteOrderJC = pMyStation->m_ArrayRouteOrder.at(j);
                            else pRouteOrderFC = pMyStation->m_ArrayRouteOrder.at(j);
                        }
                    }

                    for(int j = 0; j < pMyStation->m_ArrayTrafficLog.size(); j++)
                    {
                        if(pMyStation->m_ArrayTrafficLog.at(j)->m_strTrainNum == pTrainNumWnd->m_strCheCi)
                        {
                            if(pMyStation->m_ArrayTrafficLog.at(j)->m_strTrainPosStatus == STATUS_LCYJZ ||
                                    pMyStation->m_ArrayTrafficLog.at(j)->m_strTrainPosStatus == STATUS_LCYKSJRGD ||
                                    pMyStation->m_ArrayTrafficLog.at(j)->m_strTrainPosStatus == STATUS_LCYWZJRGD ||
                                    pMyStation->m_ArrayTrafficLog.at(j)->m_strTrainPosStatus == STATUS_LCYCZ ||
                                    pMyStation->m_ArrayTrafficLog.at(j)->m_strTrainPosStatus == STATUS_LCYWC)
                            {
                                if(pRouteOrderFC)
                                    vecAction = getRouteActionByRouteId(pRouteOrderFC->route_id);
                                else if(pRouteOrderJC && pRouteOrderJC->m_btBeginOrEndFlg == 0xCC)
                                    vecAction = getRouteActionByRouteId(pRouteOrderJC->route_id);
                            }
                            else if(pRouteOrderJC)
                                vecAction = getRouteActionByRouteId(pRouteOrderJC->route_id);

                        }
                    }
                    //QVector<QAction*> vecAction = getRouteActionByRouteId(pTrainNumWnd->m_strCheCi);
                    QAction* pAction = nullptr;
                    for(int j = 0; j < vecAction.size(); j++)
                    {
                        pAction = vecAction.at(j);
                        if(pAction)
                        {
                            menuCheCi->addAction(pAction);
                            if(pMyStation->m_nFCZKMode || (pMyStation->m_nRoutePermission != 1) ||
                                    (pMyStation->m_ArrayRouteOrder.size() == 0) || (pMyStation->m_nModeState != 1))
                                pAction->setEnabled(false);
                            else pAction->setEnabled(true);
                        }
                        else menuCheCi->addSeparator();
                    }
                }
            }
            else
            {
                SetMenuState(menuCheCi,"加车次号",false,true);
            }

            emit SendTextAnalysisSignal(85, QCursor::pos(), pMyStation->GetGDNameByCode(pTrainNumWnd->m_nPosCode));
            selectAct = menuCheCi->exec(QCursor::pos());

            if(bTrainWidgetRBtnMenu)
            {
                QAction* pAction = nullptr;
                for(int j = 0; j < vecAction.size(); j++)
                {
                    pAction = vecAction.at(j);
                    if(pAction)
                        menuCheCi->removeAction(pAction);
                }
            }

            if(selectAct != nullptr)
            {
                QString text=selectAct->text();
//                qDebug()<<"车次操作-"<<text;
                if(text.contains("设置车次启动"))
                {
                    emit SendTextAnalysisSignal(94, QCursor::pos());
                    pTrain->m_bFlash=false;
                    SetCHECIFiring(pMyStation,pTrain,text);
                    return true;
                }
                else if(text.contains("设置车次停稳"))
                {
                    emit SendTextAnalysisSignal(93, QCursor::pos());
                    pTrain->m_bFlash=false;
                    SetCHECIStop(pMyStation,pTrain,text);
                    return true;
                }
                //缺少车次框，缺少处理加车命令，其余可处理？？？
                else if(text.contains("删车次号"))
                {
                    emit SendTextAnalysisSignal(89, QCursor::pos());
                    pTrain->m_bFlash=true;
                    SetCHECIDel(pMyStation,pTrain,text);
                    return true;
                }
                else if(text.contains("车次号确认")||text.contains("修正车次号")||text.contains("变更车次号"))
                {
                    if(text.contains("变更车次号"))
                    {
                        emit SendTextAnalysisSignal(91, QCursor::pos());
                    }
                    else
                    {
                        emit SendTextAnalysisSignal(90, QCursor::pos());
                    }
                    pTrain->m_bFlash=true;
                    SetCHECIConfrim(pMyStation,pTrain,text);
                    return true;
                }
                else if(text.contains("改车次属性"))
                {
                    emit SendTextAnalysisSignal(92, QCursor::pos());
                    pTrain->m_bFlash=true;
                    SetCHECIChange(pMyStation,pTrain,text);
                    return true;
                }
                else if(text.contains("加车次号"))
                {
                    emit SendTextAnalysisSignal(88, QCursor::pos());
                    SetCHECIAdd(pMyStation,pTrainNumWnd->m_nPosCode,text,pTrainNumWnd->misqj);
                    return true;
                }
                else
                {
                    QAction* pAction = nullptr;
                    for(int j = 0; j < vecAction.size(); j++)
                    {
                        pAction = vecAction.at(j);
                        if(pAction && text.contains(pAction->text()))
                            return true;
                    }
                }
                return false;
            }           
            return false;
        }
    }
    return false;
}
//站场操作
bool StationCtrlDispKSK::StationOperate(MyStation* pMyStation)
{
    CGDDC *pCGDDC;
    CXHD *pXHD;
    CGD *pGD;
    CText *pText;
    int ncode=0;
    if(pCurrStation->m_nModeState != 0)
    {
        for(int j=0;j<pMyStation->DevArray.size();j++)
        {
            if(pMyStation->DevArray[j]->getDevType() == Dev_DC)
            {
                pCGDDC=(CGDDC*)(pMyStation->DevArray[j]);
                if(pCGDDC->GetMousePoint(mcoursePoint))
                {
                    QAction *actionMenu=nullptr;
                    for(int i=0;i<menuDC->actions().size();i++)
                    {
                        if(menuDC->actions()[i]->text()=="确认空闲")
                        {
                            if((pCGDDC->flblStatusDW)||(pCGDDC->flblStatusFW)||(pCGDDC->flblStatusCQ))
                            {
                                menuDC->actions()[i]->setVisible(true);
                            }
                            else
                            {
                                menuDC->actions()[i]->setVisible(false);
                            }
                        }
                    }
                    SetMenuDCShow(pCGDDC);
                    emit SendTextAnalysisSignal(83, QCursor::pos(), pCGDDC->m_strName);
                    actionMenu=menuDC->exec(QCursor::pos());
                    if(actionMenu!=nullptr)
                    {
                        ncode=pCGDDC->getCode();
                        QString text=actionMenu->text();
                        if(actionMenu->text() == MENU_DC)
                        {
                            emit SendTextAnalysisSignal(99, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = SWITCH_NORMAL;
                                GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_FC)
                        {
                            emit SendTextAnalysisSignal(100, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = SWITCH_REVERSE;
                                GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_DS)
                        {
                            emit SendTextAnalysisSignal(128, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = SWITCH_LOCK;
                                GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_DJ)
                        {
                            emit SendTextAnalysisSignal(129, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = SWITCH_RELEASE;
                                GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_FSJF)
                        {
                            emit SendTextAnalysisSignal(101, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                if(pCGDDC->getIsFS())
                                {
                                    pStationFunButtonWidget->nFunButtonPressState = UNLOCK_FUNCTION;
                                }
                                else
                                {
                                    pStationFunButtonWidget->nFunButtonPressState = LOCK_FUNCTION;
                                }
                                GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_QGJ)
                        {
                            emit SendTextAnalysisSignal(130, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                mPassWordEntryDlg_NEW->strDescrib = "区故解,请输入密码";
                                mPassWordEntryDlg_NEW->InitPassWord();
                                mPassWordEntryDlg_NEW->exec();
                                if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    //mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                    mFunButtonDevInfo.nCode=pCGDDC->getQDCode();
                                    pStationFunButtonWidget->nFunButtonPressState = SECTION_RELEASE;
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    emit SendDataToSecerSignal(4);
                                    OperateOrderClean();
                                }
                            }
                        }
                        if(actionMenu->text() == MENU_FLBL_CQ)
                        {
                            emit SendTextAnalysisSignal(102, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                mPassWordEntryDlg_NEW->InitPassWord();
                                mPassWordEntryDlg_NEW->exec();
                                if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                    pStationFunButtonWidget->nFunButtonPressState = CQBAD_SHUNT;
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    emit SendDataToSecerSignal(4);
                                    OperateOrderClean();
                                }
                            }
                        }
                        if(actionMenu->text() == MENU_FLBL_DW)
                        {
                            emit SendTextAnalysisSignal(103, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                mPassWordEntryDlg_NEW->InitPassWord();
                                mPassWordEntryDlg_NEW->exec();
                                if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                    pStationFunButtonWidget->nFunButtonPressState = DWBAD_SHUNT;
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    emit SendDataToSecerSignal(4);
                                    OperateOrderClean();
                                }
                            }
                        }
                        if(actionMenu->text() == MENU_FLBL_FW)
                        {
                            emit SendTextAnalysisSignal(104, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                mPassWordEntryDlg_NEW->InitPassWord();
                                mPassWordEntryDlg_NEW->exec();
                                if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                    pStationFunButtonWidget->nFunButtonPressState = FWBAD_SHUNT;
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    emit SendDataToSecerSignal(4);
                                    OperateOrderClean();
                                }
                            }
                        }
                        if(actionMenu->text() == MENU_QRKX)
                        {
                            emit SendTextAnalysisSignal(105, QCursor::pos());
                            if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {
                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = BAD_SHUNT_KX;
                                GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_JCW_DWWD)
                        {
                            emit SendTextAnalysisSignal(107, QCursor::pos());
                            //if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {

                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCDW;
                                if(pCGDDC->getIsWD_DW())
                                {
                                    mFunButtonDevInfo.strButtonName = "道岔设置有电";
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    mFunButtonDevInfo.strButtonName = "道岔设置无电";
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                mFunButtonDevInfo.strButtonName = "";
                            }
                        }
                        if(actionMenu->text() == MENU_JCW_FWWD)
                        {
                            emit SendTextAnalysisSignal(108, QCursor::pos());
                            //if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                            {

                                mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCFW;
                                if(pCGDDC->getIsWD_FW())
                                {
                                    mFunButtonDevInfo.strButtonName = "道岔设置有电";
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    mFunButtonDevInfo.strButtonName = "道岔设置无电";
                                    GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                mFunButtonDevInfo.strButtonName = "";
                            }
                        }
                        emit SendDataToSecerSignal(4);
                        OperateOrderClean();
                        return true;
                    }
                    return true;
                }
            }
            else if(pMyStation->DevArray[j]->getDevType() == Dev_XH)
            {
                pXHD=(CXHD*)(pMyStation->DevArray[j]);
                if(mFunButtonDevInfo.nDevCode[0] == -1)
                {
                    if(pXHD->getSignalType() == false)
                    {
                        if ((pXHD->GetMousePoint_XHD(mcoursePoint))||(pXHD->GetMousePoint_Name(mcoursePoint)))
                        {
                            QAction *actionMenu=nullptr;
                            for(int i=0;i<menuXH->actions().size();i++)
                            {
                                //通过
                                if(pXHD->m_TGXHDButtonShowFlag)
                                {
                                    if(menuXH->actions()[i]->objectName()=="通过")
                                    {
                                        menuXH->actions()[i]->setVisible(true);
                                    }
                                }
                                else
                                {
                                    if(menuXH->actions()[i]->objectName()=="通过")
                                    {
                                        menuXH->actions()[i]->setVisible(false);
                                    }
                                }
                                //列车
                                if(pXHD->m_LCXHDButtonShowFlag)
                                {
                                    if(menuXH->actions()[i]->objectName()=="列车")
                                    {
                                        menuXH->actions()[i]->setVisible(true);
                                    }
                                }
                                else
                                {
                                    if(menuXH->actions()[i]->objectName()=="列车")
                                    {
                                        menuXH->actions()[i]->setVisible(false);
                                    }
                                }
                                //调车
                                if(pXHD->m_DCXHDButtonShowFlag)
                                {
                                    if(menuXH->actions()[i]->objectName()=="调车")
                                    {
                                        menuXH->actions()[i]->setVisible(true);
                                    }
                                }
                                else
                                {
                                    if(menuXH->actions()[i]->objectName()=="调车")
                                    {
                                        menuXH->actions()[i]->setVisible(false);
                                    }
                                }
                                //引导
                                if(pXHD->m_YDXHDButtonShowFlag)
                                {
                                    if(menuXH->actions()[i]->objectName()=="引导")
                                        menuXH->actions()[i]->setVisible(true);
                                }
                                else
                                {
                                    if(menuXH->actions()[i]->objectName()=="引导")
                                        menuXH->actions()[i]->setVisible(false);
                                }
                                //坡道解锁
                                if(menuXH->actions()[i]->objectName()=="坡道解锁")
                                {
                                    bool bd=pXHD->m_PDJSFalg;
                                    QString sss=pXHD->getName();
                                    menuXH->actions()[i]->setVisible(pXHD->m_PDJSFalg);
                                }
                                //点灯灭灯
                                if(menuXH->actions()[i]->text().contains("点灯/灭灯"))
                                {
                                    if(pXHD->getXHDType()==DC_XHJ)//调车信号机
                                    {
                                        menuXH->actions()[i]->setVisible(false);
                                    }
                                    else//列车信号机
                                    {
                                        //信号机上下行属性   //上下行咽喉 1:S 0:X
                                        if(pXHD->getSX()==1)
                                        {
                                            menuXH->actions()[i]->setVisible(pCurrStation->nDDMDStateS);
                                        }
                                        else if(pXHD->getSX()==0)
                                        {
                                            menuXH->actions()[i]->setVisible(pCurrStation->nDDMDStateX);
                                        }
                                        //menuXH->actions()[i]->setVisible(true);
                                    }
                                }
                                //封锁/解封
                                if(menuXH->actions()[i]->text()=="封锁/解封")
                                {
                                    menuXH->actions()[i]->setVisible(false);
                                }
                                if(menuXH->actions()[i]->text().contains(MENU_FSJF))//信号机封锁解封对钩单独处理,其余的股道道岔对钩使用函数SetMenuCheck
                                {
                                    if(pXHD->getIsFB())
                                    {
                                        menuXH->actions()[i]->setChecked(true);
                                    }
                                    else
                                    {
                                        menuXH->actions()[i]->setChecked(false);
                                    }
                                }
                            }
                            SetMenuXHShow(pXHD);
                            emit SendTextAnalysisSignal(84, QCursor::pos(), pXHD->m_strName);
                            actionMenu=menuXH->exec(QCursor::pos());
                            if(actionMenu!=nullptr)
                            {
                                QString text=actionMenu->text();
                                if(actionMenu->text() == MENU_JL_TG)
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(116, QCursor::pos());
                                    int nFunType = 0;
                                    if(mFunButtonDevInfo.nDevCode[0]!=-1)
                                    {
                                        return false;//通过按钮只能第一个被按下
                                    }
                                    nFunType=0x03;
                                    pXHD->m_nTgIsDown = 1;
                                    pCurrStation->ANFlashFlag = 0;
                                    pCurrStation->ButtonFlashDown = true;

                                    emit SendDataToSecerSignal(1,nFunType,pXHD->getCode());
                                    if(mFunButtonDevInfo.nDevCode[mFunButtonDevInfo.RecordDevNum] == -1)
                                    {
                                        mFunButtonDevInfo.SetCode(SetRouteCode(pXHD->nTxtTGButtonCode,nFunType),mFunButtonDevInfo.RecordDevNum);
                                    }
                                    mFunButtonDevInfo.nDevType=Dev_XH;
                                    mFunButtonDevInfo.RecordDevNum++;//按钮按下个数
                                    if(mFunButtonDevInfo.RecordDevNum>0)
                                    {
                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                    }

                                }
                                if(actionMenu->text() == MENU_JL_LC)
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(117, QCursor::pos());
                                    int nFunType = 0;
                                    if(mFunButtonDevInfo.nDevCode[0]!=-1)
                                    {
                                        return false;//右键点击排列进路只能存在第一个按钮
                                    }
                                    //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                                    emit SendDataToSecerSignal(2);
                                    //列车信号机
                                    nFunType=0x01;
                                    pXHD->setIsLCDown(1);
                                    emit SendDataToSecerSignal(1,nFunType,pXHD->getCode());

                                    if(mFunButtonDevInfo.nDevCode[mFunButtonDevInfo.RecordDevNum] == -1)
                                    {
                                        mFunButtonDevInfo.SetCode(SetRouteCode(pXHD->getCode(),nFunType),mFunButtonDevInfo.RecordDevNum);
                                    }

                                    mFunButtonDevInfo.nDevType=Dev_XH;
                                    mFunButtonDevInfo.RecordDevNum++;//按钮按下个数
                                    if(mFunButtonDevInfo.RecordDevNum>0)
                                    {
                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                    }

                                }
                                if(actionMenu->text() == MENU_JL_DC)
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(118, QCursor::pos());
                                    int nFunType=0;
                                    //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                                    emit SendDataToSecerSignal(2);
                                    //调车信号机
                                    nFunType=0x02;
                                    pXHD->setIsDCDown(1);
                                    emit SendDataToSecerSignal(1,nFunType,pXHD->getCode());

                                    if(mFunButtonDevInfo.nDevCode[mFunButtonDevInfo.RecordDevNum] == -1)
                                    {
                                        mFunButtonDevInfo.SetCode(SetRouteCode(pXHD->getCode(),nFunType),mFunButtonDevInfo.RecordDevNum);
                                    }

                                    mFunButtonDevInfo.nDevType=Dev_XH;
                                    mFunButtonDevInfo.RecordDevNum++;//按钮按下个数
                                    if(mFunButtonDevInfo.RecordDevNum>0)
                                    {
                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                    }

                                }
                                if(actionMenu->text().contains(MENU_JL_CANCLE))
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(111, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {

                                        if(ShowDlgByCheck(text,"信号机",pXHD->getName()))
                                        {
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                            //pXHD->setIsLCDown(1);
                                            pCurrStation->ANFlashFlag = 0;
                                            pCurrStation->ButtonFlashDown = true;
                                            emit SendDataToSecerSignal(1,0x14,pXHD->getCode());
                                            mFunButtonDevInfo.nCode=pXHD->getCode();
                                            mFunButtonDevInfo.nDevType=Dev_XH;
                                            pStationFunButtonWidget->nFunButtonPressState = ALL_CANCEL;
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                            OperateOrderClean();

                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                                if(actionMenu->text().contains(MENU_XHREOPEN))
                                {

                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(112, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {
                                        if(ShowDlgByCheck(text,"信号机",pXHD->getName()))
                                        {
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                            //pXHD->setIsLCDown(1);
                                            pCurrStation->ANFlashFlag = 0;
                                            pCurrStation->ButtonFlashDown = true;
                                            emit SendDataToSecerSignal(1,0x05,pXHD->getCode());
                                            mFunButtonDevInfo.nCode=pXHD->getCode();
                                            mFunButtonDevInfo.nDevType=Dev_XH;
                                            pStationFunButtonWidget->nFunButtonPressState = SIGNAL_RESTART;
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                            OperateOrderClean();

                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                                if(actionMenu->text().contains(MENU_ZRS))
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(5, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {
                                        if(ShowDlgByCheck(text,"信号机",pXHD->getName()))
                                        {
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                            mPassWordEntryDlg_NEW->strDescrib = "总人解,请输入密码";
                                            mPassWordEntryDlg_NEW->InitPassWord();
                                            mPassWordEntryDlg_NEW->exec();
                                            if(mPassWordEntryDlg_NEW->onOK == true)
                                            {
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                //pXHD->setIsLCDown(1);
                                                pCurrStation->ANFlashFlag = 0;
                                                pCurrStation->ButtonFlashDown = true;
                                                emit SendDataToSecerSignal(1,0x14,pXHD->getCode());
                                                mFunButtonDevInfo.nCode=pXHD->getCode();
                                                mFunButtonDevInfo.nDevType=Dev_XH;
                                                pStationFunButtonWidget->nFunButtonPressState = ALLMAN_RELEASE;
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                OperateOrderClean();
                                            }
                                            else
                                            {
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                            }
                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                                if(actionMenu->text().contains(MENU_FSJF))
                                {
                                    emit SendTextAnalysisSignal(101, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {
                                        if(ShowDlgByCheck(text,"信号机",pXHD->getName()))
                                        {
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                            //pXHD->setIsLCDown(1);
                                            pCurrStation->ANFlashFlag = 0;
                                            pCurrStation->ButtonFlashDown = true;
                                            emit SendDataToSecerSignal(1,0x14,pXHD->getCode());
                                            mFunButtonDevInfo.nCode=pXHD->getCode();
                                            mFunButtonDevInfo.nDevType=Dev_XH;
                                            if(pXHD->getIsFB())
                                            {

                                                pStationFunButtonWidget->nFunButtonPressState = UNLOCK_FUNCTION;
                                            }
                                            else
                                            {
                                                pStationFunButtonWidget->nFunButtonPressState = LOCK_FUNCTION;
                                            }
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                            OperateOrderClean();
                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                                if(actionMenu->text() == MENU_JL_YD)
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(113, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {
                                        if(ShowDlgByCheck(text,"信号机",pXHD->getName()))
                                        {
                                            QString name = pXHD->m_strName;
                                            ncode=pXHD->getCode();
//                                            //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
//                                            emit SendDataToSecerSignal(1,0x12,pXHD->getCode());
                                            if((pXHD->getTimeCount()>0) && pXHD->getXHDState(XHD_YD))
                                            {
                                                pCurrStation->ANFlashFlag = 0;
                                                pCurrStation->ButtonFlashDown = true;
                                                if(mFunButtonDevInfo.nCode==-1)
                                                {
                                                    mFunButtonDevInfo.nCode=ncode;
                                                }
                                                mFunButtonDevInfo.nDevType=Dev_XH;
                                                pStationFunButtonWidget->nFunButtonPressState = CALLON_BTN;
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                OperateOrderClean();

                                            }
                                            else
                                            {
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                mPassWordEntryDlg_NEW->strDescrib = name + "引导进路,请输入密码";
                                                mPassWordEntryDlg_NEW->InitPassWord();
                                                mPassWordEntryDlg_NEW->exec();
                                                if(mPassWordEntryDlg_NEW->onOK == true)
                                                {
                                                    //pXHD->setIsYDDown(1);
                                                    pCurrStation->ANFlashFlag = 0;
                                                    pCurrStation->ButtonFlashDown = true;
                                                    if(mFunButtonDevInfo.nCode==-1)
                                                    {
                                                        mFunButtonDevInfo.nCode=ncode;
                                                    }
                                                    mFunButtonDevInfo.nDevType=Dev_XH;
                                                    pStationFunButtonWidget->nFunButtonPressState = CALLON_BTN;
                                                    //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                    GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    OperateOrderClean();
                                                }
                                                else
                                                {
                                                    emit SendDataToSecerSignal(4);
                                                    OperateOrderClean();
                                                }
                                            }
                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                                if(actionMenu->text() == MENU_PDJS)
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(114, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {
                                        if(ShowDlgByCheck(text,"信号机",pXHD->getName()))
                                        {
                                            //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                            mPassWordEntryDlg_NEW->strDescrib = "坡道解锁,请输入密码";
                                            mPassWordEntryDlg_NEW->InitPassWord();
                                            mPassWordEntryDlg_NEW->exec();
                                            if(mPassWordEntryDlg_NEW->onOK == true)
                                            {
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                //pXHD->setIsLCDown(1);
                                                pCurrStation->ANFlashFlag = 0;
                                                pCurrStation->ButtonFlashDown = true;
                                                emit SendDataToSecerSignal(1,0x15,pXHD->getCode());
                                                mFunButtonDevInfo.nCode=pXHD->getCode();
                                                mFunButtonDevInfo.nDevType=Dev_XH;
                                                pStationFunButtonWidget->nFunButtonPressState = RAMP_RELEASE;
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                OperateOrderClean();

                                            }
                                            else
                                            {
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                            }
                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                                if(actionMenu->text() == MENU_DDMD)
                                {
                                    if(pXHD->getIsFB() == true)
                                    {
                                        return false;
                                    }
                                    emit SendTextAnalysisSignal(186, QCursor::pos());
                                    if(mFunButtonDevInfo.nCode==-1)
                                    {
                                        if(pXHD->getIsMD())
                                        {
                                            QString XHJType;
                                            if(pXHD->getXHDType() == JZ_XHJ)
                                            {
                                                XHJType = "进站信号机";
                                            }
                                            else if(pXHD->getXHDType() == CZ_XHJ)
                                            {
                                                XHJType = "出站信号机";
                                            }
                                            else
                                            {
                                                XHJType = "信号机";
                                            }
                                            if(ShowDlgByCheck(text,XHJType,pXHD->getName()))
                                            {
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                //pXHD->setIsLCDown(1);
                                                pCurrStation->ANFlashFlag = 0;
                                                pCurrStation->ButtonFlashDown = true;
                                                emit SendDataToSecerSignal(1,0x15,pXHD->getCode());
                                                mFunButtonDevInfo.nCode=pXHD->getCode();
                                                mFunButtonDevInfo.nDevType=Dev_XH;
                                                pStationFunButtonWidget->nFunButtonPressState = LIGHT_ON;
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                OperateOrderClean();

                                            }
                                            else
                                            {
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                            }
                                        }
                                        else
                                        {
                                            QString XHJType;
                                            if(pXHD->getXHDType() == JZ_XHJ)
                                            {
                                                XHJType = "进站信号机";
                                            }
                                            else if(pXHD->getXHDType() == CZ_XHJ)
                                            {
                                                XHJType = "出站信号机";
                                            }
                                            else
                                            {
                                                XHJType = "信号机";
                                            }
                                            if(ShowDlgByCheck(text,XHJType,pXHD->getName()))
                                            {
                                                //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                mPassWordEntryDlg_NEW->strDescrib = "开关信号,请输入密码";
                                                mPassWordEntryDlg_NEW->InitPassWord();
                                                mPassWordEntryDlg_NEW->exec();
                                                if(mPassWordEntryDlg_NEW->onOK == true)
                                                {
                                                    //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                    //pXHD->setIsLCDown(1);
                                                    pCurrStation->ANFlashFlag = 0;
                                                    pCurrStation->ButtonFlashDown = true;
                                                    emit SendDataToSecerSignal(1,0x15,pXHD->getCode());
                                                    mFunButtonDevInfo.nCode=pXHD->getCode();
                                                    mFunButtonDevInfo.nDevType=Dev_XH;
                                                    pStationFunButtonWidget->nFunButtonPressState = LIGHT_OFF;
                                                    //pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                    GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    OperateOrderClean();

                                                }
                                                else
                                                {
                                                    emit SendDataToSecerSignal(4);
                                                    OperateOrderClean();
                                                }
                                            }
                                            else
                                            {
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                            }
                                        }
                                    }
                                }
                                return true;
                            }
                            return true;
                        }
                    }
                    //                else//虚信号没有右键菜单
                    //                {
                    //                    if ((pXHD->GetMousePoint_LCAN(mcoursePoint))||(pXHD->GetMousePoint_DCAN(mcoursePoint)))
                    //                    {
                    //                        QAction *actionMenu=nullptr;
                    //                        for(int i=0;i<menuXH->actions().size();i++)
                    //                        {
                    //                            //通过
                    //                            if(menuXH->actions()[i]->objectName()=="通过")
                    //                            {
                    //                               menuXH->actions()[i]->setVisible(false);
                    //                            }
                    //                            //列车
                    //                            if(menuXH->actions()[i]->objectName()=="列车")
                    //                            {
                    //                               menuXH->actions()[i]->setVisible(false);
                    //                            }
                    //                            //调车
                    //                            if(menuXH->actions()[i]->objectName()=="调车")
                    //                            {
                    //                              menuXH->actions()[i]->setVisible(false);
                    //                            }
                    //                            //引导
                    //                            if(menuXH->actions()[i]->objectName()=="引导")
                    //                            {
                    //                                menuXH->actions()[i]->setVisible(false);
                    //                            }
                    //                            //坡道解锁
                    //                            if(menuXH->actions()[i]->objectName()=="坡道解锁")
                    //                            {
                    //                                menuXH->actions()[i]->setVisible(false);
                    //                            }
                    //                            //点灯灭灯
                    //                            if(menuXH->actions()[i]->text().contains("点灯/灭灯"))
                    //                            {
                    //                                menuXH->actions()[i]->setVisible(false);
                    //                            }
                    //                            //封锁/解封
                    //                            if(menuXH->actions()[i]->text()=="封锁/解封")
                    //                            {
                    //                                menuXH->actions()[i]->setVisible(true);
                    //                            }
                    //                        }
                    //                        SetMenuXHShow(pXHD);
                    //                        actionMenu=menuXH->exec(QCursor::pos());
                    //                        if(actionMenu!=nullptr)
                    //                        {
                    //                            if(actionMenu->text().contains(MENU_FSJF))
                    //                            {
                    //                                if(mFunButtonDevInfo.nCode==-1)
                    //                                {
                    //                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                    //                                    pXHD->setIsLCDown(1);
                    //                                    pCurrStation->ANFlashFlag = 0;
                    //                                    pCurrStation->ButtonFlashDown = true;
                    //                                    emit SendDataToSecerSignal(1,0x14,pXHD->getCode());
                    //                                    mFunButtonDevInfo.nCode=pXHD->getCode();
                    //                                    mFunButtonDevInfo.nDevType=Dev_XH;
                    //                                    if(pXHD->getIsFB())
                    //                                    {
                    //                                        pStationFunButtonWidget->nFunButtonPressState = UNLOCK_FUNCTION;
                    //                                    }
                    //                                    else
                    //                                    {
                    //                                        pStationFunButtonWidget->nFunButtonPressState = LOCK_FUNCTION;
                    //                                    }
                    //                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                    //                                }
                    //                            }
                    //                            return true;
                    //                        }
                    //                    }
                    //                }
                }
            }
            else if(pMyStation->DevArray[j]->getDevType() == Dev_GD)
            {
                pGD=(CGD*)(pMyStation->DevArray[j]);
                if(pGD->GetMousePoint(mcoursePoint))
                {
                    for(int i=0;i<menuGD->actions().size();i++)
                    {
                        if(menuGD->actions()[i]->text()=="确认空闲")
                        {
                            if(!pGD->getGDFLBL())
                            {
                                menuGD->actions()[i]->setVisible(false);
                            }
                            else
                            {
                                menuGD->actions()[i]->setVisible(true);
                            }
                        }
                        else if(menuGD->actions()[i]->text()=="股道无电"||menuGD->actions()[i]->text()=="区故解")
                        {
                            if(pGD->getGDType()==JJ_QD)
                            {
                                menuGD->actions()[i]->setVisible(false);
                            }
                            else
                            {
                                menuGD->actions()[i]->setVisible(true);
                            }
                        }
                    }
                    QAction *actionMenu=nullptr;
                    SetMenuGDShow(pGD);
                    emit SendTextAnalysisSignal(82, QCursor::pos(), pGD->m_strName);
                    actionMenu=menuGD->exec(QCursor::pos());
                    if(actionMenu!=nullptr)
                    {
                        ncode=pGD->getCode();
                        QString text=actionMenu->text();
                        QString strtype;
                        if(pGD->getGDType()==GD_QD)
                        {
                            strtype="股道";
                        }
                        else if(pGD->getGDType()==JJ_QD)
                        {
                            strtype="区间轨道";
                        }
                        else if(pGD->getGDType()==QD)
                        {
                            strtype="无岔轨道";
                        }
                        if(actionMenu->text() == MENU_FSJF)
                        {
                            emit SendTextAnalysisSignal(101, QCursor::pos());
                            if(ShowDlgByCheck(text,strtype,pGD->getName()))
                            {
                                mFunButtonDevInfo.nCode=pGD->getCode();
                                if(pGD->getFS())
                                {
                                    pStationFunButtonWidget->nFunButtonPressState = UNLOCK_FUNCTION;
                                }
                                else
                                {
                                    pStationFunButtonWidget->nFunButtonPressState = LOCK_FUNCTION;
                                }
                                GetFunButtonType(Dev_GD,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_QGJ)
                        {
                            emit SendTextAnalysisSignal(130, QCursor::pos());
                            if(ShowDlgByCheck(text,strtype,pGD->getName()))
                            {
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                mPassWordEntryDlg_NEW->strDescrib = "区故解,请输入密码";
                                mPassWordEntryDlg_NEW->InitPassWord();
                                mPassWordEntryDlg_NEW->exec();
                                if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    mFunButtonDevInfo.nCode=pGD->getCode();
                                    pStationFunButtonWidget->nFunButtonPressState = SECTION_RELEASE;
                                    GetFunButtonType(Dev_GD,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    emit SendDataToSecerSignal(4);
                                    OperateOrderClean();
                                }
                            }
                        }
                        if(actionMenu->text() == MENU_FLBL)
                        {
                            emit SendTextAnalysisSignal(115, QCursor::pos());
                            if(ShowDlgByCheck(text,strtype,pGD->getName()))
                            {
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                mPassWordEntryDlg_NEW->InitPassWord();
                                mPassWordEntryDlg_NEW->exec();
                                if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    mFunButtonDevInfo.nCode=pGD->getCode();
                                    pStationFunButtonWidget->nFunButtonPressState = BAD_SHUNT;
                                    GetFunButtonType(Dev_GD,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);

                                }
                                else
                                {
                                    emit SendDataToSecerSignal(4);
                                    OperateOrderClean();
                                }
                            }
                        }
                        if(actionMenu->text() == MENU_QRKX)
                        {
                            emit SendTextAnalysisSignal(105, QCursor::pos());
                            if(ShowDlgByCheck(text,strtype,pGD->getName()))
                            {
                                mFunButtonDevInfo.nCode=pGD->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = BAD_SHUNT_KX;
                                GetFunButtonType(Dev_GD,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                            }
                        }
                        if(actionMenu->text() == MENU_GDWD)
                        {
                            //if(ShowDlgByCheck(text,strtype,pGD->getName()))
                            {
                                emit SendTextAnalysisSignal(106, QCursor::pos());
                                mFunButtonDevInfo.nCode=pGD->getCode();
                                pStationFunButtonWidget->nFunButtonPressState = POWEROFF_GD;
                                if(pGD->getPowerCut())
                                {
                                    mFunButtonDevInfo.strButtonName = "股道设置有电";
                                    GetFunButtonType(Dev_GD,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                else
                                {
                                    mFunButtonDevInfo.strButtonName = "股道设置无电";
                                    GetFunButtonType(Dev_GD,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                }
                                mFunButtonDevInfo.strButtonName = "";

                            }
                        }
                        emit SendDataToSecerSignal(4);
                        OperateOrderClean();
                        return true;
                    }
                    return true;
                }
            }
            else if(pMyStation->DevArray[j]->getDevType() == Dev_TEXT)
            {
                pText=(CText*)(pMyStation->DevArray[j]);
                if(pText->GetMousePoint(mcoursePoint))
                {
                    if(pText->getTextType() == DCQDNAMETEXT)
                    {
                        emit SendTextAnalysisSignal(82, QCursor::pos(), pText->m_strName);
                        if(pText->m_strName.right(2) == "DG")
                        {
                            QString DCText = pText->m_strName.left(pText->m_strName.length()-2);
                            QString ch = "-";
                            QStringList strArr;
                            int c = StringSplit(DCText, ch, strArr);
                            for(int i=0;i<pMyStation->DevArray.size();i++)
                            {
                                if(pMyStation->DevArray[i]->getDevType() == Dev_DC)
                                {

                                    pCGDDC=(CGDDC*)(pMyStation->DevArray[i]);
                                    if (2 == c)
                                    {
                                        if(pCGDDC->m_strName == strArr[0])
                                        {
                                            if(mCBaseDataOld!=nullptr)
                                            {
                                                mCBaseDataOld->m_RangeVisible=false;
                                            }
                                            mCBaseDataOld=pMyStation->DevArray[i];
                                            pCGDDC->m_RangeVisible = true;
                                            QAction *actionMenu=nullptr;
                                            for(int k=0;k<menuDC->actions().size();k++)
                                            {
                                                if(menuDC->actions()[k]->text()=="确认空闲")
                                                {
                                                    //if(pCGDDC->getIsFLBL_CQ()||pCGDDC->getIsFLBL_DW()||pCGDDC->getIsFLBL_FW())
                                                    if((pCGDDC->flblStatusDW)||(pCGDDC->flblStatusFW)||(pCGDDC->flblStatusCQ))
                                                    {
                                                        menuDC->actions()[k]->setVisible(true);
                                                    }
                                                    else
                                                    {
                                                        menuDC->actions()[k]->setVisible(false);
                                                    }
                                                }
                                            }
                                            SetMenuDCShow(pCGDDC);
                                            actionMenu=menuDC->exec(QCursor::pos());
                                            if(actionMenu!=nullptr)
                                            {
                                                ncode=pCGDDC->getCode();
                                                QString text=actionMenu->text();
                                                if(actionMenu->text() == MENU_DC)
                                                {
                                                    emit SendTextAnalysisSignal(99, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_NORMAL;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FC)
                                                {
                                                    emit SendTextAnalysisSignal(100, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_REVERSE;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_DS)
                                                {
                                                    emit SendTextAnalysisSignal(128, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_LOCK;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_DJ)
                                                {
                                                    emit SendTextAnalysisSignal(129, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_RELEASE;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FSJF)
                                                {
                                                    emit SendTextAnalysisSignal(101, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        if(pCGDDC->getIsFS())
                                                        {
                                                            pStationFunButtonWidget->nFunButtonPressState = UNLOCK_FUNCTION;
                                                        }
                                                        else
                                                        {
                                                            pStationFunButtonWidget->nFunButtonPressState = LOCK_FUNCTION;
                                                        }
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_QGJ)
                                                {
                                                    emit SendTextAnalysisSignal(130, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "区故解,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = SECTION_RELEASE;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FLBL_CQ)
                                                {
                                                    emit SendTextAnalysisSignal(102, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = CQBAD_SHUNT;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FLBL_DW)
                                                {
                                                    emit SendTextAnalysisSignal(103, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = DWBAD_SHUNT;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FLBL_FW)
                                                {
                                                    emit SendTextAnalysisSignal(104, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = FWBAD_SHUNT;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_QRKX)
                                                {
                                                    emit SendTextAnalysisSignal(105, QCursor::pos());
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = BAD_SHUNT_KX;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_JCW_DWWD)
                                                {
                                                    emit SendTextAnalysisSignal(107, QCursor::pos());
                                                    //if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {

                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCDW;
                                                        if(pCGDDC->getIsWD_DW())
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置有电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置无电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        mFunButtonDevInfo.strButtonName = "";

                                                    }
                                                }
                                                if(actionMenu->text() == MENU_JCW_FWWD)
                                                {
                                                    emit SendTextAnalysisSignal(108, QCursor::pos());
                                                    //if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {

                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCFW;
                                                        if(pCGDDC->getIsWD_FW())
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置有电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置无电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        mFunButtonDevInfo.strButtonName = "";

                                                    }
                                                }
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                                return true;
                                            }
                                            return true;
                                        }
                                        else
                                        {
                                            pCGDDC->m_RangeVisible = false;
                                        }
                                    }
                                    else
                                    {
                                        if(pCGDDC->m_strName == DCText)
                                        {
                                            if(mCBaseDataOld!=nullptr)
                                            {
                                                mCBaseDataOld->m_RangeVisible=false;
                                            }
                                            mCBaseDataOld=pMyStation->DevArray[i];
                                            pCGDDC->m_RangeVisible = true;
                                            QAction *actionMenu=nullptr;
                                            for(int i=0;i<menuDC->actions().size();i++)
                                            {
                                                if(menuDC->actions()[i]->text()=="确认空闲")
                                                {
                                                    //if(pCGDDC->getIsFLBL_CQ()||pCGDDC->getIsFLBL_DW()||pCGDDC->getIsFLBL_FW())
                                                    if((pCGDDC->flblStatusDW)||(pCGDDC->flblStatusFW)||(pCGDDC->flblStatusCQ))
                                                    {
                                                        menuDC->actions()[i]->setVisible(true);
                                                    }
                                                    else
                                                    {
                                                        menuDC->actions()[i]->setVisible(false);
                                                    }
                                                }
                                            }
                                            SetMenuDCShow(pCGDDC);
                                            actionMenu=menuDC->exec(QCursor::pos());
                                            if(actionMenu!=nullptr)
                                            {
                                                ncode=pCGDDC->getCode();
                                                QString text=actionMenu->text();
                                                if(actionMenu->text() == MENU_DC)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_NORMAL;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FC)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_REVERSE;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_DS)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_LOCK;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_DJ)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = SWITCH_RELEASE;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FSJF)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        if(pCGDDC->getIsFS())
                                                        {
                                                            pStationFunButtonWidget->nFunButtonPressState = UNLOCK_FUNCTION;
                                                        }
                                                        else
                                                        {
                                                            pStationFunButtonWidget->nFunButtonPressState = LOCK_FUNCTION;
                                                        }
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_QGJ)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "区故解,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = SECTION_RELEASE;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FLBL_CQ)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = CQBAD_SHUNT;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FLBL_DW)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = DWBAD_SHUNT;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_FLBL_FW)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                                                        mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                                        mPassWordEntryDlg_NEW->InitPassWord();
                                                        mPassWordEntryDlg_NEW->exec();
                                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                                        {
                                                            mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                            pStationFunButtonWidget->nFunButtonPressState = FWBAD_SHUNT;
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            emit SendDataToSecerSignal(4);
                                                            OperateOrderClean();
                                                        }
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_QRKX)
                                                {
                                                    if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = BAD_SHUNT_KX;
                                                        GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_JCW_DWWD)
                                                {
                                                    //if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCDW;
                                                        if(pCGDDC->getIsWD_DW())
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置有电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置无电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        mFunButtonDevInfo.strButtonName = "";
                                                    }
                                                }
                                                if(actionMenu->text() == MENU_JCW_FWWD)
                                                {
                                                    //if(ShowDlgByCheck(text,"道岔",pCGDDC->getName()))
                                                    {
                                                        mFunButtonDevInfo.nCode=pCGDDC->getCode();
                                                        pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCFW;
                                                        if(pCGDDC->getIsWD_FW())
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置有电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        else
                                                        {
                                                            mFunButtonDevInfo.strButtonName = "道岔设置无电";
                                                            GetFunButtonType(Dev_DC,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                                                        }
                                                        mFunButtonDevInfo.strButtonName = "";
                                                    }
                                                }
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                                return true;
                                            }
                                            return true;
                                        }
                                        else
                                        {
                                            pCGDDC->m_RangeVisible = false;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if(pMyStation->DevArray[j]->getDevType() == Dev_TG)
            {}
            else if(pMyStation->DevArray[j]->getDevType() == Dev_JTX)
            {}
            else if(pMyStation->DevArray[j]->getDevType() == Dev_QD)
            {}
            else
            {
                //            SetMenuStaShow();
                //            menuSTA->exec(QCursor::pos());
                //            return true;
            }
        }

    }
    //SetMenuStaShow(nEnlargeNarrow);
    SetMenuStaShow();
    menuSTA->exec(QCursor::pos());
    return true;
    //return false;
}
//文字操作
bool StationCtrlDispKSK::TextAnnotationOperate(MyStation* pMyStation)
{
    TextAnnotation *TextAnnotation=nullptr;
    for(int j=0;j<pMyStation->vectTextAnnotation.size();j++)
    {
        TextAnnotation=pMyStation->vectTextAnnotation[j];
        if(TextAnnotation->GetMousePoint(mcoursePoint))
        {
            //文字标注右键菜单
            QAction* action=nullptr;
            action = menuTextAnnotation->exec(QCursor::pos());
            if(action)
            {
                QString text=action->text();
                if(text.contains("修改"))
                {
                    TextAnnotation->isShow = false;
                    mTextAnnotationWidget=new TextAnnotationWidget(this, TextAnnotation);
                    mTextAnnotationWidget->setObjectName("TextAnnotationWidget");
                    //文字标注 mTextAnnotationWidget
                    connect(mTextAnnotationWidget,SIGNAL(SaveSignal(TextAnnotation*)),this,SLOT(SaveSolt(TextAnnotation*)));
                    int width =0;
                    if(TextAnnotation->m_Rectcont.width() <= 80)
                        width = 80;
                    else width = TextAnnotation->m_Rectcont.width();
                    mTextAnnotationWidget->setGeometry(TextAnnotation->m_Rect.x() - ui->scrollArea->horizontalScrollBar()->sliderPosition(),
                                                       TextAnnotation->m_Rect.y() - ui->scrollArea->verticalScrollBar()->sliderPosition(), width, TextAnnotation->m_Rectcont.height());
                    mTextAnnotationWidget->SetRect(TextAnnotation->m_Rect);
                    mTextAnnotationWidget->SetText(TextAnnotation->strAnnotation);
                    mTextAnnotationWidget->show();
                    mTextAnnotationWidget->setEditFocus();
                }
                else if(text.contains("删除"))
                {
                    curTextAnnotationWidget = nullptr;
                    pMyStation->vectTextAnnotation.remove(j);
                }
                else if(text.contains("字体"))
                {
                    TextAnnotation->SetTextFont();
                }
                else if(text.contains("前景色"))
                {
                    TextAnnotation->SetTextcolor(0);
                }
                else if(text.contains("背景色"))
                {
                    TextAnnotation->SetTextcolor(1);
                }
                else if(text.contains("清空所有标记"))
                {
                    int ret = QMessageBox::information(this, tr("提示"), "确定是否清空当前所有标志", tr("确定"), tr("取消"), "", 0);
                    if(ret==0)
                    {
                        pCurrStation->vectTextAnnotation.clear();
                    }
                }
                return true;
            }
            else return false;
        }
    }
    return false;
}
//文字操作 拖动
TextAnnotation * StationCtrlDispKSK::TextAnnotationCheck(MyStation* pMyStation)
{
    TextAnnotation* TextAnnotation = nullptr;
    for(int j=0;j<pMyStation->vectTextAnnotation.size();j++)
    {
        TextAnnotation=pMyStation->vectTextAnnotation[j];
        if(TextAnnotation->GetMousePoint(mcoursePoint))
        {
            return  TextAnnotation;
        }
    }
    return nullptr;
}
//车次预告窗操作
bool StationCtrlDispKSK::RoutePreWndOperate(MyStation* pMyStation)
{
    QSettings* settings = new QSettings("Data/Global.ini", QSettings::IniFormat);
    // 指定为中文
    settings->setIniCodec("UTF-8");

    //读取配置-通信信息
    bool bTrainWidgetRBtnMenu = settings->value("CONFIG/TrainWidgetRBtnMenu").toBool();
    if(!bTrainWidgetRBtnMenu)
        return false;

    RoutePreWnd* pRoutePreWnd = nullptr;
    for(int i = 0; i < pMyStation->vectRoutePreWnd.size(); i++)
    {
        pRoutePreWnd = pMyStation->vectRoutePreWnd[i];
        if(pRoutePreWnd->GetMousePoint(mcoursePoint))
        {
            if(pRoutePreWnd->vectRouteInfo.size() > 0)
            {
                RoutePreWnd::RouteInfo info = pRoutePreWnd->vectRouteInfo.at(0);
                QMenu pRoutePreMenu;
                QVector<QAction*> vecAction = getRouteActionByRouteId(pRoutePreWnd->vectRouteInfo[0].route_id);
                QAction* pAction = nullptr;
                for(int j = 0; j < vecAction.size(); j++)
                {
                    pAction = vecAction.at(j);
                    if(pAction)
                    {
                        pRoutePreMenu.addAction(pAction);
                        if(pMyStation->m_nFCZKMode || (pMyStation->m_nRoutePermission != 1) ||
                                (pMyStation->m_ArrayRouteOrder.size() == 0) || (pMyStation->m_nModeState != 1))
                            pAction->setEnabled(false);
                        else pAction->setEnabled(true);
                    }
                    else pRoutePreMenu.addSeparator();

                }

                pRoutePreMenu.exec(QCursor::pos());
                return true;
            }
        }
    }
    return false;
}
//车次窗操作
bool StationCtrlDispKSK::TrainNumWndOperate(MyStation* pMyStation)
{
    TrainNumWnd* pTrainNumWnd = nullptr;
    for(int i = 0; i < pMyStation->m_ArrayTrainNumWnd.size(); i++)
    {
        pTrainNumWnd = pMyStation->m_ArrayTrainNumWnd[i];
        if(pTrainNumWnd->GetMousePoint(mcoursePoint))
        {
            QMenu pRoutePreMenu;
            //QVector<QAction*> vecAction = getRouteActionByTrainNum(pTrainNumWnd->m_strCheCi);
            //QAction* pAction = nullptr;
            //for(int j = 0; j < vecAction.size(); j++)
            //{
            //    pAction = vecAction.at(j);
            //    if(pAction)
            //        pRoutePreMenu.addAction(pAction);
            //    else pRoutePreMenu.addSeparator();
            //    pRoutePreMenu.exec(QCursor::pos());
            //    return true;
            //}
        }
    }
    return false;
}

QVector<QAction*> StationCtrlDispKSK::getRouteActionByRouteId(int route_id)
{
    QVector<QAction*> vecAction;
    TrainRouteOrder* pRouteOrder = nullptr;
    TrainRouteOrder* pRouteOrder2 = nullptr;
    for (int i = 0; i < pCurrStation->m_ArrayRouteOrder.size(); i++)
    {
        if(pCurrStation->m_ArrayRouteOrder.at(i)->route_id == route_id)
        {
            pRouteOrder = pCurrStation->m_ArrayRouteOrder.at(i);
            continue;
        }
    }

    if(pRouteOrder->m_btBeginOrEndFlg == 0xdd && pRouteOrder->m_btRecvOrDepart == 0)
    {
        for (int i = 0; i < pCurrStation->m_ArrayRouteOrder.size(); i++)
        {
            if(pCurrStation->m_ArrayRouteOrder.at(i)->m_strTrainNum == pRouteOrder->m_strTrainNum &&
                    pCurrStation->m_ArrayRouteOrder.at(i)->route_id != pRouteOrder->route_id &&
                    pCurrStation->m_ArrayRouteOrder.at(i)->father_id == 0)
            {
                pRouteOrder2 = pCurrStation->m_ArrayRouteOrder.at(i);
                continue;
            }
        }
    }

    if(!pRouteOrder)
        return vecAction;

    //车次号
    QAction* pActionNum = new QAction(pRouteOrder->m_strTrainNum);
    pActionNum->setEnabled(false);
    vecAction.push_back(pActionNum);
    vecAction.push_back(nullptr);

    bool bTouch = true;
    bool bAutoTouch = true;
    bool bAutoTouch2 = false;
    bool bOperate = false;
    if(pRouteOrder->m_bZHJL)
    {
        for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
        {
            bTouch &= (pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nOldRouteState != 0 && pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nOldRouteState != 5);
            bOperate |= (pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nOldRouteState != 0 && pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nOldRouteState != 5);
            bAutoTouch &= pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nAutoTouch;
            bAutoTouch2 |= pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nAutoTouch;
        }
    }
    else bTouch = (pRouteOrder->m_nOldRouteState != 0 && pRouteOrder->m_nOldRouteState != 5);

    if(!bTouch)
    {
        //进路办理
        {
            QAction* pActionRoute = new QAction(QString("办理%1进路(%2)")
                                                .arg(pRouteOrder->m_btRecvOrDepart ? "发车" : "接车")
                                                .arg(GetRouteDescrip(pRouteOrder)));

            connect(pActionRoute, &QAction::triggered, [=](){

                QString tips = QString("确定要触发'股道:%1','%2'次列车,'%3','%4'进路?")
                        .arg(pRouteOrder->m_strTrack)
                        .arg(pRouteOrder->m_strTrainNum)
                        .arg(GetRouteDescrip(pRouteOrder))
                        .arg((pRouteOrder->m_btRecvOrDepart==0)?"接车":"发车");

                int ret = QMessageBox::question(nullptr, "信息对话框", tips, tr("是"), tr("否"), "", 0);
                if(ret==0)
                {
                    if(pRouteOrder->m_bZHJL)
                    {
                        for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                        {
                            emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, 0xaa);
                        }
                    }
                    else emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder->route_id, 0xaa);
                }

            });
            vecAction.push_back(pActionRoute);
            //进路办理-通过
            if(pRouteOrder->m_btBeginOrEndFlg == 0xdd && pRouteOrder->m_btRecvOrDepart == 0)
            {
                QAction* pActionRouteTG = new QAction(QString("办理通过进路(%1;%2)")
                                                      .arg(GetRouteDescrip(pRouteOrder))
                                                      .arg(GetRouteDescrip(pRouteOrder2)));
                connect(pActionRouteTG, &QAction::triggered, [=](){

                    QString tips = QString("确定要触发'股道:%1','%2'次列车,'%3;%4','%5'进路?")
                            .arg(pRouteOrder->m_strTrack)
                            .arg(pRouteOrder->m_strTrainNum)
                            .arg(GetRouteDescrip(pRouteOrder))
                            .arg(GetRouteDescrip(pRouteOrder2))
                            .arg((pRouteOrder->m_btRecvOrDepart==0)?"接车":"发车");

                    if(QMessageBox::question(nullptr,"信息对话框",tips,tr("是"),tr("否"),"",0) == 0)
                    {
                        if(pRouteOrder->m_bZHJL)
                        {
                            for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                            {
                                emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, 0xaa);
                            }
                        }
                        else emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder->route_id, 0xaa);

                        if(pRouteOrder2)
                        {
                            if(pRouteOrder2->m_bZHJL)
                            {
                                for(int i = 0; i < pRouteOrder2->m_vecSonTrainRouteOrder.size(); i++)
                                {
                                    emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder2->m_vecSonTrainRouteOrder.at(i)->route_id, 0xaa);
                                }
                            }
                            else emit sendRouteOrderMsg(MSGTYPE_MANTRIGGER, pRouteOrder2->route_id, 0xaa);
                        }
                    }
                });
                vecAction.push_back(pActionRouteTG);
            }
        }

        //自触
        {
            QAction* pActionRouteAuto = new QAction(QString("%1自触(%2)")
                                                    .arg(pRouteOrder->m_btRecvOrDepart ? "发车" : "接车")
                                                    .arg(GetRouteDescrip(pRouteOrder, true)));

            connect(pActionRouteAuto, &QAction::triggered, [=](){
                if(pRouteOrder->m_bZHJL)
                {
                    for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                    {
                        emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nAutoTouch ? 0xbb : 0xaa);
                    }
                }
                else emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->route_id, pRouteOrder->m_nAutoTouch ? 0xbb : 0xaa);
            });
            vecAction.push_back(pActionRouteAuto);
            //自触-通过
            if(pRouteOrder->m_btBeginOrEndFlg == 0xdd && pRouteOrder->m_btRecvOrDepart == 0)
            {
                QAction* pActionRouteAutoTG = new QAction(QString("通过自触(%1;%2)")
                                                          .arg(GetRouteDescrip(pRouteOrder, true))
                                                          .arg(GetRouteDescrip(pRouteOrder2, true)));
                connect(pActionRouteAutoTG, &QAction::triggered, [=](){
                    bool bAutoTouch = true;
                    bool bAutoTouch2 = false;

                    if(pRouteOrder->m_bZHJL)
                    {
                        for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                        {
                            bAutoTouch &= pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nAutoTouch;
                        }
                    }
                    else bAutoTouch = pRouteOrder->m_nAutoTouch;

                    if(pRouteOrder2)
                    {
                        if(pRouteOrder2->m_bZHJL)
                        {
                            for(int i = 0; i < pRouteOrder2->m_vecSonTrainRouteOrder.size(); i++)
                            {
                                bAutoTouch2 &= pRouteOrder2->m_vecSonTrainRouteOrder.at(i)->m_nAutoTouch;
                            }
                        }
                        else bAutoTouch2 = pRouteOrder2->m_nAutoTouch;

                        if(pRouteOrder->m_bZHJL)
                        {
                            for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                            {
                                emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, (bAutoTouch && bAutoTouch2) ? 0xbb : 0xaa);
                            }
                        }
                        else emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->route_id, pRouteOrder->m_nAutoTouch&&pRouteOrder2->m_nAutoTouch ? 0xbb : 0xaa);

                        if(pRouteOrder2->m_bZHJL)
                        {
                            for(int i = 0; i < pRouteOrder2->m_vecSonTrainRouteOrder.size(); i++)
                            {
                                emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder2->m_vecSonTrainRouteOrder.at(i)->route_id, (bAutoTouch && bAutoTouch2) ? 0xbb : 0xaa);
                            }
                        }
                        else emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder2->route_id, pRouteOrder->m_nAutoTouch&&pRouteOrder2->m_nAutoTouch ? 0xbb : 0xaa);
                    }
                    else
                    {
                        if(pRouteOrder->m_bZHJL)
                        {
                            for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
                            {
                                emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->m_vecSonTrainRouteOrder.at(i)->route_id, bAutoTouch ? 0xbb : 0xaa);
                            }
                        }
                        else emit sendRouteOrderMsg(MSGTYPE_SETTRIGGERTYPE, pRouteOrder->route_id, pRouteOrder->m_nAutoTouch ? 0xbb : 0xaa);
                    }
                });
                vecAction.push_back(pActionRouteAutoTG);
            }
        }
        vecAction.push_back(nullptr);
    }

    //股道
    for(int i = 0; i < pCurrStation->DevArray.size(); i++)
    {
        if(pCurrStation->DevArray[i]->getDevType() == Dev_GD)
        {
            CGD* pGD=(CGD*)(pCurrStation->DevArray[i]);
            if(pGD->pCenter.x() < 0 || pGD->pCenter.y() < 0)
            {
                continue;
            }
            if(pGD->m_nGDType == GD_QD)
            {
                QAction* pAction = new QAction(pGD->m_strName);
                pAction->setCheckable(true);
                vecAction.push_back(pAction);
                if(pGD->m_strName == pRouteOrder->m_strTrack)
                {
                   pAction->setChecked(true);
                   pAction->setEnabled(false);
                }
                connect(pAction, &QAction::triggered, [=](){

                    bool bTouch = true;
                    bool bOperate = false;
                    QVector<TrainRouteOrder> vecTempRouteOrder = pCurrStation->getRouteOrderByTrainNum(pRouteOrder->m_strTrainNum, pRouteOrder->m_btBeginOrEndFlg);
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
                });
            }
        }
    }

    return vecAction;
}

QString StationCtrlDispKSK::GetRouteDescrip(TrainRouteOrder* pRouteOrder, bool bAutoTouth)
{
    QString routeDescrip;
    if(!pRouteOrder)
        return "";
    if(pRouteOrder->m_bZHJL == 0)
    {
        routeDescrip = !bAutoTouth ? pRouteOrder->m_strRouteDescrip :
                                    (pRouteOrder->m_strRouteDescrip + QString("[%1]")
                                     .arg(pRouteOrder->m_nAutoTouch ? "√" : "×"));
    }
    else
    {
        for(int i = 0; i < pRouteOrder->m_vecSonTrainRouteOrder.size(); i++)
        {
            routeDescrip += pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_strRouteDescrip;
            if(bAutoTouth)
                routeDescrip += QString("[%1]").arg(pRouteOrder->m_vecSonTrainRouteOrder.at(i)->m_nAutoTouch ? "√" : "×");
            routeDescrip += ",";
        }
        routeDescrip = routeDescrip.left(routeDescrip.length() - 1);
    }
    return routeDescrip;
}

//binvert:是否反选
void StationCtrlDispKSK::SetMenuState(QMenu *menu,QString str,bool b,bool binvert)
{
    for(int i=0;i<menu->actions().size();i++)
    {
        if(!binvert)
        {
            if(menu->actions()[i]->text()==str)
            {
                menu->actions()[i]->setEnabled(b);
            }
            else
            {
                menu->actions()[i]->setEnabled(!b);
            }
        }
        else
        {
            if(menu->actions()[i]->text()!=str)
            {
                menu->actions()[i]->setEnabled(b);
            }
            else
            {
                menu->actions()[i]->setEnabled(!b);
            }
        }
    }
}
//SetMenuStateInFCZK
void StationCtrlDispKSK::SetMenuStateInFCZK(QMenu *menu,QString str,bool b,bool binvert)
{
    for(int i=0;i<menu->actions().size();i++)
    {
        if(str!="")
        {
            if(menu->actions()[i]->text()==str)
            {
                menu->actions()[i]->setEnabled(b);
            }
        }
        else
        {
           menu->actions()[i]->setEnabled(b);
        }
    }
}

//设置车次启动
void StationCtrlDispKSK::SetCHECIFiring(MyStation* pMyStation,Train* pTrain,QString text)
{
    QString qMsg = QString("你确定要设置车次%1启动么？").arg(pTrain->m_strCheCi);
    int ret = QMessageBox::information(this,tr("车次启动与停稳"),qMsg,tr("确定"),tr("取消"),"",0);
    if(ret==0)
    {
        //集中处理
        emit SendTextAnalysisSignal(77, QCursor::pos());
        pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);
    }
    else emit SendTextAnalysisSignal(78, QCursor::pos());
    pTrain->m_bFlash=false;
}
//设置车次停稳
void StationCtrlDispKSK::SetCHECIStop(MyStation* pMyStation,Train* pTrain,QString text)
{
    QString qMsg = QString("你确定要设置车次%1停稳么？").arg(pTrain->m_strCheCi);
     int ret = QMessageBox::information(this,tr("车次启动与停稳"),qMsg,tr("确定"),tr("取消"),"",0);
     if(ret==0)
     {
         //集中处理
         emit SendTextAnalysisSignal(77, QCursor::pos());
         pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);
     }
     else emit SendTextAnalysisSignal(78, QCursor::pos());
     pTrain->m_bFlash=false;
}
//加车次号
void StationCtrlDispKSK::SetCHECIAdd(MyStation* pMyStation,int ncode,QString text,bool btitle)
{
    mAddTrain=new AddTrain_Dlg();
    mAddTrain->Init(pMyStation->getStationName(),pMyStation->GetGDNameByCode(ncode),btitle);
    connect(mAddTrain, &AddTrain_Dlg::SendTextAnalysisSignal,  [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    mAddTrain->exec();
    if(mAddTrain->onOK == true)
    {
        emit SendTextAnalysisSignal(77, QCursor::pos());
        Train* pTrain2=new Train();
        pTrain2->m_bElectric=mAddTrain->GetElectric();
        pTrain2->m_strCheCi=mAddTrain->GetCCH();
        pTrain2->m_nPosCode=ncode;
        pTrain2->m_bManType=mAddTrain->GetSimTrain();
        pTrain2->m_nSpeed=160;
        if(pTrain2->m_strCheCi != "")
        {
            pMyStation->OnMenuCheCiClicked(text, pTrain2->m_nPosCode, pTrain2);
        }
        delete pTrain2;
    }
    else emit SendTextAnalysisSignal(78, QCursor::pos());
}
//删车次号
void StationCtrlDispKSK::SetCHECIDel(MyStation* pMyStation,Train* pTrain,QString text)
{
    mDelCheCi=new DelCheCi();
    mDelCheCi->Init(pTrain->m_strCheCi);
    mDelCheCi->show();
    connect(mDelCheCi,&DelCheCi::ReturnResultSignal,[=](){
        if(mDelCheCi->bok)
        {
            pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);
            emit SendTextAnalysisSignal(77, QCursor::pos());
        }
        else emit SendTextAnalysisSignal(78, QCursor::pos());
        pTrain->m_bFlash=false;
        delete mDelCheCi;
    });
}
//车次号确认
void StationCtrlDispKSK::SetCHECIConfrim(MyStation* pMyStation,Train* pTrain,QString text)
{
    mConfirmCCH=new ConfirmCCH_Dlg();
    connect(mConfirmCCH, &ConfirmCCH_Dlg::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    mConfirmCCH->Init(text,"车次号操作",pTrain->m_strCheCi);
    mConfirmCCH->exec();
    if(mConfirmCCH->onOK == true)
    {
//        if(mConfirmCCH->bok)
//        {
//            emit SendTextAnalysisSignal(77, QCursor::pos());
//            pTrain->m_strCheCiOld=pTrain->m_strCheCi;
//            pTrain->m_strCheCi=mConfirmCCH->GetNewCCH();
//            pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);
//        }
//        else emit SendTextAnalysisSignal(78, QCursor::pos());
//        pTrain->m_bFlash=false;
//        delete mConfirmCCH;
        emit SendTextAnalysisSignal(77, QCursor::pos());
        pTrain->m_strCheCiOld=pTrain->m_strCheCi;
        pTrain->m_strCheCi=mConfirmCCH->GetNewCCH();
        pTrain->m_bElectric=false;
        pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);
        pTrain->m_bFlash=false;
        delete mConfirmCCH;
    }
    else
    {
        emit SendTextAnalysisSignal(78, QCursor::pos());
        pTrain->m_bFlash=false;
        delete mConfirmCCH;
    }
}
//改车次属性
void StationCtrlDispKSK::SetCHECIChange(MyStation* pMyStation,Train* pTrain,QString text)
{
    mChangeCCProperty=new ChangeCCProperty_Dlg();
    connect(mChangeCCProperty, &ChangeCCProperty_Dlg::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    mChangeCCProperty->SetCCH(pTrain->m_strCheCi,pTrain->m_bElectric,pTrain->m_strLocomotiveNumber);
    mChangeCCProperty->exec();
    if(mChangeCCProperty->onOK == true)
    {
//        if(mChangeCCProperty->bok)
//        {
//            emit SendTextAnalysisSignal(77, QCursor::pos());
//            pTrain->m_nSpeed=mChangeCCProperty->GetTrainSpeed().toInt();
//            pTrain->m_bElectric=mChangeCCProperty->GetElectric();
//            //TODO:pTrain中缺少机车号
//            pTrain->m_strLocomotiveNumber=mChangeCCProperty->GetTrainNum();
//            pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);
//        }
//        else emit SendTextAnalysisSignal(78, QCursor::pos());
//        pTrain->m_bFlash=false;

        emit SendTextAnalysisSignal(77, QCursor::pos());
        pTrain->m_nSpeed=mChangeCCProperty->GetTrainSpeed().toInt();
        pTrain->m_bElectric=mChangeCCProperty->GetElectric();
        //TODO:pTrain中缺少机车号
        pTrain->m_strLocomotiveNumber=mChangeCCProperty->GetTrainNum();
        pMyStation->OnMenuCheCiClicked(text, pTrain->m_nPosCode, pTrain);

        pTrain->m_bFlash=false;
        delete mChangeCCProperty;
    }
    else
    {
        emit SendTextAnalysisSignal(78, QCursor::pos());
        pTrain->m_bFlash=false;
        delete mChangeCCProperty;
    }
}

int StationCtrlDispKSK::GetFunButtonType(int nDev,int nDevCode,int nDevCode2,QString str,bool isfs,bool ismd)
{
    int ntype=0;
    QString strdevname="";
    QString strSX="";
    if(nDev==Dev_DC)
    {
        if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)//总定位
        {
            ntype=0x06;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)//总反位
        {
            ntype=0x07;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)//单锁
        {
            ntype=0x08;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)//单解
        {
            ntype=0x09;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)//封锁
        {
           ntype=0x10;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)//解封
        {
           ntype=0x11;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)//区故解
        {
            ntype=0x25;
            strSX=pCurrStation->GetDevSX(nDev,nDevCode,true);
            strdevname=strSX+"区故解";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == CQBAD_SHUNT)
        {
            ntype=0x30;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == DWBAD_SHUNT)
        {
            ntype=0x31;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == FWBAD_SHUNT)
        {
            ntype=0x32;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
        {
            ntype=0x43;
            strdevname="确认空闲";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_JG)
        {
            ntype=0x47;
            strSX=pCurrStation->GetANNameByCode(Dev_DCBTN,nDevCode);

            strdevname=strSX+"尖轨";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_XG)
        {
            ntype=0x48;
            strSX=pCurrStation->GetANNameByCode(Dev_DCBTN,nDevCode);
            strdevname=strSX+"心轨";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCDW)
        {
            if(str.contains("有电"))
            {
                ntype=0x56;
            }
            else
            {
                ntype=0x55;
            }
            mFunButtonDevInfo.strButtonName = "";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCFW)
        {
            if(str.contains("有电"))
            {
                ntype=0x58;
            }
            else
            {
                ntype=0x57;
            }
            mFunButtonDevInfo.strButtonName = "";
        }
        else if(str.contains("腰岔解锁"))
        {
            ntype=0x5B;
            strdevname = "腰岔解锁";
        }
    }
    else if(nDev==Dev_XH)
    {
        if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
        {
            if(((mFunButtonDevInfo.nDevCode[0]&0xC000)>>14)==0x02)//如果是通过进路(第一个按钮为通过按钮)
            {
                ntype=0x03;
            }
            else if(((mFunButtonDevInfo.nDevCode[0]&0xC000)>>14)==0x01)//调车进路
            {
                ntype=0x02;
            }
            else if(((mFunButtonDevInfo.nDevCode[0]&0xC000)>>14)==0x00)//列车进路
            {
                ntype=0x01;
            }
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
        {
            ntype=0x04;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
        {
            CXHD* pXHD = pCurrStation->GetXHDByCode(nDevCode);
            if(nXHType == 1)//列车
            {
                nXHType = 0;
                if(pXHD->m_nXHDStateLast == XHD_BD)
                    return -1;
            }
            if(nXHType == 2)//调车
            {
                nXHType = 0;
                if(pXHD->m_nXHDStateLast != XHD_BD)
                    return -1;
            }
            ntype=0x05;
            nXHType = 0;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
        {
            ntype=0x10;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
        {
            ntype=0x11;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
        {
            ntype=0x14;
            strSX=pCurrStation->GetDevSX(nDev,nDevCode);
            strdevname=strSX+"总人解";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
        {
            //ntype=0x12;
            ntype=GetYDState(nDevCode);
            strSX=pCurrStation->GetStrNameByCode(nDevCode);
            strdevname=strSX+"引导";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState==LIGHT_ON)
        {
           ntype=0x39;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState==LIGHT_OFF)
        {
            ntype=0x40;
            strSX=pCurrStation->GetDevSX(nDev,nDevCode);
            strdevname=strSX+"灭灯";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState==RAMP_RELEASE)
        {
            ntype=0x15;
//            strSX=pCurrStation->GetDevSX(nDev,nDevCode);
//            strdevname=strSX+"坡道解锁";
            strSX="";
            strdevname=strSX+"坡道解锁";
        }
    }
    else if(nDev==Dev_GD)
    {
        if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
        {
            ntype=0x10;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
        {
            ntype=0x11;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
        {
            ntype=0x25;
            strSX=pCurrStation->GetDevSX(nDev,nDevCode);
            strdevname=strSX+"区故解";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
        {
            ntype=0x16;
//            strSX=pCurrStation->GetDevSX(nDev,nDevCode);
//            strdevname=strSX+"分路不良";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
        {
            ntype=0x43;
            strdevname="确认空闲";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_GD)
        {
            if(str.contains("有电"))
            {
                ntype=0x46;
            }
            else
            {
                ntype=0x45;
            }
            mFunButtonDevInfo.strButtonName = "";
        }
        else if(str.contains("添加调机号"))
        {
            //ntype=0x07;
        }
        else if(str.contains("删除调机号"))
        {
           // ntype=0x07;
        }
        else if(str.contains("修改调机号"))
        {
           // ntype=0x07;
        }
        else if(str.contains("股道确认"))
        {
            ntype=0x5a;
            strdevname = "股道确认";
        }
    }
    else if(nDev==Dev_ZDBS)//自动闭塞
    {
        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
        {
            if(str.contains("总辅助"))
            {
                ntype=0x18;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"总辅助";
            }
            else if(str.contains("发车辅助"))
            {
                ntype=0x19;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"发车辅助";
            }
            else if(str.contains("接车辅助"))
            {
                ntype=0x20;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"接车辅助";
            }
            else if(str.contains("改方"))
            {
                ntype=0x21;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"允许改方";
            }
        }
    }
    else if(nDev==Dev_BZDBS)//半自动闭塞
    {
        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
        {
            if(str.contains("闭塞"))
            {
                ntype=0x36;
            }
            else if(str.contains("事故"))
            {
                ntype=0x37;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"事故";
            }
            else if(str.contains("复原"))
            {
                ntype=0x38;
            }
            //闭塞切换
            else if(str.contains("切换"))
            {
                //ntype=0x21;
            }
        }
    }
    else if(nDev==Dev_JZ)//计轴
    {
        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
        {
            if(str.contains("复零"))
            {
                ntype=0x50;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"计轴复零";
            }
            else if(str.contains("使用"))
            {
                ntype=0x51;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"计轴使用";
            }
            else if(str.contains("停用"))
            {
                ntype=0x52;
                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
                strdevname=strSX+"计轴停用";
            }
            //闭塞切换
            else if(str.contains("切换"))
            {
                //ntype=0x21;
            }
        }
    }
    else if(nDev==Dev_FUNCB)//功能按钮
    {
        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
        {
            if(str.contains("语音暂停"))
            {
               // ntype=0x36;
            }
            else if(str.contains("上电解锁"))
            {
                ntype=0x23;
                strdevname="上电解锁";
            }
            else if(str.contains("故障通知"))
            {
                if(pCurrStation->m_bGZTZ == true)
                {
                    pCurrStation->m_bGZTZ = false;
                }
                else
                {
                    pCurrStation->m_bGZTZ = true;
                }
                strdevname="故障通知";
                pCurrStation->SetQFTJAdd(strdevname);
                if(mQFTJ!=nullptr)
                {
                  mQFTJ->InitShow(pCurrStation->getStationName(),pCurrStation->mQianFengVect);
                }
            }
        }
        if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
        {
            ntype=0x13;
            //strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
            strSX=pCurrStation->GetDevSX(Dev_XH,nDevCode);

            strdevname=strSX+"引导总锁";
        }
    }
    else if(nDev==Dev_DCBTN)//功能按钮
    {
//        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
//        {
//            if(str.contains("尖轨故障"))
//            {
//                ntype=0x47;
//                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
//                strdevname=strSX+"尖轨";
//            }
//            else if(str.contains("心轨故障"))
//            {
//                ntype=0x48;
//                strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
//                strdevname=strSX+"心轨";
//            }
//        }
        if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_JG)
        {
            ntype=0x47;
            strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
            strdevname=strSX+"尖轨";
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_XG)
        {
            ntype=0x48;
            strSX=pCurrStation->GetANNameByCode(nDev,nDevCode);
            strdevname=strSX+"心轨";
        }
    }

    if(ntype!=0)
    {
        //当道岔分路不良时，对道岔进行定操 反操操作时需要弹出操作报警窗口

       if(ntype==0x06||ntype==0x07)
       {
           if(!OperationAlarmShow(nDevCode,str))
           {
               return ntype;
           }
       }

        emit SendDataToSecerSignal(0,ntype,nDevCode,nDevCode2);
        //qDebug()<<"*-*-*-*-------------" <<strdevname;
        for(int j=0;j<pCurrStation->FuncBtnArray.size();j++)
        {
            if(pCurrStation->FuncBtnArray[j]->getDevType() == Dev_FUNCB)
            {
                FUNCBUTTON *pfUNCBUTTON=nullptr;
                 pfUNCBUTTON=(FUNCBUTTON*)(pCurrStation->FuncBtnArray[j]);
                 if(pfUNCBUTTON->Button_Name==strdevname&&!pfUNCBUTTON->haveAN)
                 {
                    pfUNCBUTTON->Button_QF++;
                 }
            }
        }
        pCurrStation->SetQFTJAdd(strdevname);
        if(mQFTJ!=nullptr)
        {
          mQFTJ->InitShow(pCurrStation->getStationName(),pCurrStation->mQianFengVect);
        }
    }

    return ntype;
}
int StationCtrlDispKSK::GetFunButtonType(FunButtonDevInfo *mFunButtonDevInfo,QString strroutenum,bool isfs,bool ismd)
{
    int ntype=0;
    int nDev=mFunButtonDevInfo->nDevType;
    QString str=mFunButtonDevInfo->strButtonName;
    if(nDev==Dev_DC)
    {
        if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)//总定位
        {
            ntype=0x06;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)//总反位
        {
            ntype=0x07;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)//单锁
        {
            ntype=0x08;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)//单解
        {
            ntype=0x09;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)//封锁
        {
           ntype=0x10;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)//解封
        {
           ntype=0x11;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)//区故解
        {
            ntype=0x25;
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
        {
            if(pStationFunButtonWidget->nFunButtonPressState == CQBAD_SHUNT)
            {
                ntype=0x30;
            }
            else if(pStationFunButtonWidget->nFunButtonPressState == DWBAD_SHUNT)
            {
                ntype=0x31;
            }
            else if(pStationFunButtonWidget->nFunButtonPressState == FWBAD_SHUNT)
            {
                ntype=0x32;
            }
            else
            {}
        }
        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
        {
            ntype=0x43;
        }
        else if(str.contains("尖轨故障"))
        {
            ntype=0x47;
        }
        else if(str.contains("心轨故障"))
        {
            ntype=0x48;
        }
        else if(str.contains("接触网定位无电"))
        {
            if(str.contains("有电"))
                ntype=0x56;
            else
                ntype=0x55;
        }
        else if(str.contains("接触网反位无电"))
        {
            if(str.contains("有电"))
                ntype=0x58;
            else
                ntype=0x57;
        }
        else if(str.contains("腰岔解锁"))
        {
            ntype=0x5B;
        }
    }
    else if(nDev==Dev_XH)
    {
        if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
        {
            if(((mFunButtonDevInfo->nDevCode[0]&0xC000)>>14)==0x02)//如果是通过进路(第一个按钮为通过按钮)
            {
                ntype=0x03;
            }
            else if(((mFunButtonDevInfo->nDevCode[0]&0xC000)>>14)==0x01)//调车进路
            {
                ntype=0x02;
            }
            else if(((mFunButtonDevInfo->nDevCode[0]&0xC000)>>14)==0x00)//列车进路
            {
                ntype=0x01;
            }
        }
        else if(str.contains("取消进路")||str.contains("总取消"))
        {
            ntype=0x04;
        }
        else if(str.contains("信号重开"))
        {
            ntype=0x05;
        }
        else if(str.contains("封锁")||str.contains("解封"))
        {
            if((str=="封锁")||(str=="封锁列车")||(str=="封锁调车")||(str=="封锁通过"))
            {
                ntype=0x10;
            }
            else if((str=="解封")||(str=="解封列车")||(str=="解封调车")||(str=="解封通过"))
            {
                ntype=0x11;
            }
            else
            {
                if(isfs)
                    ntype=0x11;
                else
                    ntype=0x10;
            }
        }
        else if(str.contains("总人解"))
        {
            ntype=0x14;
        }
        else if(str.contains("引导"))
        {
            ntype=0x12;
        }
        else if(str.contains("点灯")||str.contains("灭灯"))
        {
            if(str=="点灯")
            {
               ntype=0x39;
            }
            else if(str=="灭灯")
            {
               ntype=0x40;
            }
            else
            {
                if(ismd)
                    ntype=0x39;
                else
                    ntype=0x40;
            }
        }
        else if(str.contains("坡道解锁"))
        {
            ntype=0x15;
        }
    }
    else if(nDev==Dev_GD)
    {
        if(str.contains("封锁")||str.contains("解封"))
        {
            if(str=="封锁")
            {
                ntype=0x10;
            }
            else if(str=="解封")
            {
                ntype=0x11;
            }
            else
            {
                if(isfs)
                    ntype=0x11;
                else
                    ntype=0x10;
            }
        }
        else if(str.contains("区故解"))
        {
            ntype=0x25;
        }
        else if(str.contains("分路不良"))
        {
            ntype=0x16;
        }
        else if(str.contains("确认空闲"))
        {
            ntype=0x43;
        }
        else if(str.contains("股道无电"))
        {
            if(str.contains("有电"))
                ntype=0x46;
            else
                ntype=0x45;
        }
        else if(str.contains("添加调机号"))
        {
            //ntype=0x07;
        }
        else if(str.contains("删除调机号"))
        {
           // ntype=0x07;
        }
        else if(str.contains("修改调机号"))
        {
           // ntype=0x07;
        }
    }
    else if(nDev==Dev_ZDBS)//自动闭塞
    {
        if(str.contains("总辅助"))
        {
            ntype=0x18;
        }
        else if(str.contains("发车辅助"))
        {
            ntype=0x19;
        }
        else if(str.contains("接车辅助"))
        {
            ntype=0x20;
        }
        else if(str.contains("改方"))
        {
            ntype=0x21;
        }
    }
    else if(nDev==Dev_BZDBS)//半自动闭塞
    {
        if(str.contains("闭塞"))
        {
            ntype=0x36;
        }
        else if(str.contains("事故"))
        {
            ntype=0x37;
        }
        else if(str.contains("复原"))
        {
            ntype=0x38;
        }
        //闭塞切换
        else if(str.contains("切换"))
        {
            //ntype=0x21;
        }
    }
    else if(nDev==Dev_FUNCB)//功能按钮
    {
        if(str.contains("语音暂停"))
        {
           // ntype=0x36;
        }
        else if(str.contains("上电解锁"))
        {
            ntype=0x23;
        }
        else if(str.contains("引导总锁"))
        {
            ntype=0x13;
        }
    }

    if(ntype!=0)
    {
        for (int i = 0; i < 8; i++)
        {
            //qDebug()<<"*-*-*-*-" << mFunButtonDevInfo->nDevCode[i] <<strroutenum;
        }
        emit SendDataRouteToSecerSignal(0,ntype,mFunButtonDevInfo->nDevCode,strroutenum);

    }
    return ntype;
}

void StationCtrlDispKSK::GetDevInfoByMousePoint(FunButtonDevInfo *mFunButtonDevInfo)
{
    if(pCurrStation->m_nModeState == 0 && pStationFunButtonWidget->nFunButtonPressState != BAD_SHUNT_KX)
    {
        return;
    }

    CGDDC *pCGDDC=nullptr;
    CXHD *pXHD=nullptr;
    CGD *pGD=nullptr;
    CZDBS *pZDBS=nullptr;
    CBZDBS *pBZDBS=nullptr;
    CJZ *JZ=nullptr;
    FUNCBUTTON *pfUNCBUTTON=nullptr;
    CDcBtn *DCBUTTON=nullptr;
    int ncode=-1;
    QString strName="";
    AlonXHBtn *AlonXhBtn;
    //站场设备

    if(pCurrStation)
    {
        emit SendTextAnalysisSignal(81, QCursor::pos());
        for(int j=0;j<pCurrStation->DevArray.size();j++)
        {
            if(pCurrStation->DevArray[j]->getDevType() == Dev_XH)
            {
                pXHD=(CXHD*)(pCurrStation->DevArray[j]);

                if((pXHD->GetMousePoint_LCAN(mcoursePoint))&&(pCurrStation->m_nModeState == 2))
                {
                    return;
                }
                //if (pXHD->GetMousePoint(mcoursePoint))
                //    emit SendTextAnalysisSignal(179, QCursor::pos(), pXHD->m_strName);
                //进路建立
                if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                {
                    if ((pXHD->GetMousePoint_DCAN(mcoursePoint))
                        ||(pXHD->GetMousePoint_LCAN(mcoursePoint))
                        ||(pXHD->GetMousePoint_TGAN(mcoursePoint))
                       )
                    {
                        if((pXHD->getSignalType()==false) &&//非虚信号
                        (((pXHD->getXHDState()!=XHD_AD&&pXHD->getXHDState()!=XHD_HD&&pXHD->getXHDState()!=XHD_DS)&&(pXHD->getXHDType()==JZ_XHJ||pXHD->getXHDType()==CZ_XHJ))
                        ||((pXHD->getXHDState()!=XHD_AD&&pXHD->getXHDState()!=XHD_HD&&pXHD->getXHDState()!=XHD_DS)&&pXHD->getXHDType()==DC_XHJ)))
                        {
                            return;
                        }
                        if(pXHD->getIsFB() == true)
                        {
                            return;
                        }
                        QString name = pXHD->m_strName;
                        ncode=pXHD->getCode();
                        //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                         emit SendDataToSecerSignal(2);
                        //鼠标左键按下设备按钮后，向逻辑发送按下信息，用于终端闪烁
                        //0x01列车按钮  0x02调车按钮 0x03通过按钮 0x04变通按钮
                        int nFunType=0x00;
                        if(pXHD->GetMousePoint_LCAN(mcoursePoint))
                        {
                            if(pXHD->getSignalType())
                            {
                                //nFunType=0x02;

                                if(mFunButtonDevInfo->nDevCode[0]==-1)
                                {
                                    return;//如果第一个点击的是虚信号,则退出
                                }

                            }
                            else
                            {
                                if(mFunButtonDevInfo->nDevCode[0]!=-1)
                                {
                                    if(pXHD->isLCANFlash == false)
                                    {
                                        return;
                                    }
                                }

                            }
                            nFunType=0x01;
                            emit SendTextAnalysisSignal(61, QCursor::pos(), pXHD->m_strName);
                            pXHD->setIsLCDown(1);
                            pCurrStation->ANFlashFlag = 0;
                            pCurrStation->ButtonFlashDown = true;
                        }
                        else if(pXHD->GetMousePoint_DCAN(mcoursePoint))
                        {
                            if(mFunButtonDevInfo->nDevCode[0]!=-1)
                            {
                                if(pXHD->isDCANFlash == false)
                                {
                                    return;
                                }
                            }
                            nFunType=0x02;
                            emit SendTextAnalysisSignal(62, QCursor::pos(), pXHD->m_strName);
                            pXHD->setIsDCDown(1);
                            pCurrStation->ANFlashFlag = 0;
                            pCurrStation->ButtonFlashDown = true;
                            mFunButtonDevInfo->strButtonName=mFunButtonDevInfo->strButtonName+"调车";
                        }
                        else if(pXHD->GetMousePoint_TGAN(mcoursePoint))
                        {
                            if(mFunButtonDevInfo->nDevCode[0]!=-1)
                            {
                                return;//通过按钮只能第一个被按下
                            }
                            nFunType=0x03;
                            emit SendTextAnalysisSignal(63, QCursor::pos(), pXHD->m_strName);
                            pXHD->m_nTgIsDown = 1;
                            pCurrStation->ANFlashFlag = 0;
                            pCurrStation->ButtonFlashDown = true;
                              mFunButtonDevInfo->strButtonName=mFunButtonDevInfo->strButtonName+"通过";
                        }
                        //只有在进路建立/办理情况下发送
                        emit SendDataToSecerSignal(1,nFunType,pXHD->getCode());
                        if(mFunButtonDevInfo->nDevCode[mFunButtonDevInfo->RecordDevNum] == -1)
                        {
                            if(nFunType==3)//该部分功能为通过按钮按下时,是需要赋值通过按钮自己的code,而不是设备的code
                            {
                                mFunButtonDevInfo->SetCode(SetRouteCode(pXHD->nTxtTGButtonCode,nFunType),mFunButtonDevInfo->RecordDevNum);
                            }
                            else
                            {
                                mFunButtonDevInfo->SetCode(SetRouteCode(pXHD->getCode(),nFunType),mFunButtonDevInfo->RecordDevNum);
                            }
                        }

                        mFunButtonDevInfo->nDevType=Dev_XH;
                        mFunButtonDevInfo->RecordDevNum++;//按钮按下个数
                        if(mFunButtonDevInfo->RecordDevNum>0)
                        {
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                        }
                        if(mFunButtonDevInfo->RecordDevNum>1)
                        {
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                        }
                        return;
                    }
                }
                //总取消/信号重开/总人解
                else if((pStationFunButtonWidget->nFunButtonPressState==ALL_CANCEL)//总取消
                        ||(pStationFunButtonWidget->nFunButtonPressState==SIGNAL_RESTART)//信号重开
                        ||(pStationFunButtonWidget->nFunButtonPressState==ALLMAN_RELEASE)//总人解
                        )
                {
                    if(mFunButtonDevInfo->nCode!=-1)
                    {
                        return;
                    }
                    if ((pXHD->GetMousePoint_DCAN(mcoursePoint))||(pXHD->GetMousePoint_LCAN(mcoursePoint)))
                    {
                        if(mFunButtonDevInfo->nCode==-1)
                        {
                            QString name = pXHD->m_strName;
                            ncode=pXHD->getCode();
                            //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                             emit SendDataToSecerSignal(2);
                            //0x01列车按钮  0x02调车按钮 0x03通过按钮 0x04变通按钮
                            int nFunType=0x00;
                            if(pXHD->GetMousePoint_LCAN(mcoursePoint))
                            {

                                if(pXHD->getSignalType())
                                {
                                    nFunType=0x02;
                                    if(mFunButtonDevInfo->nCode==-1)
                                    {
                                        return;//如果第一个点击的是虚信号,则退出
                                    }
                                    pXHD->setIsDCDown(1);
                                    emit SendTextAnalysisSignal(64, QCursor::pos(), pXHD->m_strName);
                                }
                                else
                                {
                                    nFunType=0x01;
                                    pXHD->setIsLCDown(1);
                                    emit SendTextAnalysisSignal(61, QCursor::pos(), pXHD->m_strName);
                                }
                                pCurrStation->ANFlashFlag = 0;
                                pCurrStation->ButtonFlashDown = true;
                            }
                            else if(pXHD->GetMousePoint_DCAN(mcoursePoint))
                            {
                                nFunType=0x02;
                                emit SendTextAnalysisSignal(62, QCursor::pos(), pXHD->m_strName);
                                pXHD->setIsDCDown(1);
                                pCurrStation->ANFlashFlag = 0;
                                pCurrStation->ButtonFlashDown = true;
                            }

                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=ncode;
                            }

                            mFunButtonDevInfo->nDevType=Dev_XH;
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);

                            return;
                        }
                    }
                }
                //引导按钮
                else if(pStationFunButtonWidget->nFunButtonPressState==CALLON_BTN)
                {
                    if(mFunButtonDevInfo->nCode!=-1)
                    {
                        return;
                    }
                    if (pXHD->GetMousePoint_YDAN(mcoursePoint))
                    {
                        if(mFunButtonDevInfo->nCode==-1)
                        {
                            emit SendTextAnalysisSignal(180, QCursor::pos(), pXHD->m_strName);
                            QString name = pXHD->m_strName;
                            ncode=pXHD->getCode();
                            //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                            emit SendDataToSecerSignal(2);

                            if(pXHD->GetMousePoint_YDAN(mcoursePoint))
                            {
                                if((pXHD->getTimeCount()>0) && pXHD->getXHDState(XHD_YD))
                                {
                                    pXHD->setIsYDDown(1);
                                    pCurrStation->ANFlashFlag = 0;
                                    pCurrStation->ButtonFlashDown = true;
                                }
                                else
                                {
                                    if(pXHD->getIsFB()==false)
                                    {
                                        mPassWordEntryDlg_NEW->strDescrib = name + "引导进路,请输入密码";
                                        mPassWordEntryDlg_NEW->InitPassWord();
                                        mPassWordEntryDlg_NEW->exec();
                                        if(mPassWordEntryDlg_NEW->onOK == true)
                                        {
                                            pXHD->setIsYDDown(1);
                                            pCurrStation->ANFlashFlag = 0;
                                            pCurrStation->ButtonFlashDown = true;
                                        }
                                        else
                                        {
                                            emit SendDataToSecerSignal(4);
                                            OperateOrderClean();
                                        }
                                    }
                                }
                            }

                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=ncode;
                            }

                            mFunButtonDevInfo->nDevType=Dev_XH;
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            return;
                        }
                    }
                }
                //封锁/解封
                else if((pStationFunButtonWidget->nFunButtonPressState==LOCK_FUNCTION)//封锁
                        ||(pStationFunButtonWidget->nFunButtonPressState==UNLOCK_FUNCTION)//解封
                        )
                {
                    if(mFunButtonDevInfo->nCode!=-1)
                    {
                        return;
                    }
                    if ((pXHD->GetMousePoint_DCAN(mcoursePoint))
                        ||(pXHD->GetMousePoint_LCAN(mcoursePoint))
                        ||(pXHD->GetMousePoint_TGAN(mcoursePoint))
                        )
                    {
                        if(mFunButtonDevInfo->nCode==-1)
                        {
                            QString name = pXHD->m_strName;
                            ncode=pXHD->getCode();
                            //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                             emit SendDataToSecerSignal(2);
                            //0x01列车按钮  0x02调车按钮 0x03通过按钮 0x04变通按钮
                            int nFunType=0x00;
                            if(pXHD->GetMousePoint_LCAN(mcoursePoint))
                            {
                                emit SendTextAnalysisSignal(61, QCursor::pos(), pXHD->m_strName);
                                if(pXHD->getSignalType())
                                {
                                    nFunType=0x02;
                                    pXHD->setIsDCDown(1);
                                }
                                else
                                {
                                    nFunType=0x01;
                                    pXHD->setIsLCDown(1);
                                }
                                pCurrStation->ANFlashFlag = 0;
                                pCurrStation->ButtonFlashDown = true;
                            }
                            else if(pXHD->GetMousePoint_DCAN(mcoursePoint))
                            {
                                nFunType=0x02;
                                emit SendTextAnalysisSignal(62, QCursor::pos(), pXHD->m_strName);
                                pXHD->setIsDCDown(1);
                                pCurrStation->ANFlashFlag = 0;
                                pCurrStation->ButtonFlashDown = true;
                            }
                            else if(pXHD->GetMousePoint_TGAN(mcoursePoint))
                            {
                                nFunType=0x03;
                                emit SendTextAnalysisSignal(63, QCursor::pos(), pXHD->m_strName);
                                pXHD->m_nTgIsDown = 1;
                                pCurrStation->ANFlashFlag = 0;
                                pCurrStation->ButtonFlashDown = true;
                            }

                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=ncode;
                            }

                            mFunButtonDevInfo->nDevType=Dev_XH;
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);

                            return;
                        }
                    }
                }

                //坡道解锁
                else if(pStationFunButtonWidget->nFunButtonPressState==RAMP_RELEASE)
                {
                    if(mFunButtonDevInfo->nCode!=-1)
                    {
                        return;
                    }
                    //if (pXHD->GetMousePoint(mcoursePoint))
                    if(pXHD->m_LCXHDButtonShowFlag)
                    {
                        if (pXHD->GetMousePoint_LCAN(mcoursePoint))
                        {
                            if(pXHD->m_PDJSFalg)
                            {

                                if(mFunButtonDevInfo->nCode==-1)
                                {
                                    ncode=pXHD->getCode();
                                    //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                                     emit SendDataToSecerSignal(2);
                                    //0x01列车按钮  0x02调车按钮 0x03通过按钮 0x04变通按钮
                                    int nFunType=0x00;

                                    if(pXHD->GetMousePoint_LCAN(mcoursePoint))
                                    {
                                        if(pXHD->getSignalType())
                                        {
                                            nFunType=0x02;
                                            emit SendTextAnalysisSignal(64, QCursor::pos(), pXHD->m_strName);
                                            if(mFunButtonDevInfo->nCode==-1)
                                            {
                                                return;//如果第一个点击的是虚信号,则退出
                                            }
                                        }
                                        else
                                        {
                                            nFunType=0x01;
                                            emit SendTextAnalysisSignal(61, QCursor::pos(), pXHD->m_strName);
                                        }
                                        if(pXHD->getSignalType())
                                        {
                                            pXHD->setIsDCDown(1);
                                        }
                                        else
                                        {
                                            pXHD->setIsLCDown(1);
                                        }
                                        pCurrStation->ANFlashFlag = 0;
                                        pCurrStation->ButtonFlashDown = true;
                                    }

                                    if(mFunButtonDevInfo->nCode==-1)
                                    {
                                        mFunButtonDevInfo->nCode=ncode;
                                    }

                                    mFunButtonDevInfo->nDevType=Dev_XH;
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                    return;
                                }
                            }
                        }
                    }
                    else if(pXHD->m_DCXHDButtonShowFlag)
                    {
                        if (pXHD->GetMousePoint_DCAN(mcoursePoint))
                        {
                            if(pXHD->m_PDJSFalg)
                            {
                                if(mFunButtonDevInfo->nCode==-1)
                                {
                                    ncode=pXHD->getCode();
                                    //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                                     emit SendDataToSecerSignal(2);
                                    //0x01列车按钮  0x02调车按钮 0x03通过按钮 0x04变通按钮
                                    int nFunType=0x00;

                                    if(pXHD->GetMousePoint_DCAN(mcoursePoint))
                                    {
                                        if(pXHD->getSignalType())
                                        {
                                            nFunType=0x02;
                                            emit SendTextAnalysisSignal(64, QCursor::pos(), pXHD->m_strName);
                                            if(mFunButtonDevInfo->nCode==-1)
                                            {
                                                return;//如果第一个点击的是虚信号,则退出
                                            }
                                        }
                                        else
                                        {
                                            nFunType=0x01;
                                            emit SendTextAnalysisSignal(61, QCursor::pos(), pXHD->m_strName);
                                        }
                                        if(pXHD->getSignalType())
                                        {
                                            pXHD->setIsDCDown(1);
                                        }
                                        else
                                        {
                                            pXHD->setIsDCDown(1);
                                        }
                                        pCurrStation->ANFlashFlag = 0;
                                        pCurrStation->ButtonFlashDown = true;
                                    }

                                    if(mFunButtonDevInfo->nCode==-1)
                                    {
                                        mFunButtonDevInfo->nCode=ncode;
                                    }

                                    mFunButtonDevInfo->nDevType=Dev_XH;
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                    return;
                                }
                            }
                        }
                    }
                }
                //点灯/灭灯
                else if((pStationFunButtonWidget->nFunButtonPressState==LIGHT_ON)||(pStationFunButtonWidget->nFunButtonPressState==LIGHT_OFF))
                {
                    if(mFunButtonDevInfo->nCode!=-1)
                    {
                        return;
                    }
                    if(pXHD->getIsFB() == true)
                    {
                        return;
                    }
                    //上下行点灯 S：1 X:0
                    if((pXHD->getSX()==1&&!pCurrStation->nDDMDStateS)||(pXHD->getSX()==0&&!pCurrStation->nDDMDStateX))
                    {
                         return;
                    }
                    if ((pXHD->GetMousePoint_XHD(mcoursePoint))||(pXHD->GetMousePoint_Name(mcoursePoint)))
                    {
                        if(mFunButtonDevInfo->nCode==-1)
                        {
                            ncode=pXHD->getCode();
                            emit SendTextAnalysisSignal(179, QCursor::pos(), pXHD->m_strName);
                            //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                            emit SendDataToSecerSignal(2);
                            //只有在进路建立/办理情况下发送
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=ncode;
                            }
                            mFunButtonDevInfo->nDevType=Dev_XH;
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            return;
                        }
                    }
                }
            }
            if(pStationFunButtonWidget->nFunButtonPressState != JL_BUILD
                && pStationFunButtonWidget->nFunButtonPressState != ALL_CANCEL
                && pStationFunButtonWidget->nFunButtonPressState != ALLMAN_RELEASE
                && pStationFunButtonWidget->nFunButtonPressState != SIGNAL_RESTART
                && pStationFunButtonWidget->nFunButtonPressState != CALLON_BTN
                && pStationFunButtonWidget->nFunButtonPressState != LIGHT_ON
                && pStationFunButtonWidget->nFunButtonPressState != LIGHT_OFF)
            {
                if(pCurrStation->DevArray[j]->getDevType() == Dev_DC)
                {
                    pCGDDC=(CGDDC*)(pCurrStation->DevArray[j]);

                    if (pCGDDC->GetMousePoint(mcoursePoint))
                    {
                        emit SendTextAnalysisSignal(178, QCursor::pos(), pCGDDC->m_strName);
                        mCBaseDataOld=pCurrStation->DevArray[j];
                        if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getQDCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                if(pCGDDC->getIsDS())
                                {
                                    mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                    mFunButtonDevInfo->nDevType=Dev_DC;
                                    //鼠标左键按下 向逻辑发送设备按下信息
                                    emit SendDataToSecerSignal(2);
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                }
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                //                            mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                //                            mPassWordEntryDlg_NEW->InitPassWord();
                                //                            mPassWordEntryDlg_NEW->exec();
                                //                            if(mPassWordEntryDlg_NEW->onOK == true)
                                {
                                    mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                    mFunButtonDevInfo->nDevType=Dev_DC;
                                    //鼠标左键按下 向逻辑发送设备按下信息
                                    emit SendDataToSecerSignal(2);
                                    mPoorShuntDlg->InitShow(Dev_DC,pCGDDC->getName(),pCGDDC->flblStatusCQ,pCGDDC->flblStatusDW,pCGDDC->flblStatusFW);
                                    mPoorShuntDlg->exec();
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                }
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                if((pCGDDC->flblStatusCQ)||(pCGDDC->flblStatusDW)||(pCGDDC->flblStatusFW))
                                {
                                    bzrjrightmenushow = true;
                                    mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                    mFunButtonDevInfo->nDevType=Dev_DC;
                                    //鼠标左键按下 向逻辑发送设备按下信息
                                    emit SendDataToSecerSignal(2);
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                }
                                else
                                {
                                    bzrjrightmenushow = false;
                                }
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_ON)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_OFF)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == FZ_LIST)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_CLEAR)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_GIVE)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == STATE_CHOOSE)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == MODLE_CHANGE)
                        {
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCDW)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                if(pCGDDC->getIsWD_DW())
                                {
                                    mFunButtonDevInfo->strButtonName = "道岔设置有电";
                                }
                                else
                                {
                                    mFunButtonDevInfo->strButtonName = "道岔设置无电";
                                }
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCFW)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                if(pCGDDC->getIsWD_FW())
                                {
                                    mFunButtonDevInfo->strButtonName = "道岔设置有电";
                                }
                                else
                                {
                                    mFunButtonDevInfo->strButtonName = "道岔设置无电";
                                }
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_JG)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_XG)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pCGDDC->getCode();
                                mFunButtonDevInfo->nDevType=Dev_DC;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        return;
                    }
                }
                if(pCurrStation->DevArray[j]->getDevType() == Dev_GD)
                {
                    pGD=(CGD*)(pCurrStation->DevArray[j]);

                    if(pGD->GetMousePoint(mcoursePoint))
                    {
                        emit SendTextAnalysisSignal(177, QCursor::pos(), pGD->m_strName);
                        if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pGD->getCode();
                                mFunButtonDevInfo->nDevType=Dev_GD;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pGD->m_RangeVisible_XZ = true;
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pGD->getCode();
                                mFunButtonDevInfo->nDevType=Dev_GD;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pGD->m_RangeVisible_XZ = true;
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pGD->getCode();
                                mFunButtonDevInfo->nDevType=Dev_GD;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pGD->m_RangeVisible_XZ = true;
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                //                            mPassWordEntryDlg_NEW->strDescrib = "分路不良,请输入密码";
                                //                            mPassWordEntryDlg_NEW->InitPassWord();
                                //                            mPassWordEntryDlg_NEW->exec();
                                //                            if(mPassWordEntryDlg_NEW->onOK == true)
                                if(pGD->isGDFLBL)//股道当前是分路不良状态 取消时弹窗才出现 设置时不显示
                                {
                                    mPoorShuntDlg->InitShow(Dev_GD,pGD->getName());
                                    mPoorShuntDlg->exec();
                                    if(mPoorShuntDlg->isGDFLBLcheck == false)
                                    {
                                        mFunButtonDevInfo->nCode=pGD->getCode();
                                        mFunButtonDevInfo->nDevType=Dev_GD;
                                        //鼠标左键按下 向逻辑发送设备按下信息
                                        emit SendDataToSecerSignal(2);
                                        pGD->m_RangeVisible_XZ = true;
                                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                    }
                                }
                                else
                                {
                                    mFunButtonDevInfo->nCode=pGD->getCode();
                                    mFunButtonDevInfo->nDevType=Dev_GD;
                                    //鼠标左键按下 向逻辑发送设备按下信息
                                    emit SendDataToSecerSignal(2);
                                    pGD->m_RangeVisible_XZ = true;
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                }
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
                        {
                            if((mFunButtonDevInfo->nCode==-1)&&(pGD->isGDFLBL))
                            {
                                mFunButtonDevInfo->nCode=pGD->getCode();
                                mFunButtonDevInfo->nDevType=Dev_GD;
                                //鼠标左键按下 向逻辑发送设备按下信息
                                emit SendDataToSecerSignal(2);
                                pGD->m_RangeVisible_XZ = true;
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                            }
                        }
                        else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_GD)
                        {
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                if(pGD->getGDType() != JJ_QD)
                                {
                                    mFunButtonDevInfo->nCode=pGD->getCode();
                                    mFunButtonDevInfo->nDevType=Dev_GD;
                                    //鼠标左键按下 向逻辑发送设备按下信息
                                    emit SendDataToSecerSignal(2);
                                    if(pGD->getPowerCut())
                                    {
                                        mFunButtonDevInfo->strButtonName = "股道设置有电";
                                    }
                                    else
                                    {
                                        mFunButtonDevInfo->strButtonName = "股道设置无电";
                                    }
                                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                }
                                else
                                {
                                    mFunButtonDevInfo->nCode=-1;
                                }
                                pGD->m_RangeVisible_XZ = true;
                            }
                        }
                        if(mFunButtonDevInfo->strButtonName.contains("确认空闲"))//接通光带
                        {
                            bzrjrightmenushow=pGD->getGDFLBL();
                            pStationFunButtonWidget->SetFunButtonEn(21,false);
                        }
                        return;
                    }
                }
            }
        }
        //功能按钮栏 功能按钮按下
        if(pStationFunButtonWidget->nFunButtonPressState==FUNC_BTN/*&&bPassWordResult*/)
        {
            if(mFunButtonDevInfo->nCode!=-1)
            {
                return;
            }
            //自动闭塞
            for(int j=0;j<pCurrStation->ZDBSArray.size();j++)
            {
                if(pCurrStation->ZDBSArray[j]->getDevType() == Dev_ZDBS)
                {
                    pZDBS=(CZDBS*)(pCurrStation->ZDBSArray[j]);
                    ncode=pZDBS->getGLXH();
                    int ntype=pZDBS->moveCursor(mcoursePoint);
                    if(ntype>0)//getIsDown_ZFZ
                    {
                        if(((ntype==2||ntype==3)&&pZDBS->getIsDown_ZFZ())||(ntype==1||ntype==4))//接车辅助 总辅助
                        {
                            mPassWordEntryDlg_NEW->strDescrib = pZDBS->m_strName+pZDBS->GetANIsDown(ntype)+"功能按钮"+",请输入密码";
                            mPassWordEntryDlg_NEW->InitPassWord();
                            if(mFunButtonDevInfo->nCode==-1)
                            {
                                mFunButtonDevInfo->nCode=pZDBS->getGLXH();
                            }
                            mFunButtonDevInfo->strButtonName=pZDBS->GetANIsDown(ntype);
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);

                            if(mFunButtonDevInfo->strButtonName == "总辅助")
                            {
                                emit SendTextAnalysisSignal(65, QCursor::pos(), pZDBS->m_strName);
                            }
                            else if(mFunButtonDevInfo->strButtonName == "接车辅助")
                            {
                                emit SendTextAnalysisSignal(66, QCursor::pos(), pZDBS->m_strName);
                            }
                            else if(mFunButtonDevInfo->strButtonName == "发车辅助")
                            {
                                emit SendTextAnalysisSignal(67, QCursor::pos(), pZDBS->m_strName);
                            }
                            else if(mFunButtonDevInfo->strButtonName == "允许改方")
                            {
                                emit SendTextAnalysisSignal(68, QCursor::pos(), pZDBS->m_strName);
                            }

                            mPassWordEntryDlg_NEW->exec();
                            if(mPassWordEntryDlg_NEW->onOK == true)
                            {
                                pZDBS->SetANIsDown(ntype);
                                mFunButtonDevInfo->nDevType=Dev_ZDBS;
                                if(mFunButtonDevInfo->nCode==-1)
                                {
                                    mFunButtonDevInfo->nCode=pZDBS->getGLXH();
                                }
                                mFunButtonDevInfo->strButtonName=pZDBS->GetANIsDown(ntype);
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                if(mFunButtonDevInfo->strButtonName == "总辅助")
                                {
                                    pZDBS->ZFZAN_QF++;
                                }
                                else if(mFunButtonDevInfo->strButtonName == "接车辅助")
                                {
                                    pZDBS->JCFZAN_QF++;
                                }
                                else if(mFunButtonDevInfo->strButtonName == "发车辅助")
                                {
                                    pZDBS->FCFZAN_QF++;
                                }
                                else if(mFunButtonDevInfo->strButtonName == "允许改方")
                                {
                                    pZDBS->GFAN_QF++;
                                }
                            }
                            else
                            {
                                emit SendDataToSecerSignal(4);
                                OperateOrderClean();
                            }
                            break;
                        }
                    }
                    else
                    {
                        pZDBS=nullptr;
                    }
                }
            }
            //半自动闭塞
            for(int j=0;j<pCurrStation->BZDBSArray.size();j++)
            {
                if(pCurrStation->BZDBSArray[j]->getDevType() == Dev_BZDBS)
                {
                     pBZDBS=(CBZDBS*)(pCurrStation->BZDBSArray[j]);
                     ncode=pBZDBS->getGLXH();
                     int ntype=pBZDBS->moveCursor(mcoursePoint);
                     if(ntype>0)
                     {
                         if(pBZDBS->GetQFByAnType(ntype))
                         {
                             mFunButtonDevInfo->strButtonName=pBZDBS->GetANIsDown(ntype);
                             if(mFunButtonDevInfo->strButtonName == "事故")
                             {
                                 emit SendTextAnalysisSignal(70, QCursor::pos(), pBZDBS->m_strName);
                             }

                             mPassWordEntryDlg_NEW->strDescrib = pBZDBS->m_strName+pBZDBS->GetANIsDown(ntype)+"功能按钮"+",请输入密码";
                             mPassWordEntryDlg_NEW->InitPassWord();
                             mPassWordEntryDlg_NEW->exec();
                             if(mPassWordEntryDlg_NEW->onOK == true)
                             {
                                 pBZDBS->SetANIsDown(ntype);
                                 mFunButtonDevInfo->nDevType=Dev_BZDBS;
                                 if(mFunButtonDevInfo->nCode==-1)
                                 {
                                    mFunButtonDevInfo->nCode=pBZDBS->getCode();
                                 }

                                 pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                 pCurrStation->ANFlashFlag = 0;
                                 pCurrStation->ButtonFlashDown = true;
                                 if(mFunButtonDevInfo->strButtonName == "事故")
                                 {
                                     pBZDBS->SGAN_QF++;
                                 }
                                 else if(mFunButtonDevInfo->strButtonName == "闭塞切换")
                                 {
                                     pBZDBS->BSQHAN_QF++;
                                 }
                             }
                             else
                             {
                                 emit SendDataToSecerSignal(4);
                                 OperateOrderClean();
                             }
                         }
                         else
                         {
                             //emit SendTextAnalysisSignal(69, QCursor::pos(), pBZDBS->m_strName);
                             pBZDBS->SetANIsDown(ntype);
                             mFunButtonDevInfo->nDevType=Dev_BZDBS;
                             if(mFunButtonDevInfo->nCode==-1)
                             {
                                mFunButtonDevInfo->nCode=pBZDBS->getCode();
                                mFunButtonDevInfo->strButtonName=pBZDBS->GetANIsDown(ntype);
                                if(mFunButtonDevInfo->strButtonName == "闭塞")
                                {
                                    emit SendTextAnalysisSignal(69, QCursor::pos(), pBZDBS->m_strName);
                                }
                                else if(mFunButtonDevInfo->strButtonName == "复原")
                                {
                                    emit SendTextAnalysisSignal(71, QCursor::pos(), pBZDBS->m_strName);
                                }
                             }

                             pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                             pCurrStation->ANFlashFlag = 0;
                             pCurrStation->ButtonFlashDown = true;
                         }
                         break;
                     }
                     else
                     {
                         pBZDBS=nullptr;
                     }
                }
            }
            //计轴
            for(int j=0;j<pCurrStation->JZArray.size();j++)
            {
                if(pCurrStation->JZArray[j]->getDevType() == Dev_JZ)
                {
                     JZ=(CJZ*)(pCurrStation->JZArray[j]);
                     ncode=JZ->getGLXH();
                     int ntype=JZ->moveCursor(mcoursePoint);
                     if(ntype>0)
                     {
//                         mPassWordEntryDlg_NEW->strDescrib = JZ->m_strName+JZ->GetANIsDown(ntype)+"功能按钮"+",请输入密码";
//                         mPassWordEntryDlg_NEW->InitPassWord();
//                         mPassWordEntryDlg_NEW->exec();
//                         if(mPassWordEntryDlg_NEW->onOK == true)
                         {
                             JZ->SetANIsDown(ntype);
                             mFunButtonDevInfo->nDevType=Dev_JZ;
                             if(mFunButtonDevInfo->nCode==-1)
                             {
                                mFunButtonDevInfo->nCode=JZ->getCode();
                             }
                             mFunButtonDevInfo->strButtonName=JZ->GetANIsDown(ntype);
                             pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                             pCurrStation->ANFlashFlag = 0;
                             pCurrStation->ButtonFlashDown = true;
                             if(mFunButtonDevInfo->strButtonName.contains("复零"))
                             {
                                 if(JZ->HaveJZFLANQFJS)
                                 {
                                     mPassWordEntryDlg_NEW->strDescrib = JZ->m_strName+JZ->GetANIsDown(ntype)+"功能按钮"+",请输入密码";
                                     mPassWordEntryDlg_NEW->InitPassWord();
                                     mPassWordEntryDlg_NEW->exec();
                                     if(mPassWordEntryDlg_NEW->onOK == true)
                                     {
                                         JZ->JZFLAN_QF++;
                                     }
                                     else
                                     {
                                         emit SendDataToSecerSignal(4);
                                         OperateOrderClean();
                                     }
                                 }
                                 emit SendTextAnalysisSignal(74, QCursor::pos(), JZ->m_strName);
                             }
                             else if(mFunButtonDevInfo->strButtonName.contains("停用"))
                             {
                                 if(JZ->HaveJZTYANQFJS)
                                 {
                                     mPassWordEntryDlg_NEW->strDescrib = JZ->m_strName+JZ->GetANIsDown(ntype)+"功能按钮"+",请输入密码";
                                     mPassWordEntryDlg_NEW->InitPassWord();
                                     mPassWordEntryDlg_NEW->exec();
                                     if(mPassWordEntryDlg_NEW->onOK == true)
                                     {
                                         JZ->JZTYAN_QF++;
                                     }
                                     else
                                     {
                                         emit SendDataToSecerSignal(4);
                                         OperateOrderClean();
                                     }
                                 }
                                 emit SendTextAnalysisSignal(73, QCursor::pos(), JZ->m_strName);
                             }
                             else if(mFunButtonDevInfo->strButtonName.contains("使用"))
                             {
                                 if(JZ->HaveJZSYANQFJS)
                                 {
                                     mPassWordEntryDlg_NEW->strDescrib = JZ->m_strName+JZ->GetANIsDown(ntype)+"功能按钮"+",请输入密码";
                                     mPassWordEntryDlg_NEW->InitPassWord();
                                     mPassWordEntryDlg_NEW->exec();
                                     if(mPassWordEntryDlg_NEW->onOK == true)
                                     {
                                         JZ->JZSYAN_QF++;
                                     }
                                     else
                                     {
                                         emit SendDataToSecerSignal(4);
                                         OperateOrderClean();
                                     }
                                 }
                                 emit SendTextAnalysisSignal(72, QCursor::pos(), JZ->m_strName);
                             }
                         }
//                         else
//                         {
//                             emit SendDataToSecerSignal(4);
//                             OperateOrderClean();
//                         }
                         break;
                     }
                     else
                     {
                         pBZDBS=nullptr;
                     }
                }
            }
            //功能按钮 上电解锁 语音（预留）
            for(int j=0;j<pCurrStation->FuncBtnArray.size();j++)
            {
                if(pCurrStation->FuncBtnArray[j]->getDevType() == Dev_FUNCB)
                {
                    pfUNCBUTTON=(FUNCBUTTON*)(pCurrStation->FuncBtnArray[j]);
                    int ntype =pfUNCBUTTON->GetMousePoint(mcoursePoint);
                    //语音暂停 上电解锁
                    if (ntype>0)
                    {
                        if (ntype==1)//语音暂停
                        {
                            if(pCurrStation->tts->state()==QTextToSpeech::Speaking)
                            {
                                pCurrStation->tts->pause();
                            }
                            //蓝色闪烁
                            pfUNCBUTTON->setIsDown_Button(1);
                        }
                        else if (ntype==2) //上电解锁
                        {
                            mPassWordEntryDlg_NEW->strDescrib = pfUNCBUTTON->Button_Name+"功能按钮"+",请输入密码";
                            mPassWordEntryDlg_NEW->InitPassWord();
                            mPassWordEntryDlg_NEW->exec();
                            if(mPassWordEntryDlg_NEW->onOK == true)
                            {
                                mFunButtonDevInfo->nDevType=Dev_FUNCB;
                                mFunButtonDevInfo->nCode=-2;
                                mFunButtonDevInfo->strButtonName="上电解锁";
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                //蓝色闪烁
                                pfUNCBUTTON->setIsDown_Button(1);
                                pfUNCBUTTON->Button_QF++;
                            }
                            else
                            {
                                emit SendDataToSecerSignal(4);
                                OperateOrderClean();
                            }
                        }
                        else if (ntype==6) //故障通知
                        {
                            mPassWordEntryDlg_NEW->strDescrib = pfUNCBUTTON->Button_Name+"功能按钮"+",请输入密码";
                            mPassWordEntryDlg_NEW->InitPassWord();
                            mPassWordEntryDlg_NEW->exec();
                            if(mPassWordEntryDlg_NEW->onOK == true)
                            {
                                mFunButtonDevInfo->nDevType=Dev_FUNCB;
                                mFunButtonDevInfo->nCode=-2;
                                mFunButtonDevInfo->strButtonName="故障通知";
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                //蓝色闪烁
                                pfUNCBUTTON->setIsDown_Button(1);
                                pfUNCBUTTON->Button_QF++;
                            }
                            else
                            {
                                emit SendDataToSecerSignal(4);
                                OperateOrderClean();
                            }
                        }
                        else if (ntype==7) //清除
                        {
//                            mPassWordEntryDlg_NEW->strDescrib = pfUNCBUTTON->Button_Name+"功能按钮"+",请输入密码";
//                            mPassWordEntryDlg_NEW->InitPassWord();
//                            mPassWordEntryDlg_NEW->exec();
//                            if(mPassWordEntryDlg_NEW->onOK == true)
                            {
                                mFunButtonDevInfo->nDevType=Dev_FUNCB;
                                mFunButtonDevInfo->nCode=-2;
                                mFunButtonDevInfo->strButtonName="清除";
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                //蓝色闪烁
                                pfUNCBUTTON->setIsDown_Button(1);
                                pfUNCBUTTON->Button_QF++;
                            }
                        }
                        else if (ntype==8) //允许改方
                        {
                            mPassWordEntryDlg_NEW->strDescrib = pfUNCBUTTON->Button_Name+"功能按钮"+",请输入密码";
                            mPassWordEntryDlg_NEW->InitPassWord();
                            mPassWordEntryDlg_NEW->exec();
                            if(mPassWordEntryDlg_NEW->onOK == true)
                            {
                                mFunButtonDevInfo->nDevType=Dev_FUNCB;
                                mFunButtonDevInfo->nCode=-2;
                                mFunButtonDevInfo->strButtonName="允许改方";
                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                //蓝色闪烁
                                pfUNCBUTTON->setIsDown_Button(1);
                                pfUNCBUTTON->Button_QF++;
                            }
                            else
                            {
                                emit SendDataToSecerSignal(4);
                                OperateOrderClean();
                            }
                        }

                    }
                    else
                    {
                        pfUNCBUTTON=nullptr;
                    }
                }
            }
            //功能按钮 尖轨心轨
            for(int j=0;j<pCurrStation->DCBtnArray.size();j++)
            {
                if(pCurrStation->DCBtnArray[j]->getDevType() == Dev_DCBTN)
                {
                    DCBUTTON=(CDcBtn*)(pCurrStation->DCBtnArray[j]);
                    bool ntype = DCBUTTON->GetMousePoint(mcoursePoint);
                    if(ntype)
                    {
                        if(DCBUTTON->m_tType == DCGZ_JG)
                        {
                            for(int k=0;k<pCurrStation->DevArray.size();k++)
                            {
                                if(pCurrStation->DevArray[k]->getDevType() == Dev_DC)
                                {
                                    pCGDDC=(CGDDC*)(pCurrStation->DevArray[k]);
                                    //if (pCGDDC->m_strName == DCBUTTON->m_strDcName)
                                    if (pCGDDC->m_nCode == DCBUTTON->m_nDcCode)
                                    {
                                        if(DCBUTTON->ButtonForm!=2)
                                        {
                                            mPassWordEntryDlg_NEW->strDescrib = DCBUTTON->Button_Name+"功能按钮"+",请输入密码";
                                            mPassWordEntryDlg_NEW->InitPassWord();
                                            mPassWordEntryDlg_NEW->exec();
                                            if(mPassWordEntryDlg_NEW->onOK == true)
                                            {
                                                mFunButtonDevInfo->nDevType=Dev_DCBTN;
                                                if(mFunButtonDevInfo->nCode==-1)
                                                {
                                                    mFunButtonDevInfo->nCode=pCGDDC->m_nCode;
                                                }
                                                mFunButtonDevInfo->strButtonName="尖轨故障";
                                                pStationFunButtonWidget->nFunButtonPressState = DCFAULT_JG;
                                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                DCBUTTON->setIsDown_Button(1);
                                                DCBUTTON->Button_QF++;
                                            }
                                            else
                                            {
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                            }
                                        }
                                        else
                                        {
                                            mFunButtonDevInfo->nDevType=Dev_DCBTN;
                                            if(mFunButtonDevInfo->nCode==-1)
                                            {
                                                mFunButtonDevInfo->nCode=pCGDDC->m_nCode;
                                            }
                                            mFunButtonDevInfo->strButtonName="尖轨故障";
                                            pStationFunButtonWidget->nFunButtonPressState = DCFAULT_JG;
                                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                            DCBUTTON->setIsDown_Button(1);
                                            DCBUTTON->Button_QF++;
                                        }
                                    }
                                }
                            }
                        }
                        else if(DCBUTTON->m_tType == DCGZ_XG)
                        {
                            for(int k=0;k<pCurrStation->DevArray.size();k++)
                            {
                                if(pCurrStation->DevArray[k]->getDevType() == Dev_DC)
                                {
                                    pCGDDC=(CGDDC*)(pCurrStation->DevArray[k]);
                                    //if (pCGDDC->m_strName == DCBUTTON->m_strDcName)
                                    if (pCGDDC->m_nCode == DCBUTTON->m_nDcCode)
                                    {
                                        if(DCBUTTON->ButtonForm!=2)
                                        {
                                            mPassWordEntryDlg_NEW->strDescrib = DCBUTTON->Button_Name+"功能按钮"+",请输入密码";
                                            mPassWordEntryDlg_NEW->InitPassWord();
                                            mPassWordEntryDlg_NEW->exec();
                                            if(mPassWordEntryDlg_NEW->onOK == true)
                                            {
                                                mFunButtonDevInfo->nDevType=Dev_DCBTN;
                                                if(mFunButtonDevInfo->nCode==-1)
                                                {
                                                    mFunButtonDevInfo->nCode=pCGDDC->m_nCode;
                                                }
                                                mFunButtonDevInfo->strButtonName="心轨故障";
                                                pStationFunButtonWidget->nFunButtonPressState = DCFAULT_XG;
                                                pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                                DCBUTTON->setIsDown_Button(1);
                                                DCBUTTON->Button_QF++;
                                            }
                                            else
                                            {
                                                emit SendDataToSecerSignal(4);
                                                OperateOrderClean();
                                            }
                                        }
                                        else
                                        {
                                            mFunButtonDevInfo->nDevType=Dev_DCBTN;
                                            if(mFunButtonDevInfo->nCode==-1)
                                            {
                                                mFunButtonDevInfo->nCode=pCGDDC->m_nCode;
                                            }
                                            mFunButtonDevInfo->strButtonName="心轨故障";
                                            pStationFunButtonWidget->nFunButtonPressState = DCFAULT_XG;
                                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                                            DCBUTTON->setIsDown_Button(1);
                                            DCBUTTON->Button_QF++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        DCBUTTON = nullptr;
                    }
                }
            }

            return;
        }

        //功能按钮栏 引导总锁按钮按下
        if(pStationFunButtonWidget->nFunButtonPressState==CALLON_LOCK)
        {
            if(mFunButtonDevInfo->nCode!=-1)
            {
                return;
            }
            for(int j=0;j<pCurrStation->FuncBtnArray.size();j++)
            {
                if(pCurrStation->FuncBtnArray[j]->getDevType() == Dev_FUNCB)
                {
                     pfUNCBUTTON=(FUNCBUTTON*)(pCurrStation->FuncBtnArray[j]);
                     int ntype =pfUNCBUTTON->GetMousePoint(mcoursePoint);
                     if((ntype==3)||(ntype==4)||(ntype==5))
                     {
                        pfUNCBUTTON->SetANIsDown();
                        mFunButtonDevInfo->nDevType=Dev_FUNCB;
                        mFunButtonDevInfo->nCode=pCurrStation->GetXHCodeByName(pfUNCBUTTON->DEVName);
                        if(pfUNCBUTTON->DEVName == "FIELDLOCK")
                        {
                            mFunButtonDevInfo->m_nQCYDZS = true;
                        }
                        else
                        {
                            mFunButtonDevInfo->m_nQCYDZS = false;
                        }

                        mFunButtonDevInfo->strButtonName="引导总锁";
                        pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                        //蓝色闪烁
                        pfUNCBUTTON->setIsDown_Button(1);
                        pfUNCBUTTON->Button_QF++;
                     }
                     else
                     {
                         pfUNCBUTTON=nullptr;
                     }
                }
            }
            return;
        }
        //独立按钮
        for(int j=0;j<pCurrStation->vectXhBtn.size();j++)
        {
            if(pCurrStation->vectXhBtn[j]->getDevType() == Dev_DLAN)
            {
                AlonXhBtn=(AlonXHBtn*)(pCurrStation->vectXhBtn[j]);
                int ntype =AlonXhBtn->GetMousePoint(mcoursePoint);
                if(pCurrStation->m_nModeState == 2 || AlonXhBtn->m_nFuncLockState)
                {
                    return;
                }
                //进路建立
                if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                {
                    if (AlonXhBtn->GetMousePoint(mcoursePoint))
                    {
                        AlonXhBtn->m_strName=getNameOfDevNode(pCurrStation, AlonXhBtn->m_nCode);
                        QString name = AlonXhBtn->m_strName;
                        ncode=AlonXhBtn->getCode();
                        //鼠标左键按下 向逻辑发送设备按下信息 用于倒计时
                         emit SendDataToSecerSignal(2);
                        //鼠标左键按下设备按钮后，向逻辑发送按下信息，用于终端闪烁
                        int nFunType=0x00;
                        if(AlonXhBtn->m_nANTYPE==DCAN)
                        {
                            if(mFunButtonDevInfo->nDevCode[0]==-1)
                            {
                                return;
                            }
                            nFunType=0x02;
                            //emit SendTextAnalysisSignal(62, QCursor::pos(), AlonXhBtn->m_strName);
                            emit SendTextAnalysisSignal(62, QCursor::pos(), AlonXhBtn->Button_Name);
                            AlonXhBtn->setIsDown_Button(1);
                            pCurrStation->ANFlashFlag = 0;
                            pCurrStation->ButtonFlashDown = true;
                            mFunButtonDevInfo->strButtonName=mFunButtonDevInfo->strButtonName+"调车";
                        }
                        else if(AlonXhBtn->m_nANTYPE==LCAN)
                        {
                            if(mFunButtonDevInfo->nDevCode[0]==-1)
                            {
                                return;
                            }
                            nFunType=0x01;
                            //emit SendTextAnalysisSignal(61, QCursor::pos(), AlonXhBtn->m_strName);
                            emit SendTextAnalysisSignal(61, QCursor::pos(), AlonXhBtn->Button_Name);
                            AlonXhBtn->setIsDown_Button(1);
                            pCurrStation->ANFlashFlag = 0;
                            pCurrStation->ButtonFlashDown = true;
                        }
                        //只有在进路建立/办理情况下发送
                        emit SendDataToSecerSignal(1,nFunType,pXHD->getCode());
                        if(mFunButtonDevInfo->nDevCode[mFunButtonDevInfo->RecordDevNum] == -1)
                        {
                            mFunButtonDevInfo->SetCode(SetRouteCode(AlonXhBtn->getCode(),nFunType),mFunButtonDevInfo->RecordDevNum);
                        }

                        mFunButtonDevInfo->nDevType=Dev_XH;
                        mFunButtonDevInfo->RecordDevNum++;//按钮按下个数
                        if(mFunButtonDevInfo->RecordDevNum>0)
                        {
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
                        }
                        if(mFunButtonDevInfo->RecordDevNum>1)
                        {
                            pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                        }
                        return;
                    }
                }
            }
        }

        //腰岔或股道确认
        CGDYC* pGDYC = nullptr;
        if(pCurrStation->GetFCZKMode()==0x22)
            return;
        for(int i = 0; i < pCurrStation->vectGDYCArray.size(); i++)
        {
            pGDYC = pCurrStation->vectGDYCArray.at(i);
            if(pGDYC->GetMousePoint(mcoursePoint))
            {
                if(pGDYC->m_nType == 1){
                    mPassWordEntryDlg_NEW->strDescrib = pGDYC->m_strGD + "腰岔解锁,请输入密码";
                    mFunButtonDevInfo->nDevType = Dev_DC;
                }
                if(pGDYC->m_nType == 2){
                    mPassWordEntryDlg_NEW->strDescrib = pGDYC->m_strGD + "股道确认,请输入密码";
                    mFunButtonDevInfo->nDevType = Dev_GD;
                }

                //mPassWordEntryDlg_NEW->strDescrib = "功能按钮,请输入密码";
                mPassWordEntryDlg_NEW->InitPassWord();
                mPassWordEntryDlg_NEW->exec();
                if(mPassWordEntryDlg_NEW->onOK == true)
                {
                    pGDYC->m_bBtnIsDown = true;
                    pCurrStation->ANFlashFlag = 0;
                    pCurrStation->ButtonFlashDown = true;

                    mFunButtonDevInfo->nCode = pGDYC->getGLXH(0);
                    mFunButtonDevInfo->nCode2 = pGDYC->getGLXH(1);
                    mFunButtonDevInfo->nCode3 = pGDYC->getGLXH(2);
                    if(pGDYC->m_nType == 1)
                        mFunButtonDevInfo->strButtonName = "腰岔解锁";
                    if(pGDYC->m_nType == 2)
                        mFunButtonDevInfo->strButtonName = "股道确认";
                    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,true);
                }
                else
                {
                    emit SendDataToSecerSignal(4);
                    OperateOrderClean();
                }
            }
        }

    }
}

//车次窗放大缩小
void StationCtrlDispKSK::SetDiploidCheCi(int n)
{
    if(n==1)//+
    {
        if(pCurrStation->nDiploidCheCi < 2)
            pCurrStation->nDiploidCheCi = pCurrStation->nDiploidCheCi + 0.1;
    }
    else if(n==2)//-
    {
        if(pCurrStation->nDiploidCheCi > 0.4)
            pCurrStation->nDiploidCheCi = pCurrStation->nDiploidCheCi - 0.1;
    }
    else
    {
        pCurrStation->nDiploidCheCi = 1;
    }
}
int StationCtrlDispKSK::GetPoorShuntType()
{
     return nPoorShuntType;
}
void StationCtrlDispKSK::ShowSeekTrain()
{
    m_SeekTrain->SetData(pCurrStation->m_ArrayTrainNumWnd,pCurrStation->getStationName());
    m_SeekTrain->show();
}

//设置站场右键菜单 放大缩小显示
void StationCtrlDispKSK::SetMenuStaShow(int n)
{

    if(n==0)
    {
        SetMenuCheck(menuSTA,"放大 站场图",false);
        SetMenuCheck(menuSTA,"缩小 站场图",false);
    }
    else if(n==1)
    {
        SetMenuCheck(menuSTA,"放大 站场图",true);
        SetMenuCheck(menuSTA,"缩小 站场图",false);
    }
    else if(n==2)
    {

        SetMenuCheck(menuSTA,"放大 站场图",false);
        SetMenuCheck(menuSTA,"缩小 站场图",true);
    }
}
void StationCtrlDispKSK::SetMenuStaShow()
{
    SetMenuCheck(menuSTA,MENU_JYJ,bShow_Jyj);
    SetMenuCheck(menuSTA,MENU_CCC_POS,bShowCheCi);
    SetMenuStateInFCZK(menuSTA,"还原 车次窗",isTrainNumWndReSize,false);
    SetMenuStateInFCZK(menuSTA,"还原 站场图",isStationViewReSize,false);
    QList<QAction*> mactions = menuSTA->actions();
    for (int i = 0; i < mactions.size(); ++i)
    {
        QAction* action = mactions.at(i);
        if (action->menu())  // 当前QAction有二级菜单
        {
            QMenu* menu = action->menu();
            QList<QAction*> subActions = menu->actions();   // 获取二级菜单的所有子项
            for(int j = 0; j < subActions.size(); ++j)
            {
                if(subActions.at(j)->objectName()=="staname")
                {
                    subActions.at(j)->setText(pCurrStation->getStationName());
                    break;
                }
            }
        }
    }
}
//设置道岔右键菜单选项状态
void StationCtrlDispKSK::SetMenuDCShow(CGDDC* pCGDDC)
{
    if(pCGDDC->getState(DCDW))
    {
        SetMenuCheck(menuDC,MENU_DC,true);
        SetMenuCheck(menuDC,MENU_FC,false);
    }
    else if(pCGDDC->getState(DCFW))
    {
        SetMenuCheck(menuDC,MENU_DC,false);
        SetMenuCheck(menuDC,MENU_FC,true);
    }
    else if(pCGDDC->getState(DCSK))
    {
        SetMenuCheck(menuDC,MENU_DC,false);
        SetMenuCheck(menuDC,MENU_FC,false);
    }

    if(pCGDDC->getIsDS())
    {
        SetMenuCheck(menuDC,MENU_DS,true);
        SetMenuCheck(menuDC,MENU_DJ,false);
        SetMenuStateInFCZK(menuDC,MENU_DJ,true,false);
        SetMenuStateInFCZK(menuDC,MENU_DC,false,false);
        SetMenuStateInFCZK(menuDC,MENU_FC,false,false);
    }
    else//单解
    {
        SetMenuCheck(menuDC,MENU_DS,false);
        SetMenuCheck(menuDC,MENU_DJ,true);
        SetMenuStateInFCZK(menuDC,MENU_DJ,false,false);
        SetMenuStateInFCZK(menuDC,MENU_DC,true,false);
        SetMenuStateInFCZK(menuDC,MENU_FC,true,false);
    }
    if(pCGDDC->getIsFS())
    {
        SetMenuCheck(menuDC,MENU_FSJF,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_FSJF,false);
    }

    //if(pCGDDC->getIsFLBL_CQ())
    if(pCGDDC->flblStatusCQ)
    {
        SetMenuCheck(menuDC,MENU_FLBL_CQ,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_FLBL_CQ,false);
    }
    //if(pCGDDC->getIsFLBL_DW())
    if(pCGDDC->flblStatusDW)
    {
        SetMenuCheck(menuDC,MENU_FLBL_DW,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_FLBL_DW,false);
    }
    //if(pCGDDC->getIsFLBL_FW())
    if(pCGDDC->flblStatusFW)
    {
        SetMenuCheck(menuDC,MENU_FLBL_FW,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_FLBL_FW,false);
    }
    if(pCGDDC->getXGGZ())
    {
        SetMenuCheck(menuDC,MENU_XGGZ,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_XGGZ,false);
    }
    if(pCGDDC->getJGGZ())
    {
        SetMenuCheck(menuDC,MENU_JGGZ,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_JGGZ,false);
    }

    if(pCGDDC->getIsWD_DW())
    {
        SetMenuCheck(menuDC,MENU_JCW_DWWD,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_JCW_DWWD,false);
    }
    if(pCGDDC->getIsWD_FW())
    {
        SetMenuCheck(menuDC,MENU_JCW_FWWD,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_JCW_FWWD,false);
    }
    if(pCGDDC->getFLBLKX())
    {
        SetMenuCheck(menuDC,MENU_QRKX,true);
    }
    else
    {
         SetMenuCheck(menuDC,MENU_QRKX,false);
    }

    //非常站控模式下禁止操作
    if(pCurrStation->GetFCZKMode()==0x22)
    {
        SetMenuStateInFCZK(menuDC,"",false,false);
        SetMenuStateInFCZK(menuDC,"接触网定位无电",true,false);
        SetMenuStateInFCZK(menuDC,"接触网反位无电",true,false);
        SetMenuStateInFCZK(menuDC,MENU_QRKX,(pCGDDC->flblStatusDW)||(pCGDDC->flblStatusFW)||(pCGDDC->flblStatusCQ),false);
    }
}
//设置信号机右键菜单选项状态
void StationCtrlDispKSK::SetMenuXHShow(CXHD *pXHD)
{
    if(pXHD!=nullptr)
    {
        //SetMenuStateInFCZK(menuXH,""/*MENU_XHREOPEN*/,true,false);
        //  ((pXHD->getXHDState()==XHD_HD&&(pXHD->getXHDType()==JZ_XHJ||pXHD->getXHDType()==CZ_XHJ))
        //||(pXHD->getXHDState()==XHD_AD&&pXHD->getXHDType()==DC_XHJ))

        for(int i=0;i<menuXH->actions().size();i++)
        {
            if(menuXH->actions()[i]->text().contains("办理")/*||menuXH->actions()[i]->text().contains("重开")*/)
            {
                if((pXHD->getXHDState()==XHD_HD&&(pXHD->getXHDType()==JZ_XHJ||pXHD->getXHDType()==CZ_XHJ)))
                {
                    SetMenuStateInFCZK(menuXH,menuXH->actions()[i]->text(),true,false);
                }
                else if(pXHD->getXHDType()==DC_XHJ)
                {
                    SetMenuStateInFCZK(menuXH,menuXH->actions()[i]->text(),true,false);
                }
                else
                {
                    SetMenuStateInFCZK(menuXH,menuXH->actions()[i]->text(),false,false);
                }
                if(pCurrStation->m_nModeState == 2)//车站调车
                {
                   SetMenuStateInFCZK(menuXH,menuXH->actions()[i]->text(),false,false);
                }
            }

        }
    }
    //非常站控模式下禁止操作
    if(pCurrStation->GetFCZKMode()==0x22)
    {
        SetMenuStateInFCZK(menuXH,"",false,false);
    }

}
//设置股道右键菜单选项状态
void StationCtrlDispKSK::SetMenuGDShow(CGD *pGD)
{
    if(pGD->getFS())
    {
        SetMenuCheck(menuGD,MENU_FSJF,true);
    }
    else
    {
        SetMenuCheck(menuGD,MENU_FSJF,false);
    }

    if(pGD->getGDFLBL())
    {
        SetMenuCheck(menuGD,MENU_FLBL,true);
    }
    else
    {
        SetMenuCheck(menuGD,MENU_FLBL,false);
    }

    if(pGD->getPowerCut())
    {
        SetMenuCheck(menuGD,MENU_GDWD,true);
    }
    else
    {
        SetMenuCheck(menuGD,MENU_GDWD,false);
    }
    //确认空闲 MENU_QRKX
    if(pGD->m_nGDFLBLKX)
    {
        SetMenuCheck(menuGD,MENU_QRKX,true);
    }
    else
    {
        SetMenuCheck(menuGD,MENU_QRKX,false);
    }
    //非常站控模式下禁止操作
    if(pCurrStation->GetFCZKMode()==0x22)
    {
        SetMenuStateInFCZK(menuGD,"",false,false);
        SetMenuStateInFCZK(menuGD,"股道无电",true,false);
        SetMenuStateInFCZK(menuGD,MENU_QRKX,pGD->getGDFLBL(),false);
    }

}
void StationCtrlDispKSK::SetMenuCheck(QMenu *menu,QString str,bool b)
{
    for(int i=0;i<menu->actions().size();i++)
    {
        if(menu->actions()[i]->text()==str)
        {
            menu->actions()[i]->setChecked(b);
        }
    }
}
bool StationCtrlDispKSK::ShowDlgByCheck(QString strorder,QString strtype,QString strdevname)
{
    QString qMsg;
    strorder=strorder.trimmed();
    if(strorder.contains("灭灯"))
    {
        strorder="开关信号";
    }
    if(strorder.contains("引导"))
    {
        strorder="引导进路";
    }
//    else
//    {
//        strtype="信号机";
//    }
//    if(strtype == "信号机")
//    {
//        qMsg = QString("下发“%1”命令吗？").arg(strorder);
//    }
//    else
    {
        qMsg = QString("下发“%1[%2:%3]”命令吗？").arg(strorder).arg(strtype).arg(strdevname);
    }

    int ret = QMessageBox::information(this,tr("STPC"),qMsg,tr("确定"),tr("取消"),"",0);
    if(ret==0)
    {
        emit SendTextAnalysisSignal(77, QCursor::pos());
        return true;
    }
    else
    {
        emit SendTextAnalysisSignal(78, QCursor::pos());
        return false;
    }
}
bool StationCtrlDispKSK::IsShowDlgByCheck(QString strorder)
{
    if(strorder.contains(MENU_DC)||strorder.contains(MENU_FC)||strorder.contains(MENU_DS)||strorder.contains(MENU_DJ)||strorder.contains(MENU_FSJF)
            ||strorder.contains(MENU_QGJ)||strorder.contains(MENU_FLBL)||strorder.contains(MENU_QRKX)
            /*||strorder.contains(MENU_GDWD)*/||strorder.contains(MENU_XHREOPEN)||strorder.contains(MENU_DDMD)
            ||strorder.contains(MENU_PDJS)||strorder.contains(MENU_ZRS)||strorder.contains(MENU_JL_YD)
            ||strorder.contains(MENU_JGGZ)||strorder.contains(MENU_XGGZ)||strorder.contains(MENU_JL_CANCLE))
        return true;
    else
        return  false;
}
bool StationCtrlDispKSK::ISLeadSwalingShow(QString strorder)
{
    if(strorder.contains(MENU_FLBL)||strorder.contains(MENU_QGJ)||strorder.contains(MENU_DDMD)
            ||strorder.contains(MENU_PDJS)||strorder.contains(MENU_ZRS)||strorder.contains(MENU_JL_YD))
        return true;
    else
        return  false;
}
void StationCtrlDispKSK::LeadSwalingShow(QString strorder)
{
//    if(strorder.contains("分路不良"))
//    {
//        strorder="分路不良，请输入密码";
//    }
    if(strorder.contains("灭灯"))
    {
        strorder="开关信号，请输入密码";
    }
    else if(strorder.contains(MENU_PDJS)||strorder.contains(MENU_ZRS)||strorder.contains(MENU_JL_YD))
    {
        //strorder=strorder.trimmed();
        strorder=strorder.remove(" ");
        if(strorder.contains(MENU_JL_YD))
        {
            strorder=strorder+"进路";
        }
        strorder=strorder+"，请输入密码";
    }
    else
    {
      strorder=strorder+"，请输入密码";
    }
    mPassWordEntryDlg->ClearText();
    mPassWordEntryDlg->SetTitle();
    mPassWordEntryDlg->SetTypeText(strorder);
    QRect rect=mPassWordEntryDlg->geometry();
    mPassWordEntryDlg->setGeometry(802,379,rect.width(),rect.height());
    mPassWordEntryDlg->show();
}
//bool StationCtrlDispKSK::LeadSwalingShow(QString strorder)
//{
//    bool onOK = false;
////    if(strorder.contains("分路不良"))
////    {
////        strorder="分路不良，请输入密码";
////    }
//    if(strorder.contains("灭灯"))
//    {
//        strorder="开关信号，请输入密码";
//    }
//    else if(strorder.contains(MENU_PDJS)||strorder.contains(MENU_ZRS)||strorder.contains(MENU_JL_YD))
//    {
//        //strorder=strorder.trimmed();
//        strorder=strorder.remove(" ");
//        if(strorder.contains(MENU_JL_YD))
//        {
//            strorder=strorder+"进路";
//        }
//        strorder=strorder+"，请输入密码";
//    }
//    else
//    {
//      strorder=strorder+"，请输入密码";
//    }
//    mPassWordEntryDlg->ClearText();
//    mPassWordEntryDlg->SetTitle();
//    mPassWordEntryDlg->SetTypeText(strorder);
//    QRect rect=mPassWordEntryDlg->geometry();
//    mPassWordEntryDlg->setGeometry(802,379,rect.width(),rect.height());
//    mPassWordEntryDlg->show();
//    onOK = mPassWordEntryDlg->onOK;
//    return onOK;
//}

void StationCtrlDispKSK::SetMouseCursor(int n)
{
    QPixmap pixmap;
    if(n==0)//还原
    {
        setCursor(Qt::ArrowCursor);
        mOldCursor=cursor();
    }
    else if(n==1)//放大
    {
        pixmap.load("://Res/mouseEnlarge.png");
        QCursor mcursor(pixmap);
        mOldCursor=mcursor;
    }
    else if(n==2)//缩小
    {
        pixmap.load("://Res/mouseNarrow.png");
        QCursor mcursor(pixmap);
        mOldCursor=mcursor;
    }

}
void StationCtrlDispKSK::RecResultPoorShuntSolt(int ntype,int ntype2,int ntype3)
{
    if(ntype!=0)
    {
        nPoorShuntType=ntype;
        nPoorShuntType2=ntype2;
        nPoorShuntType3=ntype3;
    }
    else
    {
        nPoorShuntType=0;
        nPoorShuntType2=0;
        nPoorShuntType3=0;
    }
}
void StationCtrlDispKSK::LeadSwalingShowSlot()
{
    mPassWordEntryDlg->ClearText();
    QString str=pStationFunButtonWidget->pFunButtonGroup->button(pStationFunButtonWidget->nFunButtonPressState)->text();
    if(str.contains("引导总锁"))
    {
        mPassWordEntryDlg->SetTypeText(str+",请输入第1重密码");
    }
    else
    {
       mPassWordEntryDlg->SetTypeText(str+",请输入密码");
    }
    QRect rect=mPassWordEntryDlg->geometry();
    mPassWordEntryDlg->setGeometry(802,379,rect.width(),rect.height());
    mPassWordEntryDlg->show();

    //引导总锁铅封窗二次确认
    if(pStationFunButtonWidget->nFunButtonPressState==4/*&&bPassWordResult*/)
    {

        connfunbtn =connect(mPassWordEntryDlg, &PassWordEntryDlg::ClosedSignal,this,[=](){

            mPassWordEntryDlg->ClearText();
            mPassWordEntryDlg->SetTypeText(pStationFunButtonWidget->pFunButtonGroup->button(pStationFunButtonWidget->nFunButtonPressState)->text()+"，请输入第2重密码");
            QRect rect=mPassWordEntryDlg->geometry();
            mPassWordEntryDlg->setGeometry(802,379,rect.width(),rect.height());
            mPassWordEntryDlg->show();
            //解绑
            QObject::disconnect(connfunbtn);
        });
    }

}
//辅助菜单选择结果槽
void StationCtrlDispKSK::MenuAuxiliaryChoseSlot(QString str)
{
    int a;
    a=0;
    if(str=="破封统计")
    {
        //pCurrStation->GetQJNameList();
//        mQFTJ = new QFTJ(this);
        //mQFTJ->hide();
        mQFTJ->InitShow(pCurrStation->getStationName(),pCurrStation->mQianFengVect);
        mQFTJ->show();

    }
    else if(str=="退出菜单")
    {
         //SetMenuStateInFCZK(menuFunOrder,"命令清除",false,false);
        if(mQFTJ!=nullptr)
        {
           mQFTJ->close();
        }
    }
    else if(str.contains("接触网"))
    {
        mFunButtonDevInfo.strButtonName=str;
    }
    else if(str.contains("股道无电"))
    {
        mFunButtonDevInfo.strButtonName=str;
    }
}
//状态选择按下槽
void StationCtrlDispKSK::StateChoseSlot()
{
    if(pCurrStation->GetFCZKMode()!=0x22)
    {
        mStateChose->AddData(pCurrStation->getStationName());
        mStateChose->SetCheckState(pCurrStation->nPlanCtrl,pCurrStation->nStateSelect);
    }
    mStateChose->show();
}
//方式转化按下槽
void StationCtrlDispKSK::ModeChangeSlot(QString str)
{
    QString strStaName=pCurrStation->getStationName();
    nCTCControlMode=pCurrStation->GetFCZKMode();
    if(str=="模式申请")
    {
        mModeChange->SetMode(strStaName,nCTCControlMode,0);
        mModeChange->show();
    }
    else if(str=="同意模式申请")
    {
        mModeChange->SetMode(strStaName,nCTCControlMode,1);
        mModeChange->show();
    }

    SetMenuStateInFCZK(menuFunOrder,"命令清除",false,false);
}
//模式转换申请按下
void StationCtrlDispKSK::ModeApplySlot()
{
    if(pCurrStation->StaConfigInfo.bChgModeNeedJSJ)
    {
        for(int i=0;i<pCurrStation->LampArray.size();i++)
        {
            if(pCurrStation->LampArray[i]->getDevType() == Dev_LAMP)
            {
                lamp *lamp_Temp = (lamp *)(pCurrStation->LampArray)[i];
                if("CTC_MODE_CENTER" == lamp_Temp->DEVName_DJGD[0])
                {
                    if(mModeChange->nCheckState == 0)
                    {
                        lamp_Temp->nModeApply = 0;
                    }
                    else
                    {
                        lamp_Temp->nModeApply = 0xFF;
                    }
                }
                if("CTC_MODE_STATION" == lamp_Temp->DEVName_DJGD[0])
                {
                    if(mModeChange->nCheckState == 1)
                    {
                        lamp_Temp->nModeApply = 1;
                    }
                    else
                    {
                        lamp_Temp->nModeApply = 0xFF;
                    }
                }
                if("CTC_MODE_NORMAL" == lamp_Temp->DEVName_DJGD[0])
                {
                    if(mModeChange->nCheckState == 2)
                    {
                        lamp_Temp->nModeApply = 2;
                    }
                    else
                    {
                        lamp_Temp->nModeApply = 0xFF;
                    }
                }
            }
        }
    }
}

//初始化模式转换申请按下
void StationCtrlDispKSK::InitModeApply()
{
    if(pCurrStation->GetFCZKMode() == 0x22)
    {
        if(pCurrStation->StaConfigInfo.bChgModeNeedJSJ)
        {
            for(int i=0;i<pCurrStation->LampArray.size();i++)
            {
                if(pCurrStation->LampArray[i]->getDevType() == Dev_LAMP)
                {
                    lamp *lamp_Temp = (lamp *)(pCurrStation->LampArray)[i];
                    if("CTC_MODE_CENTER" == lamp_Temp->DEVName_DJGD[0])
                    {
                        if(mModeChange->nCheckState == 0)
                        {
                            lamp_Temp->nModeApply = 0xFF;
                        }
                    }
                    if("CTC_MODE_STATION" == lamp_Temp->DEVName_DJGD[0])
                    {
                        if(mModeChange->nCheckState == 1)
                        {
                            lamp_Temp->nModeApply = 0xFF;
                        }
                    }
                    if("CTC_MODE_NORMAL" == lamp_Temp->DEVName_DJGD[0])
                    {
                        if(mModeChange->nCheckState == 2)
                        {
                            lamp_Temp->nModeApply = 0xFF;
                        }
                    }
                }
            }
        }
    }
}
void StationCtrlDispKSK::OrderButtonSlot(int ntype)
{
    if(ntype==0)//命令清除
    {
        //emit SendDataToSecerSignal(4);
        //emit SendTextAnalysisSignal(20, QCursor::pos());
        OperateOrderClean();
    }
    else if(ntype==1)//命令下发
    {
        //emit SendTextAnalysisSignal(98, QCursor::pos());
        StaOrderSendOperate();
        //OperateOrderClean();
    }
}
void StationCtrlDispKSK::PassWordResult(bool b)
{
    bPassWordResult=b;
}
void StationCtrlDispKSK::StateAndModeSendSolt(int mode,int type1,int type2)
{
    emit SendDataToSecerSignal(3,mode,type1,type2);
    ModeApplySlot();
}
void StationCtrlDispKSK::TextShowSolt(QString name,bool b)
{
    if(name=="进站信号机")
    {
        pCurrStation->SetXHJANShow(3,b);
    }
    else if(name=="出站信号机")
    {
        pCurrStation->SetXHJANShow(4,b);
    }
    else if(name=="调车信号机")
    {
        pCurrStation->SetXHJANShow(5,b);
    }
    else if(name=="区间信号机")
    {
        pCurrStation->SetXHJANShow(6,b);
    }
    else if(name=="站名")
    {
        pCurrStation->SetXHJANShow(7,b);
    }
    else if(name=="方向")
    {
        pCurrStation->SetXHJANShow(8,b);
    }
    else if(name=="进路窗外框")
    {
        pCurrStation->SetXHJANShow(9,b);
    }
    else if(name=="股道")
    {
        pCurrStation->SetXHJANShow(10,b);
    }
    else if(name=="无岔区段")
    {
        pCurrStation->SetXHJANShow(11,b);
    }
    else if(name=="区间轨道")
    {
        pCurrStation->SetXHJANShow(12,b);
    }
    else if(name=="道岔")
    {
        pCurrStation->SetXHJANShow(13,b);
    }
    else if(name=="道岔区段")
    {
        pCurrStation->SetXHJANShow(14,b);
    }
    else if(name=="挤岔报警灯")
    {
        pCurrStation->SetXHJANShow(15,b);
    }
    else if(name=="报警灯")
    {
        pCurrStation->SetXHJANShow(16,b);
    }
    else if(name=="半自动闭塞")
    {
        pCurrStation->SetXHJANShow(17,b);
    }
    else if(name=="一般文字")
    {
        pCurrStation->SetXHJANShow(18,b);
    }
    else if(name=="绝缘节")
    {
        bShow_Jyj=b;
    }
    else if(name=="按钮")
    {
        pCurrStation->SetXHJANShow(19,b);
    }
    else if(name=="计数器")
    {
        pCurrStation->SetXHJANShow(20,b);
    }
    else if(name=="定时器")
    {
        pCurrStation->SetXHJANShow(21,b);
    }
}

//列车进路办理车次输入结果
void StationCtrlDispKSK::TrainRouteResult(QString mtrainnum)
{
    mFunButtonDevInfo.strTrainNum=mtrainnum;
}
//文字标注
void StationCtrlDispKSK::SaveSolt(TextAnnotation* pTextAnnotation)
{
    if(mTextAnnotationWidget)
    {
        if(pTextAnnotation)
        {
            pTextAnnotation->isShow = true;
        }
        else
        {
            TextAnnotation* mTextAnnotation = new TextAnnotation(this);
            mTextAnnotation->Init(mTextAnnotationWidget->m_Rect, mTextAnnotationWidget->strAnnotation, pCurrStation->nDiploid);
            pCurrStation->vectTextAnnotation.append(mTextAnnotation);
        }
        mTextAnnotationWidget->close();
        mTextAnnotationWidget = nullptr;
    }

}

void StationCtrlDispKSK::timerEvent(QTimerEvent *event)//定时器
{
    if(event->timerId()==Timer_ID_500)
    {
        //接通光带
        nLightband_15++;
        nLightband_30++;
        if(nLightband_30>60)
        {
           BLightband_30=false;
           nLightband_30=0;
        }
        if(nLightband_15>30)
        {
           BLightband_15=false;
           nLightband_15=0;
        }
        SetDC_JTGD();
        //qDebug()<<"*-*-*-*-*"<<BLightband;
        if (nElapsed>1000)
        {
            nElapsed=0;
        }
        nElapsed++;
        if(pCurrStation!=nullptr)
        {

            if(pCurrStation->commToServer<1000)
            {
                pCurrStation->commToServer++;
            }
            else
            {
                pCurrStation->commToServer=30;
            }
            //if((pCurrStation->GetFCZKMode()==0x22)||(pCurrStation->m_nModeState == 0))
            //ModeApplySlot();
            if(pCurrStation->GetFCZKMode()==0x22)
            {
                pStationFunButtonWidget->SetFunButtonEn(false);
                pCurrStation->SetFCZKState(true);
            }
            else if(pCurrStation->m_nModeState == 0)//中心控制
            {
                pStationFunButtonWidget->SetFunButtonEn(false);
            }
            else
            {
                pStationFunButtonWidget->SetFunButtonEn(true);
                pCurrStation->SetFCZKState(false);
                if(pCurrStation->m_nModeState == 2)//车站调车
                {
                    pStationFunButtonWidget->bStationDCFlag=true;
                    pStationFunButtonWidget->SetFunButtonEn_id(CALLON_LOCK,false);
                }
                else
                {
                     pStationFunButtonWidget->bStationDCFlag=false;
                     pStationFunButtonWidget->SetFunButtonEn_id(CALLON_LOCK,true);
                }

            }
            if(pCurrStation->m_bModeChanged)
            {
                pStationFunButtonWidget->pFunButtonGroup->buttons()[0]->setChecked(true);
            }
        }
        if(pCurrStation->ButtonFlashDown)
        {
            if(pCurrStation->ANFlashFlag>30)
            {
                //pCurrStation->ResetState();
                pCurrStation->ANFlashFlag = 0;
                pCurrStation->ButtonFlashDown = false;
                emit SendDataToSecerSignal(4);
                OperateOrderClean();
            }
            else
            {
                pCurrStation->ANFlashFlag++;
            }
        }
        InitModeApply();
    }
    update();
}
bool StationCtrlDispKSK::eventFilter(QObject *obj,QEvent *event)
{
    if(event->type() == QEvent::Paint)
    {
        if(obj == ui->widget)
        {
            magicTime();
        }
    }

    if(event->type() == QEvent::MouseButtonPress)
    {
        if(obj->objectName() == "PassWordTittle")
        {
            curPassWordWidget = mPassWordEntryDlg;
        }
    }
    if(event->type() == QEvent::Enter)
    {

    }
    return StationCtrlDisp::eventFilter(obj, event);
}
//右键事件
void StationCtrlDispKSK::contextMenuEvent(QContextMenuEvent *event)
{

    //文字标注操作
    if(TextAnnotationOperate(pCurrStation))
    {
        event->accept();
        return StationCtrlDisp::contextMenuEvent(event);
    }

    if(RoutePreWndOperate(pCurrStation))
    {
        event->accept();
        return StationCtrlDisp::contextMenuEvent(event);
    }


//    if(pCurrStation->m_nModeState == 0)
//    {
//        SetMenuStaShow();
//        menuSTA->exec(QCursor::pos());
//        return StationCtrlDisp::contextMenuEvent(event);
//    }

    bool bmousepress = false;
    //非常站控模式下禁止操作
    if(pCurrStation->GetFCZKMode()==0x22)
    {
        SetMenuStateInFCZK(menuDC,"",false,false);
        SetMenuStateInFCZK(menuGD,"",false,false);
        SetMenuStateInFCZK(menuXH,"",false,false);

        SetMenuStateInFCZK(menuDC,"接触网定位无电",true,false);
        SetMenuStateInFCZK(menuDC,"接触网反位无电",true,false);
        SetMenuStateInFCZK(menuGD,"股道无电",true,false);
    }
    else
    {
        SetMenuStateInFCZK(menuDC,"",true,false);
        SetMenuStateInFCZK(menuGD,"",true,false);
        SetMenuStateInFCZK(menuXH,"",true,false);
    }

    if(((pStationFunButtonWidget->nFunButtonPressState>=0)&&(mFunButtonDevInfo.RecordDevNum>1))
        ||((pStationFunButtonWidget->nFunButtonPressState>=0)&&(mFunButtonDevInfo.nCode!=-1))
        ||((pStationFunButtonWidget->nFunButtonPressState>=0)&&(mFunButtonDevInfo.m_nQCYDZS==true)))
    {
        if(pCurrStation->m_nModeState == 0)
        {
            return StationCtrlDisp::contextMenuEvent(event);
        }
        SetMenuStateInFCZK(menuFunOrder,"命令下达",true,false);
        SetMenuStateInFCZK(menuFunOrder,"命令清除",true,false);

        QAction* aClick=menuFunOrder->exec(QCursor::pos());
        if(aClick!=nullptr)
        {
            if(aClick->text().contains("命令下达"))
            {
                //StaOrderSendOperate();
                emit SendTextAnalysisSignal(21, QCursor::pos());
                OrderButtonSlot(1);
                //pStationFunButtonWidget->pFunButtonGroup->button(ORDER_GIVE)->click();
                bmousepress=true;
            }
            else if(aClick->text().contains("清除"))
            {
                emit SendDataToSecerSignal(4);
                emit SendTextAnalysisSignal(20, QCursor::pos());
                //OperateOrderClean();
                OrderButtonSlot(0);
                //pStationFunButtonWidget->pFunButtonGroup->button(ORDER_CLEAR)->click();
                bmousepress=true;
            }
        }
        event->accept();
        return StationCtrlDisp::contextMenuEvent(event);
    }
    else if(((pStationFunButtonWidget->nFunButtonPressState==JL_BUILD)&&(mFunButtonDevInfo.RecordDevNum==1))||(pStationFunButtonWidget->nFunButtonPressState>1))
    {
        if(pCurrStation->m_nModeState == 0)
        {
            return StationCtrlDisp::contextMenuEvent(event);
        }
        pStationFunButtonWidget->SetFunButtonEn(20,true);
        SetMenuStateInFCZK(menuFunOrder,"命令下达",false,false);
        SetMenuStateInFCZK(menuFunOrder,"命令清除",true,false);

        QAction* aClick = menuFunOrder->exec(QCursor::pos());
        bmousepress=false;
        if(aClick!=nullptr)
        {
            if(aClick->text().contains("清除"))
            {
                emit SendDataToSecerSignal(4);
                //OperateOrderClean();
                emit SendTextAnalysisSignal(20, QCursor::pos());
                OrderButtonSlot(0);

                //pStationFunButtonWidget->pFunButtonGroup->button(ORDER_CLEAR)->click();
                bmousepress=true;
            }
        }
        event->accept();
        return StationCtrlDisp::contextMenuEvent(event);
    }
    else if(pCurrStation && !bmousepress)
    {

        //车次操作
        if(CHECIOperate(pCurrStation) && pCurrStation->m_nModeState != 0)
        {
            event->accept();
            return StationCtrlDisp::contextMenuEvent(event);
        }
        //站场操作
        else if(StationOperate(pCurrStation) /*&& pCurrStation->m_nModeState != 0*/)
        {
            event->accept();
            return StationCtrlDisp::contextMenuEvent(event);
        }
    }


    return StationCtrlDisp::contextMenuEvent(event);
}
//鼠标按下
void StationCtrlDispKSK::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton) //鼠标左键按下处理
    {
        QPoint coursePoint= QCursor::pos();
        mcoursePoint=ui->widget->mapFromGlobal(coursePoint);
        if(bTextAnnotation) //新增文字标注
        {
            if(mTextAnnotationWidget)
            {
                mTextAnnotationWidget->close();
                mTextAnnotationWidget = nullptr;
            }

            mTextAnnotationWidget = new TextAnnotationWidget(this);
            mTextAnnotationWidget->setObjectName("TextAnnotationWidget");
            //文字标注 mTextAnnotationWidget
            connect(mTextAnnotationWidget, SIGNAL(SaveSignal(TextAnnotation*)), this, SLOT(SaveSolt(TextAnnotation*)));
            mTextAnnotationWidget->setGeometry(mcoursePoint.x() - ui->scrollArea->horizontalScrollBar()->sliderPosition(),
                                               mcoursePoint.y() - ui->scrollArea->verticalScrollBar()->sliderPosition(), 80, 24);
            mTextAnnotationWidget->SetRect(QRectF(mcoursePoint.x()/* + ui->scrollArea->horizontalScrollBar()->sliderPosition()*/,
                                                  mcoursePoint.y()/* + ui->scrollArea->verticalScrollBar()->sliderPosition()*/, 80 ,24));
            mTextAnnotationWidget->show();
            mTextAnnotationWidget->setEditFocus();
            bTextAnnotation=false;
            return StationCtrlDisp::mousePressEvent(event);
        }

        curMousePoint = ui->widget->mapFromGlobal(coursePoint);
        TextAnnotationcheck = TextAnnotationCheck(pCurrStation);
        if(TextAnnotationcheck)
        {
            TextAnnotationPoint.setX(TextAnnotationcheck->m_Rect.x());
            TextAnnotationPoint.setY(TextAnnotationcheck->m_Rect.y());
            return StationCtrlDisp::mousePressEvent(event);
        }
        //if(pCurrStation->m_nModeState == 0)
        //{
        //    return StationCtrlDisp::mousePressEvent(event);
        //}
        if(!mPassWordEntryDlg->isHidden())
        {
            if(curPassWordWidget)
            {
                curMousePoint=ui->widget->mapFromGlobal(coursePoint);
                curPoint = curPassWordWidget->pos();
            }
            return StationCtrlDisp::mousePressEvent(event);
        }

        //功能按钮被按下
        if(pStationFunButtonWidget->nFunButtonPressState>=0)
        {
            GetDevInfoByMousePoint(&mFunButtonDevInfo);
            return StationCtrlDisp::mousePressEvent(event);
        }

    }
    else if(event->button() == Qt::RightButton)//鼠标右键按下处理
    {
        this->setCursor(Qt::ArrowCursor);
        mOldCursor = cursor();
        mcoursePoint=ui->widget->mapFromGlobal(QCursor::pos());
        return StationCtrlDisp::mousePressEvent(event);
    }

    return StationCtrlDisp::mousePressEvent(event);
}

void StationCtrlDispKSK::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        //取消拖动
        if(TextAnnotationcheck)
        {
            TextAnnotationcheck = nullptr;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        TextAnnotationcheck = nullptr;
    }
    return StationCtrlDisp::mouseReleaseEvent(event);
}
void StationCtrlDispKSK::mouseMoveEvent(QMouseEvent *event)
{
    if(!mPassWordEntryDlg->isHidden())
    {
      //  pStationFunButtonWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 屏蔽 鼠标事件
        if(curPassWordWidget)
        {
            QPoint pos = event->pos() - curMousePoint + curPoint;
            if(pos.x() < 0)
                pos.setX(0);
            if(pos.y() < 0)
                pos.setY(0);
            curPassWordWidget->move(pos);
            StationCtrlDisp::mouseMoveEvent(event);
        }
        StationCtrlDisp::mouseMoveEvent(event);
    }

    //文字标注拖动
    if(TextAnnotationcheck)
    {
        int ptX = event->pos().x() - curMousePoint.x();
        int ptY = event->pos().y() - curMousePoint.y();
        int w = TextAnnotationcheck->m_Rect.width();
        int h = TextAnnotationcheck->m_Rect.height();
        TextAnnotationcheck->m_Rect.setX(TextAnnotationPoint.x() + ptX + ui->scrollArea->horizontalScrollBar()->sliderPosition());
        TextAnnotationcheck->m_Rect.setY(TextAnnotationPoint.y() + ptY + ui->scrollArea->verticalScrollBar()->sliderPosition());
        TextAnnotationcheck->m_Rect.setWidth(w);
        TextAnnotationcheck->m_Rect.setHeight(h);

        TextAnnotationcheck->m_Rectcont.setX(TextAnnotationcheck->m_Rect.x() / TextAnnotationcheck->m_nDiploid);
        TextAnnotationcheck->m_Rectcont.setY(TextAnnotationcheck->m_Rect.y() / TextAnnotationcheck->m_nDiploid);
        TextAnnotationcheck->m_Rectcont.setWidth(w / TextAnnotationcheck->m_nDiploid);
        TextAnnotationcheck->m_Rectcont.setHeight(h / TextAnnotationcheck->m_nDiploid);
    }

    mcoursePoint = ui->widget->mapFromGlobal(QCursor::pos());

    //非常站控模式下禁止操作
//    if(pCurrStation->GetFCZKMode()==0x22)
//        StationCtrlDisp::mouseMoveEvent(event);
    if(pCurrStation)
    {
        TrainNumWnd *pTrainNumWnd=nullptr;
        CGDDC *pCGDDC=nullptr;
        CXHD *pXHD=nullptr;
        CGD *pGD=nullptr;
        lamp *pLamp=nullptr;
        CZDBS *pZDBS=nullptr;
        CBZDBS *pBZDBS=nullptr;
        CJZ *pJZ=nullptr;
        FUNCBUTTON *pFUNCB=nullptr;
        CDcBtn *pDcBtn=nullptr;
        AlonXHBtn *alonXHBtn=nullptr;
        if(pTrainNumWndOld!=nullptr)
        {
            pTrainNumWndOld->m_bVisibleRange=false;
        }
        for(int i=0; i<pCurrStation->m_ArrayTrainNumWnd.size(); i++)
        {
            pTrainNumWnd = pCurrStation->m_ArrayTrainNumWnd[i];
            if(pTrainNumWnd->m_RectWnd.contains(mcoursePoint)/*&&(pTrainNumWnd->m_bShow||pTrainNumWnd->m_strCheCi!="")*/)
            {
                if(pTrainNumWnd->m_strCheCi=="")
                {
                    pTrainNumWnd->m_bVisibleRange=true;
                    pTrainNumWndOld=pTrainNumWnd;
                }
                else
                {
                    pTrainNumWnd->m_bVisibleRange=false;
                }
                InitCursor();
                // setCursor(Qt::PointingHandCursor);
                StationCtrlDisp::mouseMoveEvent(event);
                break;
            }
            else
            {
                pTrainNumWnd->m_bVisibleRange=false;
                ResetCursor();
                StationCtrlDisp::mouseMoveEvent(event);
            }
        }
        //pCurrStation->SetDevMouseMoveShow(mcoursePoint,mCBaseDataOld);
        for(int j=0;j<pCurrStation->DevArray.size();j++)
        {
            if(pCurrStation->DevArray[j]->getDevType() == Dev_XH)
            {
                pXHD=(CXHD*)(pCurrStation->DevArray[j]);

                if(pXHD->GetMousePoint_XHD(mcoursePoint))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];
                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_ON)
                    {
                        pXHD->m_RangeVisible_XHD_DDMD = true;
                        setCursor(Qt::PointingHandCursor);
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_OFF)
                    {
                        pXHD->m_RangeVisible_XHD_DDMD = true;
                        setCursor(Qt::PointingHandCursor);
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FZ_LIST)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_CLEAR)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_GIVE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == STATE_CHOOSE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == MODLE_CHANGE)
                    {
                        pXHD->m_RangeVisible_XHD = true;
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pXHD->m_RangeVisible_XHD = false;
                    pXHD->m_RangeVisible_XHD_DDMD=false;
                }
                if((pXHD->GetMousePoint_DCAN(mcoursePoint))&&(pCurrStation->GetFCZKMode()!=0x22))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isDCANFlash)
                                {
                                    pXHD->m_RangeVisible_DCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isDCANFlash)
                                {
                                    pXHD->m_RangeVisible_DCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isDCANFlash)
                                {
                                    pXHD->m_RangeVisible_DCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isDCANFlash)
                                {
                                    pXHD->m_RangeVisible_DCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isDCANFlash)
                                {
                                    pXHD->m_RangeVisible_DCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        else
                        {
                            pXHD->m_RangeVisible_DCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        if(pXHD->m_DCXHDButtonShowFlag)
                        {
                            if(pXHD->getSignalType()==true)
                            {
                                if(mFunButtonDevInfo.nCode!=-1)
                                {
                                    if(pXHD->isLCANFlash)
                                    {
                                        pXHD->m_RangeVisible_DCAN = true;
                                        setCursor(Qt::PointingHandCursor);
                                    }
                                }
                            }
                            else
                            {
                                pXHD->m_RangeVisible_DCAN = true;
                                setCursor(Qt::PointingHandCursor);
                            }
                        }
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pXHD->m_RangeVisible_DCAN = false;
                }
                if((pXHD->GetMousePoint_LCAN(mcoursePoint))&&(pCurrStation->GetFCZKMode()!=0x22))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isLCANFlash)
                                {
                                    pXHD->m_RangeVisible_LCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isLCANFlash)
                                {
                                    pXHD->m_RangeVisible_LCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isLCANFlash)
                                {
                                    pXHD->m_RangeVisible_LCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isLCANFlash)
                                {
                                    pXHD->m_RangeVisible_LCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            if(mFunButtonDevInfo.nCode!=-1)
                            {
                                if(pXHD->isLCANFlash)
                                {
                                    pXHD->m_RangeVisible_LCAN = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        if(pXHD->getSignalType()==true)
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        else
                        {
                            pXHD->m_RangeVisible_LCAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        if(pXHD->m_LCXHDButtonShowFlag)
                        {
                            if(pXHD->getSignalType()==true)
                            {
                                if(mFunButtonDevInfo.nCode!=-1)
                                {
                                    if(pXHD->isLCANFlash)
                                    {
                                        pXHD->m_RangeVisible_LCAN = true;
                                        setCursor(Qt::PointingHandCursor);
                                    }
                                }
                            }
                            else
                            {
                                pXHD->m_RangeVisible_LCAN = true;
                                setCursor(Qt::PointingHandCursor);
                            }
                        }
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pXHD->m_RangeVisible_LCAN = false;
                }
                if((pXHD->GetMousePoint_TGAN(mcoursePoint))&&(pCurrStation->GetFCZKMode()!=0x22))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        if(mFunButtonDevInfo.nCode == -1)
                        {
                            pXHD->m_RangeVisible_TGAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        if(mFunButtonDevInfo.nCode == -1)
                        {
                            pXHD->m_RangeVisible_TGAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        if(mFunButtonDevInfo.nCode == -1)
                        {
                            pXHD->m_RangeVisible_TGAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        if(mFunButtonDevInfo.nCode == -1)
                        {
                            pXHD->m_RangeVisible_TGAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        if(mFunButtonDevInfo.nCode == -1)
                        {
                            pXHD->m_RangeVisible_TGAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pXHD->m_RangeVisible_TGAN = false;
                }
                if(pXHD->GetMousePoint_Name(mcoursePoint))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_ON)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_OFF)
                    {
                        pXHD->m_RangeVisible_Name = true;
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pXHD->m_RangeVisible_Name = false;
                }
                if((pXHD->GetMousePoint_YDAN(mcoursePoint))&&(pCurrStation->GetFCZKMode()!=0x22))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
                    {
                        if(mFunButtonDevInfo.nCode == -1)
                        {
                            pXHD->m_RangeVisible_XHD_YDAN = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pXHD->m_RangeVisible_XHD_YDAN = false;
                }
            }
            else if(pCurrStation->DevArray[j]->getDevType() == Dev_DC)
            {
                pCGDDC=(CGDDC*)(pCurrStation->DevArray[j]);
                if(pCGDDC->GetMousePoint(mcoursePoint))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            if(pCGDDC->getIsDS())
                            {
                                setCursor(Qt::PointingHandCursor);
                            }
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
                    {
                        pCGDDC->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            if((pCGDDC->flblStatusDW)||(pCGDDC->flblStatusFW)||(pCGDDC->flblStatusCQ))
                            {
                                setCursor(Qt::PointingHandCursor);
                            }
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_ON)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_OFF)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FZ_LIST)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_CLEAR)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_GIVE)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == STATE_CHOOSE)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == MODLE_CHANGE)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCDW)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCFW)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_JG)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_XG)
                    {
                        pCGDDC->m_RangeVisible = true;
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pCGDDC->m_RangeVisible = false;
                }
            }
            else  if(pCurrStation->DevArray[j]->getDevType() == Dev_GD)
            {
                pGD=(CGD*)(pCurrStation->DevArray[j]);
                if(pGD->GetMousePoint(mcoursePoint))
                {
                    SetButtonLightFalse(mCBaseDataOld);
                    mCBaseDataOld=pCurrStation->DevArray[j];

                    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALL_CANCEL)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SIGNAL_RESTART)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ALLMAN_RELEASE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
                    {
                        pGD->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
                    {
                        pGD->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
                    {
                        pGD->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == RAMP_RELEASE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
                    {
                        pGD->m_RangeVisible = true;
                        if(mFunButtonDevInfo.nCode==-1)
                        {
                            setCursor(Qt::PointingHandCursor);
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
                    {
                        pGD->m_RangeVisible = true;
                        if(pGD->isGDFLBL)
                        {
                            if(mFunButtonDevInfo.nCode==-1)
                            {
                                setCursor(Qt::PointingHandCursor);
                            }
                        }
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_ON)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == LIGHT_OFF)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == FZ_LIST)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_GD)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_CLEAR)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == ORDER_GIVE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == STATE_CHOOSE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    else if(pStationFunButtonWidget->nFunButtonPressState == MODLE_CHANGE)
                    {
                        pGD->m_RangeVisible = true;
                    }
                    StationCtrlDisp::mouseMoveEvent(event);
                }
                else
                {
                    pGD->m_RangeVisible = false;
                }
            }
        }

        CGDYC* pGDYC = nullptr;
        for(int i = 0; i < pCurrStation->vectGDYCArray.size(); i++)
        {
            pGDYC = pCurrStation->vectGDYCArray.at(i);
            if(pGDYC->GetMousePoint(mcoursePoint))
            {
                if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
                {
                    setCursor(Qt::PointingHandCursor);
                }
            }
        }

        if(pCurrStation->GetFCZKMode()!=0x22)
        {
            //自动闭塞
            for(int i=0;i<pCurrStation->ZDBSArray.size();i++)
            {
                if(pCurrStation->ZDBSArray[i]->getDevType() == Dev_ZDBS)
                {
                    pZDBS = (CZDBS *)pCurrStation->ZDBSArray[i];
                    if(pZDBS->GetMousePoint_ZFZ(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->ZDBSArray[i];
                            pZDBS->m_RangeVisible_ZDBS_ZFZ = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pZDBS->m_RangeVisible_ZDBS_ZFZ = false;
                    }
                    if(pZDBS->GetMousePoint_JCFZ(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->ZDBSArray[i];
                            pZDBS->m_RangeVisible_ZDBS_JCFZ = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pZDBS->m_RangeVisible_ZDBS_JCFZ = false;
                    }
                    if(pZDBS->GetMousePoint_FCFZ(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->ZDBSArray[i];
                            pZDBS->m_RangeVisible_ZDBS_FCFZ = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pZDBS->m_RangeVisible_ZDBS_FCFZ = false;
                    }
                    if(pZDBS->GetMousePoint_GF(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->ZDBSArray[i];
                            pZDBS->m_RangeVisible_ZDBS_GF = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pZDBS->m_RangeVisible_ZDBS_GF = false;
                    }
                }
            }
            //半自动闭塞
            for(int i=0;i<pCurrStation->BZDBSArray.size();i++)
            {
                if(pCurrStation->BZDBSArray[i]->getDevType() == Dev_BZDBS)
                {
                    pBZDBS = (CBZDBS *)pCurrStation->BZDBSArray[i];
                    if(pBZDBS->GetMousePoint_BSQH(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->BZDBSArray[i];
                            pBZDBS->m_RangeVisible_BZDBS_BSQH = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pBZDBS->m_RangeVisible_BZDBS_BSQH = false;
                    }
                    if(pBZDBS->GetMousePoint_BS(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->BZDBSArray[i];
                            pBZDBS->m_RangeVisible_BZDBS_BS = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pBZDBS->m_RangeVisible_BZDBS_BS = false;
                    }
                    if(pBZDBS->GetMousePoint_SG(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->BZDBSArray[i];
                            pBZDBS->m_RangeVisible_BZDBS_SG = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pBZDBS->m_RangeVisible_BZDBS_SG = false;
                    }
                    if(pBZDBS->GetMousePoint_FY(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->BZDBSArray[i];
                            pBZDBS->m_RangeVisible_BZDBS_FY = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pBZDBS->m_RangeVisible_BZDBS_FY = false;
                    }
                }
            }
            //计轴
            for(int i=0;i<pCurrStation->JZArray.size();i++)
            {
                if(pCurrStation->JZArray[i]->getDevType() == Dev_JZ)
                {
                    pJZ = (CJZ *)pCurrStation->JZArray[i];
                    if(pJZ->GetMousePoint_BSQH(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->JZArray[i];
                            pJZ->m_RangeVisible_JZ_BSQH = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pJZ->m_RangeVisible_JZ_BSQH = false;
                    }
                    if(pJZ->GetMousePoint_JZSY(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->JZArray[i];
                            pJZ->m_RangeVisible_JZ_JZSY = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pJZ->m_RangeVisible_JZ_JZSY = false;
                    }
                    if(pJZ->GetMousePoint_JZTY(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->JZArray[i];
                            pJZ->m_RangeVisible_JZ_JZTY = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pJZ->m_RangeVisible_JZ_JZTY = false;
                    }
                    if(pJZ->GetMousePoint_JZFL(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->JZArray[i];
                            pJZ->m_RangeVisible_JZ_JZFL = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pJZ->m_RangeVisible_JZ_JZFL = false;
                    }
                }
            }
            //功能按钮
            for(int i=0;i<pCurrStation->FuncBtnArray.size();i++)
            {
                if(pCurrStation->FuncBtnArray[i]->getDevType() == Dev_FUNCB)
                {
                    pFUNCB = (FUNCBUTTON *)pCurrStation->FuncBtnArray[i];
                    int ntype = pFUNCB->GetMousePoint(mcoursePoint);
    //                if(!((ntype == 3)||(ntype == 4)||(ntype == 5)))
                    if(pFUNCB->GetMousePoint(mcoursePoint))
                    {
                        if((pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)||(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN))
                        {
                            if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
                            {
                                if((ntype == 3)||(ntype == 4)||(ntype == 5))
                                {
                                    SetButtonLightFalse(mCBaseDataOld);
                                    mCBaseDataOld=pCurrStation->FuncBtnArray[i];
                                    pFUNCB->m_RangeVisible = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
                            else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                            {
                                if(!((ntype == 3)||(ntype == 4)||(ntype == 5)))
                                {
                                    SetButtonLightFalse(mCBaseDataOld);
                                    mCBaseDataOld=pCurrStation->FuncBtnArray[i];
                                    pFUNCB->m_RangeVisible = true;
                                    setCursor(Qt::PointingHandCursor);
                                }
                            }
    //                        else
    //                        {
    //                            SetButtonLightFalse(mCBaseDataOld);
    //                            mCBaseDataOld=pCurrStation->FuncBtnArray[i];
    //                            pFUNCB->m_RangeVisible = true;
    //                            setCursor(Qt::PointingHandCursor);
    //                        }
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pFUNCB->m_RangeVisible = false;
                    }
                }
            }
            //道岔故障按钮
            for(int i=0;i<pCurrStation->DCBtnArray.size();i++)
            {
                if(pCurrStation->DCBtnArray[i]->getDevType() == Dev_DCBTN)
                {
                    pDcBtn = (CDcBtn *)pCurrStation->DCBtnArray[i];
                    if(pDcBtn->GetMousePoint(mcoursePoint))
                    {
                        if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
                        {
                            SetButtonLightFalse(mCBaseDataOld);
                            mCBaseDataOld=pCurrStation->DCBtnArray[i];
                            pDcBtn->m_RangeVisible = true;
                            setCursor(Qt::PointingHandCursor);
                        }
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pDcBtn->m_RangeVisible = false;
                    }
                }
            }
            //表示灯
            for(int i=0;i<pCurrStation->LampArray.size();i++)
            {
                if(pCurrStation->LampArray[i]->getDevType() == Dev_LAMP)
                {
                    pLamp = (lamp *)pCurrStation->LampArray[i];
                    if(pLamp->GetMousePoint(mcoursePoint))
                    {
                        SetButtonLightFalse(mCBaseDataOld);
                        mCBaseDataOld=pCurrStation->LampArray[i];
                        pLamp->m_RangeVisible = true;
                        //setCursor(Qt::PointingHandCursor);
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        pLamp->m_RangeVisible = false;
                    }
                }
            }
            //独立按钮
            for(int i=0;i<pCurrStation->vectXhBtn.size();i++)
            {
                if(pCurrStation->vectXhBtn[i]->getDevType() == Dev_DLAN)
                {
                    alonXHBtn = (AlonXHBtn *)pCurrStation->vectXhBtn[i];
                    if(alonXHBtn->GetMousePoint(mcoursePoint))
                    {
                        SetButtonLightFalse(mCBaseDataOld);
                        mCBaseDataOld=pCurrStation->vectXhBtn[i];
                        alonXHBtn->m_RangeVisible = true;
                        //setCursor(Qt::PointingHandCursor);
                        StationCtrlDisp::mouseMoveEvent(event);
                    }
                    else
                    {
                        alonXHBtn->m_RangeVisible = false;
                    }
                }
            }
        }

        for(int i = 0; i < pCurrStation->vectRoutePreWnd.size(); i++)
        {
            if(pCurrStation->vectRoutePreWnd.at(i)->rcLogicLemp.contains(mcoursePoint))
                pCurrStation->vectRoutePreWnd.at(i)->m_bShowLogicLempText = true;
            else pCurrStation->vectRoutePreWnd.at(i)->m_bShowLogicLempText = false;
        }
    }
    return StationCtrlDisp::mouseMoveEvent(event);
}

void StationCtrlDispKSK::keyReleaseEvent(QKeyEvent *event)
{
    if(event->text() == '\r' || event->text() == '\n')
    {
        if(mTextAnnotationWidget)
        {
            mTextAnnotationWidget->EditEnter();
        }
    }
}

void StationCtrlDispKSK::SetDC_JTGD()
{
    if(pCurrStation->m_nModeState == 0)
    {
        return;
    }
    bool BLightband;
    if(BLightband_15||BLightband_30)
    {
        BLightband = true;
    }
    else
    {
        BLightband = false;
    }
    CGDDC *pCGDDC=nullptr;
    for(int j=0;j<pCurrStation->DevArray.size();j++)
    {
        if(pCurrStation->DevArray[j]->getDevType() == Dev_DC)
        {
            pCGDDC=(CGDDC*)(pCurrStation->DevArray[j]);
            pCGDDC->BLightband = BLightband;
        }
    }
}
//命令下达 菜单 功能按钮
void StationCtrlDispKSK::OperateOrderSend()
{

}
//清除指令 菜单 功能按钮
void StationCtrlDispKSK::OperateOrderClean()
{
    mFunButtonDevInfo.Init();
    pCurrStation->ResetState();
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,false);
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_GIVE,false);
    pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->setChecked(true);
    if(pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->isEnabled()==true)
    {
        if(pStationFunButtonWidget->nFunButtonPressState!=JL_BUILD)
        {
            //pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->click();
            pStationFunButtonWidget->nFunButtonPressState = JL_BUILD;
            OrderButtonSlot(0);
        }
    }
    else
    {
        pStationFunButtonWidget->nFunButtonPressState=JL_BUILD;
    }
}
bool StationCtrlDispKSK::GetOrderRightMenuShow(bool bleft)
{
    if(pCurrStation->m_nModeState == 0)
    {
        return false;
    }
    bool b=false;
    int nnn=pStationFunButtonWidget->nFunButtonPressState;
    int sss=mFunButtonDevInfo.nCode;
    int asd=mFunButtonDevInfo.nDevType;
    if(bleft&&(pStationFunButtonWidget->nFunButtonPressState==0
               &&mFunButtonDevInfo.nCode!=-1&&mFunButtonDevInfo.nDevType==Dev_XH))
    {
       b=true;
    }
    else if(pStationFunButtonWidget->nFunButtonPressState==0
            &&mFunButtonDevInfo.nCode!=-1&&mFunButtonDevInfo.nCode2!=-1
            &&mFunButtonDevInfo.nDevType==Dev_XH)
    {
        b=true;
    }
    else if(pStationFunButtonWidget->nFunButtonPressState==1&&(mFunButtonDevInfo.nDevType==Dev_XH)&&mFunButtonDevInfo.nCode!=-1)
    {
        b=true;
    }
    else if((pStationFunButtonWidget->nFunButtonPressState==1||pStationFunButtonWidget->nFunButtonPressState==2
             ||pStationFunButtonWidget->nFunButtonPressState==3||pStationFunButtonWidget->nFunButtonPressState==5
             ||pStationFunButtonWidget->nFunButtonPressState==14||pStationFunButtonWidget->nFunButtonPressState==17
             ||pStationFunButtonWidget->nFunButtonPressState==18)
            &&(mFunButtonDevInfo.nDevType==Dev_XH)
            &&mFunButtonDevInfo.nCode!=-1)
    {
        b=true;
    }
    else if((/*pStationFunButtonWidget->nFunButtonPressState==4||*/pStationFunButtonWidget->nFunButtonPressState==7
             ||pStationFunButtonWidget->nFunButtonPressState==8||pStationFunButtonWidget->nFunButtonPressState==9
             ||pStationFunButtonWidget->nFunButtonPressState==10)
            &&(mFunButtonDevInfo.nDevType==Dev_DC)
            &&mFunButtonDevInfo.nCode!=-1)
    {
        b=true;
    }
    else if((pStationFunButtonWidget->nFunButtonPressState==6||pStationFunButtonWidget->nFunButtonPressState==15
             ||pStationFunButtonWidget->nFunButtonPressState==16)
            &&(mFunButtonDevInfo.nDevType==Dev_DC||mFunButtonDevInfo.nDevType==Dev_GD)
            &&mFunButtonDevInfo.nCode!=-1)
    {
        b=true;
    }
    else if((pStationFunButtonWidget->nFunButtonPressState==11||pStationFunButtonWidget->nFunButtonPressState==12)
            &&(mFunButtonDevInfo.nDevType==Dev_GD||mFunButtonDevInfo.nDevType==Dev_DC||mFunButtonDevInfo.nDevType==Dev_XH)
            &&mFunButtonDevInfo.nCode!=-1)
    {
        b=true;
    }
    //功能按钮
    else if((pStationFunButtonWidget->nFunButtonPressState==4||pStationFunButtonWidget->nFunButtonPressState==13
             ||pStationFunButtonWidget->nFunButtonPressState==19)
            /*&&(mFunButtonDevInfo.nDevType==Dev_DC&&mFunButtonDevInfo.nDevType==Dev_XH)*/
            &&mFunButtonDevInfo.nCode!=-1)
    {
        b=true;
    }
    else
    {
        b=false;
    }
//    if(b)
//    {
//        pStationFunButtonWidget->SetFunButtonEn(21,true);
//    }
//    else
//    {
//        pStationFunButtonWidget->SetFunButtonEn(21,false);
//    }
    return b;
}
int StationCtrlDispKSK::SetRouteCode(int ncode,int nFunType)
{
    int n=0;
    int code=0;
    if(nFunType==0x01)//列车按钮
        n=0x0000;
    else if(nFunType==0x02)//调车按钮
        n=0x4000;
    else if(nFunType==0x03)//通过按钮
        n=0x8000;
    code=ncode|n;
    return code;
}
void StationCtrlDispKSK::StaOrderSendOperate()//命令下达
{
    if(pCurrStation->m_nModeState == 0 && pStationFunButtonWidget->nFunButtonPressState != BAD_SHUNT_KX)
    {
        return;
    }
    QString strroutename="";

    //信号机按钮控制
    if(pStationFunButtonWidget->nFunButtonPressState == JL_BUILD)
    {
        if((((mFunButtonDevInfo.nDevCode[0]&0xC000)>>14)==0x02)||(((mFunButtonDevInfo.nDevCode[0]&0xC000)>>14)==0x00))//列车进路//如果是通过进路(第一个按钮为通过按钮)
        {
            QStringList strGDList;
            for(int i=0;i<8;i++)
            {
                if(mFunButtonDevInfo.nDevCode[i]!=-1)
                {
                    strroutename += pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xFF);
                }
                if((mFunButtonDevInfo.nDevCode[i+1]!=-1)&&(i<7))
                {
                    strroutename += "-";
                }
                CXHD* pXHD = pCurrStation->GetXHDByCode(mFunButtonDevInfo.nDevCode[i]);
                if(pXHD && (pXHD->getXHDType() == CZ_XHJ
                        || pXHD->getXHDType() == JZ_XHJ))
                {
                    strGDList << pCurrStation->getGDQDListByXHD(pXHD);
                }
            }

            QString strTrainNum="";
            for(int i = 0; i < pCurrStation->m_ArrayTrain.size(); i++)
            {
                for(int j = 0; j < strGDList.size(); j++)
                {
                    if(pCurrStation->m_ArrayTrain.at(i)->m_nPosCode == pCurrStation->GetCodeByStrName(strGDList.at(j))
                        || pCurrStation->m_ArrayTrain.at(i)->m_nPosCodeReal == pCurrStation->GetCodeByStrName(strGDList.at(j)))
                    {
                        strTrainNum = pCurrStation->m_ArrayTrain.at(i)->m_strCheCi;
                    }
                }
            }

            mTrainRouteHandle->SetTitle(" 列车进路办理");
            mTrainRouteHandle->strDescrib = QString("请为进路%1输入车次号:").arg(strroutename);
            mTrainRouteHandle->InitPassWord();
            mTrainRouteHandle->setOldTrainNum(strTrainNum);
            mTrainRouteHandle->exec();

            if(mTrainRouteHandle->onOK==true)
            {
                bool isSameNum = mTrainRouteHandle->isTrainNumChange(strTrainNum);
                if(!isSameNum)
                {
                    int ret = QMessageBox::information(this, tr("提示"), "输入的车次号与当前车次不符,是否强制执行?", tr("确定"), tr("取消"), "", 0);
                    if(ret == 0)
                    {
                        GetFunButtonType(&mFunButtonDevInfo,mTrainRouteHandle->GetTrainRouteNum(),0,0);
                        OperateOrderClean();
                    }
                }
                else
                {
                    GetFunButtonType(&mFunButtonDevInfo,mTrainRouteHandle->GetTrainRouteNum(),0,0);
                    OperateOrderClean();
                }
            }
            else
            {
                emit SendDataToSecerSignal(4);
                OperateOrderClean();
            }
        }
        else if(((mFunButtonDevInfo.nDevCode[0]&0xC000)>>14)==0x01)//调车进路
        {
            mTrainRouteHandle->SetTitle(" 调车进路办理");
            for(int i=0;i<8;i++)
            {
                if(mFunButtonDevInfo.nDevCode[i]!=-1)
                {
                    if(i==0)
                    {
                        strroutename=pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xff);
                    }
                    else
                    {
                       strroutename=strroutename+"-"+ pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xff);
                    }
                }
            }
//            mDCRouteTimeWidget->Init(strroutename);
//            QRect rect=mDCRouteTimeWidget->geometry();
//            mDCRouteTimeWidget->setGeometry(802,379,rect.width(),rect.height());
//            mDCRouteTimeWidget->exec();
//            if(mDCRouteTimeWidget->onOK==true)
//            {
//                GetFunButtonType(&mFunButtonDevInfo,mDCRouteTimeWidget->GetTimeAndType(),0,0);
//                OperateOrderClean();
//            }
//            else
//            {
//                emit SendDataToSecerSignal(4);
//                OperateOrderClean();
//            }
            if(pCurrStation->StaConfigInfo.DlgDCTimeShow)
            {
                mDCRouteTimeWidget->Init(strroutename);
                QRect rect=mDCRouteTimeWidget->geometry();
                mDCRouteTimeWidget->setGeometry(802,379,rect.width(),rect.height());
                mDCRouteTimeWidget->exec();
                if(mDCRouteTimeWidget->onOK==true)
                {
                    emit SendTextAnalysisSignal(77, QCursor::pos());
                    GetFunButtonType(&mFunButtonDevInfo,mDCRouteTimeWidget->GetTimeAndType(),0,0);
                    OperateOrderClean();
                }
                else
                {
                    emit SendTextAnalysisSignal(78, QCursor::pos());
                    emit SendDataToSecerSignal(4);
                    OperateOrderClean();
                }
            }
            else
            {
                GetFunButtonType(&mFunButtonDevInfo,mDCRouteTimeWidget->GetTimeAndType(),0,0);
                OperateOrderClean();
            }
        }
        else if(mFunButtonDevInfo.strButtonName =="股道确认" || mFunButtonDevInfo.strButtonName =="腰岔解锁")
        {
            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
            OperateOrderClean();
        }

    }
    else if((pStationFunButtonWidget->nFunButtonPressState==ALL_CANCEL)||(pStationFunButtonWidget->nFunButtonPressState==SIGNAL_RESTART)
            ||(pStationFunButtonWidget->nFunButtonPressState==ALLMAN_RELEASE))
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_BTN)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if((pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)||(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION))
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState==RAMP_RELEASE)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if((pStationFunButtonWidget->nFunButtonPressState==LIGHT_ON)||(pStationFunButtonWidget->nFunButtonPressState==LIGHT_OFF))
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }

    //道岔控制
    else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_NORMAL)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_REVERSE)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_LOCK)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == SWITCH_RELEASE)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
    {
        if(mFunButtonDevInfo.nDevType==Dev_DC)
        {
            if(nPoorShuntType==0x30)
            {
                pStationFunButtonWidget->nFunButtonPressState = CQBAD_SHUNT;
                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
            }
            if(nPoorShuntType2==0x31)
            {
                pStationFunButtonWidget->nFunButtonPressState = DWBAD_SHUNT;
                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
            }
            if(nPoorShuntType3==0x32)
            {
                pStationFunButtonWidget->nFunButtonPressState = FWBAD_SHUNT;
                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
            }
        }
        else
        {
            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        }
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    //股道控制
    if(pStationFunButtonWidget->nFunButtonPressState == LOCK_FUNCTION)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == UNLOCK_FUNCTION)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == SECTION_RELEASE)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == BAD_SHUNT_KX)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == FUNC_BTN)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == CALLON_LOCK)
    {
        if((mFunButtonDevInfo.nCode == -1)&&(mFunButtonDevInfo.strButtonName == "引导总锁")&&(mFunButtonDevInfo.m_nQCYDZS == true))
        {
            if((pCurrStation->m_nSXYDZS == false)&&(pCurrStation->m_nXXYDZS == false))
            {
                int code = -1;
                code=pCurrStation->GetXHCodeByName("S");
                GetFunButtonType(mFunButtonDevInfo.nDevType,code,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                int code1 = -1;
                code1=pCurrStation->GetXHCodeByName("X");
                GetFunButtonType(mFunButtonDevInfo.nDevType,code1,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                OperateOrderClean();
            }
            else if((pCurrStation->m_nSXYDZS == true)&&(pCurrStation->m_nXXYDZS == false))
            {
//                int code = -1;
//                code=pCurrStation->GetXHCodeByName("S");
//                GetFunButtonType(mFunButtonDevInfo.nDevType,code,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                int code1 = -1;
                code1=pCurrStation->GetXHCodeByName("X");
                GetFunButtonType(mFunButtonDevInfo.nDevType,code1,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                OperateOrderClean();
            }
            else if((pCurrStation->m_nSXYDZS == false)&&(pCurrStation->m_nXXYDZS == true))
            {
                int code = -1;
                code=pCurrStation->GetXHCodeByName("S");
                GetFunButtonType(mFunButtonDevInfo.nDevType,code,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
//                int code1 = -1;
//                code1=pCurrStation->GetXHCodeByName("X");
//                GetFunButtonType(mFunButtonDevInfo.nDevType,code1,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                OperateOrderClean();
            }
            else if((pCurrStation->m_nSXYDZS == true)&&(pCurrStation->m_nXXYDZS == true))
            {
                int code = -1;
                code=pCurrStation->GetXHCodeByName("S");
                GetFunButtonType(mFunButtonDevInfo.nDevType,code,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                int code1 = -1;
                code1=pCurrStation->GetXHCodeByName("X");
                GetFunButtonType(mFunButtonDevInfo.nDevType,code1,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
                OperateOrderClean();
            }

        }
        else
        {
            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
            OperateOrderClean();
        }
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_GD)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCDW)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == POWEROFF_DCFW)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_JG)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }
    else if(pStationFunButtonWidget->nFunButtonPressState == DCFAULT_XG)
    {
        GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
        OperateOrderClean();
    }




//    QString strroutename23=mFunButtonDevInfo.strButtonName;

//    if(mFunButtonDevInfo.strButtonName=="进路建立"||mFunButtonDevInfo.strButtonName=="进路建立通过")
//    {
//        for(int i=0;i<8;i++)
//        {
//            if(mFunButtonDevInfo.nDevCode[i]!=-1)
//            {
//                if(i==0)
//                {
//                    if(mFunButtonDevInfo.strButtonName=="进路建立通过")
//                    {
//                        strroutename=pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nCode&0xff);
//                    }
//                    else
//                    {
//                       strroutename=pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xff);
//                    }

//                }
//                else
//                {
//                   strroutename=strroutename+"-"+ pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xff);
//                }
//            }
//        }
//        //QString strorder=QString("请为进路%1%2输入车次号:").arg(pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nCode)).arg("-"+pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nCode2));
//        QString strorder=QString("请为进路%1输入车次号:").arg(strroutename);
//        mTrainRouteHandle->ClearText();
//        if(mFunButtonDevInfo.strButtonName=="进路建立通过")
//        {
//            mTrainRouteHandle->SetTitle(" 通过进路办理");
//        }
//        else
//        {
//            mTrainRouteHandle->SetTitle(" 列车进路办理");
//        }

//        mTrainRouteHandle->SetTypeText(strorder);
//        QRect rect=mTrainRouteHandle->geometry();
//        mTrainRouteHandle->setGeometry(802,379,rect.width(),rect.height());
//        mTrainRouteHandle->show();
//        connjlbj =connect(mTrainRouteHandle, &TrainRouteHandle::ClosedSignal,this,[=](){
//            GetFunButtonType(&mFunButtonDevInfo,mTrainRouteHandle->GetTrainRouteNum(),0,0);
//            OperateOrderClean();
//            //解绑
//            QObject::disconnect(connjlbj);
//        });
//        connjlbj =connect(mTrainRouteHandle, &TrainRouteHandle::CancleSignal,this,[=](){

//            OperateOrderClean();
//            //解绑
//            QObject::disconnect(connjlbj);
//        });
//    }
//    else if(mFunButtonDevInfo.strButtonName=="进路建立调车")
//    {
//        for(int i=0;i<8;i++)
//        {
//            if(mFunButtonDevInfo.nDevCode[i]!=-1)
//            {
//                if(i==0)
//                {
//                    strroutename=pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xff);
//                }
//                else
//                {
//                   strroutename=strroutename+"-"+ pCurrStation->GetStrNameByCode(mFunButtonDevInfo.nDevCode[i]&0xff);
//                }
//            }
//        }
//        mDCRouteTimeWidget->Init(strroutename);
//        QRect rect=mDCRouteTimeWidget->geometry();
//        mDCRouteTimeWidget->setGeometry(802,379,rect.width(),rect.height());
//        mDCRouteTimeWidget->show();
//        //GetTimeAndType
//        connjlbj =connect(mDCRouteTimeWidget, &DCRouteTimeWidget::ClosedSignal,this,[=](){
//            GetFunButtonType(&mFunButtonDevInfo,mDCRouteTimeWidget->GetTimeAndType(),0,0);
//            OperateOrderClean();
//            //解绑
//            QObject::disconnect(connjlbj);
//        });
//        connjlbj =connect(mDCRouteTimeWidget, &DCRouteTimeWidget::CancleSignal,this,[=](){

//            OperateOrderClean();
//            //解绑
//            QObject::disconnect(connjlbj);
//        });
//    }
//    else
//    {
//        if(mFunButtonDevInfo.strButtonName=="分路不良"&&mFunButtonDevInfo.nDevType==Dev_DC)
//        {
//            int ntype=GetPoorShuntType();
//            if(nPoorShuntType==0x30)
//            {
//                mFunButtonDevInfo.strButtonName="分路不良岔前";
//                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
//            }
//            if(nPoorShuntType2==0x31)
//            {
//                mFunButtonDevInfo.strButtonName="分路不良定位";
//                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
//            }
//            if(nPoorShuntType3==0x32)
//            {
//                mFunButtonDevInfo.strButtonName="分路不良反位";
//                GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
//            }
//        }
//        else
//        {
////            GetFunButtonType(mFunButtonDevInfo.nDevType,mFunButtonDevInfo.nCode,mFunButtonDevInfo.nCode2,mFunButtonDevInfo.strButtonName,0,0);
//        }
////        OperateOrderClean();
//    }

}

void StationCtrlDispKSK::SetButtonLightFalse(CBaseData *mCBaseDataOld_Temp)
{
    if(mCBaseDataOld_Temp!=nullptr)
    {
        mCBaseDataOld_Temp->m_RangeVisible_XHD=false;
        mCBaseDataOld_Temp->m_RangeVisible_LCAN=false;
        mCBaseDataOld_Temp->m_RangeVisible_TGAN=false;
        mCBaseDataOld_Temp->m_RangeVisible_DCAN=false;
        mCBaseDataOld_Temp->m_RangeVisible_Name=false;
        mCBaseDataOld_Temp->m_RangeVisible_XHD_DDMD=false;
        mCBaseDataOld_Temp->m_RangeVisible_XHD_YDAN=false;
        mCBaseDataOld_Temp->m_RangeVisible=false;
        mCBaseDataOld_Temp->m_RangeVisible_ZDBS_ZFZ=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_ZDBS_JCFZ=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_ZDBS_FCFZ=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_ZDBS_GF=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_BZDBS_BSQH=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_BZDBS_BS=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_BZDBS_SG=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_BZDBS_FY=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_JZ_JZSY=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_JZ_JZTY=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_JZ_JZFL=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
        mCBaseDataOld_Temp->m_RangeVisible_JZ_BSQH=false; //设置设备范围显示（或高亮显示）//点灯灭灯按下
    }
}
//获取尖轨心轨按钮配置
bool StationCtrlDispKSK::GetDCBtnJGXG()
{
    if(pCurrStation->DCBtnArray.size()>0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool StationCtrlDispKSK::OperationAlarmShow(int ncode,QString str1)
{
    CGDDC *pCGDDC=nullptr;
    pCGDDC=pCurrStation->GetDCByCode(ncode);
    if(pCGDDC==nullptr)
    {
        return true;
    }
    //当道岔分路不良时，对道岔进行定操 反操操作时需要弹出操作报警窗口
    if((pCGDDC->flblStatusCQ==1)||(pCGDDC->flblStatusDW==1)||(pCGDDC->flblStatusFW==1))
    {
        QString str="道岔"+str1+"操作  名称:"+pCGDDC->getName()+"\n";
        str=str+"命令与联锁冲突\n";
        str=str+"单操不能进行,道岔分路不良"+pCGDDC->getName();
        mOperationAlarmDialog.InitShow(str);
        int nResult = mOperationAlarmDialog.exec();
        int a;
        if(nResult == QDialog::Accepted) //点击了确定按钮
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

//股道无电
void StationCtrlDispKSK::GDWDSolt()
{
    pStationFunButtonWidget->nFunButtonPressState = POWEROFF_GD;
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
    pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->setCheckable(false);
}
//接触网定位无电
void StationCtrlDispKSK::JCWDWWDSolt()
{
    pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCDW;
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
    pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->setCheckable(false);
}
//接触网反位无电
void StationCtrlDispKSK::JCWFWWDSolt()
{
    pStationFunButtonWidget->nFunButtonPressState = POWEROFF_DCFW;
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
    pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->setCheckable(false);
}
//尖轨故障
void StationCtrlDispKSK::JGGZSolt()
{
    pStationFunButtonWidget->nFunButtonPressState = DCFAULT_JG;
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
    pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->setCheckable(false);
}
//心轨故障
void StationCtrlDispKSK::XGGZSolt()
{
    pStationFunButtonWidget->nFunButtonPressState = DCFAULT_XG;
    pStationFunButtonWidget->SetFunButtonEn_id(ORDER_CLEAR,true);
    pStationFunButtonWidget->pFunButtonGroup->button(JL_BUILD)->setCheckable(false);
}
//破封统计
void StationCtrlDispKSK::PFTJSolt()
{
    mQFTJ->InitShow(pCurrStation->getStationName(),pCurrStation->mQianFengVect);
    mQFTJ->show();
}
int StationCtrlDispKSK::GetYDState(unsigned int nDevCode)
{
    CXHD *pXHD=nullptr;
    if(pCurrStation)
    {
        for(int j=0;j<pCurrStation->DevArray.size();j++)
        {
            if(pCurrStation->DevArray[j]->getDevType() == Dev_XH)
            {
                pXHD=(CXHD*)(pCurrStation->DevArray[j]);
                if(pXHD->getCode() == nDevCode)
                {
                    if((pXHD->getTimeCount()>0) && pXHD->getXHDState(XHD_YD))
                    {
                        return 0x01;
                    }
                }
            }
        }
    }
    return 0x12;
}
//根据设备编号获取设备名称
QString StationCtrlDispKSK::getNameOfDevNode(MyStation* pStation, unsigned int devnode)
{
    for(auto ement:pStation->DevArray)
    {
        if(ement->getDevType() == Dev_DC)
        {
            CGDDC *gddc=(CGDDC *)ement;
            if(devnode == (int)gddc->getCode())
            {
                return gddc->getName();
            }
        }
        else if(ement->getDevType() == Dev_XH)
        {
            CXHD *xhd=(CXHD *)ement;
            if(devnode == (int)xhd->getCode())
            {
                return xhd->getName();
            }
        }
        else if(ement->getDevType() == Dev_GD)
        {
            CGD *gd=(CGD*)ement;
            if(devnode == (int)gd->getCode())
            {
                return gd->getName();
            }
        }
    }
    return "";
}
