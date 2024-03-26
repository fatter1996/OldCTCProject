#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
namespace Ui {
class KeyBoard;
}

class KeyBoard : public QWidget
{
    Q_OBJECT

public:
    explicit KeyBoard(QWidget *parent = nullptr);
    ~KeyBoard();
    QString strAssObjName;//关联对象名称
    //设置关联对象名称
    void SetAssObjName(QString str);
    //获取关联对象名称
    QString GetAssObjName();

private:
    Ui::KeyBoard *ui;
signals:
    //接收数据信号
    void recvstrDataSignal(QString,QString);
public slots:
        void slot_btnGroupClicked(int nId);
        void CapsPressSlot();
        void RecvClosedSlot();

};

#endif // KEYBOARD_H
