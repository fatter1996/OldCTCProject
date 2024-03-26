#ifndef MULTISTATIONDISPKSK_H
#define MULTISTATIONDISPKSK_H

#include <QWidget>
#include "StationInterface/multistationdisp.h"
#include "StagePlan/stageplanwidget.h"
//站间透明窗口
namespace Ui {
class MultiStationDispKSK;
}

class MultiStationDispKSK : public MultiStationDisp
{
    Q_OBJECT

public:
    explicit MultiStationDispKSK(QWidget *parent = nullptr);
    ~MultiStationDispKSK();

    void AddMultiStationInfo(MyStation* station, int index) override;
    void StationViewResize(int nEnlargeNarrow = 0) override;
    void TrainNOViewResize(int nEnlargeNarrow = 0) override;
    void WidgetReSize() override;
    void TrainNumWndShowStateChenge(bool b) override;

    void TextShowSolt(QString name,bool b);
    //进路预告窗显示状态
    void RoutePreWndShowStateChenge(bool b) override;

private:
    bool eventFilter(QObject* obj,QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;//定时器
public:
    bool bShowJYJ = true;//绝缘节显示标志
    bool bShowCheCi = false;//车次显示标志
    bool bRoutePreWndShowState = true;//进路预告窗

private:

    int maxWidth = 0;
    int maxHeight = 0;
    QWidget* curWidget = nullptr;
    bool isRePos = false;

    QPoint curPoint = { 0,0 };
    QPoint curMousePoint = { 0,0 };
    QSize initialSize = { 0,0 };
    double diploid = 1.0f;

    int Timer_ID_500 = 0;
    int nElapsed = 0;
    //接通光带时间计数
    int nLightband_30=0;
    int nLightband_15=0;
    //接通光带
    bool BLightband_30=false;
    bool BLightband_15=false;
private:
    Ui::MultiStationDispKSK *ui;
};

#endif // MULTISTATIONDISPKSK_H
