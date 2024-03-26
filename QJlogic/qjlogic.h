#ifndef QJLOGIC_H
#define QJLOGIC_H

#include <QMainWindow>
#include <QTabWidget>
#include "logicpublic/logicpublic.h"
#include "MyStation/mystation.h"

namespace Ui {
class QJlogic;
}

class QJlogic : public QMainWindow
{
    Q_OBJECT

public:
    explicit QJlogic(QWidget *parent = nullptr);
    ~QJlogic();

    void Init(MyStation* pStation);
    void initSoltsFunction();
    void Clear();

signals:
     void SendLogicCheck(int nType, QString strName);

public slots:
     void onRecvLogicCheck(int type, int result, QString msg);
protected:
    virtual void timerEvent(QTimerEvent *event);

private:
    Ui::QJlogic *ui;
    logicpublic *m_logicpublic;
    QTabWidget *mQTabWidget;
    int nTimerId = -1;
};

#endif // QJLOGIC_H
