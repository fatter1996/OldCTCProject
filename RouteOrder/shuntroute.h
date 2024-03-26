#ifndef SHUNTROUTEORDERWIDGET_H
#define SHUNTROUTEORDERWIDGET_H

#include <QWidget>
#include "hookorder.h"
#include "shuntrouteorder.h"

namespace Ui {
class ShuntRoute;
}

class ShuntRoute : public QWidget
{
    Q_OBJECT

public:
    explicit ShuntRoute(QWidget *parent = nullptr);
    ~ShuntRoute();  

    HookOrder* getHookOrder();
    ShuntRouteOrder* getShuntRouteOrder();
    void PermissionChange(bool bOnlyRead);

private:
    Ui::ShuntRoute *ui;
};

#endif // SHUNTROUTEORDERWIDGET_H
