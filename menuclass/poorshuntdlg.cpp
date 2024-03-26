#include "poorshuntdlg.h"
#include "ui_poorshuntdlg.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")
PoorShuntDlg::PoorShuntDlg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PoorShuntDlg)
{
    ui->setupUi(this);
    setWindowFlags(/*Qt::Tool|*//*Qt::FramelessWindowHint|*/Qt::WindowStaysOnTopHint|Qt::Dialog);
    setWindowModality(Qt::ApplicationModal);
    Init();
    isGDFLBLcheck = true;
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        int ntype=0;
        int ntype2=0;
        int ntype3=0;
        int ntype4=0;
        bool bnewcheck[3]={false};
        if(ui->checkBox->isChecked())
        {
            ntype=0x16;
        }
        bnewcheck[0]=ui->checkBox_2->isChecked();
        bnewcheck[1]=ui->checkBox_3->isChecked();
        bnewcheck[2]=ui->checkBox_4->isChecked();

        {
            this->close();
            if(nWidgettpye==Dev_GD)
            {
                isGDFLBLcheck = ui->checkBox->isChecked();
                emit resultsignal(ntype);
            }
            else if(nWidgettpye==Dev_DC)
            {
                bool bsendcheck[3]={false};
                for (int i = 0; i < 3; i++)
                {
                    if (boldcheck[i] != bnewcheck[i])
                    {
                        bsendcheck[i]=true;
                    }
                    else
                    {
                        bsendcheck[i]=false;
                    }
                }
                SendSigal(bsendcheck);
            }
        }
    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        this->close();
    });

}

PoorShuntDlg::~PoorShuntDlg()
{
    delete ui;
}
void PoorShuntDlg::Init()
{
    connect(ui->checkBox, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox->isChecked());
    });
    connect(ui->checkBox_2, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_2->isChecked());
    });
    connect(ui->checkBox_3, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_3->isChecked());
    });
    connect(ui->checkBox_4, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_4->isChecked());
    });
}
void PoorShuntDlg::InitShow(int ntpye,QString str,bool ncheckCQ,bool ncheckDW,bool ncheckFW)
{
    ui->checkBox->setCheckable(true);
    ui->checkBox_2->setCheckable(true);
    ui->checkBox_3->setCheckable(true);
    ui->checkBox_4->setCheckable(true);
    ui->checkBox->setChecked(false);
    ui->checkBox_2->setChecked(false);
    ui->checkBox_3->setChecked(false);
    ui->checkBox_4->setChecked(false);
    nWidgettpye=ntpye;
    if(ntpye==Dev_DC)
    {
        ui->checkBox->setEnabled(false);
        ui->checkBox_2->setEnabled(true);
        ui->checkBox_3->setEnabled(true);
        ui->checkBox_4->setEnabled(true);

        ui->checkBox_2->setChecked(ncheckCQ);
        ui->checkBox_3->setChecked(ncheckDW);
        ui->checkBox_4->setChecked(ncheckFW);
        boldcheck[0]=ncheckCQ;
        boldcheck[1]=ncheckDW;
        boldcheck[2]=ncheckFW;
        str="道岔："+str;
        if(ui->checkBox_2->isChecked()||ui->checkBox_3->isChecked()||ui->checkBox_4->isChecked())
        {
            ui->lineEdit_2->setText("√分路不良已设置");
        }
        else
        {
            ui->lineEdit_2->setText("请设置分路不良状态");
        }
    }
    else if(ntpye==Dev_GD)
    {
        ui->checkBox->setEnabled(true);
        ui->checkBox_2->setEnabled(false);
        ui->checkBox_3->setEnabled(false);
        ui->checkBox_4->setEnabled(false);

        ui->checkBox->setChecked(true);
        str="股道："+str;
        ui->lineEdit_2->setText("√分路不良已设置");
    }
    ui->lineEdit->setText(str);

}

void PoorShuntDlg::SetStateShow(bool b)
{
//    if(b)
//    {
//        ui->lineEdit_2->setText("√分路不良已设置");
//    }
//    else
//    {
//        ui->lineEdit_2->setText("请设置分路不良状态");
//    }
}
void PoorShuntDlg::SendSigal(bool n[3])
{
    int ntype[3]={-1,-1,-1};
    if(n[0])
    {
        ntype[0]=0x30;
    }
    if(n[1])
    {
        ntype[1]=0x31;
    }
    if(n[2])
    {
        ntype[2]=0x32;
    }
     emit resultsignal(ntype[0],ntype[1],ntype[2]);
}
