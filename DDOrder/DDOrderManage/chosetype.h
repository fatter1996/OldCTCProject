#ifndef CHOSETYPE_H
#define CHOSETYPE_H

#include <QWidget>
#include <QPushButton>
#include<QButtonGroup>
#include "newlp.h"
#include "newddml.h"
namespace Ui {
class ChoseType;
}

class ChoseType : public QWidget
{
    Q_OBJECT

public:
    explicit ChoseType(QWidget *parent = nullptr);
    ~ChoseType();

    void Init();
    void InitBtnGroup();
    void SetDlgShow(int nId);
    NewLP *m_NewLP;
    Newddml *m_Newddml;
signals:
    void ChoseTypeSignal(int nId);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private:
    Ui::ChoseType *ui;
};

#endif // CHOSETYPE_H
