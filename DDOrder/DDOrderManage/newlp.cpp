#include "newlp.h"
#include "ui_newlp.h"
#include <QMessageBox>
#pragma execution_character_set("utf-8")
NewLP::NewLP(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewLP)
{
    ui->setupUi(this);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    ui->comboBox_3->addItem("DF4");
    ui->comboBox_3->addItem("HXD1");
    ui->comboBox_3->addItem("HXD2");
    ui->comboBox_3->addItem("HXD3");
    ui->comboBox_3->addItem("QJ(001)");
    ui->comboBox_3->addItem("QJ(002)");
    ui->comboBox_3->addItem("QJ(003)");
    ui->comboBox_3->addItem("SS4");
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_2->layout()->addWidget(mCTableView);
    //添加
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(216, QCursor::pos());
        if(ui->comboBox_2->currentText()==""||ui->lineEdit->text()==""||ui->lineEdit_4->text()=="")
        {
            bAddFalg=false;
             QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未录入"),tr("确定"),tr("取消"),"",0);
        }
        else
        {
            QStringList strlist;
            strlist<<"1"<<ui->comboBox_2->currentText()<<ui->lineEdit_4->text();

            mTableShow.Init();
            mTableShow.strcheci=ui->comboBox_2->currentText();
            mTableShow.strtrainnum=ui->lineEdit_4->text();
            mVectTableShow.append(mTableShow);
            UpDateTableView();
             bAddFalg=true;
        }
    });

    //修改
    connect(ui->pushButton_2,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(214, QCursor::pos());
    });
    //删除
    connect(ui->pushButton_3,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(215, QCursor::pos());
    });
    //查询
    connect(ui->pushButton_4,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(217, QCursor::pos());
    });

    //发送
    connect(ui->pushButton_5,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(210, QCursor::pos());
        if(mVectTableShow.size()>0)
        {
            DispatchOrderLocomotive *mDispatchOrderLocomotive=new DispatchOrderLocomotive();
            //命令号
            mDispatchOrderLocomotive->uNumber=ui->lineEdit->text().toInt();
            //命令类型
            mDispatchOrderLocomotive->strType=ui->label->text();
            //创建时间
            mDispatchOrderLocomotive->timCreate=QDateTime::currentDateTime();
            //发送时间
            mDispatchOrderLocomotive->timSend=QDateTime::currentDateTime();
            //调度中心
            mDispatchOrderLocomotive->strStation=ui->lineEdit_2->text();
            //调度员
            mDispatchOrderLocomotive->strDutyName=ui->lineEdit_3->text();
            //命令内容
            mDispatchOrderLocomotive->strContent=ui->textEdit->toPlainText();
            //        //车次
            //        mDispatchOrderLocomotive->strCheci=ui->comboBox_2->currentText();
            //        //机车号
            //        mDispatchOrderLocomotive->strTrainnum=ui->lineEdit_4->text();
            //命令种类
            mDispatchOrderLocomotive->orderType=GetOrderType(ui->label->text());
            //发送状态
            mDispatchOrderLocomotive->bSend=true;

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
                mDispatchOrderLocomotive->vectLocmtInfo.append(mLocomotiveInfo);
            }
            emit NewLPDataSignal(2,mDispatchOrderLocomotive);
            //bAddFalg=false;

            close();
        }
        else
        {
            QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未录入"),tr("确定"),tr("取消"),"",0);
        }
    });
    //打印
    connect(ui->pushButton_6,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(211, QCursor::pos());
    });
    //保存
    connect(ui->pushButton_7,&QPushButton::clicked,this,[=](){
        emit SendTextAnalysisSignal(213, QCursor::pos());
        if(mVectTableShow.size()>0)
        {
            DispatchOrderLocomotive *mDispatchOrderLocomotive=new DispatchOrderLocomotive();
            //命令号
            mDispatchOrderLocomotive->uNumber=ui->lineEdit->text().toInt();
            //命令类型
            mDispatchOrderLocomotive->strType=ui->label->text();
            //接收时间
            mDispatchOrderLocomotive->timCreate=QDateTime::currentDateTime();
            //调度中心
            mDispatchOrderLocomotive->strStation=ui->lineEdit_2->text();
            //调度员
            mDispatchOrderLocomotive->strDutyName=ui->lineEdit_3->text();
            //命令内容
            mDispatchOrderLocomotive->strContent=ui->textEdit->toPlainText();
            //命令种类
            mDispatchOrderLocomotive->orderType=GetOrderType(ui->label->text());
            //发送状态
            mDispatchOrderLocomotive->bSend=false;

            for(int i=0;i<mVectTableShow.size();i++)
            {
                LocomotiveInfo mLocomotiveInfo;
//                if(i==0)
//                {
//                    mLocomotiveInfo.strLocomotive=mVectTableShow[i].strtrainnum;
//                    mLocomotiveInfo.strCheCi=mVectTableShow[i].strcheci;
//                }
//                else
//                {
//                    mLocomotiveInfo.strLocomotive=mLocomotiveInfo.strLocomotive+","+mVectTableShow[i].strtrainnum;
//                    mLocomotiveInfo.strCheCi=mLocomotiveInfo.strCheCi+","+mVectTableShow[i].strcheci;
//                }
                mLocomotiveInfo.strLocomotive=mVectTableShow[i].strtrainnum;
                mLocomotiveInfo.strCheCi=mVectTableShow[i].strcheci;
                mDispatchOrderLocomotive->vectLocmtInfo.append(mLocomotiveInfo);
            }
            emit NewLPDataSignal(1,mDispatchOrderLocomotive);
            close();
        }
        else
        {
            QMessageBox::warning(this, tr("STPC"), tr("存在关键信息未录入"),tr("确定"),tr("取消"),"",0);
        }

    });
    //取消
    connect(ui->pushButton_8,&QPushButton::clicked,this,[=](){
       emit SendTextAnalysisSignal(78, QCursor::pos());
       close();
    });

}

NewLP::~NewLP()
{
    delete ui;
}
void NewLP::ClearShow()
{
    ui->lineEdit->setText("");
    ui->lineEdit_4->setText("");
    ui->comboBox_2->clear();
    ui->comboBox_2->clearEditText();
    mVectTableShow.clear();
    UpDateTableView();
}
void NewLP::Init(QString strsta,QString strname,int norder)
{
    ui->lineEdit_2->setText(strsta);
    ui->lineEdit_3->setText(strname);
    ui->lineEdit->setText(QString::number(norder+1));
}
void NewLP::SetTitel(QString str)
{
    this->setWindowTitle(str);
}
void NewLP::SetLable(QString str)
{
    ui->label->setText(str);
}
void NewLP::SetLableBlackColor(int n)
{
    if(n==1)//路票
    {
        ui->label->setStyleSheet("background-color: rgb(255, 255, 255);");
        ui->textEdit->setText("                   电话记录第  号\n\n                 车次\n\n                  XXX站至  站 \n\n                   （XXX）编号");
    }
    else if(n==2)//绿色许可证
    {
        ui->label->setStyleSheet("background-color: rgb(0, 255, 0);");
        //ui->textEdit->setText("");
        ui->textEdit->setText("                        许可证\n                                                第    号\n    在出站(进路)信号机故障、未设出站信号机、列车头部越过出站(进路)信号机的情况下，准许第\n次列车由    线上发车。\n                            XXX站，  车站值班员：\n                                 年  月  日填发");

    }
    else if(n==3)//红色许可证
    {
        ui->label->setStyleSheet("background-color: rgb(255, 0, 0);");
        //ui->textEdit->setText("");
        ui->textEdit->setText("                        许可证\n                                                第    号\n   现在一切电话中断，准许第   次列车自    站至   站，本列车前于   时   分发出的第   次列车，邻站到达通知  收到。\n                        通知书\n   1.第   次列车到达你站后，准接你站发出的列车。\n   2.于  时  分发出第   次列车，并于  时  分再发出第    次列车。\n                            XXX站，  车站值班员：\n                                 年  月  日填发");
    }
    else if(n==4)//调车作业单
    {
        ui->label->setStyleSheet("background-color: rgb(0, 113, 83);");
         ui->textEdit->setText("");
    }
    else if(n==5)//书面通知
    {
        ui->label->setStyleSheet("background-color: rgb(0, 255, 255);");
        ui->textEdit->setText("                        书面通知\n 第          次司机\n    监督器上不能确认第一个闭塞分区空闲，以在瞭望距离内能随时停车的速度，最高不超过20km/h，运行至第一架通过信号机，按其显示的要求执行。\n           xx站(站名印)车站值班员（签名）：\n                                 年  月  日填发");
    }
    else if(n==6)//半自动
    {
        ui->label->setStyleSheet("background-color:rgb(255, 255, 0);");
         ui->textEdit->setText("                 半自动闭塞发车进路通知书\n                                             第    号\n     1.在列车头部越过发车进路信号机的情况下，准许第            次列车由            线发车。\n     2.在            发车进路信号机故障的情况下，准许第            次列车越过该发车进路信号机。\n           xx站(站名印)车站值班员（签名）：\n                                 年  月  日填发");
    }
    else if(n==7)//自动站间
    {
        ui->label->setStyleSheet("background-color:rgb(182, 182, 182);");
        ui->textEdit->setText("               自动站间闭塞发车进路通知书\n                                             第    号\n    1.在列车头部越过发车进路信号机的情况下，准许第          次列车由          线发车。\n    2.在          发车进路信号机故障的情况下，准许第          次列车越过该发车进路信号机。\n           xx站(站名印)车站值班员（签名）：\n                                 年  月  日填发");
    }
}
void NewLP::InitTableView()
{
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();

    mCTableView->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("车次号");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("机车号");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(115);

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
            "}");

}
void NewLP::UpDateTableView()
{
    QStringList strlist1;
    for(int i=0;i<mVectTableShow.size();i++)
    {
        strlist1<<QString::number(i+1)<<mVectTableShow[i].strcheci<<mVectTableShow[i].strtrainnum;
    }
     mCTableView->SetTableData(strlist1);
}
int NewLP::GetOrderType(QString str)
{
    int ntype=0;
    if(str.contains("调度命令"))
    {
        ntype=0x01;
    }
    else if(str.contains("路票"))
    {
        ntype=0x02;
    }
    else if(str.contains("绿色许可证"))
    {
        ntype=0x03;
    }
    else if(str.contains("红色许可证"))
    {
        ntype=0x04;
    }
    else if(str.contains("调车作业单"))
    {
        ntype=0x05;
    }
    else if(str.contains("书面通知"))
    {
        ntype=0x06;
    }
    else if(str.contains("半自动闭塞发车进路通知书"))
    {
        ntype=0x07;
    }
    else if(str.contains("自动站间闭塞发车进路通知书"))
    {
        ntype=0x08;
    }

    return ntype;
}
