#include "shuntroute.h"
#include "ui_shuntroute.h"
#pragma execution_character_set("utf-8")

ShuntRoute::ShuntRoute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShuntRoute)
{
    ui->setupUi(this);

}

ShuntRoute::~ShuntRoute()
{
    delete ui;
}

HookOrder* ShuntRoute::getHookOrder()
{
    return ui->widget;
}

ShuntRouteOrder* ShuntRoute::getShuntRouteOrder()
{
    return ui->widget_2;
}

void ShuntRoute::PermissionChange(bool bOnlyRead)
{
    if(bOnlyRead)
    {
        ui->groupBox->setTitle("调车进路 只读(不可修改)");
    }
    else
    {
        ui->groupBox->setTitle("调车进路 可修改");
    }
}

