#ifndef QFTJ_H
#define QFTJ_H

#include <QWidget>
#include "TableView/tableview.h"
#include "GlobalHeaders/Global.h"
#include "GlobalHeaders/GlobalFuntion.h"
namespace Ui {
class QFTJ;
}

class QFTJ : public QWidget
{
    Q_OBJECT

public:
    explicit QFTJ(QWidget *parent = nullptr);
    ~QFTJ();

    QVector<QianFeng*>mQianFengVect;
    TableView *mCTableViewQFTJ;
    void InitShow(QString strStationName,QVector<QianFeng> QianFengVect);
//public slots:
//    void AddQFCount(QString str);

private:
    Ui::QFTJ *ui;
};

#endif // QFTJ_H
