#include "confirmidle.h"
#include "ui_confirmidle.h"

ConfirmIdle::ConfirmIdle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfirmIdle)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    Init();
}

ConfirmIdle::~ConfirmIdle()
{
    delete ui;
}
void ConfirmIdle::Init()
{


}
void ConfirmIdle::InitShow(int ntpye,QString str,bool ncheckCQ,bool ncheckDW,bool ncheckFW)
{
    ui->checkBox->setCheckable(true);
    ui->checkBox->setChecked(false);
    //nWidgettpye=ntpye;
    if(ntpye==Dev_DC)
    {
        ui->checkBox->setEnabled(false);
        str="道岔："+str;
        if(ui->checkBox->isChecked())
        {
            ui->lineEdit_2->setText("√分路不良已设置");
        }
        else
        {
            ui->lineEdit_2->setText("请设置分路不良状态");
        }
    }
    else if(ntpye==Dev_GD)
    {
        ui->checkBox->setEnabled(true);

        ui->checkBox->setChecked(true);
        str="股道："+str;
        ui->lineEdit_2->setText("√分路不良已设置");
    }
    ui->lineEdit->setText(str);
}
