#include "FormList.h"
#include "ui_FormList.h"
#include <QMessageBox>
#include <QtDebug>
#include <QLayout>
#include <QTimer>
#include <QComboBox>
#pragma execution_character_set("utf-8")
QFormList::QFormList(QWidget *parent) :
    QTableWidget(parent),
    ui(new Ui::QFormList)
{
    ui->setupUi(this);
    init();
}

QFormList::~QFormList()
{
    delete ui;
}
void QFormList::init()
{
    m_strTableListName="";
    isHave_Hor_Header = false;   //是否有横向表头标识--初始化横向表头时置真
    isHave_Ver_Header = false;   //是否有纵向表头标识--初始化纵向表头时置真
    isALL_Item_Center = true;   //所有单元格是否全部居中--两个表头初始化需全部居中，所有单元格才居中，否则需要用户单独对每个单元格进行设置-默认居中
     this->setContentsMargins(0,0,0,0);

    ui->headerTab->setVisible(false);
    ui->hor_headerTab->setVisible(false);
    ui->ver_headerTab->setVisible(false);
    ui->dataTab->setVisible(false);

    pTime = new QTimer();
    pTime->setInterval(50);
    connect(pTime,SIGNAL(timeout()),this,SLOT(slot_Time()));
    pTime->start();

    connect(ui->hor_headerTab, SIGNAL(itemChanged(QTableWidgetItem  *)), this, SLOT(HorHeaderChange(QTableWidgetItem  *)));
    connect(ui->ver_headerTab, SIGNAL(itemChanged(QTableWidgetItem  *)), this, SLOT(VerHeaderChange(QTableWidgetItem  *)));
    connect(ui->dataTab, SIGNAL(itemChanged(QTableWidgetItem  *)), this, SLOT(dataChange(QTableWidgetItem  *)));

}

QTableWidget* QFormList::getTableWidgetFormType(int tabType)
{
    if(tabType == 0x11)
    {
        return ui->hor_headerTab;
    }
    else if(tabType == 0x22)
    {
        return ui->ver_headerTab;
    }
    else if(tabType == 0x33)
    {
        return ui->dataTab;
    }
    else if(tabType == 0x44)
    {
        return ui->headerTab;
    }
    else
    {
        return nullptr;
    }
}

void QFormList::init_HeaderTab(int headerType,int row, int col, bool isSelectEnable, QVector<int>v_Hight, QVector<int>v_Width, QVector<QVector<int>>v_Span,QVector<QVector<int>>v_ItemPos,QVector<QString>v_itemName,bool isItemCenter,bool isEditTrigger)
{
    //初始化横向表头-初始完成后数据体表格列数列宽属性随之确定
    int HeaderHight=0;  //表头总高度-为所有行高总和
    int HeaderWidth=0;  //表头总宽度-为所有列宽总和
    QTableWidget *table=nullptr;
    if(headerType == 0x33)
    {
        QMessageBox::information(nullptr,"警告","初始化非表头数据表格请调用函数upData_Info(),此处操作判定为非法，本次初始化操作失败，程序已做容错处理，以防内存泄漏！ ");
        return;
    }
    table = getTableWidgetFormType(headerType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格样式表失败！ ");
        return;
    }
    if(table->isVisible() == true)
    {
        QMessageBox::information(nullptr,"警告","检测到已存在完成初始化的表头，请检查相关逻辑是否正确，本次初始化操作失败，程序已做容错处理，以防内存泄漏！ ");
        return;
    }
    else
    {
        table->setVisible(true);
        if(ui->dataTab->isVisible() == false)
        {
            ui->dataTab->setVisible(true);
        }
    }
    if(isItemCenter == false)
    {
        isALL_Item_Center = false;
    }
    table->horizontalHeader()->setVisible(false);//设置表头不可见
    table->verticalHeader()->setVisible(false);

    if(isSelectEnable == false)
    {
        table->setSelectionMode(QAbstractItemView::NoSelection);//设置单元格不可选中
        table->setEnabled(false);
    }
    if(isEditTrigger == false)
    {
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置单元格不可编辑
    }
    table->horizontalHeader()->setStretchLastSection(true);//最后一个单元格扩展
    table->setItemDelegate(new QItemDelegate(0));//设置绘画代理（主要在代理中画出来header）
    table->viewport()->stackUnder(ui->dataTab);//设置窗口层次
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏滚动条
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);//隐藏滚动条
    table->setColumnCount(col);//header列
    table->setRowCount(row);//header行


    if((headerType == 0x11) || (headerType == 0x44))
    {
        for (int i = row; i < table->rowCount(); ++i)//隐藏n行后的行
              table->setRowHidden(i, true);
    }
    else if((headerType == 0x22)|| (headerType == 0x44))
    {
        for (int i = col; i < table->columnCount(); ++i)//隐藏n列后的列
              table->setColumnHidden(i, true);
    }
    for(int i = 0; ((i < row)&&(i<v_Hight.size())); i++)
    {
        table->setRowHeight(i, v_Hight[i]);//设置每行高度
        HeaderHight += v_Hight[i];
    }
    for(int i=0;((i<v_Width.size())&&(i<col));i++)  //设置每列宽度
    {
        table->setColumnWidth(i,v_Width[i]);
        HeaderWidth += v_Width[i];
    }
    if((headerType == 0x11)|| (headerType == 0x44))
    {
        table->setMaximumHeight(HeaderHight + 2); //设置表头总高度
        table->setMinimumHeight(HeaderHight + 2); //设置表头总高度
    }
    if((headerType == 0x22)|| (headerType == 0x44))
    {
        table->setMaximumWidth(HeaderWidth); //设置表头总宽度
        table->setMinimumWidth(HeaderWidth); //设置表头总宽度
    }
    for (int i=0;i<table->rowCount();i++)
    {
       for (int j=0;j<table->columnCount();j++)
       {
           table->setItem(i,j, new QTableWidgetItem(tr("")));
       }
    }
    for(int i=0;i<v_Span.size();i++)  //合并单元格处理
    {
        if((v_Span[i].size() == 4) && (v_Span[i][0] < table->rowCount()) && (v_Span[i][1] < table->columnCount())&& ((v_Span[i][0]+v_Span[i][2]) <= table->rowCount()) && ((v_Span[i][1]+v_Span[i][3]) <= table->columnCount()))
        {
            table->setSpan(v_Span[i][0],v_Span[i][1],v_Span[i][2],v_Span[i][3]);
        }
    }
    for(int i=0;((i<v_ItemPos.size()) && (i<v_itemName.size())) ;i++)
    {
        QString f=v_itemName[i];
        if((v_ItemPos[i].size() == 2) && (v_ItemPos[i][0] < table->rowCount()) && (v_ItemPos[i][1] < table->columnCount()))
        {
            table->item(v_ItemPos[i][0], v_ItemPos[i][1])->setText(v_itemName[i]);
        }
    }
    if(isItemCenter == true)
    {
        for (int i=0;i<table->rowCount();i++)
        {
           for (int j=0;j<table->columnCount();j++)
           {
               table->item(i,j)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
           }
        }
    }
    //数据表跟随横向表头相关初始化
    ui->dataTab->horizontalHeader()->setVisible(false);//设置表头不可见
    ui->dataTab->verticalHeader()->setVisible(false);//设置表头不可见
    ui->dataTab->horizontalHeader()->setStretchLastSection(true);//最后一个单元格扩展
    if(headerType == 0x11)
    {
        ui->dataTab->setColumnCount(col);//header列
        for(int i=0;((i<v_Width.size())&&(i<col));i++)  //设置每列宽度
        {
            ui->dataTab->setColumnWidth(i,v_Width[i]);
        }
    }
    else if(headerType == 0x22)
    {
        ui->dataTab->setRowCount(row);//header行
        for(int i=0;((i<v_Hight.size())&&(i<row));i++)  //设置每列宽度
        {
            ui->dataTab->setRowHeight(i,v_Hight[i]);
        }
    }

    if(headerType == 0x11)  //滚动条联动实现
    {
        connect(table->horizontalScrollBar(), &QAbstractSlider::valueChanged,
                ui->dataTab->horizontalScrollBar(), &QAbstractSlider::setValue);
        connect(ui->dataTab->horizontalScrollBar(), &QAbstractSlider::valueChanged,
                table->horizontalScrollBar(), &QAbstractSlider::setValue);
        connect(ui->dataTab->horizontalScrollBar(), &QAbstractSlider::valueChanged,
                [=]{dataHorScrollChange();});
        isHave_Hor_Header=true;
    }
    else if(headerType == 0x22) //滚动条联动实现
    {
        connect(table->verticalScrollBar(), &QAbstractSlider::valueChanged,
                ui->dataTab->verticalScrollBar(), &QAbstractSlider::setValue);
        connect(ui->dataTab->verticalScrollBar(), &QAbstractSlider::valueChanged,
                table->verticalScrollBar(), &QAbstractSlider::setValue);
        connect(ui->dataTab->verticalScrollBar(), &QAbstractSlider::valueChanged,
                [=]{dataVerScrollChange();});
        isHave_Ver_Header=true;
    }
}

void QFormList::setTabStyleSheet(int tabType,QString str)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格样式表失败！ ");
        return;
    }
    table->setStyleSheet(str);
}

void QFormList::setTabIsEditTrigger(int tabType,bool isEditTrigger)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格样式表失败！ ");
        return;
    }
    if(isEditTrigger == false)
    {
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);//设置单元格不可编辑
    }
    else
    {
        table->setEditTriggers(QAbstractItemView::DoubleClicked);//设置单元格双击修改
    }
}

void QFormList::SpanCell_Info(int tabType, int rowStart, int colStart, int rowCount, int colCount)
{   QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格样式表失败！ ");
        return;
    }
    if((rowStart < 0) || (colStart < 0) || ((rowStart+rowCount-1) >= table->rowCount()) || ((colStart+colCount-1) >= table->columnCount()))
    {
        QMessageBox::information(nullptr,"警告","指定合并区域跨表头与数据区域，或超出范围，程序异常返回，设置单元格合并失败！ ");
        return;
    }
    table->setSpan(rowStart,colStart,rowCount,colCount);
}

void QFormList::upData_Info(int tabType, int rowCount, int colCount, QString strInfo)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置单元格信息失败！ ");
        return;
    }
    if((rowCount >= table->rowCount()) || (colCount >= table->columnCount()))
    {
        QMessageBox::information(nullptr,"警告","指定单元格所在行或列超出表格范围，程序异常返回，设置单元格信息失败！ ");
        return;
    }
    ui->dataTab->blockSignals(true);
    table->item(rowCount,colCount)->setText(strInfo);
    if(isALL_Item_Center == true)
    {
        table->item(rowCount,colCount)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    ui->dataTab->blockSignals(false);
}
void QFormList::dataTableClear()
{
    ui->dataTab->blockSignals(true);
    for (int i=0;i<ui->dataTab->rowCount();i++)
    {
        for (int j=0;j<ui->dataTab->columnCount();j++)
        {
            if(ui->dataTab->item(i, j))
            {
                ui->dataTab->item(i, j)->setText(tr(""));
                //setCellBackgroundColor(0x33, i, j, QColor(255, 255, 255));
            }
            else ui->dataTab->setItem(i, j, new QTableWidgetItem(tr("")));

            //setCellBackgroundColor(0x33, i, 26, QColor(255,255,255));
        }
    }
    ui->dataTab->blockSignals(false);
}
void QFormList::headerVerReSet(QVector<QVector<int>>v_ItemPos, QVector<QString>v_itemName)
{
    for(int i=0;((i<v_ItemPos.size()) && (i<v_itemName.size())) ;i++)
    {
        QString f=v_itemName[i];
        if((v_ItemPos[i].size() == 2) && (v_ItemPos[i][0] < ui->ver_headerTab->rowCount()) && (v_ItemPos[i][1] < ui->ver_headerTab->columnCount()))
        {
            ui->ver_headerTab->item(v_ItemPos[i][0], v_ItemPos[i][1])->setText(v_itemName[i]);
        }
        ui->ver_headerTab->item(i, 0)->setData(Qt::TextColorRole, QColor(80,80,80));
    }
}

void QFormList::headerHorIndexReSet()
{
    int index = 1;
    for(int i = 0; i < ui->hor_headerTab->columnCount(); i++)
    {
        if(!ui->hor_headerTab->isColumnHidden(i))
        {
            ui->hor_headerTab->item(4, i)->setText(QString("%1").arg(index++));
        }
    }
}

void QFormList::upData_Info(int tabType,QVector<QVector<int>>v_ItemPos,QVector<QString>v_itemName)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格数据信息失败！ ");
        return;
    }
    //先清空表格内容，再填入新的表格内容
    dataTableClear();
    ui->dataTab->blockSignals(true);
    for(int i=0;((i<v_ItemPos.size()) && (i<v_itemName.size())) ;i++)
    {
        if(v_ItemPos[i].size() == 2)
        {
            //table->setItem(v_ItemPos[i][0], v_ItemPos[i][1], new QTableWidgetItem(v_itemName[i]));
            ui->dataTab->item(v_ItemPos[i][0], v_ItemPos[i][1])->setText(v_itemName[i]);
        }
    }
    ui->dataTab->blockSignals(false);
}

void QFormList::reSetRowColCount(int tabType,int rowCount,int colCount)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格数据信息失败！ ");
        return;
    }
    table->setRowCount(rowCount);
    table->setColumnCount(colCount);
}

QVector<QVector<QString>> QFormList::getChangeData_Rows(int tabType,QColor color)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    QVector<QVector<QString>>v_strItems;
    v_strItems.clear();
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格数据信息失败！ ");
        return v_strItems;
    }
    for(int i=0;i<table->rowCount();i++)
    {
        for(int j=0;j<table->columnCount();j++)
        {
            if(table->item(i,j)->background().color() == color)
            {
                QVector<QString>v_strItemsText_Row;
                for(int k=0;k<table->columnCount();k++)
                {
                    if(table->item(i,k)!=NULL)
                    {
                        v_strItemsText_Row.append(table->item(i,k)->text());
                    }
                }
                v_strItems.append(v_strItemsText_Row);
                break;
            }
        }
    }
    return v_strItems;
}

void QFormList::setAllItemsBackGColor(int tabType,QColor color)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格数据信息失败！ ");
        return;
    }
    for(int i=0;i<table->rowCount();i++)
    {
        for(int j=0;j<table->columnCount();j++)
        {
            table->item(i,j)->setBackground(color);
        }
    }
}

void QFormList::reSetRowHeight(int tabType,QVector<int>nHeaderH)
{
    QTableWidget *table=nullptr;
    if(tabType == 0x33)
    {
        QMessageBox::information(nullptr,"警告","设置数据区域各行高请通过设置纵向表头各行高实现！ ");
        return;
    }
    else if(tabType == 0x44)
    {
        QMessageBox::information(nullptr,"警告","设置横向表头与纵向表头交叉区域各行高请通过设置横向表头各行高实现！ ");
        return;
    }
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置表格数据信息失败！ ");
        return;
    }

    for(int i=0;((i<table->rowCount()) && (i<nHeaderH.size()));i++)
    {
        table->setRowHeight(i, nHeaderH[i]);//设置每行高度
        if(tabType == 0x22)  //如果设置的是纵向表头，则数据体行高跟随
        {
            if(ui->dataTab->isVisible() == true)
            {
                if(i < ui->dataTab->rowCount())
                {
                    ui->dataTab->setRowHeight(i, nHeaderH[i]);//设置每行高度
                }
            }
        }
        else if(tabType == 0x11)   //如果设置的是横向表头，则纵向表头与横向表头交叉位置表格跟随
        {
            if(ui->headerTab->isVisible() == true)
            {
                if(i < ui->headerTab->rowCount())
                {
                    ui->headerTab->setRowHeight(i, nHeaderH[i]);//设置每行高度
                }
            }
        }
    }
    if(nHeaderH.size() < table->rowCount())  //设置行高数组不足，剩余行按行高数组最后一个数据进行设置
    {
        for(int i=nHeaderH.size();i<table->rowCount();i++)
        {
            table->setRowHeight(i, nHeaderH[nHeaderH.size()-1]);//设置每行高度
            if(tabType == 0x22)  //如果设置的是纵向表头，则数据体行高跟随
            {
                if(ui->dataTab->isVisible() == true)
                {
                    if(i < ui->dataTab->rowCount())
                    {
                        ui->dataTab->setRowHeight(i, nHeaderH[nHeaderH.size()-1]);//设置每行高度
                    }
                }
            }
            else if(tabType == 0x11)   //如果设置的是横向表头，则纵向表头与横向表头交叉位置表格跟随
            {
                if(ui->headerTab->isVisible() == true)
                {
                    if(i < ui->headerTab->rowCount())
                    {
                        ui->headerTab->setRowHeight(i, nHeaderH[nHeaderH.size()-1]);//设置每行高度
                    }
                }
            }
        }
    }
}

void QFormList::setCellBackgroundColor(int tabType,int rowCount,int colCount,QColor color)
{
    ui->dataTab->blockSignals(true);
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置指定单元格背景色失败！ ");
        return;
    }

    if(rowCount<table->rowCount())
    {
        if(colCount<table->columnCount())
        {
            table->item(rowCount,colCount)->setBackground(color);
        }
    }
    ui->dataTab->blockSignals(false);
}

void QFormList::setRowBackgroundColor(int tabType,int rowCount,QColor color)
{
    ui->dataTab->blockSignals(true);
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        QMessageBox::information(nullptr,"警告","未能正确获取表格的有效指针，程序异常返回，设置指定单元格背景色失败！ ");
        return;
    }

    for(int i=0;i<ui->dataTab->columnCount();i++)
    {
        ui->dataTab->item(rowCount, i)->setBackground(color);
    }
    ui->dataTab->blockSignals(false);
}

QVector<QVector<int>> QFormList::getSpanVector(int tabType)
{
    QVector<QVector<int>> v_spanBuff;
    v_spanBuff.clear();
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    if(table == nullptr)
    {
        return v_spanBuff;
    }
    for(int i=0;i<table->rowCount();i++)
    {
        for(int j=0;j<table->columnCount();j++)
        {
            if(table->item(i,j) != nullptr)
            {
                if((table->rowSpan(i,j) > 1) || (table->columnSpan(i,j)>1))
                {
                    bool flag=false;
                    for(auto ement:v_spanBuff)
                    {
                        if(ement.size() != 4)
                        {
                            continue;
                        }
                        if((i>=ement[0]) && (i<=ement[0]+ement[2]-1) && (j>=ement[1]) && (j<=ement[1]+ement[3]-1))
                        {
                            flag=true;
                            break;
                        }
                    }
                    if(flag==false)
                    {
                        v_spanBuff.push_back({i,j,table->rowSpan(i,j),table->columnSpan(i,j)});
                    }
                }
            }
        }
    }
    return v_spanBuff;
}

void QFormList::getAllCellInfo(int tabType,QVector<QVector<int>>*item_pos,QVector<QString>*item_Name,QVector<QColor>*item_color)
{
    QTableWidget *table=nullptr;
    table=getTableWidgetFormType(tabType);
    QVector<QVector<int>> v_spanBuff;
    v_spanBuff.clear();
    item_pos->clear();
    item_Name->clear();
    item_color->clear();
    if(table == nullptr)
    {
        return;
    }
    for(int i=0;i<table->rowCount();i++)
    {
        for(int j=0;j<table->columnCount();j++)
        {
            if(table->item(i,j) != nullptr)
            {
                bool flag=false;
                for(auto ement:v_spanBuff)
                {
                    if(ement.size() != 4)
                    {
                        continue;
                    }
                    if((i>=ement[0]) && (i<=ement[0]+ement[2]-1) && (j>=ement[1]) && (j<=ement[1]+ement[3]-1))
                    {
                        flag=true;
                        break;
                    }
                }
                if(flag==false)
                {
                    v_spanBuff.push_back({i,j,table->rowSpan(i,j),table->columnSpan(i,j)});
                    item_pos->push_back({i,j});
                    item_Name->push_back(table->item(i,j)->text());
                    item_color->push_back(table->item(i,j)->background().color());
                }
            }
        }
    }
}

void QFormList::SetRowTextColor(int row, QColor txtcolor)
{
    ui->dataTab->blockSignals(true);
    for(int i=0;i<ui->dataTab->columnCount();i++)
    {
        ui->dataTab->item(row, i)->setData(Qt::TextColorRole, txtcolor);
    }
    ui->dataTab->blockSignals(false);
}
void QFormList::SetItemTextColor(int row, int col, QColor txtcolor)
{
    ui->dataTab->blockSignals(true);
    ui->dataTab->item(row, col)->setData(Qt::TextColorRole, txtcolor);
    ui->dataTab->blockSignals(false);
}

void QFormList::AddComboBox(int tabType, int row, int col, QStringList strList, QString currText)
{
    QComboBox *pCombox = new QComboBox(ui->dataTab); // 下拉选择框控件
    pCombox->setObjectName(QString("%1,%2").arg(row).arg(col));
    pCombox->installEventFilter(this);
    pCombox->addItems(strList);
    ui->dataTab->setCellWidget(row, col, (QWidget*)pCombox);
    pCombox->setCurrentText(currText);
    connect(pCombox,static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](int index){
            QString str = pCombox->itemText(index);
            ui->dataTab->removeCellWidget(row, col);
            ui->dataTab->item(row, col)->setText(str);
            emit comboBoxIndexChanged(str, row, col);
        });
}

void QFormList::HideRow(int row)
{
    ui->ver_headerTab->hideRow(row);
    ui->dataTab->hideRow(row);

}

void QFormList::ShowRow(int row)
{
    ui->ver_headerTab->showRow(row);
    ui->dataTab->showRow(row);
}

void QFormList::setColReadOnly(int col)
{
    ui->dataTab->setItemDelegateForColumn(col, new ReadOnlyDelegate());
}

void QFormList::InitDataTable()
{

}

int QFormList::getDataIndexByText(int type, int col, QString text)
{
    QTableWidget *table=nullptr;
    table = getTableWidgetFormType(type);
    for(int i = 0; i < ui->ver_headerTab->rowCount(); i++)
    {
        if(table->item(i, col)->text() == text)
            return i;
    }
    return -1;
}


void QFormList::setTableListName(QString str)
{
    m_strTableListName=str;
}
QString QFormList::getTableListName()
{
    return m_strTableListName;
}
void QFormList::setTabListCode(int x)
{
    m_nTabListCode=x;
}
int QFormList::getTabListCode()
{
    return m_nTabListCode;
}
//槽函数
void QFormList::dataChange(QTableWidgetItem  *item)
{
    emit tableWidgetDataChanged(0x33,item);
}
void QFormList::HorHeaderChange(QTableWidgetItem  * item)
{
    emit tableWidgetDataChanged(0x11,item);
}
void QFormList::VerHeaderChange(QTableWidgetItem  * item)
{
    emit tableWidgetDataChanged(0x22,item);
}
void QFormList::slot_Time()
{
    int rowHeight_Sum=0;
    int colWidth_Sum=0;
    ui->dataTab->resize(ui->gridLayout->itemAtPosition(1,1)->widget()->width(),ui->gridLayout->itemAtPosition(1,1)->widget()->height());
    rowHeight_Sum=0;
    colWidth_Sum=0;
    for(int i=0;i<ui->dataTab->rowCount();i++)
    {
        rowHeight_Sum += ui->dataTab->rowHeight(i);
    }
    for(int i=0;i<ui->dataTab->columnCount();i++)
    {
        colWidth_Sum += ui->dataTab->columnWidth(i);
    }
    if(isHave_Hor_Header == true)
    {
        if(ui->dataTab->height() >= rowHeight_Sum+19)
        {
            ui->hor_headerTab->resize(ui->gridLayout->itemAtPosition(1,1)->widget()->width(),ui->gridLayout->itemAtPosition(1,1)->widget()->height());
        }
        else
        {
            ui->hor_headerTab->resize(ui->gridLayout->itemAtPosition(1,1)->widget()->width()-17,ui->gridLayout->itemAtPosition(1,1)->widget()->height());
        }
    }
    if(isHave_Ver_Header == true)
    {
        if(ui->dataTab->width() >= colWidth_Sum + 19)
        {
            ui->ver_headerTab->resize(ui->gridLayout->itemAtPosition(1,1)->widget()->width(),ui->gridLayout->itemAtPosition(1,1)->widget()->height());
        }
        else
        {
            ui->ver_headerTab->resize(ui->gridLayout->itemAtPosition(1,1)->widget()->width(),ui->gridLayout->itemAtPosition(1,1)->widget()->height() - 22);
        }
    }

    //行高跟随--数据体跟随纵向表头--纵向表头与横向表头交叉区域跟随横向表头
    for(int i=0;((i<ui->dataTab->rowCount()) && (i<ui->ver_headerTab->rowCount()));i++)
    {
        if(ui->dataTab->rowHeight(i) != ui->ver_headerTab->rowHeight(i))
        {
            ui->dataTab->setRowHeight(i, ui->ver_headerTab->rowHeight(i));//设置每行高度
        }
    }
    for(int i=0;((i<ui->headerTab->rowCount()) && (i<ui->hor_headerTab->rowCount()));i++)
    {
        if(ui->headerTab->rowHeight(i) != ui->hor_headerTab->rowHeight(i))
        {
            ui->headerTab->setRowHeight(i, ui->hor_headerTab->rowHeight(i));//设置每行高度
        }
    }
}
void QFormList::dataHorScrollChange()
{
    emit signal_dataHorScrollChane(ui->dataTab->horizontalScrollBar()->value());
}
void QFormList::dataVerScrollChange()
{
    emit signal_dataHorScrollChane(ui->dataTab->verticalScrollBar()->value());
}
void QFormList::slot_setdataHorScrollChange(int value)
{
    ui->dataTab->horizontalScrollBar()->setValue(value);
}
void QFormList::slot_setdataVerScrollChange(int value)
{
    ui->dataTab->verticalScrollBar()->setValue(value);
}

bool QFormList::eventFilter(QObject* o, QEvent* e)
{
    if(e->type() == QEvent::FocusOut)
    {
        QComboBox* combo = (QComboBox*)o;
        if(!combo->view()->isVisible())
        {
            QString text = combo->currentText();
            QStringList pt = o->objectName().split(',');
            int row = pt[0].toInt();
            int col = pt[1].toInt();
            ui->dataTab->removeCellWidget(row, col);
            ui->dataTab->item(row, col)->setText(text);
        }
    }
    return QWidget::eventFilter(o, e);
}

void QFormList::setAdaptiveWidth(int col)
{
    //ui->hor_headerTab->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    //ui->dataTab->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->hor_headerTab->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    ui->dataTab->horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
    //ui->hor_headerTab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //ui->dataTab->setColumnWidth(col, ui->hor_headerTab->columnWidth(col));
}
