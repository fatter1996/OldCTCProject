#include "addtrain.h"
#include "ui_addtrain.h"
#pragma execution_character_set("utf-8")
AddTrain::AddTrain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddTrain)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Widget |Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    setObjectName("  站内加车");
    mKeyBoard=ui->widget_2;
    mKeyBoard->SetAssObjName(this->objectName());
    bok=false;
    connect(mKeyBoard,SIGNAL(recvstrDataSignal(QString,QString)),this,SLOT(SetCCH(QString,QString)));
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        bok=true;
        emit ReturnAddSignal(GetCCH());
//        emit ClosedSignal();
//        close();
    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        bok=false;
        emit ReturnAddSignal(GetCCH());
//        emit ClosedSignal();
//        close();
    });
}

AddTrain::~AddTrain()
{
    emit ClosedSignal();
    delete ui;
}
//初始化车站 股道编辑框
void AddTrain::Init(QString strSta,QString strGD,bool btitle)
{
    // ui->label_3
    if(btitle)
    {
       ui->label_3->setText("区间加车");
    }
    else
    {
       ui->label_3->setText("站内加车");
    }
    ui->lineEdit_2->setText(strSta);
    ui->lineEdit_3->setText(strGD);
}
//获取车次号
QString AddTrain::GetCCH()
{
    return ui->lineEdit->text();
}
//设置车次号
void AddTrain::SetCCH(QString strCCH,QString strboj)
{
    if(strboj==objectName())
    {
        QString text=GetCCH();
        if(strCCH=="<")
        {
            ui->lineEdit->setText(text.left(text.length()-1));
        }
        else
            ui->lineEdit->setText(text+strCCH);
    }
}
bool AddTrain::GetElectric()
{
    return ui->checkBox->isChecked();
}
bool AddTrain::GetSimTrain()
{
    bool b=ui->checkBox_2->isChecked();
    return !b;
}
