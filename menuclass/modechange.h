#ifndef MODECHANGE_H
#define MODECHANGE_H

#include <QWidget>
#include<QVBoxLayout>
#include<QHBoxLayout>
#include <QButtonGroup>
#include "GlobalHeaders/GlobalStructure.h"

namespace Ui {
class ModeChange;
}

class ModeChange : public QWidget
{
    Q_OBJECT
public:
    QVBoxLayout *layoutv;
    int nMode;
    int nCheckState;
    QStringList strStationName;//车站名称
      QButtonGroup *mbg;
      int WindowShowType=-1;
      bool bargee=false;
public:
    explicit ModeChange(QWidget *parent = nullptr);
    ~ModeChange();
    void SetMode(QString str,int mode,int type);
    void Init(int ntype);
    void AddData(int ntype,QString str,int ncheck);
    void SetAllCheck(int type,QString str);//设置全选
signals:
    void SendStationCtrModeChangeSignal(int mode,int type1,int type2);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
    void slot_btnGroupClicked(int n);
private:
    Ui::ModeChange *ui;
};

#endif // MODECHANGE_H
