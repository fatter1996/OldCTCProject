#include "systemalarm.h"
#include "ui_systemalarm.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#pragma execution_character_set("utf-8")
SystemAlarm::SystemAlarm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SystemAlarm)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    //setWindowModality(Qt::ApplicationModal);
    //this->setWindowFlags(Qt::WindowCloseButtonHint);
     setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
     //setFixedSize(this->width(),this->height());
    InitTable();
    mCTableView->CreatTableView(false);
    mCTableView->SetTableDefaultAlignment(1);
    mCheckState.Init();
    mCheckState.Level1=ui->checkBox->isChecked();
    mCheckState.Level2=ui->checkBox_2->isChecked();
    mCheckState.Level3=ui->checkBox_3->isChecked();
    mCheckState.Sys=ui->checkBox_5->isChecked();
    mCheckState.Train=ui->checkBox_6->isChecked();
    connect(ui->checkBox,&QCheckBox::stateChanged,this,[=](){
       mCheckState.Level1=ui->checkBox->isChecked();
       AddData();
    });
    connect(ui->checkBox_2,&QCheckBox::stateChanged,this,[=](){
        mCheckState.Level2=ui->checkBox_2->isChecked();
        AddData();
    });
    connect(ui->checkBox_3,&QCheckBox::stateChanged,this,[=](){
         mCheckState.Level3=ui->checkBox_3->isChecked();
         AddData();
    });
    connect(ui->checkBox_5,&QCheckBox::stateChanged,this,[=](){
        mCheckState.Sys=ui->checkBox_5->isChecked();
        AddData();
    });
    connect(ui->checkBox_6,&QCheckBox::stateChanged,this,[=](){
      mCheckState.Train=ui->checkBox_6->isChecked();
      AddData();
    });
    ui->comboBox->addItem("全部");
    mSystemaAlarmInfoWidget =new SystemaAlarmInfoWidget(this);
}

SystemAlarm::~SystemAlarm()
{
    delete ui;
}
void SystemAlarm::InitTable()
{
    mCTableView= new TableView(ui->widget);
    ui->widget->layout()->addWidget(mCTableView);
    mCTableView->mTableViewInfo.Init();
    mCTableView->mTableViewInfo.Vect_HeaderData.append("级别");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("类型");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("时间");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("描述");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(230);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("来源");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(50);
    mCTableView->mTableViewInfo.Vect_HeaderData.append(" ");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(0);

    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHeight(0,3);
    mCTableView->SetTableHeadHeight(1,20);
    //mCTableView->SetTableHeadWidth(0,2);
    //mCTableView->sett
    mCTableView->IintTableColAndRow(1000, 6);
    mCTableView->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    mCTableView->SetTableHeadsetStyleSheet(0,
        "QTableView{"
            "background:rgb(255, 255, 255);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}"
                                           );
//    mCTableView->SetTableHeadsetStyleSheet(1,
//        "QHeaderView{"
//            "background:rgb(255, 255, 255);"
//        "}"
//        "QHeaderView::section:horizontal {"
//            "background:rgb(240, 240, 240);"
//            "font-weight: normal;"
//        "}");
    //itemRBtnDoubClicked
    connect(mCTableView,SIGNAL(itemLBtnClicked(QModelIndex)),this,SLOT(itemLBtnClicked(QModelIndex)));
    connect(mCTableView,SIGNAL(itemRBtnDoubClicked(QModelIndex)),this,SLOT(itemRBtnDoubClickedSolt(QModelIndex)));
}


void SystemAlarm::SetServerAlarm(QByteArray recvArray, int recvlen)
{
    TableData mTableData;

    if((int)recvArray[9]==0x40)
    {
        //系统消息框内容
        if((int)recvArray[10]==0x01)
        {
            mTableData.Init();
            //信息级别
            if((int)recvArray[11]==0x01)
            {
                mTableData.strLevel="错误";
            }
            else if((int)recvArray[11]==0x02)
            {
                mTableData.strLevel="警告";
            }
            else if((int)recvArray[11]==0x03)
            {
                mTableData.strLevel="信息";
            }
            else if((int)recvArray[11]==0x04)
            {
                mTableData.strLevel="防错办返回信息";
            }
            //信息类型
            if((int)recvArray[12]==0x01)
            {
                mTableData.strType="系统";
            }
            else if((int)recvArray[12]==0x02)
            {
                mTableData.strType="行车";
            }
            //信息时间 7
            {
                int year;
                int a =((int)recvArray[13])&0xff;
                int b=((int)recvArray[14])<<8;
                year=a+b;
                int month=(int)recvArray[15];
                int data=(int)recvArray[16];
                int hour=(int)recvArray[17];
                int minute=(int)recvArray[18];
                int second=(int)recvArray[19];
                mTableData.strData=QString::number(year)+"-"+QString::number(month)+"-"
                        +QString::number(data)+" "+QString::number(hour)+":"
                        +QString::number(minute)+":"+QString::number(second);



            }
            //信息描述长度 1
            int nlen1=0;
            nlen1=(int)recvArray[20];
            //信息描述内容
            QByteArray Data;
            Data=recvArray.mid(21,nlen1);
            mTableData.strDescribe = QString::fromLocal8Bit(Data);
            //信息来源长度 1
            int nlen2=0;
            nlen2=(int)recvArray[20+nlen1+1];
            //信息来源内容
            Data=recvArray.mid(20+nlen1+2,nlen2);
            mTableData.strSource = QString::fromLocal8Bit(Data);
            mVectTableDate.append(mTableData);
            AddData();
        }
        else if((int)recvArray[10]==0x02)
        {
            //类型
            int ntype =(int)recvArray[11];
            //信息描述长度 1
            int nlen1=0;
            nlen1=(int)recvArray[12];
            //信息描述内容
            QByteArray Data;
            Data=recvArray.mid(13,nlen1);
            QString strDescribe = QString::fromLocal8Bit(Data);
            if(ntype==0x01)
            {
                QMessageBox::information(this, tr("信息"), strDescribe,tr("确定"),tr("取消"),"",0);
            }
            else if(ntype==0x02)
            {
                QMessageBox::question(this, tr("问题"), strDescribe,tr("确定"),tr("取消"),"",0);
            }
            else if(ntype==0x03)
            {
                QMessageBox::warning(this, tr("警告"), strDescribe,tr("确定"),tr("取消"),"",0);
            }
            else if(ntype==0x04)
            {
                QMessageBox::critical(this, tr("错误"), strDescribe,tr("确定"),tr("取消"),"",0);
            }
        }
    }
}
void SystemAlarm::InitData(QVector<TableData> VectTableDate)
{
    mVectTableDate.clear();
    for(int i=0;i<VectTableDate.size();i++)
    {
        mVectTableDate.append(VectTableDate[i]);
    }
    AddData();
}

void SystemAlarm::AddData()
{
    QStringList strlist;

//    for(int i=0;i<mVectTableDate.size();i++)
    for(int i=mVectTableDate.size()-1;i>=0;i--)
    {
        if((mVectTableDate[i].strLevel=="错误"&&mCheckState.Level1)
                ||(mVectTableDate[i].strLevel=="警告"&&mCheckState.Level2)
                ||(mVectTableDate[i].strLevel=="信息"&&mCheckState.Level3)
                )
        {
            if((mVectTableDate[i].strType=="系统"&&mCheckState.Sys)
                    ||(mVectTableDate[i].strType=="行车"&&mCheckState.Train))
            {
                strlist.append(mVectTableDate[i].strLevel);
                strlist.append(mVectTableDate[i].strType);
                strlist.append(mVectTableDate[i].strData);
                strlist.append(mVectTableDate[i].strDescribe);
                strlist.append(mVectTableDate[i].strSource);
                strlist.append("");
            }
        }

    }
    mCTableView->SetTableData(strlist,Qt::AlignLeft);
}
void SystemAlarm::UpDate()
{
    QStringList strlist;
    for(int i=0;i<mVectTableDate.size();i++)
    {
        if((mVectTableDate[i].strLevel=="错误"&&mCheckState.Level1)
                ||(mVectTableDate[i].strLevel=="警告"&&mCheckState.Level2)
                ||(mVectTableDate[i].strLevel=="信息"&&mCheckState.Level3)
                ||(mVectTableDate[i].strType=="系统"&&mCheckState.Sys)
                ||(mVectTableDate[i].strType=="行车"&&mCheckState.Train))
        {
            strlist.append(mVectTableDate[i].strLevel);
            strlist.append(mVectTableDate[i].strType);
            strlist.append(mVectTableDate[i].strData);
            strlist.append(mVectTableDate[i].strDescribe);
            strlist.append(mVectTableDate[i].strSource);
            strlist.append("");
        }

    }
    mCTableView->SetTableData(strlist);
}
void SystemAlarm::closeEvent (QCloseEvent *e)
{
    emit WidgetCloseSignal();
    //e->accept();
    e->ignore();
}
void SystemAlarm::itemRBtnDoubClickedSolt(const QModelIndex &index)
{
    if(index.row()<mCTableView->mTableViewInfo.Vect_RowItem.size())
    {
//        qDebug()<<"---左键双击"<<index.row()<<mCTableView->mTableViewInfo.Vect_RowItem.size();
        SetBacklackRow(mCTableView,index.row());
        //emit BtnEnSignal(bbtnen);
        mSystemaAlarmInfoWidget->InitData(mVectTableDate[mVectTableDate.size()-index.row()-1]);
        mSystemaAlarmInfoWidget->show();
    }
}
void SystemAlarm::itemLBtnClicked(const QModelIndex &index)
{
    if(index.row()<mCTableView->mTableViewInfo.Vect_RowItem.size())
    {
         SetBacklackRow(mCTableView,index.row());
    }
    else
    {
        SetBacklackRow(mCTableView,-1);
    }
}
void SystemAlarm::SetBacklackRow(TableView* mCTableView,int index)
{
    QColor color,color2;
    color = QColor(5, 125, 255);
    color2= Qt::white;
    for(int i=0;i<mCTableView->mTableViewInfo.Vect_RowItem.size();i++)
    {
        if(i==index)
        {
            mCTableView->SetRowItemsBKColor(i, color);
        }
        else
        {
            mCTableView->SetRowItemsBKColor(i, color2);
        }
    }
}
