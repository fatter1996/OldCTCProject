#ifndef ADJACENTSTATIONCHANGE_H
#define ADJACENTSTATIONCHANGE_H

#include <QWidget>
#include "MyStation/mystation.h"

namespace Ui {
class AdjacentStationChange;
}

class AdjacentStationChange : public QWidget
{
    Q_OBJECT

public:
    explicit AdjacentStationChange(QWidget *parent = nullptr);
    ~AdjacentStationChange();
    void Init(MyStation* pMyStation, TrafficLog* pTraffic);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
signals:
    void AdjStationChange(QString, QString);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private:
    Ui::AdjacentStationChange *ui;
};

#endif // ADJACENTSTATIONCHANGE_H
