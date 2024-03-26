#include "statechose.h"
#include "ui_statechose.h"
#include <QFrame>
#include <QLabel>
#include <QRadioButton>
#include <QCheckBox>
#include <QButtonGroup>
#pragma execution_character_set("utf-8")
StateChose::StateChose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StateChose)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);

    this->setContentsMargins(5,0,5,9);
    nPlanState=0;
    nPLState=0;
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(77, QCursor::pos());
        emit SendStationCtrModeChoseSignal(0x02,nPLState,nPlanState);
        close();
    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        close();
    });
    Init();
}

StateChose::~StateChose()
{
    delete ui;
}
void StateChose::Init()
{
    strStationName.append("");
    //layoutv= (QVBoxLayout*)this->layout();
    layoutv=new QVBoxLayout(ui->widget);
    layoutv->setSpacing(0);
    layoutv->setContentsMargins(1,1,1,1);
    QWidget *w1= new QWidget(this);
    w1->setObjectName("");
    QHBoxLayout *layout1=new QHBoxLayout(ui->widget);
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
    f2->setStyleSheet("background-color: rgb(255, 235, 247);");
    f3->setStyleSheet("background-color: rgb(255, 235, 247);");
    f4->setStyleSheet("background-color: rgb(255, 235, 247);");
    f1->setFixedSize(100,35);
    f2->setFixedSize(90,35);
    f3->setFixedSize(90,35);
    f4->setFixedSize(90,35);
    QLabel *lab1=new QLabel(ui->widget);
    lab1->setText("站名");
    QCheckBox *radio1=new QCheckBox(ui->widget);
    radio1->setText("全选");
    radio1->setObjectName("计划控制");
    QRadioButton *check1=new QRadioButton(ui->widget);
    check1->setText("全选");
    check1->setObjectName("按图排路");
    QRadioButton *check2=new QRadioButton(ui->widget);
    check2->setText("全选");
    check2->setObjectName("手工排路");
    connect(radio1,&QCheckBox::stateChanged,this,[=](){
        if(radio1->isChecked())
        {
            SetAllCheck(0,radio1->objectName());
        }
    });
    connect(check1,&QRadioButton::clicked,this,[=](){
        if(check1->isChecked())
        {
            SetAllCheck(1,check1->objectName());
        }
    });
    connect(check2,&QRadioButton::clicked,this,[=](){
        if(check2->isChecked())
        {
            SetAllCheck(1,check2->objectName());
        }
    });
    QHBoxLayout *layout11=new QHBoxLayout(f1);
    layout11->addWidget(lab1);
    QHBoxLayout *layout12=new QHBoxLayout(f2);
    layout12->addWidget(radio1);
    QHBoxLayout *layout13=new QHBoxLayout(f3);
    layout13->addWidget(check1);
    QHBoxLayout *layout14=new QHBoxLayout(f4);
    layout14->addWidget(check2);
    layout1->addWidget(f1,1);
    layout1->addWidget(f2,1);
    layout1->addWidget(f3,1);
    layout1->addWidget(f4,1);
    layout1->addStretch(2);

    layout1->setMargin(0);
    layout1->setSpacing(0);
    w1->setLayout(layout1);

    layoutv->addWidget(w1);
    layoutv->addStretch();
    //ui->widget->setLayout(layoutv);



}
void StateChose::AddData(QString str)
{
    //Init();
    for(int i=0;i<strStationName.size();i++)
    {
        if(strStationName[i]==str)
           return;
    }
    strStationName.append(str);
    QWidget *w1= new QWidget(ui->widget);
    w1->setObjectName(str);
    QHBoxLayout *layout1=new QHBoxLayout(ui->widget);
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
    checkPlan=new QCheckBox(ui->widget);
    checkPlan->setText("计划控制");
    checkPlan->setObjectName(str+checkPlan->text());
    radioPLAuto=new QRadioButton(ui->widget);
    radioPLAuto->setText("按图排路");
    radioPLAuto->setObjectName(str+radioPLAuto->text());
    radioPLUnAuto=new QRadioButton(ui->widget);
    radioPLUnAuto->setText("手工排路");
    radioPLUnAuto->setObjectName(str+radioPLUnAuto->text());

    connect(checkPlan,&QCheckBox::stateChanged,this,[=](){
        if(checkPlan->isChecked())
        {
            nPlanState=1;
        }
        else
            nPlanState=0;
    });
    QButtonGroup *mbg=new  QButtonGroup(ui->widget);
    mbg->addButton(radioPLAuto,1);
    mbg->addButton(radioPLUnAuto,0);
    connect(mbg, SIGNAL(buttonClicked(int)), this, SLOT(slot_btnGroupClicked(int)));
    QHBoxLayout *layout11=new QHBoxLayout(f1);
    layout11->addWidget(lab1);
    QHBoxLayout *layout12=new QHBoxLayout(f2);
    layout12->addWidget(checkPlan);
    QHBoxLayout *layout13=new QHBoxLayout(f3);
    layout13->addWidget(radioPLAuto);
    QHBoxLayout *layout14=new QHBoxLayout(f4);
    layout14->addWidget(radioPLUnAuto);
    layout1->addWidget(f1,1);
    layout1->addWidget(f2,1);
    layout1->addWidget(f3,1);
    layout1->addWidget(f4,1);
    layout1->addStretch(2);

    layout1->setMargin(0);
    layout1->setSpacing(0);
    w1->setLayout(layout1);

    layoutv->insertWidget(layoutv->count()-1,w1);

}
void StateChose::SetAllCheck(int type,QString str)
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
void StateChose::SetCheckState(bool bcheck1,bool bcheck2)
{
    if(bcheck1)
    {
        checkPlan->setChecked(true);
        nPlanState=1;
    }
    else
    {
        checkPlan->setChecked(false);
        nPlanState=0;
    }
    if(bcheck2)
    {
        radioPLAuto->setChecked(true);
        radioPLUnAuto->setChecked(false);
        radioPLAuto->setStyleSheet("color:red;");
        radioPLUnAuto->setStyleSheet("color:black;");
        nPLState=1;
    }
    else
    {
        radioPLAuto->setChecked(false);
        radioPLUnAuto->setChecked(true);
        radioPLAuto->setStyleSheet("color:black;");
        radioPLUnAuto->setStyleSheet("color:red;");
        nPLState=0;
    }
}
void StateChose::slot_btnGroupClicked(int n)
{
    nPLState=n;
}
