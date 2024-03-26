#ifndef TRANSMITTRAIN_H
#define TRANSMITTRAIN_H

#include <QWidget>
#include "MyStation/dispatchorderstation.h"
#include "TableView/tableview.h"
namespace Ui {
class TransmitTrain;
}

class TransmitTrain : public QWidget
{
    Q_OBJECT

public:
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
    DispatchOrderStation *mDispatchOrderStation;
    int nCheckRowLeft=-1;
    explicit TransmitTrain(QWidget *parent = nullptr);
    ~TransmitTrain();
    TableView* mCTableView;
    void InitTableView();
    void Init(QString strtitle,DispatchOrderStation *pDispatchOrderStation);
    void AddTableView();
    void DeleteTableView(int n);
    void EditTableView(int n);
signals:
    void TransmitTrainSignal(DispatchOrderStation mDispatchOrderStation);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
      void itemLBtnClicked(const QModelIndex &index);
private:
    Ui::TransmitTrain *ui;
};

#endif // TRANSMITTRAIN_H
