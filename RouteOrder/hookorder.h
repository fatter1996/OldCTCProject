#ifndef HOOKORDER_H
#define HOOKORDER_H

#include <QWidget>
#include "TableView/tableview.h"

namespace Ui {
class HookOrder;
}

class HookOrder : public QWidget
{
    Q_OBJECT

public:
    explicit HookOrder(QWidget *parent = nullptr);
    ~HookOrder();

    void InitTableViewHook();
    void SetTableStyleSheet();
    void FontReSize(int fontSize, int height = 24, double diploid = 1.0f);

private:
    Ui::HookOrder *ui;
    TableView *mCTableViewHook;
    int nFontSize = 9;
};

#endif // HOOKORDER_H
