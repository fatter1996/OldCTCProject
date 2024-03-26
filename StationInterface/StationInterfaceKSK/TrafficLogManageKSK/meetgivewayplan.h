#ifndef MEETGIVEWAYPLAN_H
#define MEETGIVEWAYPLAN_H

#include <QWidget>
#include "TableView/tableview.h"

namespace Ui {
class MeetGiveWayPlan;
}

class MeetGiveWayPlan : public QWidget
{
    Q_OBJECT

public:
    explicit MeetGiveWayPlan(QWidget *parent = nullptr);
    ~MeetGiveWayPlan();

    void InitTableView();
    void SetTableStyleSheet();
signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private slots:
    void on_pushButton_clicked();

private:
    Ui::MeetGiveWayPlan *ui;
    TableView* pMeetGiveWayTable = nullptr;
};

#endif // MEETGIVEWATPLAN_H
