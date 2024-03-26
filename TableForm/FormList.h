#ifndef FORMLIST_H
#define FORMLIST_H

#include <QWidget>
#include <QHeaderView>
#include <QTableView>
#include <QStandardItemModel>
#include <QTableWidget>
#include <QItemDelegate>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QAbstractItemDelegate>

namespace Ui {
class QFormList;
}

class ReadOnlyDelegate: public QStyledItemDelegate
{

public:
    ReadOnlyDelegate(QWidget *parent = NULL):QStyledItemDelegate(parent)
    {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
const QModelIndex &index) const override //final
    {
        Q_UNUSED(parent)
        Q_UNUSED(option)
        Q_UNUSED(index)
        return NULL;
    }
};

class QFormList : public QTableWidget
{
    Q_OBJECT

public:
    explicit QFormList(QWidget *parent = nullptr);
    ~QFormList();

private:
    Ui::QFormList *ui;

private:
    bool isHave_Hor_Header;   //是否有横向表头标识--初始化横向表头时置真
    bool isHave_Ver_Header;   //是否有纵向表头标识--初始化纵向表头时置真
    bool isALL_Item_Center;   //所有单元格是否全部居中--两个表头初始化需全部居中，所有单元格才居中，否则需要用户单独对每个单元格进行设置-默认居中
    QTimer *pTime; //定时器申明--主要用与各表格之间联动实现
    QString m_strTableListName;       //表格名称
    int m_nTabListCode;       //表格编号
private:
    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::getTableWidgetFormType
    //函数说明：初始化操作-在构造函数中调用
    *****************************************************************************************************/
    void init();

public:
    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::getTableWidgetFormType
    //函数说明：根据表格类型标识获取到对应表格指针
    //参数说明：
    //int tabType----------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格
    *****************************************************************************************************/
    QTableWidget* getTableWidgetFormType(int tabType);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.27
    //函数名：QTableForm::init_HeaderTab
    //函数说明：初始化横(纵)向表头相关属性--初始完成后数据体表格列数列宽属性随之确定
    //参数说明：
    //int headerType----------------------表头类型标识符--0x11为横向表头   0x22为纵向表头(每次调用仅可初始化一个表头)  0x44-横纵向表头交叉位置
    //int row-----------------------------横(纵)向表头所需总行数
    //int col-----------------------------横(纵)向表头所需总列数
    //bool isSelectEnable-----------------横(纵)向表头单元格是否允许选中，合并单元格选中时为整体选中
    //QVector<int>v_Hight-----------------横(纵)向表头单元格行高数组容器，每行可不同
    //QVector<int>v_Width-----------------横(纵)向表头单元格列宽数组容器，每列可不同
    //QVector<QVector<int>>v_Span---------横(纵)向表头单元格合并信息数组容器，例如：v_Span[i]={1, 20, 2, 4}表示从（1，20）位置为左上角起始位置，覆盖两行四列合并
    //QVector<QVector<int>>v_ItemPos------横(纵)向表头单元格名称位置数组容器，配合v_itemName使用，确定表头单元格名称位置及名称内容
    //QVector<QString>v_itemName----------横(纵)向表头单元格名称数组容器，配合v_ItemPos使用，确定表头单元格名称位置及名称内容
    //bool isItemCenter-------------------所有单元格是否全部居中--两个表头初始化需全部居中，所有单元格才居中，否则只对当前表头有效，数据体需要用户单独对每个单元格进行设置
    //bool isEditTrigger------------------单元格是否允许编辑修改
    *****************************************************************************************************/
    void init_HeaderTab(int headerType,int row, int col, bool isSelectEnable, QVector<int>v_Hight, QVector<int>v_Width, QVector<QVector<int>>v_Span,QVector<QVector<int>>v_ItemPos,QVector<QString>v_itemName,bool isItemCenter,bool isEditTrigger);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::setTabStyleSheet
    //函数说明：设置横(纵)向表头样式
    //参数说明：
    //int tabType-------------------------表头类型标识符--0x11为横向表头   0x22为纵向表头  0x33为数据体表格(每次调用仅可设置一个表头)   0x44-横纵向表头交叉位置
    //QString str-------------------------样式表内容字符串
    *****************************************************************************************************/
    void  setTabStyleSheet(int tabType,QString str);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.23
    //函数名：QTableForm::setTabIsEditTrigger
    //函数说明：设置表格是否允许编辑修改
    //参数说明：
    //int tabType-------------------------表头类型标识符--0x11为横向表头   0x22为纵向表头  0x33为数据体表格(每次调用仅可设置一个表头)   0x44-横纵向表头交叉位置
    //bool isEditTrigger------------------单元格是否允许编辑修改
    *****************************************************************************************************/
    void setTabIsEditTrigger(int tabType,bool isEditTrigger);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::SpanCell_Info
    //函数说明：设置表格指定单元格范围合并
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格   0x44-横纵向表头交叉位置
    //int rowStart------------------------指定单元格范围起始行
    //int colStart------------------------指定单元格范围起始列
    //int rowCount------------------------指定单元格范围行数
    //int colCount------------------------指定单元格范围列数
    *****************************************************************************************************/
    void SpanCell_Info(int tabType, int rowStart, int colStart, int rowCount, int colCount);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::upData_Info
    //函数说明：设置表格指定单元格内容信息
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格   0x44-横纵向表头交叉位置
    //int rowCount------------------------指定单元格所在行
    //int colCount------------------------指定单元格所在列
    //QString strInfo---------------------需设置字符串
    *****************************************************************************************************/
    void upData_Info(int tabType, int rowCount, int colCount, QString strInfo);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::upData_Info
    //函数说明：批量设置表格所有单元格内容信息--使用前需重新reSetRowColCount，以免本次更新内容范围小于原范围，导致出现大量空行或空列
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格   0x44-横纵向表头交叉位置
    //QVector<QVector<int>>v_ItemPos------表格单元格名称位置数组容器，配合v_itemName使用，确定表头单元格名称位置及名称内容
    //QVector<QString>v_itemName----------表格单元格名称数组容器，配合v_ItemPos使用，确定表头单元格名称位置及名称内容（这两个数组分开是为了兼容有合并单元格的存在）
    *****************************************************************************************************/
    void upData_Info(int tabType,QVector<QVector<int>>v_ItemPos,QVector<QString>v_itemName);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::reSetRowColCount
    //函数说明：重新设置表格行数列数
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    //int rowCount------------------------新的行数
    //int colCount------------------------新的列数
    *****************************************************************************************************/
    void reSetRowColCount(int tabType,int rowCount,int colCount);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::getChangeData_Rows
    //函数说明：按行获取含有指定背景色单元格的数据
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    //QColor color------------------------指定需要检索的单元格背景色
    *****************************************************************************************************/
    QVector<QVector<QString>> getChangeData_Rows(int tabType,QColor color);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::setAllItemsBackGColor
    //函数说明：设置所有表格单元格背景色为指定颜色
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    //QColor color------------------------指定需要设置的单元格背景色
    *****************************************************************************************************/
    void setAllItemsBackGColor(int tabType,QColor color);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2022.8.29
    //函数名：QTableForm::reSetRowColCount
    //函数说明：重新设置表格行高
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    //QVector<int>nHeaderH----------------新的行高数组，超出范围不予处理，如果不够则剩余部分按容器最后一个数据处理
    *****************************************************************************************************/
    void reSetRowHeight(int tabType,QVector<int>nHeaderH);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.1.18
    //函数名：QTableForm::dataHorScrollChange
    //函数说明：数据体表格滚动条改变时向外部发送信号函数
    *****************************************************************************************************/
    void dataHorScrollChange();

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.1.18
    //函数名：QTableForm::dataVerScrollChange
    //函数说明：数据体表格滚动条改变时向外部发送信号函数
    *****************************************************************************************************/
    void dataVerScrollChange();

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.7
    //函数名：QTableForm::setTableListName
    //函数说明：设置表格名称
    //QString str-----------------表格名称
    *****************************************************************************************************/
    void setTableListName(QString str);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.7
    //函数名：QTableForm::getTableListName
    //函数说明：获取表格名称
    *****************************************************************************************************/
    QString getTableListName();

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.7
    //函数名：QTableForm::setTabListCode
    //函数说明：设置表格编号
    //int x-----------------表格编号
    *****************************************************************************************************/
    void setTabListCode(int x);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.7
    //函数名：QTableForm::getTabListCode
    //函数说明：获取表格编号
    *****************************************************************************************************/
    int getTabListCode();

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.23
    //函数名：QTableForm::setCellBackgroundColor
    //函数说明：设置指定单元格背景色为指定颜色
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    //int rowCount------------------------指定单元格所在行
    //int colCount------------------------指定单元格所在列
    //QColor color------------------------指定需要设置的单元格背景色
    *****************************************************************************************************/
    void setCellBackgroundColor(int tabType,int rowCount,int colCount,QColor color);
    void setRowBackgroundColor(int tabType,int rowCount,QColor color);
    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.27
    //函数名：QTableForm::getSpanVector
    //函数说明：获取表格合并单元格信息
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    *****************************************************************************************************/
    QVector<QVector<int>> getSpanVector(int tabType);

    /*****************************************************************************************************
    //作者：白锦天
    //时间：2023.2.27
    //函数名：QTableForm::getSpanVector
    //函数说明：获取表格合并单元格内容信息
    //参数说明：
    //int tabType-------------------------表格类型标识符--0x11为横向表头   0x22为纵向表头  0x33-数据体表格  0x44-横纵向表头交叉位置
    //QVector<QVector<int>>*item_pos------单元格所在位置数组容器
    //QVector<QString>*item_Name----------单元格内容信息数组容器
    //QVector<QColor>*item_color----------单元格背景颜色数组容器（上述三个数组容器，下标同步，同一个下标分别记录了单元格不同属性）
    *****************************************************************************************************/
    void getAllCellInfo(int tabType, QVector<QVector<int>>*item_pos,QVector<QString>*item_Name,QVector<QColor>*item_color);

    void dataTableClear();
    void headerVerReSet(QVector<QVector<int>>v_ItemPos, QVector<QString>v_itemName);
    void headerHorIndexReSet();
    void SetRowTextColor(int row, QColor txtcolor);
    void SetItemTextColor(int row, int col, QColor txtcolor);
    void AddComboBox(int tabType,int row, int col, QStringList strList, QString currText);
    void HideRow(int row);
    void ShowRow(int row);
    void setColReadOnly(int col);
    void InitDataTable();
    bool getDataRownum(int row);
    int getDataIndexByText(int type, int col, QString text);
    void setAdaptiveWidth(int col);
signals:
    void tableWidgetDataChanged(int type, QTableWidgetItem  * item);  //监视有表格单元格数据发生更改时发送
    void signal_dataHorScrollChane(int value);
    void signal_dataVerScrollChane(int value);
    void comboBoxIndexChanged(QString text, int row, int col);
public slots:
    void slot_Time();//定时器槽函数
    void dataChange(QTableWidgetItem  * item);  //数据表格单元格内容发生修改
    void HorHeaderChange(QTableWidgetItem  * item);  //横向表头单元格内容发生修改
    void VerHeaderChange(QTableWidgetItem  * item);  //纵向表头单元格内容发生修改
    void slot_setdataHorScrollChange(int value);         //数据体表格滚动条改变时向外部发送信号函数
    void slot_setdataVerScrollChange(int value);         //数据体表格滚动条改变时向外部发送信号函数
protected:
    bool eventFilter(QObject* o, QEvent* e) override;
};

#endif // FORMLIST_H
