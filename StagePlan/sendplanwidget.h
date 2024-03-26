#ifndef SENDPLANWIDGET_H
#define SENDPLANWIDGET_H

#include <QWidget>
#include "TableView/tableview.h"
#include "MyStation/mystation.h"

namespace Ui {
class SendPlanWidget;
}

class SendPlanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendPlanWidget(QWidget *parent = nullptr);
    ~SendPlanWidget();

    void InitTableView();
    void InitTableView2();
    void SendPlanUpdata();
    void InitStation(MyStation* pMyStation) { m_pMyStation = pMyStation; }

signals:
    void sendPlan();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::SendPlanWidget *ui;
    TableView* mCTableView = nullptr;
    TableView* mCTableView2 = nullptr;
    MyStation* m_pMyStation = nullptr;
};

#endif // SENDPLANWIDGET_H
