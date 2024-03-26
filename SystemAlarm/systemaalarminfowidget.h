#ifndef SYSTEMAALARMINFOWIDGET_H
#define SYSTEMAALARMINFOWIDGET_H

#include <QWidget>
#include "GlobalHeaders/GlobalStructure.h"
namespace Ui {
class SystemaAlarmInfoWidget;
}

class SystemaAlarmInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SystemaAlarmInfoWidget(QWidget *parent = nullptr);
    ~SystemaAlarmInfoWidget();
    //TableData mTableData;

    void InitData(TableData mTableData);
private:
    Ui::SystemaAlarmInfoWidget *ui;
};

#endif // SYSTEMAALARMINFOWIDGET_H
