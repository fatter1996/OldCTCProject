#ifndef SYSTEMALARM_H
#define SYSTEMALARM_H

#include <QMainWindow>
#include <QCloseEvent>
#include "TableView/tableview.h"
#include "GlobalHeaders/GlobalStructure.h"
#include "systemaalarminfowidget.h"
#include <QDateTime>
namespace Ui {
class SystemAlarm;
}

class SystemAlarm : public QMainWindow
{
    Q_OBJECT

public:
    explicit SystemAlarm(QWidget *parent = nullptr);
    ~SystemAlarm();
//    typedef struct
//    {
//        QString strLevel;
//        QString strType;
//        QString strData;
//        QString strDescribe;
//        QString strSource;
//        void Init()
//        {
//            strLevel="";
//            strType="";
//            strData="";
//            strDescribe="";
//            strSource="";
//        }
//    }TableData;
    QVector<TableData>mVectTableDate;
    typedef struct
    {
        bool Level1;
        bool Level2;
        bool Level3;
        bool Sys;
        bool Train;
        bool Source;
        void Init()
        {
            Level1=true;
            Level2=true;
            Level3=true;
            Sys=true;
            Train=true;
            Source=true;
        }
    }CheckState;//选中条件状态
    CheckState mCheckState;
 TableView *mCTableView;
 SystemaAlarmInfoWidget *mSystemaAlarmInfoWidget;
 void InitTable();
 void InitData(QVector<TableData> VectTableDate);
 void AddData();
 void UpDate();
 void SetBacklackRow(TableView* mCTableView,int index);
  void SetAllBacklackRow(TableView* mCTableView,int index);
private:
    Ui::SystemAlarm *ui;
    void closeEvent (QCloseEvent *e);
signals:
    void WidgetCloseSignal();


public slots:
    void SetServerAlarm(QByteArray recvArray, int recvlen);
    void itemLBtnClicked(const QModelIndex &index);
    void itemRBtnDoubClickedSolt(const QModelIndex &index);
};

#endif // SYSTEMALARM_H
