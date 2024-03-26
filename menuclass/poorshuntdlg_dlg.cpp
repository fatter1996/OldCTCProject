#include "poorshuntdlg_dlg.h"
#include "ui_poorshuntdlg_dlg.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")

PoorShuntDlg_Dlg::PoorShuntDlg_Dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PoorShuntDlg_Dlg)
{
    ui->setupUi(this);
    setWindowTitle("设置分路不良窗口");
    Init();
    isGDFLBLcheck = true;
}

PoorShuntDlg_Dlg::~PoorShuntDlg_Dlg()
{
    delete ui;
}

void PoorShuntDlg_Dlg::Init()
{
    connect(ui->checkBox_FLBL, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_FLBL->isChecked());
    });
    connect(ui->checkBox_CQFLBL, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_CQFLBL->isChecked());
    });
    connect(ui->checkBox_DWFLBL, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_DWFLBL->isChecked());
    });
    connect(ui->checkBox_FWFLBL, &QCheckBox::clicked, this, [=](){
        SetStateShow(ui->checkBox_FWFLBL->isChecked());
    });
}
void PoorShuntDlg_Dlg::InitShow(int ntpye,QString str,bool ncheckCQ,bool ncheckDW,bool ncheckFW)
{
    ui->checkBox_FLBL->setCheckable(true);
    ui->checkBox_CQFLBL->setCheckable(true);
    ui->checkBox_DWFLBL->setCheckable(true);
    ui->checkBox_FWFLBL->setCheckable(true);
    ui->checkBox_FLBL->setChecked(false);
    ui->checkBox_CQFLBL->setChecked(false);
    ui->checkBox_DWFLBL->setChecked(false);
    ui->checkBox_FWFLBL->setChecked(false);
    nWidgettpye=ntpye;
    if(ntpye==Dev_DC)
    {
        ui->checkBox_FLBL->setEnabled(false);
        ui->checkBox_CQFLBL->setEnabled(true);
        ui->checkBox_DWFLBL->setEnabled(true);
        ui->checkBox_FWFLBL->setEnabled(true);

        ui->checkBox_CQFLBL->setChecked(ncheckCQ);
        ui->checkBox_DWFLBL->setChecked(ncheckDW);
        ui->checkBox_FWFLBL->setChecked(ncheckFW);
        boldcheck[0]=ncheckCQ;
        boldcheck[1]=ncheckDW;
        boldcheck[2]=ncheckFW;
        str="道岔："+str;
        if(ui->checkBox_CQFLBL->isChecked()||ui->checkBox_DWFLBL->isChecked()||ui->checkBox_FWFLBL->isChecked())
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
        ui->checkBox_FLBL->setEnabled(true);
        ui->checkBox_CQFLBL->setEnabled(false);
        ui->checkBox_DWFLBL->setEnabled(false);
        ui->checkBox_FWFLBL->setEnabled(false);

        ui->checkBox_FLBL->setChecked(true);
        str="股道："+str;
        ui->lineEdit_2->setText("√分路不良已设置");
    }
    ui->lineEdit->setText(str);

}

void PoorShuntDlg_Dlg::SetStateShow(bool b)
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
void PoorShuntDlg_Dlg::SendSigal(bool n[3])
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

void PoorShuntDlg_Dlg::on_pushButton_clicked()
{
    emit SendTextAnalysisSignal(77, QCursor::pos());
    int ntype=0;
    int ntype2=0;
    int ntype3=0;
    int ntype4=0;
    bool bnewcheck[3]={false};
    if(ui->checkBox_FLBL->isChecked())
    {
        ntype=0x16;
    }
    bnewcheck[0]=ui->checkBox_CQFLBL->isChecked();
    bnewcheck[1]=ui->checkBox_DWFLBL->isChecked();
    bnewcheck[2]=ui->checkBox_FWFLBL->isChecked();

    {
        this->close();
        if(nWidgettpye==Dev_GD)
        {
            isGDFLBLcheck = ui->checkBox_FLBL->isChecked();
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
}

void PoorShuntDlg_Dlg::on_pushButton_2_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    this->close();
}
