#ifndef NEWDDML_H
#define NEWDDML_H

#include <QWidget>
#include "GlobalHeaders/Global.h"
#include "GlobalHeaders/GlobalFuntion.h"
#include "TableView/tableview.h"
#include "MyStation/dispatchorderstation.h"
#include "MyStation/dispatchorderlocomotive.h"
#include "MyStation/dispatchorderdispatcher.h"
namespace Ui {
class Newddml;
}

class Newddml : public QWidget
{
    Q_OBJECT

public:
    explicit Newddml(QWidget *parent = nullptr);
    ~Newddml();
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
    typedef struct
    {
        QString strDDT;
        void Init()
        {
          strDDT="";
        }
    }TableShowDDT;
    TableShowDDT mTableShowDDT;
    QVector<TableShow>mVectTableShow;
    QVector<TableShowDDT>mVectTableShowDDT;
    bool bAddFalg=false;//添加标志
    TableView* mCTableView;
    TableView* mCTableViewDDML;
    DispatchOrderDispatcher *mDispatchOrderDispatcher;
    DispatchOrderLocomotive *mArrayDisOrderLocomot;
    QVector<DisOrderTemplate> mVectDisOrderTemplate;
    void ClearShow();
    void Init(QString strsta,QString strname,int norder);
    void InitDisTemplate();
    void InitTableView();
    void InitTableViewDDML();
    void UpDateTableView();
    void UpDateTableViewDDML();
signals:
    void NewDDOrderData(DispatchOrderDispatcher *mDispatchOrderDispatcher,QString strobj);
    void NewDDOrderData(DispatchOrderLocomotive *mDispatchOrderLocomotive,QString strobj);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
    void currentIndexChangedSlot(int i);
private:
    Ui::Newddml *ui;
};

#endif // NEWDDML_H
