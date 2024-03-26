#include "stationinterface.h"

#include <QStatusBar>

StationInterface::StationInterface(QMainWindow *parent) :
    QMainWindow(parent)
{
    //初始化状态栏,显示鼠标坐标
    QStatusBar* statusbar = new QStatusBar(this);
    statusbar->setFixedHeight(24);
    statusbar->setStyleSheet("QStatusBar{border-top: 1px solid gray;"
                             "background-color: rgb(240, 240, 240);}");

    m_mousePointLabel = new QLabel(statusbar);
    m_mousePointLabel->setFixedSize(200,24);
    m_mousePointLabel->setStyleSheet("background-color: rgb(240, 240, 240);");
    m_mousePointLabel->setText(QString("X:%1,Y:%2").arg(QCursor::pos().x()).arg(QCursor::pos().y()));

    statusbar->addPermanentWidget(m_mousePointLabel);
    setStatusBar(statusbar);
    setMouseTracking(true);

    this->setContextMenuPolicy(Qt::NoContextMenu);
    Timer_ID_250 = startTimer(100);
    Timer_ID_500 = startTimer(500);
    Timer_ID_1000 = startTimer(1000);
    //Timer_ID_2000 = startTimer(2000);
}

StationInterface::~StationInterface()
{

}

void StationInterface::mouseMoveEvent(QMouseEvent *event)
{
    ShowMousePoint();
}

void StationInterface::mousePressEvent(QMouseEvent * event)
{
    if(event->button() == Qt::LeftButton)
    {
        //鼠标左键按下处理
        if(nEnlargeNarrow==1 || nEnlargeNarrow==2)
        {
            StationViewResize();
        }
    }
    if(event->button() == Qt::RightButton)
    {
        onStationViewScale(false, 0);
    }
}
void StationInterface::InitStationWnd(MyStation* pMyStation)
{
    pCurrStation = pMyStation;
    if(pCurrStation)
    {
        pStationCtrl = CreateStationCtrlDisp(this, pMyStation);
        pMultiStation = CreateMultiStationDisp(this, pMyStation);
        pTrafficLog = CreateTrafficLogManage(this, pMyStation);
        pDDOrderManageWidget=CreateDDOrderManage(this, pMyStation);
        puserexit=new UserExitWidget();
        pTextAnalysis =new TextAnalysis();
        pTextAnalysis->TextAnalysisInfoInit(pCurrStation);
        connect(pStationCtrl, &StationCtrlDisp::SendTextAnalysisSignal,[=](int index, QPoint pt, QString dev){
            pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
        });
        connect(pMultiStation, &MultiStationDisp::SendTextAnalysisSignal,[=](int index, QPoint pt, QString dev){
            pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
        });
        connect(pRouteOrderWidget->GetTrainRouteOrderWidget(), &TrainRouteOrderWidget::SendTextAnalysisSignal,[=](int index, QPoint pt, QString dev){
            pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
        });

        connect(pTrafficLog, &TrafficLogManage::SendTextAnalysisSignal,[=](int index, QPoint pt, QString dev){
            pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
        });

        connect(pDDOrderManageWidget, &DDOrderManage::SendTextAnalysisSignal,[=](int index, QPoint pt, QString dev){
            pTextAnalysis->sendTextAnalysisMsg(index, mapFromGlobal(pt), dev);
        });

        InitStationCtrlDisp();
    }
}

void StationInterface::AddStationInfo(MyStation* station, int index)
{
    pMultiStation->AddMultiStationInfo(station, index);
    pMultiStation->WidgetReSize();
}

void StationInterface::ShowPoint(QPoint p)
{
    m_mousePointLabel->setText(QString("X:%1,Y:%2").arg(p.x()).arg(p.y()));
}
void StationInterface::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == Timer_ID_1000)
    {
        ShowCurrDateTime();
        UpDataSysTitle((QMainWindow*)this->parent()->parent());
    }
    if(event->timerId() == Timer_ID_500)
    {
        SignForBtnFlash();
        pMultiStation->update();
        pTrafficLog->update();
        pStationCtrl->update();
    }
    if(event->timerId() == Timer_ID_250)
    {

    }
}


QAction* StationInterface::MenuInfo::addNewAction(QWidget* parent, int level, int index, QString text, bool Enabled, bool isCheckable, bool isChecked)
{
    nLevel = level;
    nIndex = index;

    pAction = new QAction(parent);
    pAction->setText(text);
    pAction->setEnabled(Enabled);
    pAction->setCheckable(isCheckable);
    if(isCheckable)
        pAction->setChecked(isChecked);
    return pAction;
}

QAction* StationInterface::MenuInfo::addNewSubAction(QWidget* parent, int level, int index, QString text, bool Enabled, bool isCheckable, bool isChecked)
{
    QMenu* menu = pAction->menu();
    if(!menu)
    {
        menu = new QMenu();
        pAction->setMenu(menu);
    }

    MenuInfo* info = new MenuInfo;
    info->nLevel = level;
    info->nIndex = index;

    info->pAction = new QAction(parent);
    info->pAction->setText(text);
    info->pAction->setEnabled(Enabled);
    info->pAction->setCheckable(isCheckable);
    if(isCheckable)
        info->pAction->setChecked(isChecked);
    vecSubMenuInfo.append(info);
    menu->addAction(info->pAction);
    return info->pAction;
}

void StationInterface::MenuInfo::addSeparator()
{
    QMenu* menu = pAction->menu();
    if(menu)
        menu->addSeparator();
}

QAction* StationInterface::MenuBarInfo::addNewAction(QWidget* parent, int level, int index, QString text, bool Enabled, bool isCheckable, bool isChecked)
{
    MenuInfo* menuInfo = new MenuInfo;
    QAction* action = menuInfo->addNewAction(parent, level, index, text, Enabled, isCheckable, isChecked);
    pMenu->addAction(action);
    vecSubMenuInfo.append(menuInfo);
    return action;
}

void StationInterface::MenuBarInfo::addSeparator()
{
    if(pMenu)
        pMenu->addSeparator();
}

StationInterface::MenuInfo* StationInterface::MenuBarInfo::getSubActionByIndex(int level, ...)
{
    va_list ptr; // 定义一个指向可变参数的指针
    va_start(ptr, level); // 初始化指针，使得指针指向固定参数lever后面的可变参数列表
    int index = va_arg(ptr, int);
    int i = 0;
    QVector<MenuInfo*>* tempMenuInfoList = &vecSubMenuInfo;
    while(i < level)
    {
        if(index < tempMenuInfoList->size())
        {
            if(tempMenuInfoList->at(index)->nLevel == level)
            {
                va_end(ptr);
                return tempMenuInfoList->at(index);
            }
            else
            {
                tempMenuInfoList = &tempMenuInfoList->at(index)->vecSubMenuInfo;
                index = va_arg(ptr, int);
                i++;
            }
        }
        else return nullptr;
    }
}

