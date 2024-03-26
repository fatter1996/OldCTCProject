#ifndef GLOBALSTRUCTURE_H
#define GLOBALSTRUCTURE_H
#include "Global.h"
typedef struct
{
    int stationID;
    QPoint offSetPt;
    QPoint staNamePt;
    int m_nCTCSoftID;
    int m_nLSSoftID;
    int m_nXLMNJSoftID;
    void init()
    {
        stationID=m_nLSSoftID=m_nCTCSoftID=m_nXLMNJSoftID=0;
        offSetPt=QPoint(0,0);
        staNamePt=QPoint(0,0);
    }
}StaInfo;

typedef struct
{
    QString str_Speech;
    int n_Count;
    void init()
    {
        str_Speech="";
        n_Count=0;
    }
}SpeechData;

typedef struct
{
    QString strName;
    int nStationID;
    QPoint pt;
    bool isRunFlag;
    int runTime;
    void init()
    {
        strName="";
        nStationID=runTime=0;
        pt=QPoint(0,0);
        isRunFlag=false;
    }
}DLB_Data;

typedef struct
{
    QString strType;//类型
    QString strTypeName;//车次类型名车
}TrainNumType;

typedef struct
{
    QVector<QString> vectBTJLChild;
}BTJL;//变通进路


//进路办理检查结果
struct CheckResult{
    int  id = 0;//id(当前时间的毫秒数取后4个字节)
    bool bEnforced = false;//强制执行
    QString checkMsg;//检查结果信息+
    int routeId = 0;//检查结果信息
};

//用户登录信息
typedef struct
{
    int nstate;//用户登录状态
    QString strUserName;//用户名
    QString strPassWord;//密码
    int nindex;//登录顺序
    int nbc;//当前登录班次
    void Init()
    {
       nstate=-1;
       strUserName="";
       strPassWord="";
       nindex=-1;
    }
}UserInfo;
//QF信息
typedef struct
{
    int Type;//类型
    QString StrName;//名称
    int  Counter;//计数器
    QString JucDevName;//关联设备名称

    void Init()
    {
        Type = 0;
        StrName = "";
        Counter = 0;
        JucDevName ="";
    }
}QianFeng;
typedef struct
{
    QString strLevel;
    QString strType;
    QString strData;
    QString strDescribe;
    QString strSource;
    void Init()
    {
        strLevel="";
        strType="";
        strData="";
        strDescribe="";
        strSource="";
    }
}TableData;
//调度命令模板
typedef struct DisOrderTemplate
{
    QString strDisOrderHeader;	//调度命令模板标题
    QString strDisOrderContent;	//调度命令模板内容
    void Init()
    {
        strDisOrderHeader="";
        strDisOrderContent="";
    }
}DisOrderTemplate;

typedef struct TextFrame
{
    unsigned char captureState = 0; //录制状态,0xA5:开始录制，0X5A：结束录制，0xAA录制中状态，0x00其他状态
    unsigned char textDisplayState= 0; //文字显示状态,0xB5:显示文字，0X5B：不显示文字，0x00其他
    QPoint point = { 0,0 }; //坐标
    int duration = 0; //时长
    int textLenght = 0; //文字长度
    unsigned int textColor = 0; //文字颜色
    QByteArray text; //显示内容
}TextFrame;

#endif // GLOBALSTRUCTURE_H
