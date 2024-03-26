#ifndef POORSHUNTDLG_H
#define POORSHUNTDLG_H

#include <QWidget>
#include "GlobalHeaders/Global.h"
#include "QButtonGroup"
namespace Ui {
class PoorShuntDlg;
}

class PoorShuntDlg : public QWidget
{
    Q_OBJECT

public:
    explicit PoorShuntDlg(QWidget *parent = nullptr);
    ~PoorShuntDlg();
    void Init();
    void InitShow(int ntpye,QString str,bool ncheckCQ=false,bool ncheckDW=false,bool ncheckFW=false);
    void SetStateShow(bool b);
    void SendSigal(bool n[3]);
    QButtonGroup *pButtonGroup;
    int nWidgettpye;
    bool boldcheck[3]={false};
    bool isGDFLBLcheck;
signals:
    void resultsignal(int ntpye,int ntype2=0,int ntype3=0);

private:
    Ui::PoorShuntDlg *ui;
};

#endif // POORSHUNTDLG_H
