#ifndef POORSHUNTDLG_DLG_H
#define POORSHUNTDLG_DLG_H

#include <QDialog>
#include "GlobalHeaders/Global.h"
#include "QButtonGroup"

namespace Ui {
class PoorShuntDlg_Dlg;
}

class PoorShuntDlg_Dlg : public QDialog
{
    Q_OBJECT

public:
    explicit PoorShuntDlg_Dlg(QWidget *parent = nullptr);
    ~PoorShuntDlg_Dlg();
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
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::PoorShuntDlg_Dlg *ui;
};

#endif // POORSHUNTDLG_DLG_H
