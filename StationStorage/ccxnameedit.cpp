#include "ccxnameedit.h"
#include "ui_ccxnameedit.h"

ccxnameedit::ccxnameedit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ccxnameedit)
{
    ui->setupUi(this);
}

ccxnameedit::~ccxnameedit()
{
    delete ui;
}
