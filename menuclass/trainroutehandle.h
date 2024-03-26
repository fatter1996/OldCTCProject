#ifndef TRAINROUTEHANDLE_H
#define TRAINROUTEHANDLE_H

#include <QWidget>
#include "keyboard.h"
namespace Ui {
class TrainRouteHandle;
}

class TrainRouteHandle : public QWidget
{
    Q_OBJECT

public:
    explicit TrainRouteHandle(QWidget *parent = nullptr);
    ~TrainRouteHandle();
    void ClearText();
    void SetTypeText(QString str);
    void SetTitle(QString str="");
    QString GetTrainRouteNum();
    KeyBoard* mKeyBoard;
private:
    Ui::TrainRouteHandle *ui;
signals:
    void ClosedSignal();
    void CheckReturnSignal(QString);
    void CancleSignal();
public slots:
    void windowclosed();//关闭整个窗口
    void ButtonOk();//确认按钮
    void ButtonCancle();//取消按钮
    void receiveKeyboardDataSlot(QString,QString);
};

#endif // TRAINROUTEHANDLE_H
