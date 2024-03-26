#include "stagechronicle.h"
#include "ui_stagechronicle.h"
#pragma execution_character_set("utf-8")
StageChronicle::StageChronicle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StageChronicle)
{
    ui->setupUi(this);
   setWindowModality(Qt::ApplicationModal);
   InitTree();

   connect(ui->pushButton,&QPushButton::clicked,[=](){
       emit SendTextAnalysisSignal(122, QCursor::pos());
       close();
   });
   connect(ui->pushButton_2,&QPushButton::clicked,[=](){
       emit SendTextAnalysisSignal(123, QCursor::pos());
       close();
   });
}

StageChronicle::~StageChronicle()
{
    delete ui;
}
void StageChronicle::InitTree()
{
    ui->treeWidget->setRootIsDecorated(false);
    ui->treeWidget->setHeaderHidden(true);
    topItem_Station = new QTreeWidgetItem(ui->treeWidget);
    topItem_Station->setText(0,"调度台");

    ui->treeWidget->addTopLevelItem(topItem_Station);



    QTreeWidgetItem *item11 = new QTreeWidgetItem(topItem_Station);
    item11->setText(0,"第一阶段");
    QTreeWidgetItem *item12 = new QTreeWidgetItem(topItem_Station);
    item12->setText(0,"第二阶段");
    QTreeWidgetItem *item13 = new QTreeWidgetItem(topItem_Station);
    item13->setText(0,"第三阶段");
//    QIcon icon13;
//    icon13.addFile("://Res/toolbar2/会让计划.png");
//    item13->setIcon(0,icon13);
    ui->treeWidget->addTopLevelItem(topItem_Station);
    ui->treeWidget->setIndentation(20);
}
