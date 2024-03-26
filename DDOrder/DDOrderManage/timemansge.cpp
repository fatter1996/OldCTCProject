#include "timemansge.h"
#include "ui_timemansge.h"

TimeMansge::TimeMansge(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeMansge)
{
    ui->setupUi(this);
}

TimeMansge::~TimeMansge()
{
    delete ui;
}
