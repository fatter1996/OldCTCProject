#ifndef TRAINROUTEORDERWIDGET_H
#define TRAINROUTEORDERWIDGET_H

#include <QWidget>
#include "TableView/tableview.h"
#include "routeorderdetail.h"

namespace Ui {
class TrainRouteOrderWidget;
}


class TrainRouteOrderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TrainRouteOrderWidget(QWidget *parent = nullptr);
    ~TrainRouteOrderWidget();

    void InitTableViewTrain();
    void SetTableStyleSheet();
    void InitTableViewMenu(MyStation* pStation);
    void RouteOrderUpData();
    void RouteOrderTableDataUpData(TrainRouteOrder* order, QStringList& strList, int& row);
    void RouteOrderTableColorUpData(TrainRouteOrder* order);
    void FontReSize(int fontSize, int height = 24, double diploid = 1.0f);
    void ClearData();
    void PermissionChange(bool bOnlyRead);
    void RouteOrderSort(QVector<TrainRouteOrder*>::iterator low, QVector<TrainRouteOrder*>::iterator high);
    bool RouteOrderCompare(TrainRouteOrder* order1, TrainRouteOrder* order2);
    TrainRouteOrder* getRouteOrderByRowIndex(int row);

public slots:
    void TableViewLeftMenuInit(const QModelIndex &index);
    void TableViewRightMenuInit(const QModelIndex &index);

signals:
    void sendRouteOrderMsg(int type, int routeId, int nData = 0, QString strData = "");
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private:
    Ui::TrainRouteOrderWidget *ui;
    TableView* mCTableViewTrain = nullptr;
    RouteOrderDetail* pRouteOrderDetail = nullptr;
    MyStation* pMyStation = nullptr;
    int nFontSize = 9;
};

#endif // TRAINROUTEORDER_H
