#include "operationalarmdialog.h"
#include "ui_operationalarmdialog.h"

OperationAlarmDialog::OperationAlarmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OperationAlarmDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
     connect(ui->pushButton,&QPushButton::clicked,[=](){
         done(Accepted); //确定
     });
     connect(ui->pushButton_2,&QPushButton::clicked,[=](){
         done(Rejected); //取消
     });
}

OperationAlarmDialog::~OperationAlarmDialog()
{
    delete ui;
}
void OperationAlarmDialog::InitShow(QString str)
{
    ui->textEdit->setText(str);
}
void OperationAlarmDialog::closeEvent (QCloseEvent *e)
{
    //e->accept();
    done(Rejected); //取消
    e->ignore();
}
