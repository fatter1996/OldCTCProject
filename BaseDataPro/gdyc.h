#ifndef CGDYC_H
#define CGDYC_H

#include <QObject>
#include <QPainter>
#include "gd.h"
#include "gddc.h"

class CGDYC : public QObject
{
    Q_OBJECT
public:
    explicit CGDYC(QObject *parent = nullptr);
    ~CGDYC();
public:
    void Init(int type, QPointF pCenter, QString strGDName, QVector<CBaseData*> vecBaseData, bool bShowFrame = true, CGD* pShowGD  = nullptr);
    //绘制(画笔，计时器，缩放倍数，偏移量)
    void Draw(QPainter *painter, long nElapsed, double nDiploid, QPoint offset);

    void DrawRect(QPainter *painter, long nElapsed, double nDiploid, QPoint offset);
    bool GetMousePoint(QPoint p);
    int getGLXH(int index);

protected:
    virtual void timerEvent(QTimerEvent *event);

private:
    QVector<CGDDC*> m_vecGDDCArray;
    QVector<CGD*> m_vecGDArray;

    QPointF m_pPointConst; //设备基准点，不被修改的，缩放及偏移使用
    QPoint m_pPoint; //设备基准点，绘制的
    QRect m_RectBtn; //按钮范围
    bool m_bShowFrame = true;

public:
    int m_nType = 0; //1-腰岔,2-股道确认
    bool m_bBtnIsDown = false;   //按钮是否按下
    QString m_strGD; //关联股道名称
    int m_nTime = 0;
    int m_nState = 0;
};

#endif // CGDYC_H
