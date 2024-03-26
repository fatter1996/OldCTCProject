#ifndef SHUNTROUTEORDER_H
#define SHUNTROUTEORDER_H

#include <QWidget>
#include "TableView/tableview.h"

namespace Ui {
class ShuntRouteOrder;
}

class ShuntRouteOrder : public QWidget
{
    Q_OBJECT

public:
    explicit ShuntRouteOrder(QWidget *parent = nullptr);
    ~ShuntRouteOrder();

    void InitTableViewShunt();
    void SetTableStyleSheet();
    void FontReSize(int fontSize, int height = 24, double diploid = 1.0f);

private:
    Ui::ShuntRouteOrder *ui;
    TableView* mCTableViewShunt;
    int nFontSize = 9;
};

#endif // SHUNTROUTEORDER_H
