#ifndef TRAFFICLOGMANAGEKSK_H
#define TRAFFICLOGMANAGEKSK_H

#include <QWidget>
#include "trafficlogtable.h"
#include "StationInterface/trafficlogmanage.h"
#include "columndisplaysetwidget.h"
#include "newtraffclogwidget.h"
#include "searchtrainnowidget.h"
#include "manualpointreportingwidgert.h"
#include "trainnunberchangewidget.h"
#include "adjacentstationchange.h"
#include "traindiagramwidget.h"
#include "meetgivewayplan.h"
#include "TrainMarshal/trainmarshal.h"
#include <TableForm/FormList.h>
#include <QVector>
#include <QLayout>

namespace Ui {
class TrafficLogManageKSK;
}

class TrafficLogManageKSK : public TrafficLogManage
{
    Q_OBJECT

public:
    explicit TrafficLogManageKSK(MyStation* pMyStation, QWidget *parent = nullptr);
    ~TrafficLogManageKSK();

    QFormList *mQFormList;
    typedef  struct
    {
        int ntype;//表头类型
        QVector<int>v_Hightl;//表头单元格高度
        QVector<int>v_Width;//表头单元格宽度
        QVector<QVector<int>>v_Span;//QVector<QVector<int>>v_Span---------横(纵)向表头单元格合并信息数组容器，例如：v_Span[i]={1, 20, 2, 4}表示从（1，20）位置为左上角起始位置，覆盖两行四列合并
        QVector<QVector<int>>v_ItemPos;//QVector<QVector<int>>v_ItemPos------横(纵)向表头单元格名称位置数组容器，配合v_itemName使用，确定表头单元格名称位置及名称内容
        QVector<QString>v_itemName;//QVector<QString>v_itemName----------横(纵)向表头单元格名称数组容器，配合v_ItemPos使用，确定表头单元格名称位置及名称内容
        void Init()
        {
            ntype=0;
            QVector<int>().swap(v_Hightl);
            QVector<int>().swap(v_Width);
            QVector<QVector<int>>().swap(v_Span);
            QVector<QVector<int>>().swap(v_ItemPos);
            QVector<QString>().swap(v_itemName);

        }
    }FormListHead;//表头结构体
    FormListHead mHFormListHead;
    FormListHead mVFormListHead;

    typedef  struct
    {
        int logId = -1;
        int heraldCol = -1;
        int colorTimer = -1;
        QColor color;
    }HeraldItemInfo;
    QVector<HeraldItemInfo> verHeraldItemInfo;
public:
    void Init();
    void InitFromListHead();
    void InitFromListHeadH();
    void InitFromListHeadV();
    void InitTrafficLogMenu();
    void InitTimeMenu();
    void TrafficLogUpData();
    void AddItemData(QVector<QVector<int>>* itemPos, QVector<QString>* itemName, int row, int col, QString str);
    void SetTableHeadVisable(bool b);
    void AddNewTrafficLog();
    void ColumnDisplaySet();
    void SearchTrainNo();
    bool isSelectedRow();
    void AddDispatchDeskMsg(QString msg);
    void ShowMeetGiveWayPlanWidget();
    void setShowType(int type) { showType = type; }
    void ColorClear();
    bool isSetBkColor(int row);
    void TextClear();

    void AdjacentStationReportingPoint(int type, int logId) override;
    void SetCellbkColor(int logId, int col, bool isMark);
    void SetDutyInfo(QVector<UserInfo> vecUser);
    void AddReachTime(int minute);
    bool ItemDataChange(int col, QString itemData, TrafficLog* pTrafficLog);
protected:
    virtual void timerEvent(QTimerEvent *event) override;//定时器

public slots:
    //邻站预告
    void AdjacentStationPreview() override;
    //同意
    void AgreeAdjacentStations(QDateTime = QDateTime()) override;
    //到达
    void TrainArrival(QDateTime = QDateTime()) override;
    //出发
    void TrainDeparture(QDateTime = QDateTime()) override;
    //通过
    void TrainPassThrough(QDateTime = QDateTime()) override;
    //邻站
    void AdjacentStation(QDateTime = QDateTime()) override;
    //取消接车
    void CancelReceiving() override;
    //取消闭塞
    void CancelBlock() override;
    //取消发车
    void CancelDepart() override;
    //邻站出发
    void AdjacentStationDepart(QDateTime = QDateTime()) override;
    //邻站到达
    void AdjacentStationArrival(QDateTime = QDateTime()) override;

signals:
    void sendTrafficLogMsg(int logId, int type, QDateTime date);
    void updataTrafficLog(int type, TrafficLog pTrafficLog);
    void updataTrafficLogFlow(int logId, int type, bool flag);
    void updataNotes(TrafficLog pTrafficLog);
    void startFlicker(TrafficLog* pTrafficLog);
    void stopFlicker(TrafficLog* pTrafficLog, int index);

private:
    Ui::TrafficLogManageKSK *ui;

    ColumnDisplaySetWidget* pColumnDisplaySet = nullptr;
    MyTreeNode* pColTreeRootNode = nullptr;
    QMenu* pTraffclogMenu = nullptr;
    QMenu* pTimeMenu = nullptr;
    QString selectedTrainNo;
    int selectedTrainNoIndex = -1;
    int lastSelectedRow = -1;
    int nCol = 67; //总列数
    int showType = 0;

    QAction* deleteFlagAction = nullptr;
    QAction* allowGDNotMatch = nullptr;
    QAction* allowCRKNotMatch = nullptr;
    QTableWidgetItem* pTimeMenuItem = nullptr;

};

#endif // TRAFFICLOG_H
