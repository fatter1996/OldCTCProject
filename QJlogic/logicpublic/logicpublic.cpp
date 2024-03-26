#include "logicpublic.h"
#include "ui_logicpublic.h"

logicpublic::logicpublic(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::logicpublic)
{
    ui->setupUi(this);
}

logicpublic::~logicpublic()
{
    delete ui;
}
