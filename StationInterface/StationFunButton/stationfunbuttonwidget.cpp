#include "stationfunbuttonwidget.h"
#include "ui_stationfunbuttonwidget.h"
#include "StationInterface/StationInterfaceKSK/stationctrldispksk.h"


StationFunButtonWidget::StationFunButtonWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StationFunButtonWidget)
{
    ui->setupUi(this);
    //this->setFixedSize(1920,44);
    this->setContextMenuPolicy(Qt::ActionsContextMenu);
    InitMouseRightMenu();
    nFunButtonPressState = JL_BUILD;
    strButtonName="";
    pushButtonDDMDFlag = false;
    dPassWord = new Dialog_PassWord();
    connect(dPassWord, &Dialog_PassWord::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    FZList = new DLG_FZList();
    connect(FZList, &DLG_FZList::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });
    CreatFunButtonKSK();
    installEventFilter(this);
    ui->pushButton_5->installEventFilter(this);
    ui->pushButton_6->installEventFilter(this);
    ui->pushButton_7->installEventFilter(this);
    ui->pushButton_15->installEventFilter(this);
    ui->pushButton_16->installEventFilter(this);
    ui->pushButton_19->installEventFilter(this);
}

StationFunButtonWidget::~StationFunButtonWidget()
{
    delete ui;
}
void StationFunButtonWidget::Init(int ntype)
{
    wndType=ntype;
    switch (wndType)
    {
    case 1: /*CreatFunButtonKSK();*/break;
    case 2: break;
    case 3: break;
    case 4: break;
    default: break;
    }
}
void StationFunButtonWidget::CreatFunButtonKSK()
{
    pFunButtonGroup = new QButtonGroup(this);
    pFunButtonGroup->setExclusive(true);               //设置这个按钮组为互斥模式
    pFunButtonGroup->addButton(ui->pushButton, JL_BUILD);//进路建立
    pFunButtonGroup->addButton(ui->pushButton_2, ALL_CANCEL);//总取消
    pFunButtonGroup->addButton(ui->pushButton_3, SIGNAL_RESTART);//信号重开
    pFunButtonGroup->addButton(ui->pushButton_4, CALLON_BTN);//引导按钮
    pFunButtonGroup->addButton(ui->pushButton_5, CALLON_LOCK);//引导总锁
    pFunButtonGroup->addButton(ui->pushButton_6, ALLMAN_RELEASE);//总人解
    pFunButtonGroup->addButton(ui->pushButton_7, SECTION_RELEASE);//区故解
    pFunButtonGroup->addButton(ui->pushButton_8, SWITCH_NORMAL);//总定位
    pFunButtonGroup->addButton(ui->pushButton_9, SWITCH_REVERSE);//总反位
    pFunButtonGroup->addButton(ui->pushButton_10, SWITCH_LOCK);//单锁
    pFunButtonGroup->addButton(ui->pushButton_11, SWITCH_RELEASE);//单解
    pFunButtonGroup->addButton(ui->pushButton_12, LOCK_FUNCTION);//封锁
    pFunButtonGroup->addButton(ui->pushButton_13, UNLOCK_FUNCTION);//解封
    pFunButtonGroup->addButton(ui->pushButton_14, FUNC_BTN);//功能按钮
    pFunButtonGroup->addButton(ui->pushButton_15, RAMP_RELEASE);//坡道解锁
    pFunButtonGroup->addButton(ui->pushButton_16, BAD_SHUNT);//分路不良
    pFunButtonGroup->addButton(ui->pushButton_17, BAD_SHUNT_KX);//确认空闲
    pFunButtonGroup->addButton(ui->pushButton_18, LIGHT_ON);//点灯
    pFunButtonGroup->addButton(ui->pushButton_19, LIGHT_OFF);//灭灯
    pFunButtonGroup->addButton(ui->pushButton_20, FZ_LIST);//辅助菜单
    pFunButtonGroup->addButton(ui->pushButton_21, ORDER_CLEAR);//命令清除
    pFunButtonGroup->addButton(ui->pushButton_22, ORDER_GIVE);//命令下达
    pFunButtonGroup->addButton(ui->pushButton_23, STATE_CHOOSE);//状态选择
    pFunButtonGroup->addButton(ui->pushButton_24, MODLE_CHANGE);//方式转换
    pFunButtonGroup->button(JL_BUILD)->setChecked(true);
    pFunButtonGroup->button(JL_BUILD)->click();
    pFunButtonGroup->button(ORDER_CLEAR)->setEnabled(false);
    pFunButtonGroup->button(ORDER_GIVE)->setEnabled(false);
}
void StationFunButtonWidget::InitMouseRightMenu()
{
    //辅助菜单
    {
        menuAuxiliary=new QMenu(this);
        QAction *Action1 = new QAction(tr("股道无电"), this);
        QAction *Action2 = new QAction(tr("接触网定位无电"), this);
        QAction *Action3 = new QAction(tr("接触网反位无电"), this);
        QAction *Action4 = new QAction(tr("尖轨故障"), this);
        QAction *Action5 = new QAction(tr("心轨故障"), this);
        QAction *Action6 = new QAction(tr("破封统计"), this);
        QAction *Action7 = new QAction(tr("退出菜单"), this);
        Action4->setObjectName("尖轨");
        Action5->setObjectName("尖轨");
        menuAuxiliary->addAction(Action1);
        menuAuxiliary->addAction(Action2);
        menuAuxiliary->addAction(Action3);
        menuAuxiliary->addAction(Action4);
        menuAuxiliary->addAction(Action5);
        menuAuxiliary->addAction(Action6);
        menuAuxiliary->addAction(Action7);
    }
    //方式转换菜单
    {
        menuModeChange=new QMenu(this);
        QAction *Action1 = new QAction(tr("模式申请"), this);
        QAction *Action2 = new QAction(tr("同意模式申请"), this);
        menuModeChange->addAction(Action1);
        menuModeChange->addAction(Action2);
    }
}
// 设置功能按钮栏状态
void StationFunButtonWidget::SetFunButtonEn(bool b)
{
    bFCZKFlag=!b;
    for(int i=0;i<pFunButtonGroup->buttons().size();i++)
    {
        if(pFunButtonGroup->buttons()[i]->text()!="确认空闲"
           &&pFunButtonGroup->buttons()[i]->text()!="辅助菜单"
           &&pFunButtonGroup->buttons()[i]->text()!="状态选择"
           &&pFunButtonGroup->buttons()[i]->text()!="方式转换"
           &&pFunButtonGroup->buttons()[i]->text()!="命令清除"
           &&pFunButtonGroup->buttons()[i]->text()!="命令下达")
        {
            pFunButtonGroup->buttons()[i]->setCheckable(b);
            pFunButtonGroup->buttons()[i]->setEnabled(b);
        }
    }
}
void StationFunButtonWidget::SetFunButtonEn(int n,bool b)
{
    pFunButtonGroup->buttons()[n]->setCheckable(b);
    pFunButtonGroup->buttons()[n]->setEnabled(b);
}
void StationFunButtonWidget::SetFunButtonEn_id(int id,bool b)
{
    pFunButtonGroup->button(id)->setCheckable(b);
    pFunButtonGroup->button(id)->setEnabled(b);
    //qDebug()<<pFunButtonGroup->button(id)->text();
}
//设置功能按钮可见
void StationFunButtonWidget::SetFunButtonVisable(int id,bool b)
{
      pFunButtonGroup->button(id)->setVisible(b);
}
//获取命令下达、清除按钮使能状态
bool StationFunButtonWidget::GetFunButtonEn(int n)
{
    return  pFunButtonGroup->buttons()[n]->isEnabled();
}
bool StationFunButtonWidget::GetFunButtonEn_id(int id)
{
    return  pFunButtonGroup->button(id)->isEnabled();
}
//设置辅助菜单尖轨心轨显示
void StationFunButtonWidget::SetMenuAuxiliary(bool b)
{
//    for(int i=0;i<menuAuxiliary->actions().size();i++)
//    {
//        if(menuAuxiliary->actions()[i]->objectName()=="尖轨")
//        {
//            menuAuxiliary->actions()[i]->setVisible(b);
//        }
//    }
    FZList->SetJGVisable(b);
    FZList->SetXGVisable(b);
}
void StationFunButtonWidget:: slot_btnGroupClicked(int nId)
{
    nFunButtonPressState=nId;
    strButtonName=pFunButtonGroup->button(nId)->text();
//    if(nFunButtonPressState==20||nFunButtonPressState==21)
//    {
//         pFunButtonGroup->button(nFunButtonPressState)->setChecked(false);
//    }
     pFunButtonGroup->button(20)->setEnabled(true);
    //铅封窗口显示信号
    if(/*nFunButtonPressState==3||*/nFunButtonPressState==4||nFunButtonPressState==5
            ||nFunButtonPressState==6/*||nFunButtonPressState==13*/||nFunButtonPressState==14
            ||nFunButtonPressState==15||nFunButtonPressState==18)
    {
        pFunButtonGroup->button(nFunButtonPressState)->setChecked(false);
        emit LeadSwalingShowSignal();
    }
    if(nFunButtonPressState==19)//辅助菜单按钮
    {
        QAction *aClick=nullptr;
        QPoint p=QCursor::pos();
        QRect r=menuAuxiliary->geometry();
        p.setY(p.y()-170);
        aClick=menuAuxiliary->exec(p);
        if(aClick!=nullptr)
        {
             pFunButtonGroup->button(0)->setChecked(true);
            if(aClick->text()=="破封统计")
            {
                emit MenuAuxiliaryChoseSignal(aClick->text());
            }
            else if(aClick->text()=="退出菜单")
            {
                pFunButtonGroup->button(0)->setChecked(true);
                emit MenuAuxiliaryChoseSignal(aClick->text());
                pFunButtonGroup->button(20)->setEnabled(false);
            }
            else
            {
                 emit MenuAuxiliaryChoseSignal(aClick->text());
                strButtonName=aClick->text();
            }
        }
    }
    else if(nFunButtonPressState==22)//状态选择
    {
        /*
         * 中无内容。对话框中
        只有当车站处于车站控制方式时，该对话框里面才有内容，否则该对话框
        列出当前具有控制权限的车站。红色文字表示系统当前采用该方式排路。
        如果有计划控制，则站场图上每个车站站名下的“计划控制”表示灯亮绿色，否则不亮灯。
        “按图排路”指示灯表示“按图排路”或“手工排路”中其一状态，即当前车站的进路控制模式为“按图排路”，则“按图排路”的表示灯显示绿色；
        当前车站的进路控制模式为“手工排路”，则“按图排路”的表示灯显示黄色，如图 4-27。*/
//        mStateChose->AddData(pCurrStation->getStationName(),0);
//        mStateChose->show();
        emit StateChoseSignal();
        pFunButtonGroup->button(20)->setEnabled(false);
    }
    else if(nFunButtonPressState==23)//方式转换
    {
        QAction *aClick=nullptr;

        pFunButtonGroup->button(20)->setEnabled(false);
        aClick=menuModeChange->exec(QCursor::pos());
        if(aClick!=nullptr)
        {
            if(aClick->text()=="模式申请")
            {
                emit ModeChangeSignal(aClick->text());
            }
            else if(aClick->text()=="同意模式申请")
            {
                 emit ModeChangeSignal(aClick->text());
            }
            else
            {
                strButtonName=aClick->text();
            }
        }

    }
    else if(nFunButtonPressState==20)//命令清除
    {
         emit OrderButtonSignal(0);
    }
    else if(nFunButtonPressState==21)//命令下达
    {
         emit OrderButtonSignal(1);
    }

}

void StationFunButtonWidget::on_pushButton_clicked()//进路建立
{
    nFunButtonPressState = JL_BUILD;  
    emit OrderButtonSignal(0);
}

void StationFunButtonWidget::on_pushButton_2_clicked()//总取消
{
    nFunButtonPressState = ALL_CANCEL;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(1, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_3_clicked()//信号重开
{
    nFunButtonPressState = SIGNAL_RESTART;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(2, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_4_clicked()//引导按钮
{
    nFunButtonPressState = CALLON_BTN;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(3, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_8_clicked()//总定位
{
    nFunButtonPressState = SWITCH_NORMAL;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(7, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_9_clicked()//总反位
{
    nFunButtonPressState = SWITCH_REVERSE;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(8, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_10_clicked()//单锁
{
    nFunButtonPressState = SWITCH_LOCK;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(9, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_11_clicked()//单解
{
    nFunButtonPressState = SWITCH_RELEASE;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(10, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_12_clicked()//封锁
{
    nFunButtonPressState = LOCK_FUNCTION;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(11, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_13_clicked()//解封
{
    nFunButtonPressState = UNLOCK_FUNCTION;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(12, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_14_clicked()//功能按钮
{
    nFunButtonPressState = FUNC_BTN;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(13, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_17_clicked()//确认空闲
{
    nFunButtonPressState = BAD_SHUNT_KX;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    emit SendTextAnalysisSignal(16, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_18_clicked()//点灯
{
    nFunButtonPressState = LIGHT_ON;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    pushButtonDDMDFlag = true;
    emit SendTextAnalysisSignal(17, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_20_clicked()//辅助菜单
{
    emit SendTextAnalysisSignal(19, QCursor::pos());
    nFunButtonPressState = FZ_LIST;
    SetFunButtonEn_id(ORDER_CLEAR,true);
    QPoint coursePoint= QCursor::pos();
    int x = coursePoint.x();
    int y = coursePoint.y();
    FZList->move(x,y-267);
    FZList->exec();

}

void StationFunButtonWidget::on_pushButton_21_clicked()//命令清除
{
    emit OrderButtonSignal(0);
    emit SendTextAnalysisSignal(20, QCursor::pos());
//    SetFunButtonEn_id(ORDER_CLEAR,false);
//    SetFunButtonEn_id(ORDER_GIVE,false);
//    pFunButtonGroup->button(JL_BUILD)->setChecked(false);
//    pFunButtonGroup->button(JL_BUILD)->click();
//    nFunButtonPressState=JL_BUILD;


}

void StationFunButtonWidget::on_pushButton_22_clicked()//命令下达
{
    emit OrderButtonSignal(1);
//    SetFunButtonEn_id(ORDER_CLEAR,false);
//    SetFunButtonEn_id(ORDER_GIVE,false);
//    pFunButtonGroup->button(JL_BUILD)->setChecked(false);
//    pFunButtonGroup->button(JL_BUILD)->click();
//    nFunButtonPressState=JL_BUILD;
    emit SendTextAnalysisSignal(21, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_23_clicked()//状态选择
{
    nFunButtonPressState = STATE_CHOOSE;
    emit StateChoseSignal();
    nFunButtonPressState = JL_BUILD;
    emit SendTextAnalysisSignal(22, QCursor::pos());
}

void StationFunButtonWidget::on_pushButton_24_clicked()//方式转换
{
    emit SendTextAnalysisSignal(23, QCursor::pos());
    nFunButtonPressState = MODLE_CHANGE;
    QAction *aClick=nullptr;
    aClick=menuModeChange->exec(QCursor::pos());
    if(aClick!=nullptr)
    {
        if(aClick->text()=="模式申请")
        {
            emit SendTextAnalysisSignal(119, QCursor::pos());
            emit ModeChangeSignal(aClick->text());
        }
        else if(aClick->text()=="同意模式申请")
        {
            emit SendTextAnalysisSignal(120, QCursor::pos());
            emit ModeChangeSignal(aClick->text());
        }
        else
        {
            strButtonName=aClick->text();
        }
    }
    nFunButtonPressState = JL_BUILD;
}

bool StationFunButtonWidget::eventFilter(QObject *obj,QEvent *event)
{
    if(bFCZKFlag)
    {
       return QWidget::eventFilter(obj,event);
    }
    if(obj->objectName() == "pushButton_5")
    {
        if(bStationDCFlag)
        {
           return QWidget::eventFilter(obj,event);
        }
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit SendTextAnalysisSignal(4, QCursor::pos());
            dPassWord->strDescrib = "引导总锁，请输入第1重密码";
            dPassWord->InitPassWord();
            dPassWord->exec();
            if(dPassWord->onOK == true)
            {
                dPassWord->strDescrib = "引导总锁，请输入第2重密码";
                dPassWord->InitPassWord();
                dPassWord->exec();
                if(dPassWord->onOK == true)
                {
                    nFunButtonPressState = CALLON_LOCK;
                    ui->pushButton_5->setChecked(true);
                    SetFunButtonEn_id(ORDER_CLEAR,true);
                }
                else
                {
                    emit OrderButtonSignal(0);
                }

            }
            else
            {
                emit OrderButtonSignal(0);
            }
            return true;
        }
    }
    if(obj->objectName() == "pushButton_6")
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit SendTextAnalysisSignal(5, QCursor::pos());
            dPassWord->strDescrib = "总人解,请输入密码";
            dPassWord->InitPassWord();
            dPassWord->exec();
            if(dPassWord->onOK == true)
            {
                nFunButtonPressState = ALLMAN_RELEASE;
                ui->pushButton_6->setChecked(true);
                SetFunButtonEn_id(ORDER_CLEAR,true);
            }
            else
            {
                emit OrderButtonSignal(0);
            }
            return true;
        }
    }
    if(obj->objectName() == "pushButton_7")
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit SendTextAnalysisSignal(6, QCursor::pos());
            dPassWord->strDescrib = "区故解,请输入密码";
            dPassWord->InitPassWord();
            dPassWord->exec();
            if(dPassWord->onOK == true)
            {
                nFunButtonPressState = SECTION_RELEASE;
                ui->pushButton_7->setChecked(true);
                SetFunButtonEn_id(ORDER_CLEAR,true);
            }
            else
            {
                emit OrderButtonSignal(0);
            }
            return true;
        }
    }
    if(obj->objectName() == "pushButton_15")
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit SendTextAnalysisSignal(14, QCursor::pos());
            dPassWord->strDescrib = "坡道解锁,请输入密码";
            dPassWord->InitPassWord();
            dPassWord->exec();
            if(dPassWord->onOK == true)
            {
                nFunButtonPressState = RAMP_RELEASE;
                ui->pushButton_15->setChecked(true);
                SetFunButtonEn_id(ORDER_CLEAR,true);
            }
            else
            {
                emit OrderButtonSignal(0);
            }
            return true;
        }
    }
    if(obj->objectName() == "pushButton_16")
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit SendTextAnalysisSignal(15, QCursor::pos());
            dPassWord->strDescrib = "分路不良,请输入密码";
            dPassWord->InitPassWord();
            dPassWord->exec();
            if(dPassWord->onOK == true)
            {
                nFunButtonPressState = BAD_SHUNT;
                ui->pushButton_16->setChecked(true);
                SetFunButtonEn_id(ORDER_CLEAR,true);
            }
            else
            {
                emit OrderButtonSignal(0);
            }
            return true;
        }
    }
    if(obj->objectName() == "pushButton_19")
    {
        if(event->type() == QEvent::MouseButtonPress)
        {
            emit SendTextAnalysisSignal(18, QCursor::pos());
            dPassWord->strDescrib = "开关信号,请输入密码";
            dPassWord->InitPassWord();
            dPassWord->exec();
            if(dPassWord->onOK == true)
            {
                nFunButtonPressState = LIGHT_OFF;
                ui->pushButton_19->setChecked(true);
                pushButtonDDMDFlag = true;
                SetFunButtonEn_id(ORDER_CLEAR,true);
            }
            else
            {
                emit OrderButtonSignal(0);
            }
            return true;
        }
    }
    return QWidget::eventFilter(obj,event);
}








