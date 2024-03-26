#ifndef STATECHOSE_H
#define STATECHOSE_H

#include <QWidget>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include <QRadioButton>
#include <QCheckBox>
#include "GlobalHeaders/GlobalStructure.h"

namespace Ui {
class StateChose;
}

class StateChose : public QWidget
{
    Q_OBJECT
public:
    QVBoxLayout *layoutv;
public:
    explicit StateChose(QWidget *parent = nullptr);
    ~StateChose();
    void Init();
    void AddData(QString str);
    void SetAllCheck(int type,QString str);//设置全选
    //设置计划控制 排路状态 选中状态
    void SetCheckState(bool bcheck1,bool bcheck2);
    QStringList strStationName;//车站名称
    int nPlanState;//计划控制
    int nPLState;//排路状态
    QCheckBox *checkPlan;
    QRadioButton *radioPLAuto;
    QRadioButton *radioPLUnAuto;
signals:
    //向服务端发送控制模式转换信息
    void SendStationCtrModeChoseSignal(int mode,int type1,int type2);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private slots:
    void slot_btnGroupClicked(int n);
private:
    Ui::StateChose *ui;
};

#endif // STATECHOSE_H
