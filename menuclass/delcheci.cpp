#include "delcheci.h"
#include "ui_delcheci.h"

DelCheCi::DelCheCi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DelCheCi)
{
    ui->setupUi(this);
    //去掉最大化、最小化按钮，保留关闭按钮
    this->setWindowFlags(Qt::WindowCloseButtonHint);
    //ui->lineEdit->setReadOnly(true);//禁止编辑
    ui->lineEdit->setEnabled(false);
    //确定
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        bok=true;
        emit ReturnResultSignal();
    });
    //取消
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        bok=false;
        emit ReturnResultSignal();
    });
}

DelCheCi::~DelCheCi()
{
    delete ui;
}
void DelCheCi::Init(QString str)
{
     bok=false;
    ui->lineEdit->setText(str);
}
