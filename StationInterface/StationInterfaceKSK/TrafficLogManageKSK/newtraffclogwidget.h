#ifndef NEWTRAFFCLOGWIDGET_H
#define NEWTRAFFCLOGWIDGET_H

#include <QWidget>
#include "MyStation/mystation.h"

namespace Ui {
class NewTraffclogWidget;
}

class NewTraffclogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NewTraffclogWidget(QWidget *parent = nullptr);
    ~NewTraffclogWidget();

    void Init(MyStation* pStation, bool isChenge = false);
    void InitTraffclog(TrafficLog* pTrafficLog);
signals:
    void addNewTraffclog(TrafficLog pTrafficLog);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::NewTraffclogWidget *ui;
    MyStation* pCurStation = nullptr;
    bool m_bIsChenge = false;
    QString m_strReachTrainNumOld;
    QString m_strDepartTrainNumOld;
    TrafficLog* m_pTrafficLog = nullptr;
};

#endif // NEWTRAFFCLOGWIDGET_H
