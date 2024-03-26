#ifndef TRAFFICLOGMANAGE_H
#define TRAFFICLOGMANAGE_H

#include <QWidget>
#include "MyStation/mystation.h"
class TrafficLogManage : public QWidget
{
    Q_OBJECT
public:
    explicit TrafficLogManage(QWidget *parent = nullptr);
    ~TrafficLogManage();
    void SetCurStation(MyStation* pStation, bool isShowSignRouteSyn) {
        pCurStation = pStation;
        m_bShowSignRouteSyn = isShowSignRouteSyn;
    }

    //邻站报点信息
    virtual void AdjacentStationReportingPoint(int type, int logId) = 0;
signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
public slots:
    //邻站预告
    virtual void AdjacentStationPreview() = 0;
    //同意
    virtual void AgreeAdjacentStations(QDateTime = QDateTime()) = 0;
    //到达
    virtual void TrainArrival(QDateTime = QDateTime()) = 0;
    //出发
    virtual void TrainDeparture(QDateTime = QDateTime()) = 0;
    //通过
    virtual void TrainPassThrough(QDateTime = QDateTime()) = 0;
    //邻站
    virtual void AdjacentStation(QDateTime = QDateTime()) = 0;
    //取消接车
    virtual void CancelReceiving() = 0;
    //取消闭塞
    virtual void CancelBlock() = 0;
    //取消发车
    virtual void CancelDepart() = 0;
    //邻站出发
    virtual void AdjacentStationDepart(QDateTime = QDateTime()) = 0;
    //邻站到达
    virtual void AdjacentStationArrival(QDateTime = QDateTime()) = 0;

protected:
    MyStation* pCurStation = nullptr;
    bool m_bShowSignRouteSyn = false;
};

#endif // TRAFFICLOGMANAGE_H
