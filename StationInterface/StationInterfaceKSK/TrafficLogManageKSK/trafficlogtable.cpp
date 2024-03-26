#include "trafficlogtable.h"
#include "ui_trafficlogtable.h"

TrafficLogTable::TrafficLogTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrafficLogTable)
{
    ui->setupUi(this);

}

TrafficLogTable::~TrafficLogTable()
{
    delete ui;
}
