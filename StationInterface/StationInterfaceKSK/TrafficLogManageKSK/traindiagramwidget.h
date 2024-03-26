#ifndef TRAINDIAGRAMWIDGET_H
#define TRAINDIAGRAMWIDGET_H

#include <QWidget>
#include "MyStation/mystation.h"
#define SPACING_LEFTORRIGHT 2
#define SPACING_TOPORBOTTOM 60
#define SPACING_TIMEAXIS 50

namespace Ui {
class TrainDiagramWidget;
}

class TrainDiagramWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainDiagramWidget(QWidget *parent = nullptr);
    ~TrainDiagramWidget();
    void Init(MyStation* pMyStation);
    void drawTimeAxis(bool isTop = true);
    void drawGrid();
    void drawStationName();
    void drawNowTimeLine();
    void drawTrainPlan();
    void drawTrainNum(QPainter* painter, int X, int Y, QString trainNum, bool isUpgoing = true);
    void drawEndFlag(QPainter* painter, int X, int Y, bool isUpgoing = true);
    int getPointXByTime(QTime time);
    int getDirectionIndex(TrafficLog* pTrafficLog);

    void setPenInfoByPoint(QPainter* painter, bool isNull, int nLHFlg);
protected:
    bool eventFilter(QObject* obj,QEvent* event) override;
    void timerEvent(QTimerEvent *event) override;
private:
    Ui::TrainDiagramWidget *ui;
    MyStation* curStation = nullptr;
    int maxStationWidgetWidth = 0;
    int Timer_ID_60000; //进路序列刷新计时器
    int nowTimeLinePx = 0;
    QTime nowTime;
};

#endif // TRAINDIAGRAMWIDGET_H
