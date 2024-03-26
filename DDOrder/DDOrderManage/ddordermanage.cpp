#include "ddordermanage.h"
#include "ui_ddordermanage.h"
#include <QLabel>
#include<QDebug>
#include <QHBoxLayout>
#include <QMessageBox>
#pragma execution_character_set("utf-8")
DDOrderManage::DDOrderManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DDOrderManage)
{
    ui->setupUi(this);
    //setWindowModality(Qt::ApplicationModal);
    //隐藏最大化、最小化
    //this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    ui->widget->setContentsMargins(0,0,0,0);
    ui->widget_8->setContentsMargins(0,0,0,0);
    ui->widget_8->layout()->setSpacing(0);
    ui->widget_8->layout()->setMargin(0);
    this->layout()->setSpacing(0);
    this->setFixedSize(this->width(),this->height());
    Init();
    InitParameter();
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget_2->setCurrentIndex(0);
    mDDOrderNoSignWidget=new DDOrderNoSignWidget();
}

DDOrderManage::~DDOrderManage()
{
    delete ui;
}
void DDOrderManage:: Init()
{
    //ui->treeWidget->hide();
    m_TimeMansge = new TimeMansge();
    layout1=new QHBoxLayout(this);
    layout2=new QHBoxLayout(this);
    //设置布局管理器中所有控件的布局方向为从右往左依次排列
    layout1->setDirection(QBoxLayout::RightToLeft);
    layout2->setDirection(QBoxLayout::RightToLeft);
    {
        QPushButton *mBtn1 = new QPushButton(this);
        mBtn1->setText("阅读");
        mBtn1->setObjectName("STA");
        connect(mBtn1,&QPushButton::clicked,[=](){
            emit SendTextAnalysisSignal(205, QCursor::pos());
        });

        QPushButton *mBtn2 = new QPushButton(this);
        mBtn2->setText("拒签");
        mBtn2->setObjectName("STA");

        QPushButton *mBtn3 = new QPushButton(this);
        mBtn3->setText("签收");
        mBtn3->setObjectName("STA");
        QPushButton *mBtn4 = new QPushButton(this);
        mBtn4->setText("打印全部");
        mBtn4->setObjectName("STA");
        mBtn4->setEnabled(false);
        connect(mBtn1,&QPushButton::clicked,[=](){
            emit SendTextAnalysisSignal(207, QCursor::pos());
        });

        QPushButton *mBtn5 = new QPushButton(this);
        mBtn5->setText("打印当前");
        mBtn5->setObjectName("STA");
        mBtn5->setEnabled(false);
        connect(mBtn1,&QPushButton::clicked,[=](){
            emit SendTextAnalysisSignal(208, QCursor::pos());
        });

        QPushButton *mBtn6 = new QPushButton(this);
        mBtn6->setText("关闭");
        connect(this,&DDOrderManage::BtnEnSignal,[=](){
           mBtn2->setEnabled(bbtnen);
           mBtn3->setEnabled(bbtnen);
        });
        connect(mBtn6,&QPushButton::clicked,[=](){
            emit SendTextAnalysisSignal(123, QCursor::pos());
            close();
        });
        QPushButton *mBtn7 = new QPushButton(this);
        mBtn7->setText("新建");
        mBtn7->setObjectName("TRAIN");
        connect(mBtn7,&QPushButton::clicked,[=](){
           emit SendTextAnalysisSignal(209, QCursor::pos());
           if(ntreecheck==2)
           {
                mNewddml->Init(strSta,strUser,GetOderNumNew(0));
               mNewddml->show();
           }
           else if(ntreecheck==3)
           {
               mChoseType->show();
           }
        });
//        QPushButton *mBtn8 = new QPushButton(this);
        mBtn8 = new QPushButton(this);
        mBtn8->setText("发送");
        mBtn8->setObjectName("TRAIN");
        //发送
        connect(mBtn8,&QPushButton::clicked,[=](){
            //emit DispatchOrderStationSignSignal(mArrayDisOrderRecv[nCheckRowLeft]->order_id,false);
            //调度台 ntreecheck=2; 机车 ntreecheck=3;
            emit SendTextAnalysisSignal(210, QCursor::pos());
            if(ui->label_3->text().contains("待发箱"))
            {
                if(ntreecheck==2)
                {
//                    qDebug()<<"发送   "<<"调度台"<<ui->label_3->text()<<nCheckRowLeft;
                    if(nCheckRowLeft<mArrayDisOrderDisp.size())
                    {
                        int nCheck=GetDFXOrderIndexDDT(nCheckRowLeft,2);
                        if(nCheck>=0)
                        {
                            mArrayDisOrderDisp[nCheck]->bSend=true;
                            emit SendOrderDispatcher(mArrayDisOrderDisp[nCheck]);
                            //mArrayDisOrderDisp.append(mArrayDisOrderDisp[nCheck])
                        }
                    }
                }
                else if(ntreecheck==3)
                {
//                    qDebug()<<"发送   "<<"机车"<<ui->label_3->text()<<nCheckRowLeft;
                    if(nCheckRowLeft<mArrayDisOrderLocomot.size())
                    {
                        int nCheck=GetDFXOrderIndexTrain(nCheckRowLeft,2);
                        if(nCheck>=0)
                        {
                            mArrayDisOrderLocomot[nCheck]->bSend=true;
                            emit SendOrderLocomotive(mArrayDisOrderLocomot[nCheck]);
                        }
                    }
                }
            }
            //close();
        });
        QPushButton *mBtn9 = new QPushButton(this);
        mBtn9->setText("打印");
        mBtn9->setEnabled(false);
        mBtn9->setObjectName("TRAIN");
        connect(mBtn9,&QPushButton::clicked,[=](){
            emit SendTextAnalysisSignal(211, QCursor::pos());
        });
        //拒绝
        connect(mBtn2,&QPushButton::clicked,[=](){
            //emit DispatchOrderStationSignSignal(mArrayDisOrderRecv[nCheckRowLeft]->order_id,false);
            emit SendTextAnalysisSignal(206, QCursor::pos());

            mDDOrderNoSignWidget->Init();
            mDDOrderNoSignWidget->show();
            mDDOrderNoSignWidget->exec();
            if(mDDOrderNoSignWidget->bok)
            {
                int nCheck=GetOrderIndexSta(nCheckRowLeft);
                if(nCheck>=0)
                {
                    emit DispatchOrderStationSignSignal(mArrayDisOrderRecv[nCheck]->order_id,false);
                }
                if(nCheckRowLeft==0/*mArrayDisOrderRecv.size()-1*/)
                {
                    emit DDOrderSignSignal();
                }
            }
        });
        //签收
        connect(mBtn3,&QPushButton::clicked,[=](){
            //emit DispatchOrderStationSignSignal(mArrayDisOrderRecv[nCheckRowLeft]->order_id,true);
            emit SendTextAnalysisSignal(122, QCursor::pos());
            if( strUser=="DEFAULT")
            {
                 QMessageBox::critical(this, tr("错误"), tr("用户未登录，请登录"),tr("确定"),tr("取消"),"",0);
            }
            else
            {
                int nCheck=GetOrderIndexSta(nCheckRowLeft);
                if(nCheck>=0)
                {
                    emit DispatchOrderStationSignSignal(mArrayDisOrderRecv[nCheck]->order_id,true);
                }
                if(nCheckRowLeft==0/*mArrayDisOrderRecv.size()-1*/)
                {
                   emit DDOrderSignSignal();
                }
            }
        });

        layout1->addWidget(mBtn6);
        layout1->addWidget(mBtn9);
        layout1->addWidget(mBtn8);
        layout1->addWidget(mBtn7);
        layout1->addWidget(mBtn5);
        layout1->addWidget(mBtn4);
        layout1->addWidget(mBtn3);
        layout1->addWidget(mBtn2);
        layout1->addWidget(mBtn1);
         ui->widget_9->setLayout(layout1);
    }

    mNewddml =new Newddml();
    mNewddml->setObjectName("调度台");
    connect(mNewddml, &Newddml::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    mChoseType =new ChoseType();
    connect(mChoseType, &ChoseType::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    mTransmitTrain =new TransmitTrain();
    connect(mTransmitTrain, &TransmitTrain::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    m_NewLP = new NewLP();
    connect(m_NewLP, &NewLP::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    m_Newddml= new Newddml();
    m_Newddml->setObjectName("机车");
    connect(m_Newddml, &Newddml::SendTextAnalysisSignal, [=](int index, QPoint pt, QString dev){
        emit SendTextAnalysisSignal(index, pt, dev);
    });

    connect(mNewddml,SIGNAL(NewDDOrderData(DispatchOrderDispatcher*,QString)),this,SLOT(NewDDOrderDataSlot(DispatchOrderDispatcher*,QString)));
    connect(m_Newddml,SIGNAL(NewDDOrderData(DispatchOrderLocomotive*,QString)),this,SLOT(NewDDOrderDataSlot(DispatchOrderLocomotive*,QString)));
    connect(mTransmitTrain,SIGNAL(TransmitTrainSignal(DispatchOrderStation)),this,SLOT(TransmitTrainSlot(DispatchOrderStation)));
    connect(mChoseType,SIGNAL(ChoseTypeSignal(int)),this,SLOT(ChoseTypeSlot(int)));
    connect(m_NewLP,SIGNAL(NewLPDataSignal(int,DispatchOrderLocomotive*)),this,SLOT(NewLPDataSlot(int,DispatchOrderLocomotive*)));
    InitTree();
}
void DDOrderManage::InitDisTemplate()
{
    mNewddml->mVectDisOrderTemplate=mVectDisOrderTemplate;
    mNewddml->InitDisTemplate();
    m_Newddml->mVectDisOrderTemplate=mVectDisOrderTemplate;
    m_Newddml->InitDisTemplate();
}
void DDOrderManage::InitParameter()
{
    ChangeButton(ntreecheck);
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView(false);
    mCTableView->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_2->layout()->addWidget(mCTableView);

    InitTableView2();
    mCTableView2->SetTableHeadHideV(false);
    mCTableView2->SetTableHeadHideH(true);
    mCTableView2->CreatTableView();
    mCTableView2->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_6->layout()->addWidget(mCTableView2);

    InitTableView3();
    mCTableView3->SetTableHeadHideV(false);
    mCTableView3->SetTableHeadHideH(true);
    mCTableView3->CreatTableView();
    mCTableView3->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_5->layout()->addWidget(mCTableView3);

    InitTableViewDDT();
    mCTableViewDDT->SetTableHeadHideV(false);
    mCTableViewDDT->SetTableHeadHideH(true);
    mCTableViewDDT->CreatTableView(false);
    mCTableViewDDT->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_10->layout()->addWidget(mCTableViewDDT);

    InitTableViewTrain();
    mCTableViewTrain->SetTableHeadHideV(false);
    mCTableViewTrain->SetTableHeadHideH(true);
    mCTableViewTrain->CreatTableView(false);
    mCTableViewTrain->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_11->layout()->addWidget(mCTableViewTrain);

    InitTableViewDDT2();
    mCTableViewDDT2->SetTableHeadHideV(false);
    mCTableViewDDT2->SetTableHeadHideH(true);
    mCTableViewDDT2->CreatTableView();
    mCTableViewDDT2->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_8->layout()->addWidget(mCTableViewDDT2);

    InitTableViewTrain2();
    mCTableViewTrain2->SetTableHeadHideV(false);
    mCTableViewTrain2->SetTableHeadHideH(true);
    mCTableViewTrain2->CreatTableView();
    mCTableViewTrain2->SetEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->widget_12->layout()->addWidget(mCTableViewTrain2);

    //鼠标左键点击响应
    connect(mCTableView,SIGNAL(itemLBtnClicked(QModelIndex)),this,SLOT(itemLBtnClicked(QModelIndex)));
    connect(mCTableViewDDT,SIGNAL(itemLBtnClicked(QModelIndex)),this,SLOT(itemLBtnClickedDDT(QModelIndex)));
    connect(mCTableViewTrain,SIGNAL(itemLBtnClicked(QModelIndex)),this,SLOT(itemLBtnClickedTrain(QModelIndex)));
    //鼠标右键点击响应
    connect(mCTableView,SIGNAL(itemRBtnClicked(QModelIndex)),this,SLOT(itemRBtnClicked(QModelIndex)));
    connect(mCTableViewDDT,SIGNAL(itemRBtnClicked(QModelIndex)),this,SLOT(itemRBtnClickedDDT(QModelIndex)));
    connect(mCTableViewTrain,SIGNAL(itemRBtnClicked(QModelIndex)),this,SLOT(itemRBtnClickedTrain(QModelIndex)));
    //时间范围按钮
    ui->pushButton->setEnabled(false);
//    connect(ui->pushButton,&QPushButton::clicked,[=](){
//        m_TimeMansge->show();
//    });
    //树目录
    connect(ui->treeWidget,&QTreeWidget::itemPressed,this,
            &DDOrderManage::TreeClickedSolt);

    menuR = new QMenu(/*this*/);
    QAction *Action1 = new QAction(tr("转发至机车"), ui->widget_2);
    menuR->addAction(Action1);
    //删除菜单
    menuDel = new QMenu(/*this*/);
    QAction *Action2 = new QAction(tr("删除"), ui->widget_10);
    menuDel->addAction(Action2);

    menuDel2 = new QMenu(/*this*/);
    QAction *Action3 = new QAction(tr("删除"), ui->widget_11);
    menuDel2->addAction(Action3);


}
void DDOrderManage::InitTree()
{
    ui->treeWidget->setRootIsDecorated(false);
    ui->treeWidget->setHeaderHidden(true);
    topItem_Station = new QTreeWidgetItem(ui->treeWidget);
    topItem_Station->setText(0,"车站");

    ui->treeWidget->addTopLevelItem(topItem_Station);

    topItem_Ddt = new QTreeWidgetItem(ui->treeWidget);
    topItem_Ddt->setText(0,"调度台");
    ui->treeWidget->addTopLevelItem(topItem_Ddt);

    topItem_Train = new QTreeWidgetItem(ui->treeWidget);
    topItem_Train->setText(0,"机车");
    ui->treeWidget->addTopLevelItem(topItem_Train);



    QTreeWidgetItem *item11 = new QTreeWidgetItem(topItem_Station);
    item11->setText(0,"收令箱");
    QTreeWidgetItem *item12 = new QTreeWidgetItem(topItem_Station);
    item12->setText(0,"预收令箱");
    ui->treeWidget->addTopLevelItem(topItem_Station);
    ui->treeWidget->setIndentation(20);

    QTreeWidgetItem *item21 = new QTreeWidgetItem(topItem_Ddt);
    item21->setText(0,"发令箱");

    QTreeWidgetItem *item22 = new QTreeWidgetItem(topItem_Ddt);
    item22->setText(0,"储令箱");
    QTreeWidgetItem *item23 = new QTreeWidgetItem(topItem_Ddt);
    item23->setText(0,"待发箱");
    ui->treeWidget->addTopLevelItem(topItem_Ddt);

    QTreeWidgetItem *item31 = new QTreeWidgetItem(topItem_Train);
    item31->setText(0,"发令箱");
    QTreeWidgetItem *item32 = new QTreeWidgetItem(topItem_Train);
    item32->setText(0,"储令箱");
    QTreeWidgetItem *item33 = new QTreeWidgetItem(topItem_Train);
    item33->setText(0,"待发箱");
    ui->treeWidget->addTopLevelItem(topItem_Train);
    //connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showSelectedImage(QTreeWidgetItem*,int)));

}

void DDOrderManage::ChangeButton(int ntype)
{
     QList<QPushButton*> pushButton = ui->widget_9->findChildren<QPushButton*>();
     for(int i = 0; i < pushButton.size(); i++)
     {
         if(ntype==1)//车站
         {
             if(pushButton.at(i)->objectName()=="STA")
             {
                 pushButton.at(i)->setVisible(true);
             }
             else if(pushButton.at(i)->objectName()=="TRAIN")
             {
                 pushButton.at(i)->setVisible(false);
             }
         }
         else if(ntype==2||ntype==3)//机车 收令箱
         {
             if(pushButton.at(i)->objectName()=="STA")
             {
                 pushButton.at(i)->setVisible(false);
             }
             else if(pushButton.at(i)->objectName()=="TRAIN")
             {
                 pushButton.at(i)->setVisible(true);
             }
         }
     }
}
void DDOrderManage::InitTableView()
{
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();
    mCTableView->mTableViewInfo.Vect_HeaderData.append("号码");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(50);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("命令类型");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(260);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("收令日期");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("发令单位");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("发令人");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("签否");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("签收人");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("签收时刻");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(50, 8);
    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");

}

void DDOrderManage::InitTableView2()
{
    mCTableView2= new TableView();
    mCTableView2->mTableViewInfo.Init();

    mCTableView2->mTableViewInfo.Vect_HeaderData.append("序号");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("受令单位");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(160);
    mCTableView2->mTableViewInfo.Vect_HeaderData.append("抄知处所");
    mCTableView2->mTableViewInfo.Vect_HeaderDataWidth.append(160);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView2->SetTableHeadHeight(0, 24);
    mCTableView2->SetTableHeadHeight(1, 16);
    mCTableView2->IintTableColAndRow(5, 3);
    mCTableView2->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}
void DDOrderManage::InitTableView3()
{
    mCTableView3= new TableView();
    mCTableView3->mTableViewInfo.Init();

    mCTableView3->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableView3->mTableViewInfo.Vect_HeaderData.append("阅读人");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(160);
    mCTableView3->mTableViewInfo.Vect_HeaderData.append("阅读时间");
    mCTableView3->mTableViewInfo.Vect_HeaderDataWidth.append(160);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView3->SetTableHeadHeight(0, 24);
    mCTableView3->SetTableHeadHeight(1, 16);
    mCTableView3->IintTableColAndRow(5, 3);
    mCTableView3->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");

}
//调度台
void DDOrderManage::InitTableViewDDT()
{
    mCTableViewDDT= new TableView();
    mCTableViewDDT->mTableViewInfo.Init();

    mCTableViewDDT->mTableViewInfo.Vect_HeaderData.append("编号");
    mCTableViewDDT->mTableViewInfo.Vect_HeaderDataWidth.append(50);
    mCTableViewDDT->mTableViewInfo.Vect_HeaderData.append("请求名称");
    mCTableViewDDT->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewDDT->mTableViewInfo.Vect_HeaderData.append("创建时间");
    mCTableViewDDT->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewDDT->mTableViewInfo.Vect_HeaderData.append("发送时间");
    mCTableViewDDT->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewDDT->mTableViewInfo.Vect_HeaderData.append("值班人");
    mCTableViewDDT->mTableViewInfo.Vect_HeaderDataWidth.append(80);

//    mCTableView->SetSectionText("序号", 48);
    mCTableViewDDT->SetTableHeadHeight(0, 24);
    mCTableViewDDT->SetTableHeadHeight(1, 16);
    mCTableViewDDT->IintTableColAndRow(10, 5);
    mCTableViewDDT->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}
//机车
void DDOrderManage::InitTableViewTrain()
{
    mCTableViewTrain= new TableView();
    mCTableViewTrain->mTableViewInfo.Init();

    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("编号");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(50);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("命令种类");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("命令类型");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("发令单位");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("发令人");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("创建时间");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewTrain->mTableViewInfo.Vect_HeaderData.append("发送时间");
    mCTableViewTrain->mTableViewInfo.Vect_HeaderDataWidth.append(120);

//    mCTableView->SetSectionText("序号", 48);
    mCTableViewTrain->SetTableHeadHeight(0, 24);
    mCTableViewTrain->SetTableHeadHeight(1, 16);
    mCTableViewTrain->IintTableColAndRow(10, 7);
    mCTableViewTrain->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}
//调度台
void DDOrderManage::InitTableViewDDT2()
{
    mCTableViewDDT2= new TableView();
    mCTableViewDDT2->mTableViewInfo.Init();

    mCTableViewDDT2->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderDataWidth.append(30);
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderData.append("调度台");
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderData.append("接收状态");
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderData.append("接收时间");
    mCTableViewDDT2->mTableViewInfo.Vect_HeaderDataWidth.append(120);

//    mCTableView->SetSectionText("序号", 48);
    mCTableViewDDT2->SetTableHeadHeight(0, 24);
    mCTableViewDDT2->SetTableHeadHeight(1, 16);
    mCTableViewDDT2->IintTableColAndRow(20, 4);
    mCTableViewDDT2->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}
//机车
void DDOrderManage::InitTableViewTrain2()
{
    mCTableViewTrain2= new TableView();
    mCTableViewTrain2->mTableViewInfo.Init();

    mCTableViewTrain2->mTableViewInfo.Vect_HeaderData.append("序");
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderDataWidth.append(30);
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderData.append("车次");
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderData.append("机车号");
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderDataWidth.append(60);
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderData.append("接收状态");
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderData.append("接收时间");
    mCTableViewTrain2->mTableViewInfo.Vect_HeaderDataWidth.append(120);

//    mCTableView->SetSectionText("序号", 48);
    mCTableViewTrain2->SetTableHeadHeight(0, 24);
    mCTableViewTrain2->SetTableHeadHeight(1, 16);
    mCTableViewTrain2->IintTableColAndRow(20, 5);
    mCTableViewTrain2->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");
}

void DDOrderManage::SelectedItemSolt(QTreeWidgetItem * item, int column)
{
    QTreeWidgetItem *parent = item->parent();
    if(NULL==parent) //注意：最顶端项是没有父节点的，双击这些项时注意(陷阱)
     return;
    int col = parent->indexOfChild(item); //item在父项中的节点行号(从0开始)

    if(0==col) //Band1
    {
         //执行对应操作
    }
    if(1==col) //Band2
    {
         //执行对应操作
    }
}
void DDOrderManage::TreeClickedSolt(QTreeWidgetItem *item, int column)
{
    //简单响应一下选中状态槽函数
    int itemCount = item->childCount();
     QTreeWidgetItem *parent = item->parent();
     QString strparent="";
     if(parent!=nullptr)
     {
//         qDebug()<<parent->text(0)<<item->text(column)<<column<<itemCount;
         strparent=parent->text(0);
     }
     else
     {
//         qDebug()<<"---"<<item->text(column)<<column<<itemCount;
         strparent="";
     }

     ui->textBrowser->setText("");
     if(item->text(column)=="车站"||item->text(column)=="收令箱"||item->text(column)=="预收令箱")
     {
         ntreecheck=1;
         ChangeButton(ntreecheck);
         ui->stackedWidget->setCurrentIndex(ntreecheck-1);
         ui->stackedWidget_2->setCurrentIndex(ntreecheck-1);
         if(item->text(column)=="预收令箱")
         {
             ui->label->setText(strparent+"命令"+item->text(column));
              DDOrderUpDate(1);
         }
         else
         {
             ui->label->setText("车站命令收令箱");
              DDOrderUpDate(0);
         }
     }
     else if(item->text(column)=="调度台"||strparent=="调度台")
     {
         ntreecheck=2;
         ChangeButton(ntreecheck);
         ui->stackedWidget->setCurrentIndex(ntreecheck-1);
         ui->stackedWidget_2->setCurrentIndex(ntreecheck-1);
         if(strparent!="")
         {
             mBtn8->setEnabled(false);
             ui->label_3->setText(strparent+"命令"+item->text(column));
             int nstate=0;
             if(ui->label_3->text().contains("发令箱"))//发送未签收
             {
                 nstate=0;
             }
             else if(ui->label_3->text().contains("储令箱"))//发送已签收
             {
                 nstate=1;
             }
             else if(ui->label_3->text().contains("待发箱"))//保存已发送
             {
                 nstate=2;
             }
             ncheckddt=nstate;
             UpDataDDTTable(mArrayDisOrderDisp,ncheckddt);
         }
         else
         {
             ui->label_3->setText(item->text(column)+"命令发令箱");
             UpDataDDTTable(mArrayDisOrderDisp,0);
         }
         mCTableViewDDT2->Clear();
     }
     else if(item->text(column)=="机车"||strparent=="机车")
     {
         ntreecheck=3;
         ChangeButton(ntreecheck);
         mCTableViewTrain2->Clear();
         ui->stackedWidget->setCurrentIndex(ntreecheck-1);
         ui->stackedWidget_2->setCurrentIndex(ntreecheck-1);
         if(strparent!="")
         {
             ui->label_4->setText(strparent+"命令"+item->text(column));
             if(strparent!="")
             {
                 ui->label_3->setText(strparent+"命令"+item->text(column));
                 mBtn8->setEnabled(false);
                 int nstate=0;
                 if(ui->label_3->text().contains("发令箱"))//发送未签收
                 {
                     nstate=0;

                 }
                 else if(ui->label_3->text().contains("储令箱"))//发送已签收
                 {
                     nstate=1;
                 }
                 else if(ui->label_3->text().contains("待发箱"))//保存已发送
                 {
                     nstate=2;
                 }
                 nchecktrain=nstate;
                  UpDataTrainTable(mArrayDisOrderLocomot,nchecktrain);
             }
             else
             {
                 ui->label_3->setText("机车命令发令箱");
                 UpDataTrainTable(mArrayDisOrderLocomot,0);
             }
         }
     }
}
void DDOrderManage::SetUserStaName(QString struser,QString strsta)
{
    if(struser=="")
    {
        strUser="DEFAULT";
    }
    else
    {
        strUser=struser;
    }
    strSta=strsta;
}
void DDOrderManage::SetOrderData(QVector<DispatchOrderStation*> m_ArrayDisOrder,QVector<DispatchOrderDispatcher*> m_ArrayDisOrderDisp,QVector<DispatchOrderLocomotive*> m_ArrayDisOrderLocomot)
{
    mArrayDisOrderRecv=m_ArrayDisOrder;
    //需要根据命令签收时间排序
    mArrayDisOrderDisp=m_ArrayDisOrderDisp;
    //for()
    mArrayDisOrderLocomot=m_ArrayDisOrderLocomot;
    DDOrderUpDate(0);
    UpDataDDTTable(mArrayDisOrderDisp,ncheckddt);
    UpDataTrainTable(mArrayDisOrderLocomot,nchecktrain);
}
void DDOrderManage::DDOrderUpDate(int nState)
{
    QStringList strList;
    if(nState==0)
    {
//        for (int i=0;i<mArrayDisOrderRecv.size();i++)
        for (int i=mArrayDisOrderRecv.size()-1;i>=0;i--)
        {
            //strList<<QString::number(i+1); uNumber
            strList<<QString::number(mArrayDisOrderRecv.at(i)->uNumber);
            strList<<mArrayDisOrderRecv.at(i)->strType;
            strList<<mArrayDisOrderRecv.at(i)->timOrder.toString("yyyy-MM-dd hh:mm");
            strList<<mArrayDisOrderRecv.at(i)->strDisCenter;
            strList<<mArrayDisOrderRecv.at(i)->strDisName;
            if(mArrayDisOrderRecv.at(i)->nStateDisOrder==2)
            {
                strList<<"未签收";
            }
            else if(mArrayDisOrderRecv.at(i)->nStateDisOrder==3)
            {
                strList<<"已签收";
            }
            else if(mArrayDisOrderRecv.at(i)->nStateDisOrder==4)
            {
                strList<<"拒签";
            }
            else
            {
                strList<<"*****";
            }

            strList<<mArrayDisOrderRecv.at(i)->strSignName;
            strList<<mArrayDisOrderRecv.at(i)->timSign.toString("yyyy-MM-dd hh:mm");

        }
        mCTableView->SetTableData(strList);
        for (int i=0;i<mArrayDisOrderRecv.size();i++)
        //for (int i=mArrayDisOrderRecv.size()-1;i>=0;i--)
        {
             QColor color= Qt::green;
             //调度命令签收状态 1:保存 2:发送未签收 3:STPC签收 4:拒签 0:信息无效
             switch (mArrayDisOrderRecv.at(i)->nStateDisOrder)
             {
             case 2: color = Qt::blue; break;
             case 3: color = Qt::red; break;
             case 4: color = Qt::black; break;
             }
             int n=mArrayDisOrderRecv.at(i)->nStateDisOrder;
             int nn=mArrayDisOrderRecv.size()-1-i;
             mCTableView->SetRowItemsTextColor(mArrayDisOrderRecv.size()-1-i, color);
        }
    }
    else
    {
       mCTableView->Clear();
    }

}
//更新调度台 表1数据
void DDOrderManage::UpDataDDTTable(QVector<DispatchOrderDispatcher*> mArrayDisOrderDisp,int nState)
{
    QStringList strList;
    for (int i=mArrayDisOrderDisp.size()-1;i>=0;i--)
    {
        bool bRecvState=false;
        for(int j=0;j<mArrayDisOrderDisp[i]->vectDispathInfo.size();j++)
        {
            if(mArrayDisOrderDisp[i]->vectDispathInfo[j].nRecvState==nState)
            {
                bRecvState=true;
                break;
            }
        }
        if((bRecvState&&nState!=2&&mArrayDisOrderDisp[i]->bSend)||(nState==2&&mArrayDisOrderDisp[i]->bSend==false))
        {
            //strList<<QString::number(i+1); uNumber
            //qDebug()<<"time    "<<mArrayDisOrderDisp.at(i)->timOrder<<mArrayDisOrderDisp.at(i)->timOrder;
            strList<<QString::number(mArrayDisOrderDisp.at(i)->uNumber);

            //命令类型
            {
                QStringList list=mArrayDisOrderDisp.at(i)->strType.split(".");
                QString strType=list[1];
                strList<<strType;
            }
            //接收时间/创建时间
            strList<<mArrayDisOrderDisp.at(i)->timCreate.toString("yyyy-MM-dd hh:mm");
            //发送时间
            strList<<mArrayDisOrderDisp.at(i)->timSend.toString("yyyy-MM-dd hh:mm");
            strList<<mArrayDisOrderDisp.at(i)->strDutyName;
        }
    }
    if(strList.size()>0&&(nState==1||nState==2))
    {
        mBtn8->setEnabled(true);
    }
    mCTableViewDDT->SetTableData(strList);
}
//更新调度台 表2数据
void DDOrderManage::UpDataDDTTable2(DispatchOrderDispatcher* mDisOrderDispatcher,int nState)
{
    QStringList strList;
    for (int i=0;i<mDisOrderDispatcher->vectDispathInfo.size();i++)
    {
        //if(mDisOrderDispatcher->vectDispathInfo[i].nRecvState==nState||(nState==2&&mDisOrderDispatcher->bSend==false))
        {
            //序号
            strList<<"1";
            //台号
            strList<<mDisOrderDispatcher->vectDispathInfo[i].strDispatcher;
            //接收状态
//            if(mDisOrderDispatcher->vectDispathInfo[i].nRecvState==1)
//            {
//                strList<<"未签收";
//            }
//            else
//            {
//                 strList<<"已签收";
//            }
            if(nState==0)
            {
                strList<<"已发送";
            }
            else if(nState==1)
            {
                strList<<"已签收";
            }
            else if(nState==2)
            {
                strList<<"未发送";
            }
            //接收时间/创建时间
            strList<<mDisOrderDispatcher->vectDispathInfo[i].timRecv.toString("yyyy-MM-dd hh:mm");
        }
    }
    mCTableViewDDT2->SetTableData(strList);
}
//更新机车表1数据
void DDOrderManage::UpDataTrainTable(QVector<DispatchOrderLocomotive*> mArrayDisOrderLocomot,int nState)
{
    QStringList strList;
//    for (int i=0;i<mArrayDisOrderLocomot.size();i++)
    for (int i=mArrayDisOrderLocomot.size()-1;i>=0;i--)
    {
        bool bRecvState=false;
        for(int j=0;j<mArrayDisOrderLocomot[i]->vectLocmtInfo.size();j++)
        {
            if(mArrayDisOrderLocomot[i]->vectLocmtInfo[j].nRecvState==nState)
            {
                bRecvState=true;
                break;
            }
        }
        if((bRecvState&&nState!=2&&mArrayDisOrderLocomot[i]->bSend)||(nState==2&&mArrayDisOrderLocomot[i]->bSend==false))
        {
            //编号
            strList<<QString::number(mArrayDisOrderLocomot.at(i)->uNumber);
            //命令种类
            QString strorderType="";
            if(mArrayDisOrderLocomot.at(i)->orderType==0x01)
            {
                strorderType="调度命令";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x02)
            {
                strorderType="路票";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x03)
            {
                strorderType="绿色许可证";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x04)
            {
                strorderType="红色许可证";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x05)
            {
                strorderType="调车作业单";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x06)
            {
                strorderType="书面通知";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x07)
            {
                strorderType="半自动闭塞发车进路通知书";
            }
            else if(mArrayDisOrderLocomot.at(i)->orderType==0x08)
            {
                strorderType="自动站间闭塞发车进路通知书";
            }
            strList<<strorderType;
            //命令类型
            if(mArrayDisOrderLocomot.at(i)->orderType==0x01&&mArrayDisOrderLocomot.at(i)->strType!="")
            {
                QStringList list=mArrayDisOrderLocomot.at(i)->strType.split(".");
                if(list.size()==2)
                {
                    QString strType=list[1];
                    strList<<strType;
                }
                else
                {
                    strList<<mArrayDisOrderLocomot.at(i)->strType;
                }
            }
            else
            {
                strList<<mArrayDisOrderLocomot.at(i)->strType;
            }

            //发令单位
            strList<<mArrayDisOrderLocomot.at(i)->strStation;
            //发令人
            strList<<mArrayDisOrderLocomot.at(i)->strDutyName;
            //接收时间/创建时间
            strList<<mArrayDisOrderLocomot.at(i)->timCreate.toString("yyyy-MM-dd hh:mm");
            //发送时间
            strList<<mArrayDisOrderLocomot.at(i)->timSend.toString("yyyy-MM-dd hh:mm");
        }
    }
    if(strList.size()>0&&(nState==1||nState==2))
    {
        mBtn8->setEnabled(true);
    }
    mCTableViewTrain->SetTableData(strList);
}
//更新机车表2数据
void DDOrderManage::UpDataTrainTable2(DispatchOrderLocomotive* mDisOrderLocomotive,int nState)
{
    QStringList strList;
    for (int i=0;i<mDisOrderLocomotive->vectLocmtInfo.size();i++)
    {
         //if(mDisOrderLocomotive->vectLocmtInfo[i].nRecvState==nState||(nState==2&&mDisOrderLocomotive->bSend==false))
         {
             //序号
             strList<<QString::number(i+1);
             //车次 vectLocmtInfo
             strList<<mDisOrderLocomotive->vectLocmtInfo[i].strCheCi.replace(",","");

             //机车号
             strList<<mDisOrderLocomotive->vectLocmtInfo[i].strLocomotive.replace(",","");
             //接收状态
//             if(mDisOrderLocomotive->vectLocmtInfo[i].nRecvState==1)
//             {
//                 strList<<"已签收";
//             }
//             else
//             {
//                  strList<<"未发送";
//             }
             if(nState==0)
             {
                 strList<<"已发送";
             }
             else if(nState==1)
             {
                 strList<<"已签收";
             }
             else if(nState==2)
             {
                 strList<<"未发送";
             }
             //接收时间/创建时间
             strList<<mDisOrderLocomotive->vectLocmtInfo[i].timRecv.toString("yyyy-MM-dd hh:mm");
         }
    }
    mCTableViewTrain2->SetTableData(strList);
}
void DDOrderManage::DDOrderUpDate2(int index)
{
    QStringList strList;
    for (int i=0;i<mArrayDisOrderRecv[index]->listRecvPlace.size();i++)
    {
        strList<<QString::number(i+1)<<mArrayDisOrderRecv[index]->listRecvPlace[i]<<"";
    }
    mCTableView2->SetTableData(strList);
}
void DDOrderManage::itemLBtnClicked(const QModelIndex &index)
{
    QColor color,color2;
    color = Qt::blue;
    color2= Qt::white;
    nCheckRowLeft=index.row();
    if(index.row()<mCTableView->mTableViewInfo.Vect_RowItem.size())
    {
//        qDebug()<<"---"<<nCheckRowLeft<<mCTableView->mTableViewInfo.Vect_RowItem.size();
        SetBacklackRow(mCTableView,nCheckRowLeft);
        ui->textBrowser->setText(mArrayDisOrderRecv[mArrayDisOrderRecv.size()-nCheckRowLeft-1]->strContent);
        DDOrderUpDate2(nCheckRowLeft);
        //if(mArrayDisOrderRecv[nCheckRowLeft]->nStateDisOrder==2)
        if(mArrayDisOrderRecv[mArrayDisOrderRecv.size()-nCheckRowLeft-1]->nStateDisOrder==2)
        {
            bbtnen=true;
        }
        else
        {
            bbtnen=false;
        }
        emit BtnEnSignal(bbtnen);
    }
}

void DDOrderManage::itemLBtnClickedDDT(const QModelIndex &index)
{
    QColor color,color2;
    color = Qt::blue;
    color2= Qt::white;
    nCheckRowLeft=index.row();

    int nstate=0;
    if(ui->label_3->text().contains("发令箱"))//发送未签收
    {
        nstate=0;
    }
    else if(ui->label_3->text().contains("储令箱"))//发送已签收
    {
        nstate=1;
    }
    else if(ui->label_3->text().contains("待发箱"))//保存已发送
    {
        nstate=2;
    }
    if(index.row()<mCTableViewDDT->mTableViewInfo.Vect_RowItem.size())
    {
//        qDebug()<<"---"<<nCheckRowLeft<<mCTableViewDDT->mTableViewInfo.Vect_RowItem.size();
        int nCheck=GetDFXOrderIndexDDT(index.row(),nstate);
        SetBacklackRow(mCTableViewDDT,nCheckRowLeft);
        if(nCheck>=0)
        {
            ui->textBrowser->setText(mArrayDisOrderDisp[nCheck]->strContent);
            UpDataDDTTable2(mArrayDisOrderDisp[nCheck],nstate);
        }

    }

}
void DDOrderManage::itemLBtnClickedTrain(const QModelIndex &index)
{
    QColor color,color2;
    color = Qt::blue;
    color2= Qt::white;
    nCheckRowLeft=index.row();

    int nstate=0;
    if(ui->label_3->text().contains("发令箱"))//发送未签收
    {
        nstate=0;
    }
    else if(ui->label_3->text().contains("储令箱"))//发送已签收
    {
        nstate=1;
    }
    else if(ui->label_3->text().contains("待发箱"))//保存已发送
    {
        nstate=2;
    }
    if(index.row()<mCTableViewTrain->mTableViewInfo.Vect_RowItem.size())
    {
//        qDebug()<<"---"<<nCheckRowLeft<<mCTableViewTrain->mTableViewInfo.Vect_RowItem.size();
        int nCheck=GetDFXOrderIndexTrain(index.row(),nstate);
        if(nCheck>=0)
        {
            SetBacklackRow(mCTableViewTrain,nCheckRowLeft);
            ui->textBrowser->setText(mArrayDisOrderLocomot[nCheck]->strContent);
            UpDataTrainTable2(mArrayDisOrderLocomot[nCheck],nstate);
        }

    }

}
void DDOrderManage::itemRBtnClicked(const QModelIndex &index)
{
//    qDebug()<<"---"<<index<<index.row()<<index.column();
    QAction *acheck=nullptr;
    nCheckRowRight=index.row();
    acheck=menuR->exec(QCursor::pos());
    if(acheck!=nullptr)
    {
        if(acheck->text()=="转发至机车")
        {
            emit SendTextAnalysisSignal(212, QCursor::pos());
            //if(mArrayDisOrderRecv[index.row()]->nStateDisOrder!=3)
            if(mArrayDisOrderRecv[mArrayDisOrderRecv.size()-index.row()-1]->nStateDisOrder!=3)
            {
                QMessageBox::critical(this, tr("错误"), tr("不能转发未签收命令"),tr("确定"),tr("取消"),"",0);
            }
            else
            {
//                mTransmitTrain->Init("转发调度台调度命令至机车",mArrayDisOrderRecv[index.row()]);
                mTransmitTrain->Init("转发调度台调度命令至机车",mArrayDisOrderRecv[mArrayDisOrderRecv.size()-index.row()-1]);
                mTransmitTrain->show();
            }

        }
    }

}
void DDOrderManage::itemRBtnClickedDDT(const QModelIndex &index)
{
//    qDebug()<<"---删除"<<index<<index.row()<<index.column();
    QAction *acheck=nullptr;
    nCheckRowRight=index.row();
    if((ntreecheck==2&&ncheckddt==2)/*||(ntreecheck==3&&nchecktrain==2)*/)
    {
        acheck=menuDel->exec(QCursor::pos());
        if(acheck!=nullptr)
        {
            if(acheck->text()=="删除")
            {
                //                if(mArrayDisOrderDisp[index.row()]->bSend)
                //                {
                //                    QMessageBox::critical(this, tr("错误"), tr("不能删除该命令"));
                //                }
                //                else
                //                {
                //                    mArrayDisOrderDisp[index.row()]->bDel=true;
                //                    emit SendOrderDispatcher(mArrayDisOrderDisp[index.row()]);
                //                }
                emit SendTextAnalysisSignal(169, QCursor::pos());
                int nCheck=GetDFXOrderIndexDDT(nCheckRowRight,2);
                if(nCheck>=0)
                {
                    if(mArrayDisOrderDisp[nCheck]->bSend)
                    {
                        QMessageBox::critical(this, tr("错误"), tr("不能删除该命令"),tr("确定"),tr("取消"),"",0);
                    }
                    else
                    {
                        mArrayDisOrderDisp[nCheck]->bDel=true;
                        emit SendOrderDispatcher(mArrayDisOrderDisp[nCheck]);
                    }
                }
            }
        }
    }
}
void DDOrderManage::itemRBtnClickedTrain(const QModelIndex &index)
{
//    qDebug()<<"---删除"<<index<<index.row()<<index.column();
    QAction *acheck=nullptr;
    nCheckRowRight=index.row();
    if(/*(ntreecheck==2&&ncheckddt==2)||*/(ntreecheck==3&&nchecktrain==2))
    {
        acheck=menuDel2->exec(QCursor::pos());
        if(acheck!=nullptr)
        {
            if(acheck->text()=="删除")
            {
                emit SendTextAnalysisSignal(169, QCursor::pos());
//                if(mArrayDisOrderLocomot[index.row()]->bSend)
//                {
//                    QMessageBox::critical(this, tr("错误"), tr("不能删除该命令"));
//                }
//                else
//                {
//                    mArrayDisOrderLocomot[index.row()]->bDel=true;
//                    emit SendOrderLocomotive(mArrayDisOrderLocomot[index.row()]);
//                }
                int nCheck=GetDFXOrderIndexTrain(nCheckRowRight,2);
                if(nCheck>=0)
                {
                    if(mArrayDisOrderLocomot[nCheck]->bSend)
                    {
                        QMessageBox::critical(this, tr("错误"), tr("不能删除该命令"),tr("确定"),tr("取消"),"",0);
                    }
                    else
                    {
                        mArrayDisOrderLocomot[nCheck]->bDel=true;
                        emit SendOrderLocomotive(mArrayDisOrderLocomot[nCheck]);
                    }
                }
            }
        }
    }
}

void DDOrderManage::SetBacklackRow(TableView* mCTableView,int index)
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
//获取当前调度命令中最新的命令号
int DDOrderManage::GetOderNumNew(int ntype)
{
    int norder=0;
    if(ntype==0)//调度台
    {
        for(int i=mArrayDisOrderDisp.size()-1;i>=0;i--)
        {
             //norder=mArrayDisOrderDisp[i]->uNumber;
            norder=mArrayDisOrderDisp.size();
             return norder;
        }
    }
    else if(ntype==1)//机车
    {
        for(int i=mArrayDisOrderLocomot.size()-1;i>=0;i--)
        {
             //norder=mArrayDisOrderLocomot[i]->uNumber;
            norder=mArrayDisOrderLocomot.size();
             return norder;
        }
    }
    return 0;
}
//获取车站中选中指令序号
int DDOrderManage::GetOrderIndexSta(int n)
{
    int num=0;
    for(int i=mArrayDisOrderRecv.size()-1;i>=0;i--)
    {
        if(num==n)
        {
            return i;
        }
        else
        {
            num++;
        }
    }
    return -1;
}
//获取待发箱中选中指令序号
int DDOrderManage::GetDFXOrderIndexTrain(int n,int nstate)
{
    //
    bool b=false;
    if(nstate==0)
    {
        return -1;
    }
    else if(nstate==1)
    {
       b=true;
    }
    else if(nstate==2)
    {
       b=false;
    }
    int num=0;
//    for(int i=0;i<mArrayDisOrderLocomot.size();i++)
    for(int i=mArrayDisOrderLocomot.size()-1;i>=0;i--)
    {
        if(mArrayDisOrderLocomot[i]->bSend==b)
        {

            if(num==n)
            {
                return i;
            }
            else
            {
              num++;
            }
        }

    }
    return -1;
}
//获取待发箱中选中指令序号
int DDOrderManage::GetDFXOrderIndexDDT(int n,int nstate)
{
    //
    bool b=false;
    if(nstate==0)
    {
        return -1;
    }
    else if(nstate==1)
    {
       b=true;
    }
    else if(nstate==2)
    {
       b=false;
    }
    int num=0;
    //for(int i=0;i<mArrayDisOrderDisp.size();i++)
    for(int i=mArrayDisOrderDisp.size()-1;i>=0;i--)
    {
        if(mArrayDisOrderDisp[i]->bSend==b)
        {

            if(num==n)
            {
                return i;
            }
            else
            {
              num++;
            }
        }

    }
    return -1;
}

void DDOrderManage::NewDDOrderDataSlot(DispatchOrderLocomotive *mDispatchOrderLocomotive, QString strobj)
{
    emit SendOrderLocomotive(mDispatchOrderLocomotive);
}
void DDOrderManage::NewDDOrderDataSlot(DispatchOrderDispatcher *mDispatchOrderDispatcher, QString strobj)
{
    emit SendOrderDispatcher(mDispatchOrderDispatcher);
}

//
void DDOrderManage::TransmitTrainSlot(DispatchOrderStation mDispatchOrderStation)
{

    emit ForWardDDOrderToTrain(mDispatchOrderStation);
}
void DDOrderManage::ChoseTypeSlot(int nId)
{
    QString stype="";
    QString slable="";
    if(nId==0)
    {
        //m_Newddml->setWindowTitle("");
        m_Newddml->Init(strSta,strUser,GetOderNumNew(1));
        m_Newddml->show();
    }
    else
    {
        m_NewLP->ClearShow();
        if(nId==1)
        {
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
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
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
            m_NewLP->show();
        }
        else if(nId==3)
        {
            m_NewLP->SetTitel(QString("新建红色许可证"));
            m_NewLP->SetLable(QString("红色许可证"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
            m_NewLP->show();
        }
        else if(nId==4)
        {
            m_NewLP->SetTitel(QString("新建调车作业单"));
            m_NewLP->SetLable(QString("调车作业单"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
            m_NewLP->show();
        }
        else if(nId==5)
        {
            m_NewLP->SetTitel(QString("新建书面通知"));
            m_NewLP->SetLable(QString("书面通知"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
            m_NewLP->show();
        }
        else if(nId==6)
        {
            m_NewLP->SetTitel(QString("新建半自动闭塞发车进路通知书"));
            m_NewLP->SetLable(QString("半自动闭塞发车进路通知书"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
            m_NewLP->show();
        }
        else if(nId==7)
        {
            m_NewLP->SetTitel(QString("新建自动站间闭塞发车进路通知书"));
            m_NewLP->SetLable(QString("自动站间闭塞发车进路通知书"));
            m_NewLP->SetLableBlackColor(nId);
            m_NewLP->Init(strSta,strUser,GetOderNumNew(1));
            m_NewLP->show();
        }
    }
}

void DDOrderManage::NewLPDataSlot(int ntype,DispatchOrderLocomotive *mDispatchOrderLocomotive)
{
     emit SendOrderLocomotive(mDispatchOrderLocomotive);
}
