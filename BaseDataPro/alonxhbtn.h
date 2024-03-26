#ifndef ALONXHBTN_H
#define ALONXHBTN_H

#include "BaseData.h"
#include <QObject>
#include <QRect>

//信号按钮（独立的）
class AlonXHBtn : public CBaseData
{
public:
    explicit AlonXHBtn();

signals:
//成员变量声明
public:
    int     m_nANTYPE;//信号按钮类型（调车按钮/列车按钮）
    QPoint  m_pCenter;//按钮中心点
//    QString m_strName;//按钮名称
    QRect   m_rectBtn;//按钮轮廓坐标
//    QRect   m_textRect;//此组件对象的名称区域
//    bool    m_RangeVisible;//范围显示
    bool    m_nBtnIsDown;//是否按下
    bool    m_nBtnFlash;//是否闪烁
    bool    m_nFuncLockState;//封锁状态
//    int     m_nCode;
    bool    m_bNameUp;//名称是否在上方
    bool bZDFlash;

    QPoint p_ButtonPt;//按钮坐标
    QPoint p_ButtonPtConst;
    QRect rect_Button;//按钮区域
    int isDown_Button;
    int m_nDownTime_1 = 0;   //当按钮按下为1时持续时间

    QString m_strType;		//按钮类型
    QString m_strJuncType;	//关联设备或类型
    QString Button_Name;

    int ButtonForm;//按钮形状,0为圆,1为方;
    bool bBtnNameShow=true;//按钮名称显示
    int wordcolor;//文字颜色
    int wordsize;//文字大小


public:
    void setButtonPt(QPoint pt);
    QPoint getButtonPt();

    void setIsDown_Button(int flag);
    int getIsDown_Button();


public:
    virtual void Draw(QPainter *painter,long nElapsed, double nDiploid,QPoint offset,int type);
    virtual unsigned int getDevType();
    virtual void setVollover(QPoint pt_Base);

    void Draw_Th_kb(QPainter *painter,long nElapsed, double nDiploid,QPoint offset);

    void Draw_circle_Button(QPainter *painter,//画笔
                            QRectF m_ButtonRect,//按钮绘制区域
                            bool m_FCZKState,//非常站控状态
                            int m_ButtonIsDownState,//按钮是否按下
                            bool m_FSState,//封锁状态
                            bool m_bZDANFlash,//终端闪烁状态
                            int mTimeCount,//倒计时时间
                            QString rectTextShow, //按钮文字显示
                            bool RangeVisible,//按钮是否高亮
                            long nElapsed,//闪烁读秒
                            double nDiploid,//放大倍数
                            QPoint offset);//绘制圆形按钮

    void Draw_Rect_Button(QPainter *painter,//画笔
                          QRectF m_ButtonRect,//按钮绘制区域
                          bool m_FCZKState,//非常站控状态
                          int m_ButtonIsDownState,//按钮是否按下
                          bool m_FSState,//封锁状态
                          bool m_bZDANFlash,//终端闪烁状态
                          int mTimeCount,//倒计时时间
                          QColor color,//按钮颜色
                          QString rectTextShow, //按钮文字显示
                          long nElapsed,//闪烁读秒
                          double nDiploid,//放大倍数
                          QPoint offset);//绘制KSK方形按钮
    QColor getLightState(int nType,int nElapsed);
    bool GetMousePoint(QPoint p);










};

#endif // ALONXHBTN_H
