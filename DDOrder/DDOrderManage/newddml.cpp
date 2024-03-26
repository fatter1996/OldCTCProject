#include "newddml.h"
#include "ui_newddml.h"
#include <QComboBox>
#include <QMessageBox>
#include <QDateTime>
#include <qdatetime.h>
#pragma execution_character_set("utf-8")
Newddml::Newddml(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Newddml)
{
    ui->setupUi(this);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    ui->comboBox_2->addItem("模拟调度中心");
    mDispatchOrderDispatcher=new DispatchOrderDispatcher();
    mArrayDisOrderLocomot=new DispatchOrderLocomotive();
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_2->layout()->addWidget(mCTableView);

    InitTableViewDDML();
    mCTableViewDDML->SetTableHeadHideV(false);
    mCTableViewDDML->SetTableHeadHideH(true);
    mCTableViewDDML->CreatTableView();
    ui->widget_4->layout()->addWidget(mCTableViewDDML);
    ui->comboBox_4->addItem("DF4");
    ui->comboBox_4->addItem("HXD1");
    ui->comboBox_4->addItem("HXD2");
    ui->comboBox_4->addItem("HXD3");
    ui->comboBox_4->addItem("QJ(001)");
    ui->comboBox_4->addItem("QJ(002)");
    ui->comboBox_4->addItem("QJ(003)");
    ui->comboBox_4->addItem("SS4");
    connect(ui->comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(currentIndexChangedSlot(int)));
    //添加 机车
    connect(ui->pushButton_10,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(216, QCursor::pos());
        if(ui->comboBox->currentIndex()==0||ui->comboBox_3->currentText()==""||ui->lineEdit->text()==""||ui->lineEdit_4->text()=="")
        {
            QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未录入"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            QStringList strlist;
            strlist<<"1"<<ui->comboBox_3->currentText()<<ui->lineEdit_4->text();
            mVectTableShow.clear();
            mTableShow.Init();
            mTableShow.strcheci=ui->comboBox_3->currentText();
            mTableShow.strtrainnum=ui->lineEdit_4->text();
            mVectTableShow.append(mTableShow);
            UpDateTableViewDDML();
            bAddFalg=true;
        }
    });
    //修改 机车
    connect(ui->pushButton_11,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(214, QCursor::pos());
    });
    //删除 机车
    connect(ui->pushButton_9,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(215, QCursor::pos());
    });
    //查询 机车
    connect(ui->pushButton_4,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(217, QCursor::pos());
    });

    //添加 调度台
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(216, QCursor::pos());
        if(ui->lineEdit->text()==""||ui->comboBox->currentIndex()==0)
        {
           QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未输入"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            //更新接收信息表
            QStringList strlist;
            strlist<<"1"<<"模拟调度中心"<<"";
            DispatcherInfo mDispatcherInfo;
            //调度台
            mDispatcherInfo.strDispatcher="模拟调度台";
            mTableShowDDT.Init();
            mTableShowDDT.strDDT="模拟调度中心";
            mVectTableShowDDT.clear();
            mVectTableShowDDT.append(mTableShowDDT);
            UpDateTableView();
        }
    });
    //修改 调度台
    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(214, QCursor::pos());
    });
    //删除 调度台
    connect(ui->pushButton_3,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(215, QCursor::pos());
    });

    //发送
    connect(ui->pushButton_5,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(210, QCursor::pos());
        if(objectName()=="机车")
        {
            if(mVectTableShow.size()>0&&ui->comboBox->currentIndex()>0)
            {
                //命令号
                mArrayDisOrderLocomot->uNumber=ui->lineEdit->text().toInt();
                //mDispatchOrderStation->strType=ui->comboBox->currentText();
                QDateTime timOrder=QDateTime::currentDateTime();
                //创建时间
                mArrayDisOrderLocomot->timCreate=timOrder;
                //发送时间
                mArrayDisOrderLocomot->timSend=timOrder;
                mArrayDisOrderLocomot->strStation=ui->lineEdit_2->text();
                mArrayDisOrderLocomot->strDutyName=ui->lineEdit_3->text();
                //命令种类
                mArrayDisOrderLocomot->orderType=0x01;
                //命令类型
                mArrayDisOrderLocomot->strType=ui->comboBox->currentText();
                //命令内容
                mArrayDisOrderLocomot->strContent=ui->textEdit->toPlainText();
                // 调度台接收信息
                LocomotiveInfo mLocomotiveInfo;
                int nn=mVectTableShow.size();
                mArrayDisOrderLocomot->vectLocmtInfo.clear();
                for(int i=0;i<mVectTableShow.size();i++)
                {
                    LocomotiveInfo mLocomotiveInfo;
                    if(i==0)
                    {
                        mLocomotiveInfo.strLocomotive=mVectTableShow[i].strtrainnum;
                        mLocomotiveInfo.strCheCi=mVectTableShow[i].strcheci;
                    }
                    else
                    {
                        mLocomotiveInfo.strLocomotive=mLocomotiveInfo.strLocomotive+","+mVectTableShow[i].strtrainnum;
                        mLocomotiveInfo.strCheCi=mLocomotiveInfo.strCheCi+","+mVectTableShow[i].strcheci;
                    }
                    mArrayDisOrderLocomot->vectLocmtInfo.append(mLocomotiveInfo);
                }
                //发送状态
                mArrayDisOrderLocomot->bSend = true;

                // mArrayDisOrderLocomot->vectLocmtInfo.append(mLocomotiveInfo);
                emit NewDDOrderData(mArrayDisOrderLocomot,"发送"/*this->objectName()*/);
                   bAddFalg=false;
                    close();
            }
            else
            {
                 QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未输入"),tr("确定"),tr("取消"),"",0);
            }
        }
        else if(objectName()=="调度台")
        {
            if(mVectTableShowDDT.size()>0&&ui->comboBox->currentIndex()>0)
            {
                mDispatchOrderDispatcher->uNumber=ui->lineEdit->text().toInt();
                //mDispatchOrderStation->strType=ui->comboBox->currentText();
                QDateTime timOrder=QDateTime::currentDateTime();
                //创建时间
                mDispatchOrderDispatcher->timCreate=timOrder;
                //发送时间
                mDispatchOrderDispatcher->timSend=timOrder;
                mDispatchOrderDispatcher->strStation=ui->lineEdit_2->text();
                mDispatchOrderDispatcher->strDutyName=ui->lineEdit_3->text();
                //命令类型
                mDispatchOrderDispatcher->strType=ui->comboBox->currentText();
                //命令内容
                mDispatchOrderDispatcher->strContent=ui->textEdit->toPlainText();
                // 调度台接收信息
                DispatcherInfo mDispatcherInfo;
                //调度台
                mDispatcherInfo.strDispatcher="模拟调度台";
                //发送状态
                mDispatchOrderDispatcher->bSend = true;
                mDispatchOrderDispatcher->vectDispathInfo.clear();
                mDispatchOrderDispatcher->vectDispathInfo.append(mDispatcherInfo);
                mVectTableShowDDT.clear();
                emit NewDDOrderData(mDispatchOrderDispatcher,"发送"/*this->objectName()*/);
                close();
            }
            else
            {
               QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未输入"),tr("确定"),tr("取消"),"",0);
            }

        }
    });
    //打印
    connect(ui->pushButton_6,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(211, QCursor::pos());
    });
    //保存
    connect(ui->pushButton_7,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(213, QCursor::pos());
        if(objectName()=="机车")
        {
            if(mVectTableShow.size()>0&&ui->comboBox->currentIndex()>0)
            {
                mArrayDisOrderLocomot->uNumber=ui->lineEdit->text().toInt();
                //mDispatchOrderStation->strType=ui->comboBox->currentText();
                QDateTime timOrder=QDateTime::currentDateTime();
                mArrayDisOrderLocomot->timCreate=timOrder;
                mArrayDisOrderLocomot->strStation=ui->lineEdit_2->text();
                mArrayDisOrderLocomot->strDutyName=ui->lineEdit_3->text();
                //命令种类
                mArrayDisOrderLocomot->orderType=0x01;
                //命令类型
                mArrayDisOrderLocomot->strType=ui->comboBox->currentText();
                //命令内容
                mArrayDisOrderLocomot->strContent=ui->textEdit->toPlainText();
                // 调度台接收信息
                LocomotiveInfo mLocomotiveInfo;
                //调度台
                mArrayDisOrderLocomot->vectLocmtInfo.clear();
                for(int i=0;i<mVectTableShow.size();i++)
                {
                    LocomotiveInfo mLocomotiveInfo;
                    if(i==0)
                    {
                        mLocomotiveInfo.strLocomotive=mVectTableShow[i].strtrainnum;
                        mLocomotiveInfo.strCheCi=mVectTableShow[i].strcheci;
                    }
                    else
                    {
                        mLocomotiveInfo.strLocomotive=mLocomotiveInfo.strLocomotive+","+mVectTableShow[i].strtrainnum;
                        mLocomotiveInfo.strCheCi=mLocomotiveInfo.strCheCi+","+mVectTableShow[i].strcheci;
                    }
                    mArrayDisOrderLocomot->vectLocmtInfo.append(mLocomotiveInfo);
                }
                //发送状态
                mArrayDisOrderLocomot->bSend = false;
                //mArrayDisOrderLocomot->vectLocmtInfo.append(mLocomotiveInfo);
                emit NewDDOrderData(mArrayDisOrderLocomot,"保存"/*this->objectName()*/);
                close();
            }
            else
            {
               QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未输入"),tr("确定"),tr("取消"),"",0);
            }
        }
        else if(objectName()=="调度台")
        {
            if(mVectTableShowDDT.size()>0)
            {
                mDispatchOrderDispatcher->uNumber=ui->lineEdit->text().toInt();
                //mDispatchOrderStation->strType=ui->comboBox->currentText();
                QDateTime timOrder=QDateTime::currentDateTime();
                mDispatchOrderDispatcher->timCreate=timOrder;
                mDispatchOrderDispatcher->strStation=ui->lineEdit_2->text();
                mDispatchOrderDispatcher->strDutyName=ui->lineEdit_3->text();
                //命令类型
                mDispatchOrderDispatcher->strType=ui->comboBox->currentText();
                //命令内容
                mDispatchOrderDispatcher->strContent=ui->textEdit->toPlainText();
                // 调度台接收信息
                DispatcherInfo mDispatcherInfo;
                //调度台
                mDispatcherInfo.strDispatcher="模拟调度台";
                //发送状态
                mDispatchOrderDispatcher->bSend = false;
                mDispatchOrderDispatcher->vectDispathInfo.clear();
                mDispatchOrderDispatcher->vectDispathInfo.append(mDispatcherInfo);
                mVectTableShowDDT.clear();
                emit NewDDOrderData(mDispatchOrderDispatcher,"保存"/*this->objectName()*/);
                close();
            }
            else
            {
               QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未输入"),tr("确定"),tr("取消"),"",0);
            }

        }


    });
    //取消
    connect(ui->pushButton_8,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        close();
    });
}

Newddml::~Newddml()
{
    delete ui;
}
void Newddml::ClearShow()
{
    ui->lineEdit->setText("");
    ui->comboBox->setCurrentIndex(0);
    mVectTableShow.clear();
    ClearShow();
}
void Newddml::Init(QString strsta,QString strname,int norder)
{
    ui->lineEdit_2->setText(strsta);
    ui->lineEdit_3->setText(strname);
    ui->comboBox->setCurrentIndex(0);
    ui->lineEdit->setText(QString::number(norder+1));
       bAddFalg=false;
    if(objectName()=="机车")
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->comboBox_3->clear();
        ui->comboBox_3->clearEditText();
        ui->lineEdit_4->setText("");
        mVectTableShow.clear();
        UpDateTableViewDDML();
    }
    else if(objectName()=="调度台")
    {
        ui->stackedWidget->setCurrentIndex(0);
        mVectTableShowDDT.clear();
        UpDateTableView();
    }
}
void Newddml::InitDisTemplate()
{
    QFontMetrics  fontMetrics(ui->comboBox->font());
     QString mStr="";
//    ui->comboBox->setStyleSheet("QComboBox { combobox-popup: 0; } QComboBox::lineEdit { padding-right: 30px; } QComboBox QAbstractItemView::item { padding-right: 30px; }");
//    ui->comboBox->setInsertPolicy(QComboBox::InsertAtBottom);
     ui->comboBox->addItem("");
    for(int i=0;i<mVectDisOrderTemplate.size();i++)
    {
       //ui->comboBox->addItem(mVectDisOrderTemplate[i].strDisOrderHeader);
        mStr=mVectDisOrderTemplate[i].strDisOrderHeader;
        int fontSize = fontMetrics.width(mStr);
        if (fontSize > ui->comboBox->width())
        {
            ui->comboBox->addItem(fontMetrics.elidedText(mStr, Qt::ElideRight, ui->comboBox->width()+150));
            ui->comboBox->setToolTip(mStr);
        }
        else
        {
            ui->comboBox->addItem(mStr);
            ui->comboBox->setToolTip("");
        }
    }

}
void Newddml::currentIndexChangedSlot(int i)
{
    if(i>0)
    {
        ui->textEdit->setText( mVectDisOrderTemplate[i-1].strDisOrderContent);
    }
    else
    {
        ui->textEdit->setText("");
    }
}
void Newddml::InitTableView()
{
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();

    mCTableView->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(50);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("调度台");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableView->mTableViewInfo.Vect_HeaderData.append(" ");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(39);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(0, 3);
//    mCTableView->SetTableHeadsetStyleSheet(1,
//        "QHeaderView{"
//            "background:rgb(240, 240, 240);"
//        "}"
//        "QHeaderView::section:horizontal {"
//            "background:rgb(240, 240, 240);"
//            "font-size: 12px;"
//            "font-weight: normal;"
//        "}");
    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        );

}
void Newddml::InitTableViewDDML()
{
    mCTableViewDDML= new TableView();
    mCTableViewDDML->mTableViewInfo.Init();

    mCTableViewDDML->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableViewDDML->mTableViewInfo.Vect_HeaderDataWidth.append(50);
    mCTableViewDDML->mTableViewInfo.Vect_HeaderData.append("车次号");
    mCTableViewDDML->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableViewDDML->mTableViewInfo.Vect_HeaderData.append("机车号");
    mCTableViewDDML->mTableViewInfo.Vect_HeaderDataWidth.append(50);

//    mCTableView->SetSectionText("序号", 48);
    mCTableViewDDML->SetTableHeadHeight(0, 24);
    mCTableViewDDML->SetTableHeadHeight(1, 16);
    mCTableViewDDML->IintTableColAndRow(0, 3);
        mCTableView->SetTableHeadsetStyleSheet(1,
            "QHeaderView{"
                "background:rgb(240, 240, 240);"
            "}");
}
void Newddml::UpDateTableView()
{
    //mVectTableShowDDT
    //mCTableView->SetTableData(strlist);
    QStringList strlist1;
    for (int i=0;i<mVectTableShowDDT.size();i++)
    {
        strlist1<<QString::number(i+1)<<mVectTableShowDDT[i].strDDT<<"";
    }
    mCTableView->SetTableData(strlist1);
}
void Newddml::UpDateTableViewDDML()
{
    QStringList strlist1;
    for (int i=0;i<mVectTableShow.size();i++)
    {
        strlist1<<QString::number(i+1)<<mVectTableShow[i].strcheci<<mVectTableShow[i].strtrainnum;
    }
    mCTableViewDDML->SetTableData(strlist1);
}
