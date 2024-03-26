#include "ddordernosignwidget.h"
#include "ui_ddordernosignwidget.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")
DDOrderNoSignWidget::DDOrderNoSignWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DDOrderNoSignWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
}
void DDOrderNoSignWidget::Init()
{
    ui->lineEdit->clear();
    bok=false;
}
bool DDOrderNoSignWidget::GetPassWord()
{
    if(ui->lineEdit->text()=="123")
    {
         ui->lineEdit->clear();
        return true;
    }
    else
    {
        QMessageBox::critical(this, tr("错误"), tr("密码错误"),tr("确定"),tr("取消"),"",0);
        ui->lineEdit->clear();
        return false;
    }
}
DDOrderNoSignWidget::~DDOrderNoSignWidget()
{
    delete ui;
}
//确定
void DDOrderNoSignWidget::on_pushButton_clicked()
{
    bok=GetPassWord();
    close();
}
//取消
void DDOrderNoSignWidget::on_pushButton_2_clicked()
{
    bok=false;
    close();
}
