#include "yt46.h"
#include "ui_yt46.h"
#include <QDebug>
#include <time.h>
#include <qmath.h>
#pragma execution_character_set("utf-8")
yt46::yt46(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::yt46)
{
    ui->setupUi(this);

    Init();
    InitTree();
}

yt46::~yt46()
{
    delete ui;
}
void yt46::Init()
{
    //新建
    QIcon icon1=QApplication::style()->standardIcon(QStyle::StandardPixmap(25));
    QAction *Action1 = new QAction(icon1,"新建",this);
    connect(Action1, &QAction::triggered, this, &yt46::SlotNew);
    ui->toolBar->addAction(Action1);
    //打开
    QAction *Action2 = new QAction(QApplication::style()->standardIcon(QStyle::StandardPixmap(29)), "打开", this);
    //connect(saveAction, &QAction::triggered, this, &MainWindow::save);
    ui->toolBar->addAction(Action2);
    //保存
    QAction *Action3 = new QAction(QApplication::style()->standardIcon(QStyle::StandardPixmap(43)), "保存", this);
    connect(Action3, &QAction::triggered, this, &yt46::SlotSave);
    ui->toolBar->addAction(Action3);
    //删除
    QAction *Action4 = new QAction(QApplication::style()->standardIcon(QStyle::StandardPixmap(44)), "保存", this);
    connect(Action4, &QAction::triggered, this, &yt46::SlotDel);
    ui->toolBar->addAction(Action4);

    // 添加打印按钮
    QAction *printAction = new QAction(QApplication::style()->standardIcon(QStyle::StandardPixmap(20)), "打印", this);
    //connect(printAction, &QAction::triggered, this, &MainWindow::print);
    ui->toolBar->addAction(printAction);

    //
    ui->comboBox->addItem("");
    ui->comboBox->addItem("线路");
    ui->comboBox->addItem("道岔");
    ui->comboBox->addItem("信号");
    ui->comboBox->addItem("集中");
    ui->comboBox->addItem("闭塞");
    ui->comboBox->addItem("通讯");

    ui->comboBox_3->addItem("");
    ui->comboBox_3->addItem("电报");
    ui->comboBox_3->addItem("电话");
    ui->comboBox_3->addItem("书面");
    ui->comboBox_3->addItem("口头");
    ui->tabWidget->setCurrentIndex(0);
    connect( ui->tabWidget, &QTabWidget::currentChanged, [=](int index){
           nChoesTable=index;
           ui->treeWidget->clearSelection();
           ui->treeWidget_2->clearSelection();
       });
    TabInfo1 * mTabInfo1=new TabInfo1();
    mTabInfo1->Init();
    TabInfo2 * mTabInfo2=new TabInfo2();
    SetTabInfo1(mTabInfo1);
    SetTabInfo2(mTabInfo2);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->dateTimeEdit_7->setEnabled(false);
    ui->lineEdit_6->setEnabled(false);
    // 连接toggled信号到槽函数
    connect(ui->groupBox, &QGroupBox::toggled, this,[=](){
        GroupboxChange(ui->groupBox);
    });
    connect(ui->groupBox_2, &QGroupBox::toggled, this,[=](){
        GroupboxChange(ui->groupBox_2);
    });
    connect(ui->groupBox_3, &QGroupBox::toggled, this,[=](){
        GroupboxChange(ui->groupBox_3);
    });
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    this->setFixedSize(this->width(),this->height());
    //ui->dateTimeEdit->clear();
}
void yt46::InitP()
{
    nChoesTable=0;//默认选中第一个table
}
void yt46::InitTree()
{
    ui->treeWidget->setRootIsDecorated(false);
    ui->treeWidget->setHeaderHidden(true);
    topItem1 = new QTreeWidgetItem(ui->treeWidget);
    // 构造中文日期字符串
    QString chineseDate;
    // 定义中文数字对应的字符串
    QString chineseNumbers[] = {"零", "一", "二", "三", "四", "五", "六", "七", "八", "九"};
    QDate date=QDate::currentDate();
    int month = date.month();
    QString monthString = QString::number(month);
    // 月
    for (int i = 0; i < monthString.length(); ++i)
    {
        chineseDate += chineseNumbers[monthString[i].digitValue()];
    }
    chineseDate += "月";
    topItem1->setText(0,chineseDate);

    ui->treeWidget_2->setRootIsDecorated(false);
    ui->treeWidget_2->setHeaderHidden(true);
    topItem2 = new QTreeWidgetItem(ui->treeWidget_2);
    topItem2->setText(0,chineseDate);
    ui->treeWidget->expandAll();
    ui->treeWidget_2->expandAll();
    // 连接itemSelectionChanged信号到槽函数
    connect(ui->treeWidget, &QTreeWidget::itemSelectionChanged, this, &yt46::onItemSelected1);
    connect(ui->treeWidget_2, &QTreeWidget::itemSelectionChanged, this, &yt46::onItemSelected2);
}
void yt46::AddTree1()
{

    ui->dateTimeEdit_7->setDateTime(QDateTime::currentDateTime());

    TabItemAndViewInfo1 *mTabItemAndViewInfo1=new TabItemAndViewInfo1();
    mTabItemAndViewInfo1->mItem= new QTreeWidgetItem(topItem1);
    int index=mVTabItemAndViewInfo1.size()+1;
    QString strdate;
    QDateTime date=QDateTime::currentDateTime();
    strdate=date.toString("dd HH:mm");
    QString stritem=QString::number(index)+"     "+strdate;
    mTabItemAndViewInfo1->mItem->setText(0,stritem);
    ui->treeWidget->addTopLevelItem(topItem1);
    ui->treeWidget->update();
    TabInfo1 *mTabInfo1=new TabInfo1();
    mTabInfo1->Init();
    mTabItemAndViewInfo1->mTabInfo1=mTabInfo1;
    mVTabItemAndViewInfo1.append(mTabItemAndViewInfo1);
    ui->treeWidget->setCurrentItem(mTabItemAndViewInfo1->mItem);
}
void yt46::AddTree2()
{
//    QTreeWidgetItem *item = new QTreeWidgetItem(topItem2);
//    int index=mVTabItem2.size()+1;
//    QString strdate;
//    QDateTime date=QDateTime::currentDateTime();
//    strdate=date.toString("dd HH:mm");
//    QString stritem=QString::number(index)+"     "+strdate;
//    item->setText(0,stritem);
//    ui->treeWidget_2->addTopLevelItem(topItem2);
//    ui->treeWidget_2->update();
//    mVTabItem2.append(item);

    indexTableItem++;
    TabItemAndViewInfo2 *mTabItemAndViewInfo2=new TabItemAndViewInfo2();
    mTabItemAndViewInfo2->mItem= new QTreeWidgetItem(topItem2);
    //int index=mVTabItemAndViewInfo2.size()+1;
    QString strdate;
    QDateTime date=QDateTime::currentDateTime();
    strdate=date.toString("dd HH:mm");
    //QString stritem=QString::number(index)+"     "+strdate;
    QString stritem=QString::number(indexTableItem)+"     "+strdate;

    mTabItemAndViewInfo2->mItem->setText(0,stritem);
    ui->treeWidget_2->addTopLevelItem(topItem2);
    ui->treeWidget_2->update();
    TabInfo2 *mTabInfo2=new TabInfo2();
    //mTabInfo2->Init();
    mTabInfo2->strWorkNum=QString::number(indexTableItem);
    mTabItemAndViewInfo2->mTabInfo2=mTabInfo2;
    mVTabItemAndViewInfo2.append(mTabItemAndViewInfo2);
    ui->lineEdit_6->setText(QString::number(indexTableItem));
    ui->treeWidget_2->setCurrentItem(mTabItemAndViewInfo2->mItem);

}
void yt46::UpdateTree(QTreeWidget *treeWidget,QVector<QTreeWidgetItem *>mVTabItem)
{
    //QTreeWidgetItem *item =nullptr;
    QString strdate;
    QString stritem;
    QStringList list;
    QTreeWidgetItem *item2 = new QTreeWidgetItem(treeWidget);
    item2= treeWidget->topLevelItem(0);
    // 获取所有二级项
    int numcol=0;
    QTreeWidgetItem* invisibleRootItem = treeWidget->invisibleRootItem();
    QTreeWidgetItem* grandChild=nullptr;
    for (int i = 0; i < invisibleRootItem->childCount(); ++i)
    {
        QTreeWidgetItem* child = invisibleRootItem->child(i);
        for (int j = 0; j < child->childCount(); ++j)
        {
            numcol++;
            /*QTreeWidgetItem* */grandChild = child->child(j);
             list=grandChild->text(0).split("     ");
             if(list.size()==2)
             {
                 strdate=list[1];
                 stritem=QString::number(j+1)+"     "+strdate;
                 grandChild->setText(0,stritem);
                 //treeWidget->addTopLevelItem(item);
             }
           // qDebug() << "---"<<grandChild->text(0)<<i<<j;
             treeWidget->setCurrentItem(grandChild);
             //treeWidget->scrollToItem(grandChild);
        }

    }
    //ui->treeWidget->setColumnCount(numcol);
     QTreeWidgetItem *item3=ui->treeWidget->currentItem();
     if(item2==item3)
     {
         TabInfo1 * mTabInfo1=new TabInfo1();
         mTabInfo1->Init();
         SetTabInfo1(mTabInfo1);
     }
     treeWidget->clearSelection();
     if(grandChild!=nullptr)
     {
         treeWidget->setCurrentItem(grandChild);
     }
     treeWidget->update();

}
//获取Tab1界面数据
void yt46::GetTabInfo1(TabInfo1 * mTabInfo1)
{
    mTabInfo1->Init();
    //设备类型
    mTabInfo1->strDevType=ui->comboBox->currentText();
    //设备名称
    mTabInfo1->strDevName=ui->lineEdit->text();
    //时间
    mTabInfo1->date1=ui->dateTimeEdit_7->dateTime();
    //检查试验结果
    mTabInfo1->strCheckResult=ui->textEdit->toPlainText();
    //通知时间复选框
    mTabInfo1->btimeTZ=ui->groupBox->isChecked();
    //通知时间
    mTabInfo1->timeTZ=ui->dateTimeEdit_8->dateTime();
    //通知方法
    mTabInfo1->strTZFF=ui->comboBox_3->currentText();
    //到达时间复选框
    mTabInfo1->btimeDD=ui->groupBox_2->isChecked();
    //到达时间
    mTabInfo1->timeDD=ui->dateTimeEdit_9->dateTime();
    //该段工作人员签名
    mTabInfo1->strSign=ui->lineEdit_2->text();
    //消除不良复选框
    mTabInfo1->btimeBL=ui->groupBox_3->isChecked();
    //消除不良时间
    mTabInfo1->timeBL=ui->dateTimeEdit_10->dateTime();
    mTabInfo1->strBL=ui->textEdit_2->toPlainText();
    //
    mTabInfo1->strSign1=ui->lineEdit_4->text();
    mTabInfo1->strSign2=ui->lineEdit_5->text();
}
void yt46::SetTabInfo1(TabInfo1 * mTabInfo1)
{
    //设备类型
    ui->comboBox->setCurrentText(mTabInfo1->strDevType);
    //设备名称
    ui->lineEdit->setText(mTabInfo1->strDevName);
    //时间
    ui->dateTimeEdit_7->setDateTime(mTabInfo1->date1);
    //检查试验结果
    ui->textEdit->setPlainText(mTabInfo1->strCheckResult) ;
    //通知时间复选框
    ui->groupBox->setChecked(mTabInfo1->btimeTZ);
    //通知时间
    ui->dateTimeEdit_8->setDateTime(mTabInfo1->timeTZ);
    //通知方法
    ui->comboBox_3->setCurrentText(mTabInfo1->strTZFF);
    //到达时间复选框
    ui->groupBox_2->setChecked(mTabInfo1->btimeDD);
    //到达时间
    ui->dateTimeEdit_9->setDateTime(mTabInfo1->timeDD);
    //该段工作人员签名
    ui->lineEdit_2->setText(mTabInfo1->strSign);
    //消除不良复选框
    ui->groupBox_3->setChecked(mTabInfo1->btimeBL);
    //消除不良时间
    ui->dateTimeEdit_10->setDateTime(mTabInfo1->timeBL);
    ui->textEdit_2->setPlainText(mTabInfo1->strBL);
    //
    ui->lineEdit_4->setText(mTabInfo1->strSign1);
    ui->lineEdit_5->setText(mTabInfo1->strSign2);
}
void yt46::GetTabInfo2(TabInfo2 * mTabInfo2)
{
    mTabInfo2->strWorkNum=ui->lineEdit_6->text();//本月施工编号
    mTabInfo2->strWorkPro=ui->lineEdit_7->text();//施工项目
    mTabInfo2->timeQQ=ui->dateTimeEdit->dateTime();//请求时间
    mTabInfo2->strTimeXY=ui->lineEdit_8->text();//所需时间
    mTabInfo2->strUserRange=ui->textEdit_3->toPlainText();//影响使用范围
    mTabInfo2->strUserSG=ui->lineEdit_9->text();//施工单位负责人
    mTabInfo2->strUserDev=ui->lineEdit_10->text();//设备单位负责人
    mTabInfo2->strUserSta=ui->lineEdit_11->text();//车站值班员
    
    mTabInfo2->strOrder=ui->lineEdit_12->text();//命令号
    mTabInfo2->timeFL=ui->dateTimeEdit_2->dateTime();//发令时间
    mTabInfo2->timeMXS=ui->dateTimeEdit_5->dateTime();//慢行时间范围 开始
    mTabInfo2->timeMXE=ui->dateTimeEdit_6->dateTime();//慢行时间范围 结束
    mTabInfo2->timeFSS=ui->dateTimeEdit_12->dateTime();//封锁时间范围 开始
    mTabInfo2->timeFSE=ui->dateTimeEdit_11->dateTime();//封锁时间范围 结束
    mTabInfo2->strUserSG2=ui->lineEdit_13->text();//施工单位负责人
    mTabInfo2->strUserSta2=ui->lineEdit_14->text();//车站值班员
    
    mTabInfo2->timeQQ2=ui->dateTimeEdit_3->dateTime();//请求时间
    mTabInfo2->strHFSY=ui->textEdit_4->toPlainText();//恢复使用的范围和条件
    mTabInfo2->strUserSG4=ui->lineEdit_19->text();//施工单位负责人
    mTabInfo2->strUserDev4=ui->lineEdit_21->text();//设备单位负责人
    mTabInfo2->strUserSta4=ui->lineEdit_20->text();//车站值班员
    
    mTabInfo2->strOrder3=ui->lineEdit_15->text();//命令号
    mTabInfo2->timeFL3=ui->dateTimeEdit_4->dateTime();//发令时间
    mTabInfo2->strUserSG3=ui->lineEdit_17->text();//施工单位负责人
    mTabInfo2->strUserDev3=ui->lineEdit_16->text();//设备单位负责人
    mTabInfo2->strUserSta3=ui->lineEdit_18->text();//车站值班员
    mTabInfo2->strBz=ui->textEdit_5->toPlainText();//备注
}
void yt46::SetTabInfo2(TabInfo2 * mTabInfo2)
{
    ui->lineEdit_6->setText(mTabInfo2->strWorkNum);//本月施工编号
    ui->lineEdit_7->setText(mTabInfo2->strWorkPro);//施工项目
    ui->dateTimeEdit->setDateTime(mTabInfo2->timeQQ);//请求时间
    ui->lineEdit_8->setText(mTabInfo2->strTimeXY);//所需时间
    ui->textEdit_3->setPlainText(mTabInfo2->strUserRange);//影响使用范围
    ui->lineEdit_9->setText(mTabInfo2->strUserSG);//施工单位负责人
    ui->lineEdit_10->setText(mTabInfo2->strUserDev);//设备单位负责人
    ui->lineEdit_11->setText( mTabInfo2->strUserSta);//车站值班员

    ui->lineEdit_12->setText(mTabInfo2->strOrder);//命令号
    ui->dateTimeEdit_2->setDateTime(mTabInfo2->timeFL);//发令时间
    ui->dateTimeEdit_5->setDateTime(mTabInfo2->timeMXS);//慢行时间范围 开始
    ui->dateTimeEdit_6->setDateTime(mTabInfo2->timeMXE);//慢行时间范围 结束
    ui->dateTimeEdit_12->setDateTime(mTabInfo2->timeFSS);//封锁时间范围 开始
    ui->dateTimeEdit_11->setDateTime(mTabInfo2->timeFSE);//封锁时间范围 结束
    ui->lineEdit_13->setText(mTabInfo2->strUserSG2);//施工单位负责人
    ui->lineEdit_14->setText(mTabInfo2->strUserSta2);//车站值班员

    ui->dateTimeEdit_3->setDateTime(mTabInfo2->timeQQ2);//请求时间
    ui->textEdit_4->setPlainText(mTabInfo2->strHFSY);//恢复使用的范围和条件
    ui->lineEdit_19->setText(mTabInfo2->strUserSG4);//施工单位负责人
    ui->lineEdit_21->setText(mTabInfo2->strUserDev4);//设备单位负责人
    ui->lineEdit_20->setText(mTabInfo2->strUserSta4);//车站值班员

    ui->lineEdit_15->setText(mTabInfo2->strOrder3);//命令号
    ui->dateTimeEdit_4->setDateTime(mTabInfo2->timeFL3);//发令时间
    ui->lineEdit_17->setText(mTabInfo2->strUserSG3);//施工单位负责人
    ui->lineEdit_16->setText(mTabInfo2->strUserDev3);//设备单位负责人
    ui->lineEdit_18->setText(mTabInfo2->strUserSta3);//车站值班员
    ui->textEdit_5->setPlainText(mTabInfo2->strBz);//备注
}

int yt46::GetTabItemAndViewInfo1(QTreeWidgetItem *mItem)
{
    for (int i=0;i<mVTabItemAndViewInfo1.size();i++)
    {
        if(mVTabItemAndViewInfo1[i]->mItem==mItem)
        {
            return i;
        }
    }
    return -1;
}
int yt46::GetTabItemAndViewInfo2(QTreeWidgetItem *mItem)
{
    for (int i=0;i<mVTabItemAndViewInfo2.size();i++)
    {
        if(mVTabItemAndViewInfo2[i]->mItem==mItem)
        {
            return i;
        }
    }
    return -1;
}
void yt46::GroupboxChange(QGroupBox *groupBox)
{
    if(groupBox->isChecked())
    {
       if(groupBox->title().contains("通知"))
       {
           ui->dateTimeEdit_8->setDateTime(QDateTime::currentDateTime());
       }
       else if(groupBox->title().contains("到达"))
       {
           ui->dateTimeEdit_9->setDateTime(QDateTime::currentDateTime());
       }
       else if(groupBox->title().contains("消除"))
       {
           ui->dateTimeEdit_10->setDateTime(QDateTime::currentDateTime());
       }
    }
}
//新建
void yt46::SlotNew()
{
    if(nChoesTable==0)
    {
        AddTree1();
    }
    else if(nChoesTable==1)
    {
        AddTree2();
    }
}
//保存
void yt46::SlotSave()
{
    if(nChoesTable==0)
    {
        QTreeWidgetItem *item = ui->treeWidget->currentItem();
        if(item!=nullptr)
        {
            int index=GetTabItemAndViewInfo1(item);
            if(index!=-1)
            {
                GetTabInfo1(mVTabItemAndViewInfo1[index]->mTabInfo1);
                //更新tab Item
                QStringList list=item->text(0).split("     ");
                if(list.size()==2)
                {
                    QString stradd="  "+ui->lineEdit->text()+ui->comboBox->currentText()+"  ";
                    item->setText(0,list[0]+stradd+list[1]);
                }
                else
                {
                    list=item->text(0).split("  ");
                    if(list.size()==3)
                    {
                        QString stradd="  "+ui->lineEdit->text()+ui->comboBox->currentText()+"  ";
                        item->setText(0,list[0]+stradd+list[2]);
                    }
                }

            }
        }
    }
    else if(nChoesTable==1)
    {
        QTreeWidgetItem *item = ui->treeWidget_2->currentItem();
        if(item!=nullptr)
        {
            int index=GetTabItemAndViewInfo2(item);
            if(index!=-1)
            {
                GetTabInfo2(mVTabItemAndViewInfo2[index]->mTabInfo2);
                //更新tab Item
                QStringList list=item->text(0).split("     ");
                if(list.size()==2)
                {
                    QString stradd="  "+ui->lineEdit_7->text()+"  ";
                    item->setText(0,list[0]+stradd+list[1]);
                }
                else
                {
                    list=item->text(0).split("  ");
                    if(list.size()==3)
                    {
                        QString stradd="  "+ui->lineEdit_7->text()+"  ";
                        item->setText(0,list[0]+stradd+list[2]);
                    }
                }
            }
        }
    }
}
//删除
void yt46::SlotDel()
{
    if(nChoesTable==0)
    {
        QTreeWidgetItem *item = ui->treeWidget->currentItem();
        if(item!=nullptr)
        {
            mVTabItem1.removeOne(item);
            ui->treeWidget->takeTopLevelItem(ui->treeWidget->indexOfTopLevelItem(item));
            delete item;
            UpdateTree(ui->treeWidget,mVTabItem1);
        }
    }
    else if(nChoesTable==1)
    {
        QTreeWidgetItem *item = ui->treeWidget_2->currentItem();
        if(item!=nullptr)
        {
//            mVTabItem2.removeOne(item);
//            ui->treeWidget_2->takeTopLevelItem(ui->treeWidget_2->indexOfTopLevelItem(item));
//            delete item;
//            UpdateTree(ui->treeWidget_2,mVTabItem2);
            ui->treeWidget_2->takeTopLevelItem(ui->treeWidget_2->indexOfTopLevelItem(item));
            int index=GetTabItemAndViewInfo2(item);
            if(index!=-1)
            {
                mVTabItemAndViewInfo2.removeOne(mVTabItemAndViewInfo2[index]);
            }
            delete item;
        }
        QTreeWidgetItem *item3 = new QTreeWidgetItem(ui->treeWidget_2);
        item3= ui->treeWidget_2->topLevelItem(0);
        if(ui->treeWidget_2->currentItem()==item3)
        {
            TabInfo2 * mTabInfo2=new TabInfo2();
            SetTabInfo2(mTabInfo2);
        }
        // 获取二级项的最后一个项
        // 获取所有二级项
//        QTreeWidgetItem* invisibleRootItem = ui->treeWidget_2->invisibleRootItem();
//        int secondLevelItemCount = invisibleRootItem->childCount();
//        if (secondLevelItemCount > 0)
//        {
//            QTreeWidgetItem* lastItem = invisibleRootItem->child(secondLevelItemCount - 1);

//            // 设置为当前项并滚动到可见位置
//            ui->treeWidget_2->setCurrentItem(lastItem);
//            //ui->treeWidget_2->scrollToItem(lastItem);
//              //treeWidget->setCurrentItem(grandChild);
//        }
        ui->treeWidget_2->clearSelection();
       ui->treeWidget_2->update();
    }
}
void yt46::onItemSelected1()
{
     QTreeWidgetItem *item = ui->treeWidget->currentItem();
     if(item!=nullptr)
     {
        //qDebug()<<item->text(0);
        int index=GetTabItemAndViewInfo1(item);
        if(index!=-1)
        {
            SetTabInfo1(mVTabItemAndViewInfo1[index]->mTabInfo1);
        }
     }
//     qDebug()<<ui->treeWidget->topLevelItemCount();
     for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
     {
         QTreeWidgetItem* topLevelItem = ui->treeWidget->topLevelItem(i);
         int childCount = topLevelItem->childCount();
         //qDebug()<<"/*/*/*///*/*/"<<ui->treeWidget->topLevelItemCount()<<topLevelItem->text(0)<<childCount;
         if(topLevelItem->text(0)==""&&childCount==0)
         {
             ui->treeWidget->takeTopLevelItem(i);
             delete topLevelItem;
         }
         else
         {
             for (int j = 0; j < childCount; ++j)
             {
                 QTreeWidgetItem* childItem = topLevelItem->child(j);
                 // 处理子项
                 // ...
                 //qDebug()<<"---**---"<<ui->treeWidget->topLevelItemCount()<<i<<j<<childItem->text(0);
             }
         }

     }
}
void yt46::onItemSelected2()
{
     QTreeWidgetItem *item = ui->treeWidget_2->currentItem();
     if(item!=nullptr)
     {
        //qDebug()<<item->text(0);
        int index=GetTabItemAndViewInfo2(item);
        if(index!=-1)
        {
            SetTabInfo2(mVTabItemAndViewInfo2[index]->mTabInfo2);
        }
        ui->treeWidget_2->setCurrentItem(item);
     }
     for (int i = 0; i < ui->treeWidget_2->topLevelItemCount(); ++i)
     {
         QTreeWidgetItem* topLevelItem = ui->treeWidget_2->topLevelItem(i);
         int childCount = topLevelItem->childCount();
         //qDebug()<<"/*/*/*///*/*/"<<ui->treeWidget_2->topLevelItemCount()<<topLevelItem->text(0)<<childCount;
         if(topLevelItem->text(0)==""&&childCount==0)
         {
             ui->treeWidget_2->takeTopLevelItem(i);
             delete topLevelItem;
         }
         else
         {
             for (int j = 0; j < childCount; ++j)
             {
                 QTreeWidgetItem* childItem = topLevelItem->child(j);
                 // 处理子项
                 // ...
                 //qDebug()<<"---**---"<<ui->treeWidget_2->topLevelItemCount()<<i<<j<<childItem->text(0);
             }
         }

     }
}
