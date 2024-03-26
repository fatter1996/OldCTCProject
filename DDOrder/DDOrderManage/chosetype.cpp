#include "chosetype.h"
#include "ui_chosetype.h"
#include "qdebug.h"
#include "qstring.h"
#include <QString>
#pragma execution_character_set("utf-8")
ChoseType::ChoseType(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChoseType)
{
    ui->setupUi(this);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    Init();
    InitBtnGroup();
    connect(ui->pushButton,&QPushButton::clicked,[=](){
         //
        emit SendTextAnalysisSignal(77, QCursor::pos());
        int a=-1;
        a=ui->buttonGroup->checkedId();
        if(a!=-1)
        {
             //SetDlgShow(a);
             emit ChoseTypeSignal(a);
             close();
        }

    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        close();
    });
}

ChoseType::~ChoseType()
{
    delete ui;
}
void ChoseType::Init()
{
   m_NewLP = new NewLP();
   m_Newddml= new Newddml();
}
void ChoseType::InitBtnGroup()
{
    ui->buttonGroup->setId(ui->radioButton,0);
    ui->buttonGroup->setId(ui->radioButton_2,1);
    ui->buttonGroup->setId(ui->radioButton_5,2);
    ui->buttonGroup->setId(ui->radioButton_6,3);
    ui->buttonGroup->setId(ui->radioButton_3,4);
    ui->buttonGroup->setId(ui->radioButton_4,5);
    ui->buttonGroup->setId(ui->radioButton_7,6);
    ui->buttonGroup->setId(ui->radioButton_8,7);
}
void ChoseType::SetDlgShow(int nId)
{
    QString stype="";
    QString slable="";
    if(nId==0)
    {
        //m_Newddml->setWindowTitle("");
        m_Newddml->Init("标准站","user1",0);
        m_Newddml->show();
    }
    else
    {
        if(nId==1)
        {
            m_NewLP->Init("标准站","user1",0);
            m_NewLP->SetTitel(QString("新建路票"));
            m_NewLP->SetLable(QString("路票"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->show();
        }
        else if(nId==2)
        {
            m_NewLP->SetTitel(QString("新建绿色许可证"));
            m_NewLP->SetLable(QString("绿色许可证"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init("标准站","user1",0);
            m_NewLP->show();
        }
        else if(nId==3)
        {
            m_NewLP->SetTitel(QString("新建红色许可证"));
            m_NewLP->SetLable(QString("红色许可证"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init("标准站","user1",0);
            m_NewLP->show();
        }
        else if(nId==4)
        {
            m_NewLP->SetTitel(QString("新建调车作业单"));
            m_NewLP->SetLable(QString("调车作业单"));
              m_NewLP->SetLableBlackColor(nId);
              m_NewLP->Init("标准站","user1",0);
              m_NewLP->show();
        }
        else if(nId==5)
        {
            m_NewLP->SetTitel(QString("新建书面通知"));
            m_NewLP->SetLable(QString("书面通知"));
              m_NewLP->SetLableBlackColor(nId);
              m_NewLP->Init("标准站","user1",0);
              m_NewLP->show();
        }
        else if(nId==6)
        {
            m_NewLP->SetTitel(QString("新建半自动闭塞发车进路通知书"));
            m_NewLP->SetLable(QString("半自动闭塞发车进路通知书"));
              m_NewLP->SetLableBlackColor(nId);
              m_NewLP->Init("标准站","user1",0);
              m_NewLP->show();
        }
        else if(nId==7)
        {
            m_NewLP->SetTitel(QString("新建自动站间闭塞发车进路通知书"));
            m_NewLP->SetLable(QString("自动站间闭塞发车进路通知书"));
              m_NewLP->SetLableBlackColor(nId);
              m_NewLP->Init("标准站","user1",0);
              m_NewLP->show();
        }
    }
}
