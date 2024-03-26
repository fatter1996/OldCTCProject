#include "changeccproperty.h"
#include "ui_changeccproperty.h"
#pragma execution_character_set("utf-8")
ChangeCCProperty::ChangeCCProperty(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangeCCProperty)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Widget |Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    setObjectName("更改车次属性");
    nlineeditCheck=-1;
    bok=false;
    mKeyBoard=ui->widget_2;
    mKeyBoard->SetAssObjName(this->objectName());
    connect(mKeyBoard,SIGNAL(recvstrDataSignal(QString,QString)),this,SLOT(SetInputStr(QString,QString)));
    //确定按钮
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        bok=true;
        emit ReturnChangeSignal(GetTrainSpeed(),GetTrainNum(),ui->checkBox->isChecked());
       // emit ClosedSignal();
        //close();

    });
    //取消按钮
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        bok=false;
        emit ReturnChangeSignal("","",false);
       // emit ClosedSignal();
       // close();
    });
    connect(ui->lineEdit_2,&QLineEdit::cursorPositionChanged,[=](){
        nlineeditCheck=0;
    });
    connect(ui->lineEdit_3,&QLineEdit::cursorPositionChanged,[=](){
        nlineeditCheck=1;
    });
    ui->lineEdit_2->setFocusPolicy(Qt::ClickFocus);
    ui->lineEdit_2->setText("0");
}

ChangeCCProperty::~ChangeCCProperty()
{
    emit ClosedSignal();
    delete ui;
}
//设置车次号
void ChangeCCProperty::SetCCH(QString str,bool bElectricTraction,QString m_strLocomotiveNumber)
{
    ui->lineEdit->setText(str);
    ui->checkBox->setChecked(bElectricTraction);
    ui->lineEdit_3->setText(m_strLocomotiveNumber);
}

//设置机车速率
void ChangeCCProperty::SetTrainSpeed(QString str)
{
    QString text=GetTrainSpeed();
    if(str=="<")
    {
        ui->lineEdit_2->setText(text.left(text.length()-1));
    }
    else
        ui->lineEdit_2->setText(text+str);
}
//设置机车号
void ChangeCCProperty::SetTrainNum(QString str)
{
    QString text=GetTrainNum();
    if(str=="<")
    {
        ui->lineEdit_3->setText(text.left(text.length()-1));
    }
    else
        ui->lineEdit_3->setText(text+str);
}
//获取机车速率
QString ChangeCCProperty::GetTrainSpeed()
{
    if(ui->lineEdit_2->text()=="0")
    {
        return "";
    }
    else
    {
        return ui->lineEdit_2->text();
    }
}
//获取机车号
QString ChangeCCProperty::GetTrainNum()
{
   return ui->lineEdit_3->text();
}
//设置软键盘输入字符
void ChangeCCProperty::SetInputStr(QString str,QString strboj)
{
    if(strboj==objectName())
    {
        if(nlineeditCheck==0)//设置机车速率
        {
            SetTrainSpeed(str);
        }
        else if(nlineeditCheck==1)//设置机车号
        {
            SetTrainNum(str);
        }
    }

}
bool ChangeCCProperty::GetElectric()
{
    return ui->checkBox->isChecked();
}
