#ifndef CONFIRMCCH_H
#define CONFIRMCCH_H

#include <QWidget>
#include "LeadSealing/keyboard.h"
namespace Ui {
class ConfirmCCH;
}

class ConfirmCCH : public QWidget
{
    Q_OBJECT

public:
    explicit ConfirmCCH(QWidget *parent = nullptr);
    ~ConfirmCCH();
    void Init(QString type,QString strTitel,QString strOld);
    void SetTitel(QString strTitel);//设置标题
    void SetOldCCH(QString strOld);//设置旧车次号

    QString GetOldCCH();//获取旧车次号
    QString GetNewCCH();//获取新车次号
    bool bok;
    int ntype;//车次确认/车次修改
    KeyBoard* mKeyBoard;
signals:
    void ClosedSignal();
    void ReturnConfirmCCHSignal(QString strNewCC);
public slots:
    void windowclosed();//关闭整个窗口
    void SetNewCCH(QString strOld,QString strboj);//设置新车次号
private:
    Ui::ConfirmCCH *ui;
};

#endif // CONFIRMCCH_H
