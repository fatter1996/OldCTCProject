#ifndef ADDTRAIN_H
#define ADDTRAIN_H

#include <QWidget>
#include "LeadSealing/keyboard.h"
namespace Ui {
class AddTrain;
}

class AddTrain : public QWidget
{
    Q_OBJECT

public:
    explicit AddTrain(QWidget *parent = nullptr);
    ~AddTrain();
    //初始化车站 股道编辑框
    void Init(QString strSta,QString strGD,bool btitle);

    //获取车次号
    QString GetCCH();
    //获取电力机车状态
    bool GetElectric();
    bool GetSimTrain();
    bool bok;
    KeyBoard* mKeyBoard;
signals:
    void ClosedSignal();
    void ReturnAddSignal(QString strCCH);
public slots:
    //设置车次号
    void SetCCH(QString strCCH,QString strboj);

private:
    Ui::AddTrain *ui;
};

#endif // ADDTRAIN_H
