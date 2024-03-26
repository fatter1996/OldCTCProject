#ifndef DDORDERSIGN_H
#define DDORDERSIGN_H

#include <QWidget>
#include <MyStation/dispatchorderstation.h>
#include "TableView/tableview.h"
#include "DDOrder/ddordernosignwidget.h"
#pragma execution_character_set("utf-8")
namespace Ui {
class DDOrderSign;
}

class DDOrderSign : public QWidget
{
    Q_OBJECT

public:
    explicit DDOrderSign(QWidget *parent = nullptr);
    ~DDOrderSign();
    void Init(QString struser);
    void InitTableView();
    void DDOrderUpDate();
    DispatchOrderStation *mDispatchOrderStation;
    TableView* mCTableView;
    DDOrderNoSignWidget *mDDOrderNoSignWidget;
signals:
    void DispatchOrderStationSignSignal(int norder,bool b);
    //调度命令签收信号
    void DDOrderSignSignal();

    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private:
    Ui::DDOrderSign *ui;
};

#endif // DDORDERSIGN_H
