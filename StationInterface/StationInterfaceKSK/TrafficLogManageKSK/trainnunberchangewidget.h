#ifndef TRAINNUNBERCHANGEWIDGET_H
#define TRAINNUNBERCHANGEWIDGET_H

#include <QWidget>
#include "MyStation/mystation.h"

namespace Ui {
class TrainNunberChangeWidget;
}

class TrainNunberChangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainNunberChangeWidget(QWidget *parent = nullptr);
    ~TrainNunberChangeWidget();
    void Init(MyStation* pMyStation, TrafficLog* pTraffic);
signals:
    void trainNunberChange(QString, QString, bool, bool);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private:
    Ui::TrainNunberChangeWidget *ui;
};

#endif // TRAINNUNBERCHANGEWIDGET_H
