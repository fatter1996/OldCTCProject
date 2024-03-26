#ifndef MANUALPOINTREPORTINGWIDGERT_H
#define MANUALPOINTREPORTINGWIDGERT_H

#include <QWidget>
#include "MyStation/mystation.h"

namespace Ui {
class ManualPointReportingWidgert;
}

class ManualPointReportingWidgert : public QWidget
{
    Q_OBJECT

public:
    explicit ManualPointReportingWidgert(QWidget *parent = nullptr);
    ~ManualPointReportingWidgert();

    void Init(MyStation* pStation, TrafficLog* pTrafficLog, int type);
signals:
    void reportingPoints(QDateTime, QDateTime);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private:
    Ui::ManualPointReportingWidgert *ui;
    QDateTime pointsTime;
};

#endif // MANUALPOINTREPORTINGWIDGERT_H
