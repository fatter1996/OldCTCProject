#include "tsrs.h"
#include "ui_tsrs.h"

tsrs::tsrs(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tsrs)
{
    ui->setupUi(this);
}

tsrs::~tsrs()
{
    delete ui;
}
