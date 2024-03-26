#include "transmittrain.h"
#include "ui_transmittrain.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")
TransmitTrain::TransmitTrain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransmitTrain)
{
    ui->setupUi(this);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    ui->comboBox_2->addItem("");
    ui->comboBox_3->addItem("DF4");
    ui->comboBox_3->addItem("HXD1");
    ui->comboBox_3->addItem("HXD2");
    ui->comboBox_3->addItem("HXD3");
    ui->comboBox_3->addItem("QJ(001)");
    ui->comboBox_3->addItem("QJ(002)");
    ui->comboBox_3->addItem("QJ(003)");
    ui->comboBox_3->addItem("SS4");
    mDispatchOrderStation=new DispatchOrderStation();
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_2->layout()->addWidget(mCTableView);
    ui->pushButton_5->setEnabled(false);
    ui->pushButton_7->setEnabled(false);
    //添加
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(216, QCursor::pos());
        if(ui->comboBox_2->currentText()==""||ui->lineEdit_4->text()=="")
        {
             QMessageBox::warning(this, tr("STPC"), tr("车次或机车信息不全"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            mTableShow.Init();
            mTableShow.strcheci=ui->comboBox_2->currentText();
            mTableShow.strtrainnum=ui->lineEdit_4->text();
            //mVectTableShow.clear();
            mVectTableShow.append(mTableShow);
            AddTableView();
             ui->pushButton_5->setEnabled(true);
        }
    });
    //修改
    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(214, QCursor::pos());
        if(nCheckRowLeft==-1)
        {
             QMessageBox::warning(this, tr("STPC"), tr("未选中修改信息"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            EditTableView(nCheckRowLeft);
        }
    });
    //删除
    connect(ui->pushButton_3,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(215, QCursor::pos());
        if(nCheckRowLeft==-1)
        {
             QMessageBox::warning(this, tr("STPC"), tr("未选中删除信息"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            DeleteTableView(nCheckRowLeft);
        }
    });
    //查询
    connect(ui->pushButton_4,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(217, QCursor::pos());
    });
    //发送
    connect(ui->pushButton_5,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(210, QCursor::pos());
        DispatchOrderStation mDispatchOrderStationsend;
        mDispatchOrderStationsend.order_id=mDispatchOrderStation->order_id;

        mDispatchOrderStationsend.uNumber=ui->lineEdit->text().toInt();
        mDispatchOrderStationsend.strType=ui->comboBox->currentText();
        mDispatchOrderStationsend.timOrder=QDateTime::currentDateTime();
        mDispatchOrderStationsend.strDisCenter=ui->lineEdit_2->text();
        mDispatchOrderStationsend.strDisName=ui->lineEdit_2->text();
        //命令内容
        mDispatchOrderStationsend.strContent=ui->textEdit->toPlainText();
        //发送
         mDispatchOrderStationsend.nStateDisOrder=mDispatchOrderStation->nStateDisOrder;
          //机车号 strTrainnum 车次号
        mDispatchOrderStationsend.strTrainnum="";
        mDispatchOrderStationsend.strCheci="";
        for(int i=0;i<mVectTableShow.size();i++)
        {
            if(i==0)
            {
                mDispatchOrderStationsend.strTrainnum=mVectTableShow[i].strtrainnum;
                mDispatchOrderStationsend.strCheci=mVectTableShow[i].strcheci;
            }
            else
            {
                mDispatchOrderStationsend.strTrainnum=mDispatchOrderStationsend.strTrainnum+","+mVectTableShow[i].strtrainnum;
                mDispatchOrderStationsend.strCheci=mDispatchOrderStationsend.strCheci+","+mVectTableShow[i].strcheci;
            }
        }
        //mDispatchOrderStation.strSignName=QDateTime::currentDateTime();
        //mDispatchOrderStation.timSign=QDateTime::currentDateTime();
        emit TransmitTrainSignal(mDispatchOrderStationsend);
        close();
    });
    //打印
    connect(ui->pushButton_6,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(211, QCursor::pos());
    });
    //保存
    connect(ui->pushButton_7,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(213, QCursor::pos());
    });
    //取消
    connect(ui->pushButton_8,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        close();
    });
}

TransmitTrain::~TransmitTrain()
{
    delete ui;
}
void TransmitTrain::InitTableView()
{
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();
    mCTableView->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(30);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("车次号");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("机车号");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(20, 3);
    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
    //鼠标左键点击响应
    connect(mCTableView,SIGNAL(itemLBtnClicked(QModelIndex)),this,SLOT(itemLBtnClicked(QModelIndex)));
}
void TransmitTrain::Init(QString strtitle,DispatchOrderStation *pDispatchOrderStation)
{
    this->setWindowTitle(strtitle);
    mDispatchOrderStation=nullptr;
    mDispatchOrderStation=pDispatchOrderStation;
    //命令类型名称
    ui->comboBox->clear();
    ui->comboBox_2->clear();
    ui->lineEdit_4->setText("");
    mCTableView->Clear();
    mVectTableShow.clear();
    ui->comboBox->addItem(mDispatchOrderStation->strType);
    ui->comboBox_2->addItem("");
    //ui->comboBox->setCurrentText(mDispatchOrderStation->strType);
    //命令号码
    ui->lineEdit->setText(mDispatchOrderStation->strNumber);
    //日期
   // ui->lineEdit_3->setText(mDispatchOrderStation->timOrder.toString("yyyy-MM-dd"));
    //发令时刻
   // ui->lineEdit_4->setText(mDispatchOrderStation->timOrder.toString("hh:mm"));
    //发令单位
     ui->lineEdit_2->setText(mDispatchOrderStation->strDisCenter);
    //调度员姓名
    ui->lineEdit_3->setText(mDispatchOrderStation->strDisName);
    //命令内容
    ui->textEdit->setText(mDispatchOrderStation->strContent);

//    if(mDispatchOrderStation->nStateDisOrder==3)
//    {
//        ui->pushButton_2->setEnabled(false);
//    }
//    else if(mDispatchOrderStation->nStateDisOrder==4)
//    {
//        ui->pushButton->setEnabled(false);
//    }
    //签收人姓名
//    if(struser!="")
//    {
//        ui->lineEdit_7->setText(struser);
//    }
//    else
//    {
//       ui->lineEdit_7->setText("DEFAULT");
//    }
}
void TransmitTrain::AddTableView()
{
    QStringList strList;
//    mVectTableShow
    for(int i=0;i<mVectTableShow.size();i++)
    {
        strList<<QString::number(i+1)<<mVectTableShow[i].strcheci<<mVectTableShow[i].strtrainnum;
    }
    mCTableView->SetTableData(strList);
}
void TransmitTrain::DeleteTableView(int n)
{
    if(n>=0&&n<mVectTableShow.size())
    {
        mVectTableShow.remove(n);
        AddTableView();
    }
}
void TransmitTrain::EditTableView(int n)
{
    if(n>=0&&n<mVectTableShow.size())
    {
        ui->comboBox_2->clear();
        ui->comboBox_2->addItem(mVectTableShow[n].strcheci);
        ui->lineEdit_4->setText(mVectTableShow[n].strtrainnum);
    }
}
void TransmitTrain::itemLBtnClicked(const QModelIndex &index)
{
    QColor color,color2;
    color = Qt::blue;
    color2= Qt::white;
    nCheckRowLeft=index.row();
//    if(index.row()<mCTableView->mTableViewInfo.Vect_RowItem.size())
//    {
//        SetBacklackRow(mCTableView,nCheckRowLeft);
//        //ui->textBrowser->setText(mArrayDisOrderRecv[index.row()]->strContent);
//        DDOrderUpDate2(index.row());

//    }

}
