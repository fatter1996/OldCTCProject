#ifndef AGAINSTWRONGWIDGET_H
#define AGAINSTWRONGWIDGET_H

#include <QWidget>
#include "GlobalHeaders/GlobalStructure.h"
namespace Ui {
class AgainstWrongWidget;
}

class AgainstWrongWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AgainstWrongWidget(QWidget *parent = nullptr);
    ~AgainstWrongWidget();
    void ShowCheckMsg(CheckResult* checkResult);
protected:
    void timerEvent(QTimerEvent *event) override;
private slots:
    void on_buttonForce_clicked();
    void on_buttonCancel_clicked();
signals:
    void sendAgainstWrongMsg(int id, int type);
private:
    Ui::AgainstWrongWidget *ui;
    CheckResult* pCheckResult = nullptr;
    QVector<CheckResult*> vecCheckResult;
    int  timerId = -1;
};

#endif // AGAINSTWRONGWIDGET_H
