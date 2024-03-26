#include "trainroutehandle.h"
#include "ui_trainroutehandle.h"

TrainRouteHandle::TrainRouteHandle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainRouteHandle)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    setWindowModality(Qt::ApplicationModal);
    setObjectName("trainroutenum");
    setMouseTracking(true);
    installEventFilter(this);
    ui->PassWordTittle->installEventFilter(parent);
    ui->PassWordTittle->setMouseTracking(true);
    ui->lineEdit->setEnabled(true);
    ui->lineEdit->clear();
    //确定
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(ButtonOk()));
    //取消
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(ButtonCancle()));
    mKeyBoard=ui->widget_2;
    mKeyBoard->SetAssObjName("trainroutenum");
    connect(mKeyBoard,SIGNAL(recvstrDataSignal(QString,QString)),this,SLOT(receiveKeyboardDataSlot(QString,QString)));
}

TrainRouteHandle::~TrainRouteHandle()
{
    delete ui;
}
void TrainRouteHandle::windowclosed()
{
    this->close();
    emit ClosedSignal();
}
void TrainRouteHandle::ButtonOk()
{
    this->close();
    emit CheckReturnSignal(ui->lineEdit->text());
    emit ClosedSignal();
}
void TrainRouteHandle::ButtonCancle()
{
    this->close();
    emit CancleSignal();
    //_sleep(5000);
    //emit ClosedSignal();
}
void TrainRouteHandle::receiveKeyboardDataSlot(QString str,QString strObj)
{
    if(strObj==objectName())
    {
        QString text;
        text=ui->lineEdit->text();
        if(str=="<")
        {
            ui->lineEdit->setText(text.left(text.length()-1));
        }
        else
            ui->lineEdit->setText(text+str);
    }
}
void TrainRouteHandle::ClearText()
{
     ui->lineEdit->clear();
     ui->label->clear();
}
void TrainRouteHandle::SetTypeText(QString str)
{
    ui->label->setWordWrap(true);
    ui->label->setFixedWidth(220);
    if(str.length()>22)
    {
       ui->label->setFixedHeight(35);
    }
    else
    {
        int n=ui->label->height();
        ui->label->setFixedHeight(30);
    }
    ui->label->setText(str);
}
void TrainRouteHandle::SetTitle(QString str)
{
    if(str!="")
    {
        ui->PassWordTittle->setText(str);
    }
    else
    {
        ui->PassWordTittle->setText("铅封按钮，请输入密码");
    }
}
QString TrainRouteHandle::GetTrainRouteNum()
{
    return ui->lineEdit->text();
}
