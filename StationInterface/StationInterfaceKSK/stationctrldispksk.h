#ifndef SINGLESTATIONWIDGET_H
#define SINGLESTATIONWIDGET_H

#include <QWidget>
#include<QDateTime>
#include<QPoint>
#include<QMouseEvent>
#include<QMoveEvent>
#include<QMessageBox>
#include "StationInterface/stationctrldisp.h"
#include "StationInterface/StationFunButton/stationfunbuttonwidget.h"

//单站界面窗口
#pragma execution_character_set("utf-8")

namespace Ui {
class StationCtrlDispKSK;
}

class StationCtrlDispKSK : public StationCtrlDisp
{
    Q_OBJECT
public:
    explicit StationCtrlDispKSK(QWidget *parent = nullptr);
    ~StationCtrlDispKSK();

    void StationViewResize(int nEnlargeNarrow = 0) override;
    void StationMenuAuxiliary() override;
    //设置站场全景
    void SetStationAllView(bool b);
    void TrainNOViewResize(int nEnlargeNarrow) override;
    void TrainNumWndShowStateChenge(bool b) override;
    //进路预告窗显示状态
    void RoutePreWndShowStateChenge(bool b) override;
    void mouseMoveEvent(QMouseEvent *event) override;
public:
    //定时器
    int Timer_ID_250;
    int Timer_ID_500;
    int Timer_ID_1000;
    int Timer_ID_2000;
    long nElapsed;
    double diploidMuit = 1.0f;
    /*非常站控模式 0x22
      其他，CTC模式（由CTC更改并转发）
      0x00-中心模式
      0x01-车站模式
      0x02-调车模式
      */
    int nCTCControlMode;//CTC控制模式状态
    int nCTCPlanControlMode;//CTC计划控制模式
    QCursor mOldCursor;//记录鼠标旧显示模式
    QDateTime mdateTime;//系统时间


    //工具栏
    bool bShow_Jyj;//绝缘节显示
    bool bShow_LCAN;//列车按钮
    bool bShow_DCAN;//调车按钮
    bool bLCANFlash;//终端闪烁 列车
    bool bDCANFlash;//终端闪烁 调车
    bool bBTANFlash;//终端闪烁 变通
    bool bShow_RouteList;//进路序列窗显示
    bool bShowCheCi=false;//车次显示标志
    bool bRoutePreWndShowState = true;//进路预告窗显示标志

    QMenu *menuDC;
    QMenu *menuXH;
    QMenu *menuGD;
    QMenu *menuSTA;
    QMenu *menuFunOrder;
    QMenu *menuCheCi;//车次操作
    QMenu *menuTextAnnotation;//文字标注右键菜单
    QPoint mcoursePoint;

    CBaseData *mCBaseDataOld;
    typedef struct
    {
        int nDevType;//设备类型
        int nCode;//设备编码
        int nCode2;//设备编码
        int nCode3;//设备编码
        int RecordDevNum;//记录下按钮的个数
        int nDevCode[8];
        QString strButtonName;//功能按钮名称
        QString strTrainNum;//列车车次 仅用于进路办理
        void InitCode()
        {
            for(int i=0;i<8;i++)
            {
                nDevCode[i]=-1;
            }
            RecordDevNum = 0;
        }
        void Init()
        {
            nDevType=-1;
            nCode=-1;
            nCode2=-1;
            nCode3=-1;
            InitCode();
            strButtonName="";
            strTrainNum="";
        }
        void SetCode(int n)
        {
            for(int i=0;i<8;i++)
            {
                if(nDevCode[i]==n)
                    return;
                if(nDevCode[i]==-1)
                {
                   nDevCode[i]=n;
                   break;
                }
            }
        }
        void SetCode(int n,int i)
        {
            nDevCode[i] = n;
        }
        bool m_nQCYDZS=false;//全场引导总锁

    }FunButtonDevInfo;
    FunButtonDevInfo mFunButtonDevInfo;
    StationFunButtonWidget *pStationFunButtonWidget;
    //DLG_FZList *FZList;
    int nPoorShuntType;//分路不良类型
    int nPoorShuntType2;
    int nPoorShuntType3;
    //PoorShuntDlg *mPoorShuntDlg;//分路不良窗口
    PoorShuntDlg_Dlg *mPoorShuntDlg;//分路不良窗口
    //ConfirmCCH *mConfirmCCH;//车次操作
    ConfirmCCH_Dlg *mConfirmCCH;//车次操作
    //ChangeCCProperty *mChangeCCProperty;//修改车次属性
    ChangeCCProperty_Dlg *mChangeCCProperty;//修改车次属性
    //AddTrain *mAddTrain;//站内加车
    AddTrain_Dlg *mAddTrain;//站内加车
    DelCheCi *mDelCheCi;//车次号删除
    PassWordEntryDlg *mPassWordEntryDlg;//铅封窗口
    Dialog_PassWord *mPassWordEntryDlg_NEW;//铅封窗口
    //TrainRouteHandle *mTrainRouteHandle;//进路办理车次输入窗
    trainroutehandle_DLG *mTrainRouteHandle;//进路办理车次输入窗
    //Dialog_PassWord* mTrainRouteHandle = nullptr;//进路办理车次输入窗
    KeyBoard *mKeyBoard;//软键盘窗口
    QMetaObject::Connection connzdbs;//自动闭塞信号槽
    QMetaObject::Connection connbzdbs;//半自动闭塞信号槽
    QMetaObject::Connection connfunbtn;//功能按钮信号槽
    QMetaObject::Connection connjlbj;//进路办理信号槽
    bool bPassWordResult;//铅封窗口回执状态
    QFTJ *mQFTJ=nullptr;//破封统计窗口
    StateChose *mStateChose;//状态选择窗口
    ModeChange *mModeChange;//模式转换窗口
    SeekTrain *m_SeekTrain;//查找车次窗口

    //DCRouteTimeWidget *mDCRouteTimeWidget;//调车进路时间参数设置
    DCRouteTimeWidget_Dlg *mDCRouteTimeWidget;//调车进路时间参数设置
    OperationAlarmDialog mOperationAlarmDialog;//操作报警窗口
    //接通光带时间计数
    int nLightband_30=0;
    int nLightband_15=0;
    //接通光带
    bool BLightband_30=false;
    bool BLightband_15=false;
    bool bzrjrightmenushow=false;
    TrainNumWnd *pTrainNumWndOld=nullptr;
    //文字标注窗口显示
    bool bTextAnnotation=false;
    TextAnnotationWidget *mTextAnnotationWidget = nullptr;
    TextAnnotation *TextAnnotationcheck = nullptr;
public:
    int countCombinations(int num1, int num2, int num3);
    int getsamenum(int num1, int num2, int num3,int i, int j, int k);
    //初始化设置
    void InitSet();
    //初始化参数
    void InitParameter();
    void InitViewSize(QSize minitialSize);
    //初始化类
    void InitClass();
    //初始化信号-槽
    void InitConnect();
    void magicTime();
    //绘制单站
    void DrawSingleStation(QPainter *painter);
    //鼠标移动
    //void mouseMove();
    //初始化光标资源
    void InitCursor();
    //重置光标为箭头
    void ResetCursor();
    //初始化右键菜单
    void InitMouseRightMenu();
    //车次操作
    bool CHECIOperate(MyStation* pMyStation);
    //站场操作
    bool StationOperate(MyStation* pMyStation);
    //文字操作
    bool TextAnnotationOperate(MyStation* pMyStation);
    //文字操作 拖动
    TextAnnotation * TextAnnotationCheck(MyStation* pMyStation);
    //车次预告窗操作
    bool RoutePreWndOperate(MyStation* pMyStation);
    //车次窗操作
    bool TrainNumWndOperate(MyStation* pMyStation);
    QVector<QAction*> getRouteActionByRouteId(int route_id);
    QString GetRouteDescrip(TrainRouteOrder* pRouteOrder, bool bAutoTouth = false);
    //设置菜单选项状态
    void SetMenuState(QMenu *menu,QString str,bool b,bool binvert);
    //设置非常站控下菜单状态
    void SetMenuStateInFCZK(QMenu *menu,QString str,bool b,bool binvert);
    //设置车次启动
    void SetCHECIFiring(MyStation* pMyStation,Train* pTrain,QString text);
    //设置车次停稳
    void SetCHECIStop(MyStation* pMyStation,Train* pTrain,QString text);
    //加车次号
    void SetCHECIAdd(MyStation* pMyStation,int ncode,QString text,bool btitle);
    //删车次号
    void SetCHECIDel(MyStation* pMyStation,Train* pTrain,QString text);
    //车次号确认
    void SetCHECIConfrim(MyStation* pMyStation,Train* pTrain,QString text);
    //改车次属性
    void SetCHECIChange(MyStation* pMyStation,Train* pTrain,QString text);
    /*************获取功能按钮类型**************/
    /* nDev:设备类型
       * str:指令名称
      */
    int GetFunButtonType(int nDev,int nDevCode,int nDevCode2,QString str,bool isfs,bool ismd);
    int GetFunButtonType(FunButtonDevInfo *mFunButtonDevInfo,QString strroutenum,bool isfs,bool ismd);
    void GetDevInfoByMousePoint(FunButtonDevInfo *mFunButtonDevInfo);
    //设置信号机按钮显示
    //void SetXHJANShow(int ntype);
    //车次窗放大缩小
    void SetDiploidCheCi(int n);
    //分路不良类型
    int GetPoorShuntType();
    //查找车次显示
    void ShowSeekTrain();
//    //文字属性显示
//    void ShowTextShowAttribute();

    //设置站场右键菜单 放大缩小显示
    void SetMenuStaShow(int n);
     void SetMenuStaShow();
    //设置道岔右键菜单选项状态
    void SetMenuDCShow(CGDDC* pCGDDC);
    //设置信号机右键菜单选项状态
    void SetMenuXHShow(CXHD *pXHD);
    //设置股道右键菜单选项状态
    void SetMenuGDShow(CGD *pGD);
    //设置菜单选项选中/取消状态
    void SetMenuCheck(QMenu *menu,QString str,bool b);
    //显示提示对话框
    bool ShowDlgByCheck(QString strorder,QString strtype,QString strdevname);
    bool IsShowDlgByCheck(QString strorder);
    bool ISLeadSwalingShow(QString strorder);
    void LeadSwalingShow(QString strorder);
    //bool LeadSwalingShow(QString strorder);
    //设置鼠标光标样式
    void SetMouseCursor(int n);
    void SetDC_JTGD();//接通光带

    //文字显示
    void TextShowSolt(QString name,bool b);

    //获取调度命令签收界面指针
    //DDOrderSign* GetDDOrderSign();

    //命令下达 菜单 功能按钮
    void OperateOrderSend();
    //清除指令 菜单 功能按钮
    void OperateOrderClean();
    //判断命令下达 清除右键菜单显示条件
    bool GetOrderRightMenuShow(bool bleft=false);
    int SetRouteCode(int ncode,int nFunType);
    //站场操作命令下发处理
    void StaOrderSendOperate();
    void SetButtonLightFalse(CBaseData *mCBaseDataOld);
    //获取尖轨心轨按钮配置
    bool GetDCBtnJGXG();
    //操作报警窗口显示
    bool OperationAlarmShow(int ncode,QString str1);
    int GetYDState(unsigned int nDevCode);
    QString getNameOfDevNode(MyStation* pStation, unsigned int devnode);


signals:
    void StaMenuTriggered(int type, int flag = 0);

private slots:
    //分路不良槽
    void RecResultPoorShuntSolt(int ntype,int ntype2=0,int ntype3=0);
    //铅封窗口显示槽
    void LeadSwalingShowSlot();
    //辅助菜单选择结果槽
    void MenuAuxiliaryChoseSlot(QString str);
    //状态选择按下槽
    void StateChoseSlot();
    //方式转化按下槽
    void ModeChangeSlot(QString str);
    //模式转换申请按下槽
    void ModeApplySlot();
    //初始化模式转换申请按下
    void InitModeApply();
    //命令清除 命令按下
    void OrderButtonSlot(int ntype);
    //口令输入结果
    void PassWordResult(bool b);
    //状态选择/方式转换发送 槽
    void StateAndModeSendSolt(int mode,int type1,int type2);

   // void SendServerAlarm(QByteArray recvArray, int recvlen);
    //进路办理车次输入窗口 槽
    void TrainRouteResult(QString mtrainnum);
    //文字标注保存槽
    void SaveSolt(TextAnnotation* pTextAnnotation = nullptr);
    //股道无电
    void GDWDSolt();
    //接触网定位无电
    void JCWDWWDSolt();
    //接触网反位无电
    void JCWFWWDSolt();
    //尖轨故障
    void JGGZSolt();
    //心轨故障
    void XGGZSolt();
    //破封统计
    void PFTJSolt();

private:
    virtual void timerEvent(QTimerEvent *event) override;//定时器
    virtual bool eventFilter(QObject *obj,QEvent *event) override;
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
    //鼠标按下事件
    virtual void mousePressEvent(QMouseEvent * event)override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
private:
    Ui::StationCtrlDispKSK *ui;
    QWidget* curPassWordWidget = nullptr;
    QWidget* curTextAnnotationWidget = nullptr;

    QSize initialSize = { 0,0 };
    QPoint curPoint = { 0,0 };
    QPointF TextAnnotationPoint = { 0,0 };
    QPoint curMousePoint = { 0,0 };

    int nXHType = 0;
};

#endif // SINGLESTATIONWIDGET_H
