#include "passwordentrydlg.h"
#include "ui_passwordentrydlg.h"
#include <QPushButton>
#include <QStyle>
#include <QMessageBox>
#include<QDebug>
#include <QMoveEvent>
#pragma execution_character_set("utf-8")
PassWordEntryDlg::PassWordEntryDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PassWordEntryDlg)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    setWindowModality(Qt::ApplicationModal);
    //setObjectName("");
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

    connect(mKeyBoard,SIGNAL(recvstrDataSignal(QString,QString)),this,SLOT(receiveKeyboardDataSlot(QString,QString)));
    //connect(this,SIGNAL(ClosedSignal()),mKeyBoard,SLOT(RecvClosedSlot()));
    //connect(this,SIGNAL(CheckReturnSignal(bool)),this,SLOT(PassWordResult(bool)));
    onOK = false;
}

PassWordEntryDlg::~PassWordEntryDlg()
{
    delete ui;
}
void PassWordEntryDlg::windowclosed()
{
    this->close();
    emit ClosedSignal();
}
void PassWordEntryDlg::ButtonOk()
{
    QString aa=ui->lineEdit->text();
    if(ui->lineEdit->text()!="123")
    {
         QMessageBox::information(nullptr,"错误","密码输入错误!",tr("确定"),tr("取消"),"",0);
    }
    else
    {
        onOK = true;
        this->close();
        emit CheckReturnSignal(true);
        //_sleep(5000);
        emit ClosedSignal();
    }
}
void PassWordEntryDlg::ButtonCancle()
{
    this->close();
    //_sleep(5000);
    emit ClosedSignal();
}
void PassWordEntryDlg::receiveKeyboardDataSlot(QString str,QString strObj)
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
        {
             ui->lineEdit->setText(text+str);
        }

    }
}
void PassWordEntryDlg::ClearText()
{
     ui->lineEdit->clear();
}
void PassWordEntryDlg::SetTypeText(QString str)
{
    ui->label->setText(str);
}
void PassWordEntryDlg::SetTitle(QString str)
{
    if(str!="")
    {
        ui->PassWordTittle->setText( str);
    }
    else
    {
        ui->PassWordTittle->setText(" 铅封按钮，请输入密码");
    }
}

//void PassWordEntryDlg::on_pushButton_clicked()
//{
//    ButtonOk();
//}
