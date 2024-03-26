#include "userexit.h"
#include "ui_userexit.h"

userexit::userexit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userexit)
{
    ui->setupUi(this);
}

userexit::~userexit()
{
    delete ui;
}
