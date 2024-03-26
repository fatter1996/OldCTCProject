#ifndef YT46_H
#define YT46_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QDate>
#include <QDateTime>
#include <QGroupBox>
namespace Ui {
class yt46;
}

class yt46 : public QMainWindow
{
    Q_OBJECT

public:
    explicit yt46(QWidget *parent = nullptr);
    ~yt46();
    QTreeWidgetItem *topItem1;
    QTreeWidgetItem *topItem2;
    int nChoesTable=0;
    QVector<QTreeWidgetItem *>mVTabItem1;
    QVector<QTreeWidgetItem *>mVTabItem2;

    typedef struct
    {
        QString strDevType;//设备类型
        QString strDevName;//设备名称
        QDateTime date1;//时间
        QString strCheckResult;//检查试验结果
        bool btimeTZ;//通知时间复选框
        QDateTime timeTZ;//通知时间
        QString strTZFF;//通知方法
        bool btimeDD;//到达时间复选框
        QDateTime timeDD;//到达时间
        QString strSign;//该段工作人员签名
        bool btimeBL;//消除不良复选框
        QDateTime timeBL;//消除不良时间
        QString strBL;
        QString strSign1;//工作人员签名
        QString strSign2;//值班人员签名
        void Init()
        {
          strDevType="";
          strDevName="";
          date1=QDateTime::currentDateTime();
          //date1=DBNull::Value;
          strCheckResult="";
          btimeTZ=false;
          timeTZ=QDateTime::currentDateTime();
          //timeTZ=QDateTime();
          strTZFF="";
          btimeDD=false;
          timeDD=QDateTime::currentDateTime();
          //timeDD=QDateTime();
          strSign="";
          strTZFF="";
          btimeBL=false;
          timeBL=QDateTime::currentDateTime();
          //timeBL=QDateTime();
          strBL="";
          strSign1="";
          strSign2="";
        }
    }TabInfo1;
    typedef struct
    {
        QString strWorkNum="";//本月施工编号
        QString strWorkPro="";//施工项目
        QDateTime timeQQ=QDateTime::currentDateTime();//请求时间
        //QDateTime timeQQ=QDateTime();//请求时间
        QString strTimeXY="";//所需时间
        QString strUserRange="";//影响使用范围
        QString strUserSG="";//施工单位负责人
        QString strUserDev="";//设备单位负责人
        QString strUserSta="";//车站值班员
        
        QString strOrder="";//命令号
        QDateTime timeFL=QDateTime::currentDateTime();//发令时间
        QDateTime timeMXS=QDateTime::currentDateTime();//慢行时间范围 开始
        QDateTime timeMXE=QDateTime::currentDateTime();//慢行时间范围 结束
        QDateTime timeFSS=QDateTime::currentDateTime();//封锁时间范围 开始
        QDateTime timeFSE=QDateTime::currentDateTime();//封锁时间范围 结束
        QString strUserSG2="";//施工单位负责人
        QString strUserSta2="";//车站值班员
        
        QDateTime timeQQ2=QDateTime::currentDateTime();//请求时间
        QString strHFSY="";//恢复使用的范围和条件
        QString strUserSG4="";//施工单位负责人
        QString strUserDev4="";//设备单位负责人
        QString strUserSta4="";//车站值班员
        
        QString strOrder3="";//命令号
        QDateTime timeFL3=QDateTime::currentDateTime();//发令时间
        QString strUserSG3="";//施工单位负责人
        QString strUserDev3="";//设备单位负责人
        QString strUserSta3="";//车站值班员
        QString strBz="";//备注
    }TabInfo2;
    typedef struct
    {
        QTreeWidgetItem *mItem;
        TabInfo1 *mTabInfo1;
    }TabItemAndViewInfo1;
    typedef struct
    {
        QTreeWidgetItem *mItem;
        TabInfo2 *mTabInfo2;
    }TabItemAndViewInfo2;
    QVector<TabItemAndViewInfo1 *>mVTabItemAndViewInfo1;
    QVector<TabItemAndViewInfo2 *>mVTabItemAndViewInfo2;
    int indexTableItem=0;
    void Init();
    void InitP();
    void InitTree();
    void AddTree1();
    void AddTree2();
    void UpdateTree(QTreeWidget *treeWidget,QVector<QTreeWidgetItem *>mVTabItem);
    void GetTabInfo1(TabInfo1 * mTabInfo1);
    void GetTabInfo2(TabInfo2 * mTabInfo2);
    void SetTabInfo1(TabInfo1 * mTabInfo1);
    void SetTabInfo2(TabInfo2 * mTabInfo2);
    int GetTabItemAndViewInfo1(QTreeWidgetItem *mItem);
    int GetTabItemAndViewInfo2(QTreeWidgetItem *mItem);
    void GroupboxChange(QGroupBox *groupBox);
private slots:
    void SlotNew();
    void SlotSave();
    void SlotDel();
    void onItemSelected1();
    void onItemSelected2();
private:
    Ui::yt46 *ui;
};

#endif // YT46_H
