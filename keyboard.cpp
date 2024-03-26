#include "keyboard.h"
#include "ui_keyboard.h"

KeyBoard::KeyBoard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::KeyBoard)
{
    ui->setupUi(this);
    strAssObjName="";
    this->setWindowFlags(Qt::Widget |Qt::FramelessWindowHint | Qt::Tool);
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_btnGroupClicked(int)));
    connect(ui->pushButton_40, SIGNAL(clicked()), this, SLOT(CapsPressSlot()));
}

KeyBoard::~KeyBoard()
{
    delete ui;
}
void KeyBoard::SetAssObjName(QString str)
{
    strAssObjName=str;
}
//获取关联对象名称
QString KeyBoard::GetAssObjName()
{
    return strAssObjName;
}
void KeyBoard::CapsPressSlot()
{
    //键盘英文变为大写
    QList<QAbstractButton *> buttonList = ui->buttonGroup->buttons();
    QString text;
    for(int i = 0; i < buttonList.size(); ++i)
    {
        text=buttonList[i]->text();
        if (text >= 'a' && text <= 'z')
        {
            text = text.toUpper();
            buttonList[i]->setText(text);
        }
        else if(text >= 'A' && text <= 'Z')
        {
            text = text.toLower();
              buttonList[i]->setText(text);
        }
    }
}
void KeyBoard::slot_btnGroupClicked(int nId)
{
    QString text,text2;
    text=ui->buttonGroup->button(nId)->text();
    if(text=="<---")
    {
       text="<";
    }
    else if(text=="CAPS")
    {

       //text2
    }
    emit recvstrDataSignal(text,GetAssObjName());
}
 void KeyBoard::RecvClosedSlot()
 {
     this->close();
 }
