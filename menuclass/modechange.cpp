#include "modechange.h"
#include "ui_modechange.h"
#include <QFrame>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <qdebug.h>
#include "BaseDataPro/lamp.h"

#include <QMessageBox>
#pragma execution_character_set("utf-8")
ModeChange::ModeChange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModeChange)
{
    ui->setupUi(this);
    this->setContentsMargins(5,0,5,9);
    setWindowModality(Qt::ApplicationModal);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    nMode=0;
    nCheckState=-1;
    layoutv=new QVBoxLayout(ui->widget);
    layoutv->setSpacing(0);
    layoutv->setContentsMargins(1,1,1,1);

    connect(ui->pushButton,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(77, QCursor::pos());
        if(WindowShowType==0)
        {
            if((nMode==0x00&&nCheckState==0x02)||(nMode==0x02&&nCheckState==0x00))
            {
                  QMessageBox::warning(this, tr("CTC-SPTC"), tr("当前控制模式转换无权申请"),tr("确定"),tr("取消"),"",0);
            }
            else
            {
                emit SendStationCtrModeChangeSignal(0x01,nCheckState,0);
                close();
            }
        }
        else if(WindowShowType==1)
        {
            if(bargee)
            {
                emit SendStationCtrModeChangeSignal(0x01,nCheckState,0);
                close();
            }
            else
            {
                close();
            }

        }


    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        close();
    });
}

ModeChange::~ModeChange()
{
    delete ui;
}
void ModeChange::SetMode(QString str,int mode,int type)
{
    nMode=mode;
    WindowShowType=type;
    bargee=false;
    Init(type);
    if(nMode!=0x22)
    {
        AddData(type,str,nMode+1);
    }
}
void ModeChange::Init(int ntype)
{
    if(!layoutv->isEmpty())
    {
        delete layoutv;
        layoutv=new QVBoxLayout(ui->widget);
        layoutv->setSpacing(0);
        layoutv->setContentsMargins(1,1,1,1);
    }
    strStationName.clear();
    strStationName.append("");
    QWidget *w1= new QWidget(ui->widget);
    w1->setObjectName("");
    QHBoxLayout *layout1=new QHBoxLayout();
    layout1->setDirection(QBoxLayout::LeftToRight);
    QFrame *f1= new QFrame(ui->widget);
    f1->setFrameShape(QFrame::Box);
    f1->setFrameShadow(QFrame::Raised);
    f1->setStyleSheet("background-color: rgb(255, 235, 247);");
    f1->setFixedSize(100,35);
    QFrame *f3= new QFrame(ui->widget);
    QFrame *f4= new QFrame(ui->widget);
    QFrame *f5= new QFrame(ui->widget);
    f3->setFrameShape(QFrame::Box);
    f3->setFrameShadow(QFrame::Raised);
    f4->setFrameShape(QFrame::Box);
    f4->setFrameShadow(QFrame::Raised);
    f5->setFrameShape(QFrame::Box);
    f5->setFrameShadow(QFrame::Raised);
    f3->setStyleSheet("background-color: rgb(255, 235, 247);");
    f4->setStyleSheet("background-color: rgb(255, 235, 247);");
    f5->setStyleSheet("background-color: rgb(255, 235, 247);");
    f3->setFixedSize(90,35);
    f4->setFixedSize(90,35);
    f5->setFixedSize(90,35);
    QLabel *lab1=new QLabel(ui->widget);
    lab1->setText("站名");
    QRadioButton *radio1=new QRadioButton(ui->widget);
    radio1->setText("全选");
    radio1->setObjectName("中心控制");
    QRadioButton *radio2=new QRadioButton(ui->widget);
    radio2->setText("全选");
    radio2->setObjectName("车站控制");
    QRadioButton *radio3=new QRadioButton(ui->widget);
    radio3->setText("全选");
    radio3->setObjectName("车站调车");
    connect(radio1,&QRadioButton::clicked,this,[=](){
        if(radio1->isChecked())
        {
            SetAllCheck(1,radio1->objectName());
        }
    });
    connect(radio2,&QRadioButton::clicked,this,[=](){
        if(radio2->isChecked())
        {
            SetAllCheck(1,radio2->objectName());
        }
    });
    connect(radio3,&QRadioButton::clicked,this,[=](){
        if(radio3->isChecked())
        {
            SetAllCheck(1,radio3->objectName());
        }
    });
    QHBoxLayout *layout11=new QHBoxLayout(f1);
    layout11->addWidget(lab1);

    QHBoxLayout *layout13=new QHBoxLayout(f3);
    layout13->addWidget(radio1);
    QHBoxLayout *layout14=new QHBoxLayout(f4);
    layout14->addWidget(radio2);
    QHBoxLayout *layout15=new QHBoxLayout(f5);
    layout15->addWidget(radio3);
    layout1->addWidget(f1,1);
    if(ntype==1)
    {
        QFrame *f2= new QFrame(ui->widget);
        f2->setFrameShape(QFrame::Box);
        f2->setFrameShadow(QFrame::Raised);
        f2->setStyleSheet("background-color: rgb(255, 235, 247);");
        f2->setFixedSize(90,35);
        QCheckBox *check=new QCheckBox(ui->widget);
        check->setText("全选");
        check->setObjectName("同意");
        connect(check,&QCheckBox::stateChanged,this,[=](){
            if(check->isChecked())
            {
                SetAllCheck(0,check->objectName());
            }
        });
        QHBoxLayout *layout12=new QHBoxLayout(f2);
        layout12->addWidget(check);
        layout1->addWidget(f2,1);
    }
    layout1->addWidget(f3,1);
    layout1->addWidget(f4,1);
    layout1->addWidget(f5,1);
    layout1->addStretch(2);

    layout1->setMargin(0);
    layout1->setSpacing(0);
    w1->setLayout(layout1);
    layoutv->addWidget(w1);
    layoutv->addStretch();
    ui->widget->setLayout(layoutv);
}
void ModeChange::AddData(int ntype,QString str,int ncheck)
{
    strStationName.append(str);
    QWidget *w1= new QWidget(ui->widget);
    w1->setObjectName(str);
    QHBoxLayout *layout1=new QHBoxLayout();
    layout1->setDirection(QBoxLayout::LeftToRight);
    QFrame *f1= new QFrame(ui->widget);
    QFrame *f2= new QFrame(ui->widget);
    QFrame *f3= new QFrame(ui->widget);
    QFrame *f4= new QFrame(ui->widget);
    f1->setFrameShape(QFrame::Box);
    f1->setFrameShadow(QFrame::Raised);
    f2->setFrameShape(QFrame::Box);
    f2->setFrameShadow(QFrame::Raised);
    f3->setFrameShape(QFrame::Box);
    f3->setFrameShadow(QFrame::Raised);
    f4->setFrameShape(QFrame::Box);
    f4->setFrameShadow(QFrame::Raised);
    f1->setStyleSheet("background-color: rgb(255, 235, 247);");
    f2->setStyleSheet("background-color: white;");
    f3->setStyleSheet("background-color: white;");
    f4->setStyleSheet("background-color: white;");
    f1->setFixedSize(100,35);
    f2->setFixedSize(90,35);
    f3->setFixedSize(90,35);
    f4->setFixedSize(90,35);
    QLabel *lab1=new QLabel(ui->widget);
    lab1->setText(str);
    QRadioButton *radio1=new QRadioButton(ui->widget);
    radio1->setText("中心控制");
    radio1->setObjectName(str+radio1->text());
    QRadioButton *radio2=new QRadioButton(ui->widget);
    radio2->setText("车站控制");
    radio2->setObjectName(str+radio2->text());
    QRadioButton *radio3=new QRadioButton(ui->widget);
    radio3->setText("车站调车");
    radio3->setObjectName(str+radio3->text());

//    // 设置未选中状态下的样式
//    radio1->setStyleSheet("QRadioButton { color: black; }"
//                          "QRadioButton:checked { color: red; }");
//    radio2->setStyleSheet("QRadioButton { color: black; }"
//                          "QRadioButton:checked { color: red; }");
//    radio3->setStyleSheet("QRadioButton { color: black; }"
//                          "QRadioButton:checked { color: red; }");



    if(ncheck==1)
    {
      radio1->setChecked(true);
      radio1->setStyleSheet("QRadioButton { color: red; }");
      radio2->setStyleSheet("QRadioButton { color: black; }");
      radio3->setStyleSheet("QRadioButton { color: black; }");
    }
    else if(ncheck==2)
    {
      radio2->setChecked(true);
      radio1->setStyleSheet("QRadioButton { color: black; }");
      radio2->setStyleSheet("QRadioButton { color: red; }");
      radio3->setStyleSheet("QRadioButton { color: black; }");
    }
    else if(ncheck==3)
    {
      radio3->setChecked(true);
      radio1->setStyleSheet("QRadioButton { color: black; }");
      radio2->setStyleSheet("QRadioButton { color: black; }");
      radio3->setStyleSheet("QRadioButton { color: red; }");
    }
    mbg=new  QButtonGroup(ui->widget);
    mbg->addButton(radio1,0);
    mbg->addButton(radio2,1);
    mbg->addButton(radio3,2);
    connect(mbg, SIGNAL(buttonClicked(int)), this, SLOT(slot_btnGroupClicked(int)));
    QHBoxLayout *layout11=new QHBoxLayout(f1);
    layout11->addWidget(lab1);
    QHBoxLayout *layout12=new QHBoxLayout(f2);
    layout12->addWidget(radio1);
    QHBoxLayout *layout13=new QHBoxLayout(f3);
    layout13->addWidget(radio2);
    QHBoxLayout *layout14=new QHBoxLayout(f4);
    layout14->addWidget(radio3);
    layout1->addWidget(f1,1);
    if(ntype==1)
    {
        QFrame *f5= new QFrame(ui->widget);
        f5->setFrameShape(QFrame::Box);
        f5->setFrameShadow(QFrame::Raised);
        f5->setStyleSheet("background-color: white;");
        f5->setFixedSize(90,35);
        QCheckBox *check=new QCheckBox(ui->widget);
        check->setText("同意");
        check->setObjectName(str+check->text());
        connect(check,&QRadioButton::clicked,this,[=](){
            bargee=check->isChecked();
        });
        QHBoxLayout *layout15=new QHBoxLayout(f5);
        layout15->addWidget(check);
        layout1->addWidget(f5,1);
    }
    layout1->addWidget(f2,1);
    layout1->addWidget(f3,1);
    layout1->addWidget(f4,1);
    layout1->addStretch(2);

    layout1->setMargin(0);
    layout1->setSpacing(0);
    w1->setLayout(layout1);
    layoutv->insertWidget(layoutv->count()-1,w1);

}
void ModeChange::SetAllCheck(int type,QString str)
{
    for(int i=0;i<layoutv->count()-1;i++)
    {
        QWidget *ww=layoutv->itemAt(i)->widget();
        QString ss=ww->objectName();
        if(ss!="")
        {
            if(type==0)//QCheckBox
            {
                QCheckBox *check=ww->findChild<QCheckBox *>(strStationName[i]+str);
                if(check!=nullptr)
                {
                    check->setChecked(true);
                }
            }
            else if(type==1)//QRadioButton
            {
                QRadioButton *radio=ww->findChild<QRadioButton *>(strStationName[i]+str);
                if(radio!=nullptr)
                {
                    radio->setChecked(true);
                }
            }
        }
    }
}
void ModeChange::slot_btnGroupClicked(int n)
{
    nCheckState=n;
}

