#ifndef STATIONINTERFACEWIDGET_H
#define STATIONINTERFACEWIDGET_H

#include <QMainWindow>
#include <QObject>
#include <QLabel>
#include <QCloseEvent>
#include "stationctrldisp.h"
#include "multistationdisp.h"
#include "trafficlogmanage.h"
#include "SystemAlarm/systemalarm.h"
#include "menuclass/textshowattribute.h"
#include "menuclass/stagechronicle.h"
#include "DDOrder/DDOrderSign/ddordersign.h"
#include "DDOrder/DDOrderManage/ddordermanage.h"
#include "StagePlan/stageplanwidget.h"
#include "StagePlan/sendplanwidget.h"
#include "RouteOrder/routeorderwidget.h"
#include "TrainMarshal/trainmarshal.h"
#include "StationStorage/stationstorage.h"
#include "StationStorage/ccxnameedit.h"
#include "QJlogic/qjlogic.h"
#include "tsrs/tsrs.h"
#include "yt46/yt46.h"
#include "Option/option.h"
#include "menuclass/LogIn/userexitwidget.h"
#include "menuclass/aboutwidget.h"
#include "TextAnalysis/TextAnalysis.h"
#include "QJlogic/qjlogic.h"

//站场界面基类
class StationInterface : public QMainWindow
{
    //Q_OBJECT
public:
    explicit StationInterface(QMainWindow *parent = nullptr);
    ~StationInterface();

public:
    virtual StationCtrlDisp* CreateStationCtrlDisp(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) = 0;
    virtual MultiStationDisp* CreateMultiStationDisp(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) = 0;
    virtual TrafficLogManage* CreateTrafficLogManage(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) = 0;
    virtual DDOrderManage* CreateDDOrderManage(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) = 0;

    virtual void InitStationCtrlDisp()=0;
    //初始化主菜单
    virtual void InitMenuMain() = 0;
    //初始化工具栏-主工具栏
    virtual void InitStationViewToolBar() = 0;
    //初始化工具栏-行车日志工具栏
    virtual void InitTrafficLogToolBar() = 0;
    //初始化工具栏-签收工具栏
    virtual void InitSignForToolBar() = 0;

    virtual void TurnToStationWnd() = 0;
    virtual void TurnToMultiSWnd() = 0;
    virtual void TurnToTrafficLog() = 0;

    virtual void SignForBtnFlash() = 0;

    virtual void ShowStationCtrl() = 0;
    virtual void ShowMultStation() = 0;
    virtual void ShowTrafficLog() = 0;
    virtual void showTrainDiagramWidget() = 0;
    virtual void ShowMousePoint() = 0;
    virtual void ShowCurrDateTime() = 0;
    virtual void RouteOrderUpdata() = 0;

    virtual void onStationViewScale(bool isScale, int nEnlargeNarrow) = 0;
    virtual void onTrainNumWndScale(int nEnlargeNarrow) = 0;
    virtual void onRoutePreWndShowStateChenge(bool b) = 0;
    virtual void SetTrainBtnShow(bool b) = 0;
    virtual void SetShuntingBtnShow(bool b) = 0;
    virtual void onTrainNumWndShowStateChenge(bool b) = 0;
    virtual void StationViewResize() = 0;

    virtual QPushButton* addToolBarBtn(QToolBar* parent, QVector<QPushButton*>& vecBarBtn, QString iconFile, QString toolTip, bool checkable = false, bool checked = false, QString disableFile = "") = 0;
    //更新软件标题
    virtual void UpDataSysTitle(QMainWindow *parent = nullptr)=0;
    void setIsShowRoute(bool b) { isShowRoutepreWnd = b; }
    void setIsShowSignRouteSyn(bool b) { isShowSignRouteSyn = b; }

    //初始化单站界面
    void InitStationWnd(MyStation* pMyStation);
    //初始化多站界面
    void AddStationInfo(MyStation* station, int index);
    //显示鼠标坐标
    void ShowPoint(QPoint p);

    MyStation* CurrStation() { return pCurrStation; }
    StationCtrlDisp* GetStationCtrlDisp() { return pStationCtrl; }
    TrafficLogManage* GetTrafficLogManage() { return pTrafficLog; }
    StagePlanWidget* GetStagePlanWidget() { return pStagePlanWidget; }
    SendPlanWidget* GetSendPlanWidget() { return pSendPlanWidget; }
    RouteOrderWidget* GetRouteOrderWidget() { return pRouteOrderWidget; }
    DDOrderSign* GetDDOrderSignWidget() { return pDDOrderSignWidget; }
    DDOrderManage* GetDDOrderManageWidget() { return pDDOrderManageWidget; }
    QJlogic* GetQJlogic() { return m_QJlogic; }
    void stopStagePlanFlash() {
        StagePlanFlash = false;
        if(pCurrStation->AutoSignStage)
        {

        }
        if(isShowSignRouteSyn)
        {
            SendPlanFlash = true;
        }
    }
    void setSendPlanFlash() {
        SendPlanFlash = true;
    }
    void stopSendPlanFlash() { SendPlanFlash = false; }
    TextAnalysis* GetTextAnalysis() { return pTextAnalysis; }

protected:
    virtual void timerEvent(QTimerEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent * event);

private:
    MyStation* pCurrStation = nullptr;

    QLabel* m_mousePointLabel = nullptr; //鼠标坐标显示窗口
    StationCtrlDisp* pStationCtrl = nullptr;
    MultiStationDisp* pMultiStation = nullptr;
    TrafficLogManage* pTrafficLog = nullptr;
    UserExitWidget *puserexit= nullptr;//用户注销

    int Timer_ID_250; //界面刷新
    int Timer_ID_500; //阶段计划按钮闪烁
    int Timer_ID_1000; //当前时间计时器
    int Timer_ID_2000; //进路序列刷新计时器

protected:
    int nEnlargeNarrow = 0;//站场图放大缩小标志
    int nWndFlag = 1;//当前窗口标志(单站界面/站间透明)
    int nWndFlag2 = 1;//当前窗口标志(运行图/行车日志)
    bool isShowRoutepreWnd = true;
    bool isShowSignRouteSyn = true;
    //阶段计划按钮闪烁标志
    bool StagePlanFlash=false;
    //阶段计划按钮红色状态
    bool isRed = false;
    //调度命令（签收）按钮闪烁标志
    bool DDOrderManageFlash=false;
    //调度命令（签收）按钮红色状态
    bool isRedDDOrderManage = false;
    //发送计划按钮闪烁标志
    bool SendPlanFlash = false;
    //发送计划按钮红色状态
    bool isRedSendPlanFlash = false;

    //主菜单-卡斯柯
    QMenuBar* pMenuBar = nullptr;
    //卡斯柯-主工具栏
    QToolBar* pStationViewToolBar = nullptr;
    //行车日志界面工具栏
    QToolBar* pTrafficLogToolBar = nullptr;
    //签收工具栏
    QToolBar* pSignForToolBar = nullptr;

    //阶段计划窗口指针
    StagePlanWidget* pStagePlanWidget = nullptr;
    //f发送计划窗口指针
    SendPlanWidget* pSendPlanWidget = nullptr;
    //进路序列窗口指针
    RouteOrderWidget* pRouteOrderWidget = nullptr;
    //调度命令签收窗口指针
    DDOrderSign* pDDOrderSignWidget=nullptr;
    //调度命令管理指针
    DDOrderManage* pDDOrderManageWidget=nullptr;
    //文字显示属性窗口
    TextShowAttribute *mTextShowAttribute = nullptr;;
    //系统报警信息窗口
    SystemAlarm* mSystemalarm = nullptr;
    QJlogic *m_QJlogic = nullptr;
protected:
    struct MenuInfo{
        int nLevel = 0;
        int nIndex = 0;

        QAction* pAction = nullptr;
        QVector<MenuInfo*> vecSubMenuInfo;
    public:
        QAction* addNewAction(QWidget* parent, int level, int index, QString text, bool Enabled = true, bool isCheckable = false, bool isChecked = false);
        QAction* addNewSubAction(QWidget* parent, int level, int index, QString text, bool Enabled = true, bool isCheckable = false, bool isChecked = false);
        void addSeparator();
    };

    struct MenuBarInfo{
        QMenu* pMenu = nullptr;
        QVector<MenuInfo*> vecSubMenuInfo;

    public:
        QAction* addNewAction(QWidget* parent, int level, int index, QString text, bool Enabled = true, bool isCheckable = false, bool isChecked = false);
        void addSeparator();
        MenuInfo* getSubActionByIndex(int level, ...);
    };
protected:
    QVector<MenuBarInfo*> m_vecMenuBarInfo;
    QVector<QPushButton*> m_vecStationViewToolBarBtn;
    QVector<QPushButton*> m_vecTrafficLogToolBarBtn;
    TextAnalysis* pTextAnalysis = nullptr;
};

#endif // STATIONINTERFACEWIDGET_H
