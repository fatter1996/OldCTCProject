#ifndef DDORDERMANAGE_H
#define DDORDERMANAGE_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include "timemansge.h"
#include<QHBoxLayout>
#include "TableView/tableview.h"
#include "MyStation/dispatchorderstation.h"
#include "MyStation/dispatchorderlocomotive.h"
#include "MyStation/dispatchorderdispatcher.h"
#include <QAction>
#include <QMenu>
#include "chosetype.h"
#include "newddml.h"
#include "transmittrain.h"
#include "DDOrder/ddordernosignwidget.h"
namespace Ui {
class DDOrderManage;
}

class DDOrderManage : public QWidget
{
    Q_OBJECT
public:
    QTreeWidgetItem *topItem_Station;
    QTreeWidgetItem *topItem_Ddt;
    QTreeWidgetItem *topItem_Train;
    QString strUser;
    QString strSta;
    TimeMansge *m_TimeMansge;
    QPushButton *mBtn8;
    QHBoxLayout *layout1;
    QHBoxLayout *layout2;
    TableView* mCTableView;
    TableView* mCTableView2;
    TableView* mCTableView3;
    TableView* mCTableViewDDT;//调度台
    TableView* mCTableViewTrain;//机车
    TableView* mCTableViewDDT2;//调度台
    TableView* mCTableViewTrain2;//机车
    //车站调度命令数组
    QVector<DispatchOrderStation*> mArrayDisOrderRecv;
    //调度台调度命令数组
    QVector<DispatchOrderDispatcher*> mArrayDisOrderDisp;
    //机车调度命令数组
    QVector<DispatchOrderLocomotive*> mArrayDisOrderLocomot;

    //调度台保存指令数组 临时
    //QVector<DispatchOrderStation*> mArrayDisOrderDDTBC;
    QVector<DispatchOrderDispatcher*> mArrayDisOrderDispBC;
    //调度台接收服务端发送指令数组 签收/未签收
    //QVector<DispatchOrderStation*> mArrayDisOrderRecvDDT;
    //机车保存指令数组  临时
    //QVector<DispatchOrderStation*> mArrayDisOrderTrainBC;
    QVector<DispatchOrderLocomotive*> mArrayDisOrderLocomotBC;
    //机车接收服务端发送指令数组 签收/未签收
    //QVector<DispatchOrderStation*> mArrayDisOrderRecvTrain;
    //调度命令模板
    QVector<DisOrderTemplate> mVectDisOrderTemplate;
    bool bbtnen=true;
    QMenu* menuR;
    QMenu* menuDel;//删除菜单
    QMenu* menuDel2;//删除菜单
    int nCheckRowRight=-1;
    int nCheckRowLeft=-1;
    int ntreecheck=1;
    Newddml *mNewddml;//调度台新建调度命令
    ChoseType *mChoseType;
    TransmitTrain *mTransmitTrain;

    NewLP *m_NewLP;//路票
    Newddml *m_Newddml;//机车新建调度命令
    int ncheckddt=0;
    int nchecktrain=0;
    DDOrderNoSignWidget *mDDOrderNoSignWidget;
public:
    explicit DDOrderManage(QWidget *parent = nullptr);
    ~DDOrderManage();
    void Init();
    void InitDisTemplate();
    void InitParameter();
    void InitTree();
    void ChangeButton(int ntype);
    void InitTableView();
    void InitTableView2();
    void InitTableView3();
    void InitTableViewDDT();//调度台
    void InitTableViewTrain();//机车
    void InitTableViewDDT2();//调度台
    void InitTableViewTrain2();//机车
    //设置车站名称及登录名称
    void SetUserStaName(QString struser,QString strsta);
    void SetOrderData(QVector<DispatchOrderStation*> m_ArrayDisOrder,QVector<DispatchOrderDispatcher*> m_ArrayDisOrderDisp,QVector<DispatchOrderLocomotive*> m_ArrayDisOrderLocomot);
    //更新车站表数据
    void DDOrderUpDate(int nState);
    //更新调度台 表1数据
    void UpDataDDTTable(QVector<DispatchOrderDispatcher*> mArrayDisOrderDisp,int nState);
    //更新调度台 表2数据
    void UpDataDDTTable2(DispatchOrderDispatcher* mDisOrderDispatcher,int nState=0);
    //更新机车表1数据
    void UpDataTrainTable(QVector<DispatchOrderLocomotive*> mArrayDisOrderLocomot,int nState=0);
    //更新机车表2数据
    void UpDataTrainTable2(DispatchOrderLocomotive* mDisOrderLocomotive,int nState=0);
    void DDOrderUpDate2(int index);
    void SetBacklackRow(TableView* mCTableView,int index);
    //获取当前调度命令中最新的命令号
    int GetOderNumNew(int ntype);
    //获取车站中选中指令序号
    int GetOrderIndexSta(int n);
    //获取待发箱中选中指令序号
    int GetDFXOrderIndexTrain(int n,int nstate);
    int GetDFXOrderIndexDDT(int n,int nstate);

private:
    Ui::DDOrderManage *ui;
signals:
    void BtnEnSignal(bool b);
    void ForWardDDOrderToTrain(DispatchOrderStation mDispatchOrderStation);
    void DispatchOrderStationSignSignal(int norder,bool b);
    void SendOrderLocomotive(DispatchOrderLocomotive *mDispatchOrderLocomotive);
    void SendOrderDispatcher(DispatchOrderDispatcher *mDispatchOrderDispatcher);
    //调度命令签收信号
    void DDOrderSignSignal();
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private slots:
  void SelectedItemSolt(QTreeWidgetItem * item, int column); //点击树节点事件
  void TreeClickedSolt(QTreeWidgetItem *item, int column);
  void itemLBtnClicked(const QModelIndex &index);
  void itemLBtnClickedDDT(const QModelIndex &index);
  void itemLBtnClickedTrain(const QModelIndex &index);
  void itemRBtnClicked(const QModelIndex &index);
  void itemRBtnClickedDDT(const QModelIndex &index);
  void itemRBtnClickedTrain(const QModelIndex &index);
  void NewDDOrderDataSlot(DispatchOrderDispatcher* mDispatchOrderStation,QString strobj);
  void NewDDOrderDataSlot(DispatchOrderLocomotive* mDispatchOrderStation,QString strobj);
  void TransmitTrainSlot(DispatchOrderStation mDispatchOrderStation);
  void ChoseTypeSlot(int nid);
  void NewLPDataSlot(int ntype,DispatchOrderLocomotive *mDispatchOrderLocomotive);

};

#endif // DDORDERMANAGE_H
