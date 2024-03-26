#ifndef STATIONINTERFACEKSK_H
#define STATIONINTERFACEKSK_H

#include <QWidget>
#include "StationInterface/stationinterface.h"
#include "stationctrldispksk.h"
#include "multistationdispksk.h"
#include "TrafficLogManageKSK/trafficlogmanageksk.h"


//站场界面主窗口
namespace Ui {
class StationInterfaceKSK;
}
#pragma execution_character_set("utf-8")

class StationInterfaceKSK : public StationInterface
{
    Q_OBJECT

public:
    explicit StationInterfaceKSK(StationInterface *parent = nullptr);
    ~StationInterfaceKSK();

    static StationInterface* CreatWnd(QWidget* parent = nullptr);
    StationCtrlDisp* CreateStationCtrlDisp(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) override;
    MultiStationDisp* CreateMultiStationDisp(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) override;
    TrafficLogManage* CreateTrafficLogManage(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) override;
    TrainDiagramWidget* CreateTrainDiagramWidget(QWidget *parent, MyStation* pMyStation = nullptr);
    DDOrderManage* CreateDDOrderManage(QWidget *parent = nullptr, MyStation* pMyStation = nullptr) override;


    void InitStationCtrlDisp() override;
    //初始化主菜单-卡斯柯
    void InitMenuMain() override;
    //初始化工具栏-卡斯柯-主工具栏
    void InitStationViewToolBar() override;
    //初始化工具栏-卡斯柯-行车日志工具栏
    void InitTrafficLogToolBar() override;
    //初始化工具栏-卡斯柯-签收工具栏
    void InitSignForToolBar() override;

    QPushButton* addToolBarBtn(QToolBar* parent, QVector<QPushButton*>& vecBarBtn, QString iconFile,
                               QString toolTip, bool checkable = false, bool checked = false, QString disableFile = "") override;

    void TurnToStationWnd() override;
    void TurnToMultiSWnd() override;
    void TurnToTrafficLog() override;

    void SignForBtnFlash() override;
    void ShowStationCtrl() override;
    void ShowMultStation() override;
    void ShowTrafficLog() override;
    void showTrainDiagramWidget() override;
    void ShowMousePoint() override;
    void ShowCurrDateTime() override;
    void RouteOrderUpdata() override;

    void onStationViewScale(bool isScale, int nEnlargeNarrow) override;
    void onTrainNumWndScale(int nEnlargeNarrow) override;
    void onRoutePreWndShowStateChenge(bool b) override;

    void SetTrainBtnShow(bool b) override;
    void SetShuntingBtnShow(bool b) override;
    void onTrainNumWndShowStateChenge(bool b) override;

    void StationViewResize() override;

    void ShowSystemAlarmWidget(bool b);
    //更新软件标题
    void UpDataSysTitle(QMainWindow *parent = nullptr)override;
 //   void ClosedSYS(QMainWindow *parent = nullptr)override;

    //获取当前登录用户名
    QString GetUserLogin();
    //根据登录名获取用户密码
    QString GetUserLoginpassword(QString strname);
    //用户登录 注销
    void ShowUserLogInExit(int ntype,QString strname="",QString strpassword="",QStringList strlist={});


private:
    void mousePressEvent(QMouseEvent * event) override;

signals:
   //发送自动签收
    void SendAutoSignStateSignal(bool bsign);
    void SendUserLongInExitSignal(int ntype,QString strUserName,QString strPassWord);

public slots:
    //更新阶段计划
    void onStagePlanUpData(bool isSync = false);
    //更新行车日志
    void onTrafficLogUpData();
    //更新进路序列
    void onTrainRouteOrderUpData();
     void SendServerAlarm(QByteArray recvArray, int recvlen);
     //更新调度命令
     void onDDOrderManageUpData();
     //站场重置
     void ResetStationSlot();
     //文字属性显示
     void TextShowSolt(QString name,bool b);
     void ShowTextShowAttribute();
     //存储用户信息
     void SetUserInfo(QString name,QString password);
     //用户登录报警
     void UserAlarm(int n);
     void UserUpDateSolt(int n,QString name,int nstate);
    void SetServerAlarm(QByteArray recvArray, int recvlen);
    void DDOrderSignSolt();
    void StagePlanSignSolt();
    //收到调度命令
    void updateDisOrderSolt();
    //自动签收
    void AutoSignStateSolt(bool bsign);
    //用户登录
    void LogInSolt(QString mUserName);
//    //用户注销
    void UserExitSolt(bool b);

public:
    QVector<TableData>mVectTableDate;
    QVector<UserInfo>mUserInfoVect;
    int curUserIndex = 0;
    LonInWidget *mlogin;//用户登录
    UserExitWidget *muserexit;//用户注销
    TrainMarshal *m_trainmarshal;
    StationStorage *m_stationstorage;
    ccxnameedit *m_ccxnameedit;

    yt46 *m_yt46;
    tsrs *m_tsrs;
    option *m_option;
    AboutWidget *mAboutWidget;
    StageChronicle *mStageChronicle;//阶段记事
    Dialog_PassWord* mPassWordEntryDlg_NEW = nullptr;
private:
    Ui::StationInterfaceKSK *ui;
    StationCtrlDispKSK *pStationCtrl = nullptr;
    MultiStationDispKSK* pMultiStation = nullptr;
    TrafficLogManageKSK* pTrafficLogManage = nullptr;
    TrainDiagramWidget* pTrainDiagram = nullptr;

};

#endif // STATIONINTERFACEKSK_H
