#ifndef PASSWORDENTRYDLG_H
#define PASSWORDENTRYDLG_H

#include <QWidget>
#include "keyboard.h"
namespace Ui {
class PassWordEntryDlg;
}

class PassWordEntryDlg : public QWidget
{
    Q_OBJECT

public:
    explicit PassWordEntryDlg(QWidget *parent = nullptr);
    ~PassWordEntryDlg();
    void ClearText();
    void SetTypeText(QString str);
    void SetTitle(QString str="");
    KeyBoard* mKeyBoard;
    bool onOK;
private:
    Ui::PassWordEntryDlg *ui;
    QWidget* curWidget = nullptr;
    QPoint curPoint = { 0,0 };
    QPoint curMousePoint = { 0,0 };
//    bool eventFilter(QObject* obj,QEvent* event) override;
//    void mousePressEvent(QMouseEvent* event) override;
//    void mouseReleaseEvent(QMouseEvent* event) override;
//    void mouseMoveEvent(QMouseEvent* event) override;
signals:
    void ClosedSignal();
    void CheckReturnSignal(bool);
public slots:
    void windowclosed();//关闭整个窗口
    void ButtonOk();//确认按钮
    void ButtonCancle();//取消按钮
    void receiveKeyboardDataSlot(QString,QString);
//private slots:
//    void on_pushButton_clicked();
};

#endif // PASSWORDENTRYDLG_H
