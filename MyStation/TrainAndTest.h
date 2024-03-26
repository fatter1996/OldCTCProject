#ifndef CTRAINANDTEST_H
#define CTRAINANDTEST_H

#include <vector>
#include "mystation.h"
//#include "MyDoc/mydoc.h"
class MyDoc;
using namespace std;
typedef  struct
{
    QString FunStr;     //功能码
    QString ParaStr;    //参数码  分割前
    QString SubFunStr;       //子功能码
    QString TipShowStr;     //提示内容  比如：箭头旁边的文字
    QString DevName;    //每个操作标识对应的操作设备名称
    unsigned int DevCode;  //设备号
    bool SetFlag;
    void init()
    {
        FunStr= "";
        ParaStr= "";
        SubFunStr= "";
        TipShowStr= "";
        DevName="";
        DevCode=0xffff;
        SetFlag = false;
    }
}OrderStr;
typedef  struct
{
    QString TipsName;
    QString TipsContent;
    void init()
    {
        TipsName ="";
        TipsContent = "";
    }
}TrainTips;//培训提示窗口显示内容对应关系
class CTrainAndTest
{
public:
    CTrainAndTest();

    QVector<TrainTips> v_TrainTips;
    QVector<OrderStr> v_OrderArray;
    MyDoc *pDoc;
    QFont m_textFont;
    bool ModelSetFlag;//模式转换设置完成
    unsigned char type;  //0 -没有进去场景设置模式   0x55-培训模式  0xaa-考试模式

    QMenu *DCmenu;
    int DDMLCount; //调度命令签收窗口弹出后的计数器 用于定时关闭调度命令接收窗口
    int XCPZCount; //行车凭证编辑窗口弹出后的计数器 用于定时关闭行车凭证编辑窗口

    unsigned int PlanCode;
    char Num;

    void CheckUdpData(unsigned char *Rec, int len);
    void AnalyUdpData(unsigned char *Rec, int len);
    void HandleTrainSence();

    void InitTrainTips(QString FileName);
    QString getNameOfDevNode(unsigned int devnode);//根据设备编号获取设备名称
    unsigned int getDevNodeOfName(QString strName);//根据设备名称获取设备编号
    QPoint setPicShowPoint(QString strName,int *XHDType);   //根据指定设备名称获取对应指向设备的位图坐标点,参数带回信号机属性
    void DrawSence(QPainter *painter);
    void DrawSence(QPainter *painter,OrderStr ExapleStr,int index);//根据场景命令绘制界面演示信息
    void DrawArrow(QPainter *painter,QPoint pt,QString m_str);
    void DrawArrow_TOP(QPainter *painter,QPoint pt,QString m_str);
    void ArrowDrawByDev(QPainter *painter,QString m_ClassName,QString m_DevName,QString m_str);//根据设备绘制箭头
    void CloseMessageBox();
    void ManageSpecialOrder();
    void CloseDlgOnTimer();

};

#endif // CTRAINANDTEST_H
