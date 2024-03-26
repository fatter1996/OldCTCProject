#ifndef TRAINMARSHAL_H
#define TRAINMARSHAL_H

#include <QWidget>
#include "TableView/tableview.h"
namespace Ui {
class TrainMarshal;
}

class TrainMarshal : public QWidget
{
    Q_OBJECT

public:
    explicit TrainMarshal(QWidget *parent = nullptr);
    ~TrainMarshal();
    void Init();
    void InitTableView();
 TableView* mCTableView;
private:
    Ui::TrainMarshal *ui;
};

#endif // TRAINMARSHAL_H
