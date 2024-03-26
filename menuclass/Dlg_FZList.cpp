#include "Dlg_FZList.h"
#include "ui_Dlg_FZList.h"
#include "StationInterface/StationInterfaceKSK/stationctrldispksk.h"

DLG_FZList::DLG_FZList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DLG_FZList)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无标题栏
    //setAttribute(Qt::WA_TranslucentBackground,true);//背景透明
}

DLG_FZList::~DLG_FZList()
{
    delete ui;
}
void DLG_FZList::SetJGVisable(bool b)
{
    ui->pushButton_4->setEnabled(b);
}
void  DLG_FZList::SetXGVisable(bool b)
{
    ui->pushButton_5->setEnabled(b);
}
void DLG_FZList::on_pushButton_clicked()//股道无电
{
    emit SendTextAnalysisSignal(106, QCursor::pos());
    emit OrderFZListSignal(0);
    emit GDWDSignal();
    close();
}

void DLG_FZList::on_pushButton_2_clicked()//接触网定位无电
{
    emit SendTextAnalysisSignal(107, QCursor::pos());
    emit OrderFZListSignal(0);
    emit JCWDWWDSignal();
    close();
}

void DLG_FZList::on_pushButton_3_clicked()//接触网反位无电
{
    emit SendTextAnalysisSignal(108, QCursor::pos());
    emit OrderFZListSignal(0);
    emit JCWFWWDSignal();
    close();
}

void DLG_FZList::on_pushButton_4_clicked()//尖轨故障
{
    emit SendTextAnalysisSignal(131, QCursor::pos());
    emit OrderFZListSignal(0);
    emit JGGZSignal();
    close();
}

void DLG_FZList::on_pushButton_5_clicked()//心轨故障
{
    emit SendTextAnalysisSignal(132, QCursor::pos());
    emit OrderFZListSignal(0);
    emit XGGZSignal();
    close();
}

void DLG_FZList::on_pushButton_6_clicked()//破封统计
{
    emit SendTextAnalysisSignal(133, QCursor::pos());
    emit OrderFZListSignal(0);
    emit PFTJSignal();
    close();

}

void DLG_FZList::on_pushButton_7_clicked()//退出菜单
{
    emit SendTextAnalysisSignal(134, QCursor::pos());
    close();
    emit OrderFZListSignal(0);
}
