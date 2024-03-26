#ifndef ROUTEORDERWIDGET_H
#define ROUTEORDERWIDGET_H

#include <QWidget>
#include "trainrouteorderwidget.h"
#include "shuntrouteorder.h"
#include "hookorder.h"
#include "TableView/tableview.h"
#include "trainrouteorderwidget.h"

namespace Ui {
class RouteOrderWidget;
}

class RouteOrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RouteOrderWidget(QWidget *parent = nullptr);
    ~RouteOrderWidget();
    void AddRouteOrderData(MyStation* pStation);
    void RouteOrderTableUpData();
    TrainRouteOrderWidget* GetTrainRouteOrderWidget();

protected:
    void timerEvent(QTimerEvent* enent);

private slots:
    void on_comboBox_3_activated(int index);
    void on_comboBox_2_activated(const QString &arg1);
    void on_pushButton_clicked();

private:
    Ui::RouteOrderWidget *ui;
    TrainRouteOrderWidget* pTrainRouteOrder = nullptr;
    MyStation* pCurStation = nullptr;
    int Timer_ID_500 = -1;
    int Timer_ID_500_2 = -1;
    int curPermission = 0;
    bool curFCZKMode = false;
    int curModeState = 0;
};

#endif // ROUTEORDERWIDGET_H
