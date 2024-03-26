#include "confirmcch.h"
#include "ui_confirmcch.h"

#pragma execution_character_set("utf-8")
ConfirmCCH::ConfirmCCH(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfirmCCH)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Widget |Qt::FramelessWindowHint | Qt::Tool);
    bok=false;
    ntype=-1;
    mKeyBoard=ui->widget_2;
    setObjectName("  车次号操作");
    mKeyBoard->SetAssObjName(this->objectName());
    this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
    connect(mKeyBoard,SIGNAL(recvstrDataSignal(QString,QString)),this,SLOT(SetNewCCH(QString,QString)));

    ui->lineEdit->setReadOnly(true);//禁止编辑
    ui->lineEdit->setEnabled(false);
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        /*在新车次号的输入框中输入车次号，并
        点击确定。如果新旧车次号相同，表示车次号确认，否则表示车次号更改。*/
        /*修正车次号完成和车次号确认一样的功能*/
        /*变更车次号：弹出的对话框和车次号确认一样，但是其原理却不同，变更
        车次号操作是先将原车次号删除，然后再添加一个新的车次号。*/
        bok=true;
        emit ReturnConfirmCCHSignal(GetNewCCH());
        //emit ClosedSignal();
    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        bok=false;
        emit ReturnConfirmCCHSignal(GetNewCCH());
       // emit ClosedSignal();
    });
}

ConfirmCCH::~ConfirmCCH()
{
    emit ClosedSignal();
    delete ui;
}
void ConfirmCCH::Init(QString type,QString strTitel,QString strOld)
{
    SetTitel(strTitel);
    SetOldCCH(strOld);
    ui->lineEdit_2->setText(strOld);
    if(type.contains("车次号确认"))
    {
     // ui->lineEdit_2->setEnabled(false);
     // ui->lineEdit_2->setReadOnly(false);
      ntype=1;
    }
    else
    {
      ntype=0;
    }
}
//设置标题
void ConfirmCCH::SetTitel(QString strTitel)
{
    ui->label_3->setText(strTitel);
}
//设置旧车次号
void ConfirmCCH::SetOldCCH(QString strOld)
{
    ui->lineEdit->setText(strOld);
}
//设置新车次号
void ConfirmCCH::SetNewCCH(QString strNew,QString strboj)
{
    if(strboj==objectName()/*&& ntype!=1*/)
    {
        QString text=GetNewCCH();
        if(strNew=="<")
        {
            ui->lineEdit_2->setText(text.left(text.length()-1));
        }
        else
            ui->lineEdit_2->setText(text+strNew);
    }

}
//获取旧车次号
QString ConfirmCCH::GetOldCCH()
{
    return ui->lineEdit->text();
}
//获取新车次号
QString ConfirmCCH::GetNewCCH()
{
    return ui->lineEdit_2->text();
}
void ConfirmCCH::windowclosed()
{
    this->close();
}
