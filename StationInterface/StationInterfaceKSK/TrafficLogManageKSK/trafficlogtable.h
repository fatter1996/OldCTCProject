#ifndef TRAFFICLOGTABLE_H
#define TRAFFICLOGTABLE_H

#include <QWidget>
#include <TableForm/FormList.h>
#include <QVector>
#include <QLayout>
namespace Ui {
class TrafficLogTable;
}

class TrafficLogTable : public QWidget
{
    Q_OBJECT

public:
    explicit TrafficLogTable(QWidget *parent = nullptr);
    ~TrafficLogTable();


private:
    Ui::TrafficLogTable *ui;
};

#endif // TRAFFICLOGTABLE_H
