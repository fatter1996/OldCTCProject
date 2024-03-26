﻿#ifndef CGDDC_H
#define CGDDC_H
#include <QPaintEvent>
#include <QPainter>
#include <QtMath>
#include "qd.h"
class CGDDC : public CQD
{
public:
    explicit CGDDC();

    //成员变量声明
private:
    unsigned int m_nQDCode;    //关联区段设备编号
    unsigned int m_nCxjy;      //超限绝缘节
    unsigned int m_nZ;         //折点
    unsigned int m_nJyj;       //绝缘节
    QPointF p1, p2, p3, p4, p5, p6, p12, p34, p56, pz12, pz34, pz56;
    QPointF p1Const, p2Const, p3Const, p4Const, p5Const, p6Const, p12Const, p34Const, p56Const, pz12Const, pz34Const, pz56Const;
    int m_nQ;        //岔前绘制连接设备
    int m_nD;        //定位绘制连接设备
    int m_nF;        //反位绘制连接设备
    //int m_nCQ_Code;  //岔前关联设备
    //int m_nDW_Code;  //定位关联设备
    //int m_nFW_Code;  //反位关联设备
    bool isMainGD;
    unsigned int m_nDCWZ;
    //unsigned int m_nDCWZ_Old;
    QPointF p34C, p56C;   //用于存放岔心关键坐标，不从数据读取，而是再init中计算得到
    QPointF p34CConst, p56CConst;
    //int m_nSDDCCode;
    //QString m_TrainNum;
    //bool m_TrainDown;
    bool m_bDrawGD;
    //bool isSDDC;
    //int m_nOneToMore;
    //int m_nSameQDDCCode;
    //bool isWGDW;
    //int GLB_DCCQ;    //公里标
    //int GLB_DCCX;    //公里标
    //int GLB_DCDW;    //公里标
    //int GLB_DCFW;    //公里标
    //bool isTSDC;
    //QString CarrierFrequency;
    //int Dir_DMH;
    //bool isDrawDPZPFlag;  //判断本道岔是否绘制低频载频信息（一个区段包含多个道岔时仅一个道岔绘制低频载频信息）
    bool isFLBL_CQ;
    bool isFLBL_DW;
    bool isFLBL_FW;

    bool isDisDCWZ;//道岔位置显示隐藏
    bool isDisLight;
public:
//    bool getDWFLBL();
//    bool getFWFLBL();
//    bool getCQFLBL();
//    bool isFLBL[3];
    bool m_nCQFLBLKX;
    bool m_nFWFLBLKX;
    bool m_nDWFLBLKX;
//    void setDWFLBL(bool flag);
//    void setFWFLBL(bool flag);
//    void setCQFLBL(bool flag);
    void setDCState(unsigned char btState);

    bool isPowerCutDW;      //定位是否停电
    bool isPowerCutFW;      //反位是否停电
    bool isPowerCutCQ;      //岔前是否停电

    bool isDS;  //单锁
    bool isFS;  //封锁
    bool isMA;    //移动授权
    bool bShow_Jyj;//绝缘节显示
    bool isYCFS;  //腰岔封锁
public:
    QString m_strCheCiNum;//车次
    bool m_nCheciLost;//车次是否丢失
    bool m_bElectric;//电力牵引
    int m_nSXCheCi;//1右行 0左行
    bool m_bLCTW;//列车挺稳标志
    int m_nKHType;//客货类型：客车/货车
    int m_nSpeed;

    bool isJGGZ;//尖轨故障
    bool isXGGZ;//心轨故障
    int GZANModle=1;//故障按钮样式;0为圆形按钮,1为方形按钮,2为圆形灯
    int flblStatusDW;//定位分路不良,0无，1分路，2确认空闲
    int flblStatusFW;//反位分路不良,0无，1分路，2确认空闲
    int flblStatusCQ;//岔前分路不良,0无，1分路，2确认空闲
    bool bJSJSetFLBLFlagDW=false;//教师机设置分路不良
    bool bJSJSetFLBLFlagFW=false;//教师机设置分路不良
    bool bJSJSetFLBLFlagCQ=false;//教师机设置分路不良
//    bool isPowerDW;//定位是否有电
//    bool isPowerFW;//反位是否有电
//    bool isPowerCQ;//岔前是否有电
    int speedLimitStatus;//限速状态,0无限速,1定位限速表示稳定,2反位限速表示稳定
    //bool m_RangeVisible; //股道范围显示（或高亮显示）
    bool bDCTextShow;//道岔名称显示
    //引导总锁状态(包含上下行)
    int m_nDCSXAllLock;	//下锁: |GDDC_X_LOCK		上锁 |GDDC_S_LOCK
    int wordcolor_DW;//文字颜色定位
    int wordcolor_FW;//文字颜色反位
    int wordcolor_SK;//文字颜色四开
    int wordsize;//文字大小
    int DCWZTimeForSK;//四开状态的读秒时间
    bool BLightband;//接通光带显示
    QPoint SHOWPOINT;
    int SHOWTYPE;
    int SHOWNAMETYPE;
    QString SHOWNAME;
    bool haveAQXDCBJD;


    QString strTGQName="";   //脱轨器名称
    QPoint strTGQNamePt=QPoint(0,0);  //脱轨器名称坐标
    QPoint TGQPT=QPoint(0,0);         //脱轨器坐标
    int TGQDIRECTION=0;     //脱轨器方向(向上为1,向下为0)
    int TGQSHOWTYPE=1;      //脱轨器类型(全三角为1,半三角为0)
    void DrawTGQ(QPainter *painter, long nElapsed, double nDiploid);//绘制脱轨器

    //成员封装函数声明
public:
    void setQDCode(unsigned int code);
    unsigned int getQDCode();
    void setCxjy(unsigned int nCxjy);
    bool getCxjy(unsigned int nCxjy);
    void setZ(unsigned int nZ);
    bool getZ(unsigned int nZ);
    void setJyj(unsigned int nJyj);
    bool getJyj(unsigned int nJyj);
    void setCQdev(int dev); //岔前一送多受属性
    int getCQdev();//岔前一送多受属性
    void setDWdev(int dev);//定位一送多受属性
    int getDWdev();//定位一送多受属性
    void setFWdev(int dev);//反位一送多受属性
    int getFWdev();//反位一送多受属性
//    void setCQCode(int code);  //岔前设备
//    int getCQCode();//岔前设备
//    void setDWCode(int code);//定位设备
//    int getDWCode();//定位设备
//    void setFWCode(int code);//反位设备
//    int getFWCode();//反位设备
    void setIsMainGD(bool nFlag);
    bool getIsMainGD();
    bool getIsMainGDFlag();
    void setDCWZ(unsigned int dcwz);
    unsigned int  getDCWZ();
    //void setDCWZ_Old(unsigned int dcwz);
    //unsigned int  getDCWZ_Old();
    //void setSDDCCode(int sddc_code);
    //int getSDDCCode();
    int setDrawGD(bool bDrawGD);
    //void setIsSDDC(bool flag);
    //bool getIsSDDC();
    //void setOneToMore(int onetomore);
    //int getOneToMore();
    void setp1(QPointF pt);
    QPointF getp1();
    void setp2(QPointF pt);
    QPointF getp2();
    void setp3(QPointF pt);
    QPointF getp3();
    void setp4(QPointF pt);
    QPointF getp4();
    void setp5(QPointF pt);
    QPointF getp5();
    void setp6(QPointF pt);
    QPointF getp6();
    void setp12(QPointF pt);
    QPointF getp12();
    void setp34(QPointF pt);
    QPointF getp34();
    void setp56(QPointF pt);
    QPointF getp56();
    void setpz12(QPointF pt);
    QPointF getpz12();
    void setpz34(QPointF pt);
    QPointF getpz34();
    void setpz56(QPointF pt);
    QPointF getpz56();
    //void setSameQDDCCode(int code);
    //int getSameQDDCCode();
    //void setIsWGDW(bool flag);
    //bool getIsWGDW();
//    void setGLB_DCCQ(int glb);
//    int getGLB_DCCQ();
//    void setGLB_DCCX(int glb);
//    int getGLB_DCCX();
//    void setGLB_DCDW(int glb);
//    int getGLB_DCDW();
//    void setGLB_DCFW(int glb);
//    int getGLB_DCFW();
//    void setIsTSDC(bool flag);
//    bool getIsTSDC();
//    void setCarrierFrequency(QString str);
//    QString getCarrierFrequency();
//    void setDir_DMH(bool flag);
//    int getDir_DMH();
//    void setIsDrawDPZPFlag(bool flag);
//    bool getIsDrawDPZPFlag();
    void setIsFLBL_CQ(bool flag);
    bool getIsFLBL_CQ();//分路不良 岔前
    void setIsFLBL_DW(bool flag);
    bool getIsFLBL_DW();//分路不良 定位
    void setIsFLBL_FW(bool flag);
    bool getIsFLBL_FW();//分路不良 反位
    void setDisDCWZ(bool flag);
    bool getDisDCWZ();
    void setDisLight(bool flag);
    bool getDisLight();
    void setIsDS(bool flag);
    bool getIsDS();//单锁
    void setIsFS(bool flag);
    bool getIsFS();//封锁
    void setIsYCFS(bool flag);
    void setXGGZ(bool flag);
    bool getXGGZ();//心轨故障
    void setJGGZ(bool flag);
    bool getJGGZ();//尖轨故障

    void setIsWD_CQ(bool flag);
    bool getIsWD_CQ();//岔前无电
    void setIsWD_DW(bool flag);
    bool getIsWD_DW();//定位无电
    void setIsWD_FW(bool flag);
    bool getIsWD_FW();//反位无电
    void SetJyj(bool b);//绝缘节显示
    //获取确认空闲状态
    bool getFLBLKX();
    //获取道岔分路不良、及空闲状态(暂时用于道岔区段名称显示)
    bool getFLBLOrKX();

    //成员功能函数声明
public:
    void GDDCInit(int type); //轨道道岔初始化
    virtual void Draw(QPainter *painter, long nElapsed, double nDiploid,QPoint offset,int type);
    void Draw_Th_kb(QPainter *painter, long nElapsed, double nDiploid,QPoint offset);
    //void Draw_FLBL_WBX(QPainter *painter,long nElapsed,QPointF pt1,QPointF pt2, double nDiploid,bool KXWZstate);//绘制分录不良外包线
    void Draw_FLBL_WBX(QPainter *painter,long nElapsed,QPointF pt1,QPointF pt2, double nDiploid,int KXWZstate);//绘制分录不良外包线
    void Draw_FLBL_WBX_WD(QPainter *painter,long nElapsed,QPointF pt1,QPointF pt2, double nDiploid,int KXWZstate);//绘制无电情况下分录不良外包线
    void Draw_WBX(QPainter *painter,QPointF pt1,QPointF pt2, double nDiploid,int nLineW,QColor color,int nSpace);//绘制设备外包线
    virtual void Draw_ToolTip(QPainter *painter, double nDiploid);
    virtual unsigned int getDevType();
    virtual int moveCursor(QPoint p);
    virtual void setVollover(QPoint pt_Base);
    void setDCColor(QColor *cq_Color, QColor *dw_Color, QColor *fw_Color); //设置道岔绘制颜色
    //void gddc_StatePro();
    virtual void setDevStateToSafe();

public:
    int m_nQDState;                              // 此组件对象的状态
    int m_nOldQDState;                           // 此组件对象的上一次状态
    QRectF m_DGtextRect;          //设备名称文本区域
    QRectF m_DGtextRectConst;     //设备名称文本区域
    QString m_DGName;
    QRectF m_MouseRect;          //设备鼠标进入区域
    void setDGTextRect(QRectF rect);      //设置设备名称文本区域
    QRectF getDGTextRect();               //获取设备名称文本区域
    void setDGName(QString name);
    QString getDGName();
    void setQDState(int state);
    bool getQDState(int state);
    int getQDState();
    bool GetMousePoint(QPoint p);
    QRectF GetDevRectf(QPointF p12,QPointF p34,QPointF p56,double nDiploid=1.0f);
    QRectF GetMouseRectf();
    QColor getLightState(int nType,int nElapsed);
    //绘制尖轨、心轨故障指示灯
    void DrawLampOfJGXG(QPainter *painter,long nElapsed,double nDiploid, QPoint offset);
    void Draw_circle_Light(QPainter *painter,//画笔
                                  QPointF LightPoint,//指示灯中心坐标
                                  QString rectTextShow, //文字显示
                                  bool Light_color,//指示灯状态
                                  int colorType,//颜色类型
                                  long nElapsed,//闪烁读秒
                                  double nDiploid,//放大倍数
                                  QPoint offset);//绘制指示灯
    void DCAQXBJDRAW(QPainter *painter, double diploid);
    bool ISAQXAlert();
    void DWBS(QPainter *painter,QColor color,int a,int b, double diploid);
    void TXT(QPainter *painter,QColor txtcolor,QColor bkcolor,QString txt,int a,int b,int c, double diploid);

};

#endif // CGDDC_H
