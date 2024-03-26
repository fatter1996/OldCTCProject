#include "ddordersign.h"
#include "ui_ddordersign.h"
#include <qdatetime.h>
#include <QMessageBox>
DDOrderSign::DDOrderSign(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DDOrderSign)
{
    ui->setupUi(this);
    setFixedSize(this->width(),this->height());
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_3->layout()->addWidget(mCTableView);
    mDDOrderNoSignWidget=new DDOrderNoSignWidget();
    //拒绝
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(204, QCursor::pos());
        mDDOrderNoSignWidget->Init();
        mDDOrderNoSignWidget->show();
        mDDOrderNoSignWidget->exec();
        if(mDDOrderNoSignWidget->bok)
        {
            emit DispatchOrderStationSignSignal(mDispatchOrderStation->order_id,false);
            mDispatchOrderStation->nStateDisOrder=4;
            emit DDOrderSignSignal();
            close();
        }
    });
    //签收
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(122, QCursor::pos());
        //确认调度命令签收人员是否登录
        if(ui->lineEdit_7->text()=="DEFAULT")
        {
             QMessageBox::critical(this, tr("错误"), tr("用户未登录，请登录"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            emit DispatchOrderStationSignSignal(mDispatchOrderStation->order_id,true);
            mDispatchOrderStation->nStateDisOrder=3;
            mDispatchOrderStation->timSign=QDateTime::currentDateTime();
            mDispatchOrderStation->strSignName="值班员";
            emit DDOrderSignSignal();
            close();
        }

    });
    //关闭
    connect(ui->pushButton_3,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(123, QCursor::pos());
        mDispatchOrderStation->nStateDisOrder=2;
        close();
    });
}

DDOrderSign::~DDOrderSign()
{
    delete ui;
}
void DDOrderSign::Init(QString struser)
{
    //mDispatchOrderStation

    //命令类型名称
    ui->lineEdit->setText(mDispatchOrderStation->strType);
    //命令号码
    ui->lineEdit_2->setText(mDispatchOrderStation->strNumber);
    //日期
    ui->lineEdit_3->setText(mDispatchOrderStation->timOrder.toString("yyyy-MM-dd"));
    //发令时刻
    ui->lineEdit_4->setText(mDispatchOrderStation->timOrder.toString("hh:mm"));
    //发令单位
//    if(mDispatchOrderStation->listRecvPlace[0].size()>0)
//    {
//       ui->lineEdit_5->setText(mDispatchOrderStation->listRecvPlace[0]);
//    }
     ui->lineEdit_5->setText(mDispatchOrderStation->strDisCenter);
    //调度员姓名
    ui->lineEdit_6->setText(mDispatchOrderStation->strDisName);
    //命令内容
    ui->textEdit->setText(mDispatchOrderStation->strContent);

    if(mDispatchOrderStation->nStateDisOrder==3)
    {
        ui->pushButton_2->setEnabled(false);
    }
    else if(mDispatchOrderStation->nStateDisOrder==4)
    {
        ui->pushButton->setEnabled(false);
    }
    //签收人姓名
    if(struser!="")
    {
        ui->lineEdit_7->setText(struser);
    }
    else
    {
       ui->lineEdit_7->setText("DEFAULT");
    }
    //DDOrderUpDate();
}
void DDOrderSign::InitTableView()
{
//    QHBoxLayout *layout=new QHBoxLayout();
//    layout->setDirection(QBoxLayout::LeftToRight);
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();

    mCTableView->mTableViewInfo.Vect_HeaderData.append("序号");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("受令单位");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(160);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("抄知处所");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(160);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(5, 3);
//    mCTableView->SetTableHeadsetStyleSheet(0,
//        "QTableView{"
//            "outline:none;"
//            "gridline-color: rgb(200, 200, 200);"
//        "}"
//        "QHeaderView::section {"
//            "background:rgb(240, 240, 240);"
//        "}"
//        "QTableView QTableCornerButton::section {"
//            "background:rgb(240, 240, 240);"
//            "font-size:9pt;"
//            "font-family:微软雅黑;"
//            "border-top:2px outset rgb(240, 240, 240);"
//            "border-left:2px outset rgb(240, 240, 240);"
//            "border-bottom:2px outset rgb(80, 80, 80);"
//            "border-right:2px outset rgb(80, 80, 80);"
//        "}"
//        "QTableView QTableCornerButton::section:Pressed {"
//            "background:rgb(240, 240, 240);"
//            "border-bottom:2px outset rgb(240, 240, 240);"
//            "border-right:2px outset rgb(240, 240, 240);"
//            "border-top:2px outset rgb(80, 80, 80);"
//            "border-left:2px outset rgb(80, 80, 80);"
//        "}"
//        "QTableView::item{"
//            "border:none;"
//            "outline:none;"
//            "background:rgb(250, 250, 250);"
//        "}"
//        "QTableView::item:selected {"
//            "outline:none;"
//            "background:rgba(5, 125, 255);"
//        "}");

    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");


//    mCTableView->SetTableHeadsetStyleSheet(2,
//        "QHeaderView{"
//            "background:rgb(240, 240, 240);"
//        "}"
//        "QHeaderView::section:vartical {"
//            "background:rgb(240, 240, 240);"
//            "font-size: 12px;"
//            "font-weight: normal;"
//        "}");

}

void DDOrderSign::DDOrderUpDate()
{
    QStringList strList;
    for (int i=0;i<mDispatchOrderStation->listRecvPlace.size();i++)
    {
        strList<<QString::number(i+1)<<mDispatchOrderStation->listRecvPlace[i]<<"";
    }
    mCTableView->SetTableData(strList);
}
