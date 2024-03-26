#include "seektrain.h"
#include "ui_seektrain.h"
#include <QtAlgorithms>
#include <QDebug>
#pragma execution_character_set("utf-8")
SeekTrain::SeekTrain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SeekTrain)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setFixedSize(this->width(),this->height());
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_3->layout()->addWidget(mCTableView);
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        QString aa=ui->pushButton->text();
        if(aa.contains("车次号"))
        {
            ui->pushButton->setText("按车站排列");
            TrainNumUpDate(false);
        }
        else if(aa.contains("车站"))
        {
             ui->pushButton->setText("按车次号排列");
             TrainNumUpDate(true);
        }
    });
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        close();
    });
    connect(ui->pushButton_3,&QPushButton::clicked,[=](){
        close();
    });
    //鼠标左键点击响应
    connect(mCTableView,SIGNAL(itemLBtnClicked(QModelIndex)),this,SLOT(itemLBtnClicked(QModelIndex)));
    //编辑框输入模糊查询功能
    connect(ui->lineEdit,SIGNAL(textChanged(const QString &)),this,SLOT(TextChangeSolt(QString )));
}

SeekTrain::~SeekTrain()
{
    delete ui;
}
void SeekTrain::InitTableView()
{
//    QHBoxLayout *layout=new QHBoxLayout();
//    layout->setDirection(QBoxLayout::LeftToRight);
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();

    mCTableView->mTableViewInfo.Vect_HeaderData.append("车次号");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("站名");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(0, 2);
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
void SeekTrain::SetData(QVector<TrainNumWnd*> mArrayTrainNumWnd,QString StationName)
{
    m_ArrayTrainNumWnd=mArrayTrainNumWnd;
    strStationName=StationName;
    TrainNumUpDate(true);
}
void SeekTrain::TrainNumUpDate(bool b)
{
    strList.clear();
    QStringList strList1;
    //按车站排序
    {
        for (int i=m_ArrayTrainNumWnd.size()-1;i>=0;i--)
        {
            if(m_ArrayTrainNumWnd[i]->m_strCheCi!="")
            {
                strList<<m_ArrayTrainNumWnd[i]->m_strCheCi<<strStationName;
            }
        }
    }
    if(!b)//按车次排序
    {
        // 获取字符串中偶数序列
        for (int i = 0; i < strList.size(); i += 2)
        {
            strList1 += strList.at(i);
        }
        std::sort(strList1.begin(), strList1.end(), [](const QString& s1, const QString& s2)
        {
            if (s1.at(0).isDigit() && s2.at(0).isDigit())
            {
                return s1.toLongLong() < s2.toLongLong();
            }
            else if (s1.at(0).isLetter() && s2.at(0).isLetter())
            {
                return s1 < s2;
            }
            else
            {
                return s1.at(0).isDigit();
            }
        });
        // 将排序后的偶数序列插回原字符串中
        for (int i = 0, j = 0; i < strList.size(); i++)
        {
            if (i % 2 == 0)
            {
                strList[i] = strList1[j++];
            }
        }
//        foreach (auto str, strList)
//        {
//            qDebug() << "/////"<<str;
//        }
    }
    if(strList.size()>0)
    {
        ShowDate(strList);
        SetBacklackRow(mCTableView,0);
        ui->lineEdit->setText(strList[0]);
    }
}
void SeekTrain::ShowDate( QStringList strList)
{
     mCTableView->SetTableData(strList);
}
void SeekTrain::itemLBtnClicked(const QModelIndex &index)
{
    QColor color,color2;
    color = Qt::blue;
    color2= Qt::white;
    //mCTableView->SetRowBKColor(index.row(), color);
    if(index.row()<mCTableView->mTableViewInfo.Vect_RowItem.size())
    {
        SetBacklackRow(mCTableView,index.row());
        ui->lineEdit->setText(strList[index.row()*2]);
    }

}
void SeekTrain::TextChangeSolt(QString str)
{
        //qDebug() << "/////"<<str;
    // 获取字符串中偶数序列
    QStringList strList1;
    for (int i = 0; i < strList.size(); i += 2)
    {
        strList1 += strList.at(i);
    }

     QRegExp rx(str, Qt::CaseInsensitive);  // 创建正则表达式对象，并设置不区分大小写
     QStringList result;
     foreach(str, strList1)
     {
         if(rx.indexIn(str) != -1)  // 判断 str 是否符合模式
             result.append(str);
     }
     for (int i = 0; i < strList1.size(); i++)
     {
          for (int j = result.size()-1; j>=0; j--)
          {
              if(strList1[i]==result[j])
              {
                 SetBacklackRow(mCTableView,i);
                 // qDebug() << "****"<<result[j]<<i;  // 输出符合模式的字符串列表
                  return;
              }
          }
     }
}
void SeekTrain::SetBacklackRow(TableView* mCTableView,int index)
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
