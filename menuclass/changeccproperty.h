#ifndef CHANGECCPROPERTY_H
#define CHANGECCPROPERTY_H

#include <QWidget>
#include "LeadSealing/keyboard.h"
namespace Ui {
class ChangeCCProperty;
}

class ChangeCCProperty : public QWidget
{
    Q_OBJECT

public:
    explicit ChangeCCProperty(QWidget *parent = nullptr);
    ~ChangeCCProperty();
    int nlineeditCheck;//机车速率 机车号编辑框选中状态
    //设置车次号
    void SetCCH(QString str,bool bElectricTraction,QString m_strLocomotiveNumber);

    //设置机车速率
    void SetTrainSpeed(QString str);
    //设置机车号
    void SetTrainNum(QString str);
    //获取机车速率
    QString GetTrainSpeed();
    //获取机车号
    QString GetTrainNum();
    bool GetElectric();
    bool bok;
    KeyBoard* mKeyBoard;
signals:
    void ClosedSignal();
    void ReturnChangeSignal(QString strSpeed,QString strTrainNum,bool bElectricTraction);
public slots:
    //设置软键盘输入字符
    void SetInputStr(QString str,QString strboj);
private:
    Ui::ChangeCCProperty *ui;
};

#endif // CHANGECCPROPERTY_H
