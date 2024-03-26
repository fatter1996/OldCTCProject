#include "systemaalarminfowidget.h"
#include "ui_systemaalarminfowidget.h"

SystemaAlarmInfoWidget::SystemaAlarmInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SystemaAlarmInfoWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    //关闭
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
      close();
    });
}

SystemaAlarmInfoWidget::~SystemaAlarmInfoWidget()
{
    delete ui;
}
void SystemaAlarmInfoWidget::InitData(TableData mTableData)
{
    QStringList strlist=mTableData.strData.split(" ");
    QString strdate=strlist[0];
    QString strtime=strlist[1];
    QString strLevel="";
    //日期
    ui->lineEdit->setText(strdate);
    //时间
    ui->lineEdit_3->setText(strtime);
    //报警级别
    if(mTableData.strLevel=="错误")
    {
        strLevel="1级";
    }
    else if(mTableData.strLevel=="警告")
    {
        strLevel="2级";
    }
    else if(mTableData.strLevel=="信息")
    {
        strLevel="3级";
    }
    else
    {
        strLevel=mTableData.strLevel;
    }
    ui->lineEdit_2->setText(strLevel);
    //报警类型
    ui->lineEdit_4->setText(mTableData.strType);
    //报警类容
    ui->textEdit->setText(mTableData.strDescribe);
    //发生地点
    ui->textEdit_2->setText(" ");
}
