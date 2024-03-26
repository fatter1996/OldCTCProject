#ifndef COLUMNDISPLAYSETWIDGET_H
#define COLUMNDISPLAYSETWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTableWidget>

struct MyTreeNode{
    int nCol = -1;
    QList<MyTreeNode*> subTreeList;
    QTreeWidgetItem* pItem = nullptr;
    bool bShow = true;

    MyTreeNode(QTreeWidgetItem* item, int col = -1)
    {
        nCol = col;
        pItem = item;
    }

    void Add(MyTreeNode* node)
    {
        subTreeList.append(node);
    }

    void UpDataState()
    {
        if(bShow)
        {
            pItem->setCheckState(0, Qt::Checked);
        }
        else
        {
            pItem->setCheckState(0, Qt::Unchecked);
        }
        for(int i = 0; i < subTreeList.size(); i++)
        {
            subTreeList.at(i)->UpDataState();
        }
    }

    void SetState()
    {
        if(pItem->checkState(0) == Qt::Checked)
        {
            bShow = true;
        }
        else
        {
            bShow = false;
        }
        for(int i = 0; i < subTreeList.size(); i++)
        {
            subTreeList.at(i)->SetState();
        }
    }

    void SetShowState(QTableWidget* pTable)
    {
        if(nCol != -1)
        {
            if(bShow)
            {
                pTable->setColumnWidth(nCol, 50);
            }
            else
            {
                pTable->setColumnWidth(nCol, 0);

            }
            pTable->setColumnHidden(nCol, !bShow);
        }

        for(int i = 0; i < subTreeList.size(); i++)
        {
            subTreeList.at(i)->SetShowState(pTable);
        }
    }

    void SetDisabled(bool b)
    {
        pItem->setDisabled(b);
    }
};

namespace Ui {
class ColumnDisplaySetWidget;
}

class ColumnDisplaySetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ColumnDisplaySetWidget(QWidget *parent = nullptr);
    ~ColumnDisplaySetWidget();

    void InitMyTree(MyTreeNode* colTreeRootNode);
    void UpDataState();
    void SetShowState(QTableWidget* pTable);
    QTreeWidgetItem* Item(QTreeWidgetItem* parent, QString text);

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
signals:
    void ShowStateChange();
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private:
    Ui::ColumnDisplaySetWidget *ui;
    MyTreeNode* pColTreeRootNode = nullptr;
};

#endif // COLUMNDISPLAYSETWIDGET_H
