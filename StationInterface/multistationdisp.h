#ifndef MULTISTATIONDISP_H
#define MULTISTATIONDISP_H

#include <QWidget>
#include "MyStation/mystation.h"
//站间透明基类
class MultiStationDisp : public QWidget
{
    Q_OBJECT
public:
    explicit MultiStationDisp(QWidget *parent = nullptr);
    ~MultiStationDisp();

    virtual void AddMultiStationInfo(MyStation* station, int index) = 0;
    virtual void StationViewResize(int nEnlargeNarrow = 0) = 0;
    virtual void TrainNOViewResize(int nEnlargeNarrow = 0) = 0;
    virtual void WidgetReSize() = 0;
    virtual void TrainNumWndShowStateChenge(bool b) = 0;
    virtual void RoutePreWndShowStateChenge(bool b) = 0;
signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
public:
    struct StationInfo
    {
        int stationId = 0;
        QString name;
        QRect route;
        MyStation* pStation = nullptr;
        //double diploid = 1;
        QWidget* wnd = nullptr;
        uint isIsOverturn = 0;
        QPoint offsetPt = QPoint(0,0);
        double factor = 0.6f;
    };
    QVector<StationInfo> pVecStationInfo;

 public:
    bool isStationViewReSize = false;
    bool isTrainNumWndReSize = false;
    int nEnlargeNarrow = 0;
};

#endif // MULTISTATIONDISP_H
