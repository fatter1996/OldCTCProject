#include "columndisplaysetwidget.h"
#include "ui_columndisplaysetwidget.h"
#include <QDebug>
#pragma execution_character_set("utf-8")

ColumnDisplaySetWidget::ColumnDisplaySetWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColumnDisplaySetWidget)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);

    ui->setupUi(this);
}

ColumnDisplaySetWidget::~ColumnDisplaySetWidget()
{
    delete ui;
}

void ColumnDisplaySetWidget::InitMyTree(MyTreeNode* colTreeRootNode)
{
    pColTreeRootNode = colTreeRootNode;
    ui->treeWidget->setColumnCount(1);

    //根节点
    QTreeWidgetItem* m_pItem = new QTreeWidgetItem();
    m_pItem->setText(0, "01 行车日志");
    m_pItem->setCheckState(0,Qt::Checked);
    m_pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);

    ui->treeWidget->insertTopLevelItem(0, m_pItem);

    pColTreeRootNode = new MyTreeNode(m_pItem);
    //子节点
    {
        MyTreeNode* node11 = new MyTreeNode(Item(m_pItem, "01 到达"));
        {
            MyTreeNode* node21 = new MyTreeNode(Item(node11->pItem, "01 列车车次"));
            {
                node21->Add(new MyTreeNode(Item(node21->pItem, "01 下行"), 0));
                node21->Add(new MyTreeNode(Item(node21->pItem, "02 上行"), 1));
            }
            node11->Add(node21);
            MyTreeNode* nodeD1 = new MyTreeNode(Item(node11->pItem, "02 接车口"), 2);
            MyTreeNode* nodeD2 = new MyTreeNode(Item(node11->pItem, "03 接车股道"), 3);
            nodeD1->pItem->setDisabled(true);
            nodeD2->pItem->setDisabled(true);
            node11->Add(nodeD1);
            node11->Add(nodeD2);
            MyTreeNode* node22 = new MyTreeNode(Item(node11->pItem, "04 时分"));
            node22->pItem->setDisabled(true);
            {
                node22->Add(new MyTreeNode(Item(node22->pItem, "01 同意邻站发车"), 4));
                node22->Add(new MyTreeNode(Item(node22->pItem, "02 邻站出发"), 5));
                MyTreeNode* node31 = new MyTreeNode(Item(node22->pItem, "03 本站到达"));
                {
                    node31->Add(new MyTreeNode(Item(node31->pItem, "01 规定"), 6));
                    node31->Add(new MyTreeNode(Item(node31->pItem, "02 实际"), 7));
                }
                node22->Add(node31);
            }
            node11->Add(node22);
            MyTreeNode* node23 = new MyTreeNode(Item(node11->pItem, "05 摘车辆数"));
            {
                MyTreeNode* node31 = new MyTreeNode(Item(node23->pItem, "01 运用车"));
                {
                    node31->Add(new MyTreeNode(Item(node31->pItem, "01 重车"), 8));
                    node31->Add(new MyTreeNode(Item(node31->pItem, "02 空车"), 9));
                }
                node23->Add(node31);
                node23->Add(new MyTreeNode(Item(node23->pItem, "02 非运用车"), 10));
                node23->Add(new MyTreeNode(Item(node23->pItem, "03 守车"), 11));
            }
            node11->Add(node23);
            node11->Add(new MyTreeNode(Item(node11->pItem, "06 占用区间凭证号码"), 12));
            node11->Add(new MyTreeNode(Item(node11->pItem, "07 机车型号"), 13));
            MyTreeNode* node24 = new MyTreeNode(Item(node11->pItem, "08 列车编组"));
            {
                node24->Add(new MyTreeNode(Item(node24->pItem, "01 车数"), 14));
                node24->Add(new MyTreeNode(Item(node24->pItem, "02 换长"), 15));
                node24->Add(new MyTreeNode(Item(node24->pItem, "03 总重"), 16));
            }
            node11->Add(node24);
            MyTreeNode* node25 = new MyTreeNode(Item(node11->pItem, "09 电话记录号码"));
            {
                node25->Add(new MyTreeNode(Item(node25->pItem, "01 承认闭塞"), 17));
                node25->Add(new MyTreeNode(Item(node25->pItem, "02 列车到达补机返回"), 18));
                node25->Add(new MyTreeNode(Item(node25->pItem, "03 取消闭塞"), 19));
                node25->Add(new MyTreeNode(Item(node25->pItem, "01 出站跟踪调车"), 20));
                node25->Add(new MyTreeNode(Item(node25->pItem, "02 出站跟踪调车完毕"), 21));
            }
            node11->Add(node25);
        }
        pColTreeRootNode->Add(node11);
        MyTreeNode* node12 = new MyTreeNode(Item(m_pItem, "02 出发"));
        {
            MyTreeNode* node21 = new MyTreeNode(Item(node12->pItem, "01 列车车次"));
            {
                node21->Add(new MyTreeNode(Item(node21->pItem, "01 下行"), 22));
                node21->Add(new MyTreeNode(Item(node21->pItem, "02 上行"), 23));
            }
            node12->Add(node21);
            MyTreeNode* nodeD3 = new MyTreeNode(Item(node12->pItem, "02 发车股道"), 24);
            MyTreeNode* nodeD4 = new MyTreeNode(Item(node12->pItem, "03 发车口"), 25);
            nodeD3->pItem->setDisabled(true);
            nodeD4->pItem->setDisabled(true);
            node12->Add(nodeD3);
            node12->Add(nodeD4);
            MyTreeNode* node22 = new MyTreeNode(Item(node12->pItem, "04 时分"));
            node22->pItem->setDisabled(true);
            {
                node22->Add(new MyTreeNode(Item(node22->pItem, "01 邻站同意发车"), 26));
                MyTreeNode* node31 = new MyTreeNode(Item(node22->pItem, "02 本站出发"));
                {
                    node31->Add(new MyTreeNode(Item(node31->pItem, "01 规定"), 27));
                    node31->Add(new MyTreeNode(Item(node31->pItem, "02 实际"), 28));
                }
                node22->Add(new MyTreeNode(Item(node22->pItem, "03 邻站到达"), 29));
                node22->Add(node31);
            }
            node12->Add(node22);
            MyTreeNode* node23 = new MyTreeNode(Item(node12->pItem, "05 摘车辆数"));
            {
                MyTreeNode* node31 = new MyTreeNode(Item(node23->pItem, "01 运用车"));
                {
                    node31->Add(new MyTreeNode(Item(node31->pItem, "01 重车"), 30));
                    node31->Add(new MyTreeNode(Item(node31->pItem, "02 空车"), 31));
                }
                node23->Add(node31);
                node23->Add(new MyTreeNode(Item(node23->pItem, "02 非运用车"), 32));
                node23->Add(new MyTreeNode(Item(node23->pItem, "03 守车"), 33));
            }
            node12->Add(node23);
            node12->Add(new MyTreeNode(Item(node12->pItem, "06 占用区间凭证号码"), 34));
            node12->Add(new MyTreeNode(Item(node12->pItem, "07 机车型号"), 35));
            MyTreeNode* node24 = new MyTreeNode(Item(node12->pItem, "08 列车编组"));
            {
                node24->Add(new MyTreeNode(Item(node24->pItem, "01 车数"), 36));
                node24->Add(new MyTreeNode(Item(node24->pItem, "02 换长"), 37));
                node24->Add(new MyTreeNode(Item(node24->pItem, "03 总重"), 38));
            }
            node12->Add(node24);
            MyTreeNode* node25 = new MyTreeNode(Item(node12->pItem, "09 电话记录号码"));
            {
                node25->Add(new MyTreeNode(Item(node25->pItem, "01 承认闭塞"), 39));
                node25->Add(new MyTreeNode(Item(node25->pItem, "02 列车到达补机返回"), 40));
                node25->Add(new MyTreeNode(Item(node25->pItem, "03 取消闭塞"), 41));
                node25->Add(new MyTreeNode(Item(node25->pItem, "01 出站跟踪调车"), 42));
                node25->Add(new MyTreeNode(Item(node25->pItem, "02 出站跟踪调车完毕"), 43));
            }
            node12->Add(node25);
        }
        pColTreeRootNode->Add(node12);
        pColTreeRootNode->Add(new MyTreeNode(Item(pColTreeRootNode->pItem, "03 本务机车调车时分"), 44));
        MyTreeNode* nodeD5 = new MyTreeNode(Item(pColTreeRootNode->pItem, "04 列车停战超过规定时间原因"), 45);
        MyTreeNode* nodeD6 = new MyTreeNode(Item(pColTreeRootNode->pItem, "05 计划属性"), 46);
        MyTreeNode* nodeD7 = new MyTreeNode(Item(pColTreeRootNode->pItem, "06 记事"), 47);
        nodeD5->pItem->setDisabled(true);
        nodeD6->pItem->setDisabled(true);
        nodeD7->pItem->setDisabled(true);
        pColTreeRootNode->Add(nodeD5);
        pColTreeRootNode->Add(nodeD6);
        pColTreeRootNode->Add(nodeD7);
        MyTreeNode* node13 = new MyTreeNode(Item(pColTreeRootNode->pItem, "07 任务"));
        {
            node13->Add(new MyTreeNode(Item(node13->pItem, "01 列检"), 48));
            node13->Add(new MyTreeNode(Item(node13->pItem, "02 交令"), 49));
            node13->Add(new MyTreeNode(Item(node13->pItem, "03 交票"), 50));
            node13->Add(new MyTreeNode(Item(node13->pItem, "04 列尾"), 51));
            node13->Add(new MyTreeNode(Item(node13->pItem, "05 机车"), 52));
            node13->Add(new MyTreeNode(Item(node13->pItem, "06 货检"), 53));
            node13->Add(new MyTreeNode(Item(node13->pItem, "07 乘降"), 54));
            node13->Add(new MyTreeNode(Item(node13->pItem, "08 上水"), 55));
            node13->Add(new MyTreeNode(Item(node13->pItem, "09 摘挂"), 56));
            node13->Add(new MyTreeNode(Item(node13->pItem, "10 换乘"), 57));
            node13->Add(new MyTreeNode(Item(node13->pItem, "11 装卸"), 58));
            node13->Add(new MyTreeNode(Item(node13->pItem, "12 吸污"), 59));
            node13->Add(new MyTreeNode(Item(node13->pItem, "13 道口"), 60));
            node13->Add(new MyTreeNode(Item(node13->pItem, "14 车号"), 61));
            node13->Add(new MyTreeNode(Item(node13->pItem, "15 站务"), 62));
            node13->Add(new MyTreeNode(Item(node13->pItem, "16 综控"), 63));
        }
        pColTreeRootNode->Add(node13);
        MyTreeNode* nodeD8 = new MyTreeNode(Item(pColTreeRootNode->pItem, "08 超限属性"), 64);
        nodeD8->pItem->setDisabled(true);
        pColTreeRootNode->Add(nodeD8);
        pColTreeRootNode->Add(new MyTreeNode(Item(pColTreeRootNode->pItem, "09 军运"), 65));
        pColTreeRootNode->Add(new MyTreeNode(Item(pColTreeRootNode->pItem, "10 重点"), 66));
    }
    m_pItem->setExpanded(true);//默认展开全部
}

void ColumnDisplaySetWidget::UpDataState()
{
    pColTreeRootNode->UpDataState();
}

void ColumnDisplaySetWidget::SetShowState(QTableWidget* pTable)
{
    pColTreeRootNode->SetShowState(pTable);
}

QTreeWidgetItem* ColumnDisplaySetWidget::Item(QTreeWidgetItem* parent, QString text)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(parent);
    item->setText(0, text);
    item->setCheckState(0, Qt::Checked);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
    item->setExpanded(true);//默认展开全部
    parent->addChild(item);
    return item;
}

void ColumnDisplaySetWidget::on_pushButton_4_clicked()
{
    emit SendTextAnalysisSignal(182, QCursor::pos());
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item)
    {
        //item->setExpanded(true);
        item->setCheckState(0, Qt::Checked);
    }
}

void ColumnDisplaySetWidget::on_pushButton_5_clicked()
{
    emit SendTextAnalysisSignal(183, QCursor::pos());
    QTreeWidgetItem* item = ui->treeWidget->currentItem();
    if(item)
    {
        //item->setExpanded(false);
        item->setCheckState(0, Qt::Unchecked);
    }
}

void ColumnDisplaySetWidget::on_pushButton_6_clicked()
{
    emit SendTextAnalysisSignal(184, QCursor::pos());
    QTreeWidgetItemIterator It(ui->treeWidget);
    while(*It)
    {
        (*It)->setCheckState(0, Qt::Checked);
        It++;
    }
    //ui->treeWidget->expandAll();
}

void ColumnDisplaySetWidget::on_pushButton_3_clicked()
{
    emit SendTextAnalysisSignal(185, QCursor::pos());
    //ui->treeWidget->collapseAll();
    QTreeWidgetItemIterator It(ui->treeWidget);
    while(*It)
    {
        (*It)->setCheckState(0, Qt::Unchecked);
        if((*It)->isDisabled())
            (*It)->setCheckState(0, Qt::Checked);
        It++;
    }
}

void ColumnDisplaySetWidget::on_pushButton_clicked()
{
    emit SendTextAnalysisSignal(77, QCursor::pos());
    pColTreeRootNode->SetState();
    emit ShowStateChange();
    this->close();
}

void ColumnDisplaySetWidget::on_pushButton_2_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    this->close();
}
