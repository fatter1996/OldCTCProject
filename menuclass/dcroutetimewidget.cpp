#include "dcroutetimewidget.h"
#include "ui_dcroutetimewidget.h"
#pragma execution_character_set("utf-8")
DCRouteTimeWidget::DCRouteTimeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DCRouteTimeWidget)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
     //this->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    //确定
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonOk()));
    //取消
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(ButtonCancle()));
}

DCRouteTimeWidget::~DCRouteTimeWidget()
{
    delete ui;
}
void DCRouteTimeWidget::Init(QString str)
{
    ui->lineEdit->setText("0");
    ui->label->setWordWrap(true);
    ui->label->setFixedWidth(220);
    ui->label->setText(QString("%1调车进路预计占用时间").arg(str));
    ui->radioButton->setChecked(true);
}
QString DCRouteTimeWidget::GetTimeAndType()
{
    QString str="";
    if(ui->radioButton->isChecked())
    {
        str="电力";
    }
    else
    {
        str="内燃";
    }
    str=str+ui->lineEdit->text();
    return str;
}
void DCRouteTimeWidget::ButtonOk()
{
    this->close();
    emit ClosedSignal();
}
void DCRouteTimeWidget::ButtonCancle()
{
    this->close();
    emit CancleSignal();
    //_sleep(5000);
    //emit ClosedSignal();
}
