#ifndef NEWLP_H
#define NEWLP_H

#include <QWidget>
#include <QColor>
#include "TableView/tableview.h"
#include "MyStation/dispatchorderstation.h"
#include "MyStation/dispatchorderlocomotive.h"
#include "MyStation/dispatchorderdispatcher.h"
namespace Ui {
class NewLP;
}

class NewLP : public QWidget
{
    Q_OBJECT

public:
    explicit NewLP(QWidget *parent = nullptr);
    ~NewLP();
    typedef struct
    {
        QString strcheci;
        QString strtrainnum;
        void Init()
        {
          strcheci="";
          strtrainnum="";
        }
    }TableShow;
    TableShow mTableShow;
    QVector<TableShow>mVectTableShow;
    TableView* mCTableView;
    bool bAddFalg=false;
    void ClearShow();
    void Init(QString strsta,QString strname,int norder);
    void SetTitel(QString str);
    void SetLable(QString str);
    void SetLableBlackColor(int n);
    void InitTableView();
    void UpDateTableView();
    int GetOrderType(QString str);

signals:
    void NewLPDataSignal(int ntype,DispatchOrderLocomotive *DispatchOrderLocomotive);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private:
    Ui::NewLP *ui;
};

#endif // NEWLP_H
