#ifndef SEEKTRAIN_H
#define SEEKTRAIN_H

#include <QWidget>
#include "TableView/tableview.h"
#include "MyStation/trainnumwnd.h"
namespace Ui {
class SeekTrain;
}

class SeekTrain : public QWidget
{
    Q_OBJECT

public:
    explicit SeekTrain(QWidget *parent = nullptr);
    ~SeekTrain();
    void InitTableView();
    void SetData(QVector<TrainNumWnd*> mArrayTrainNumWnd,QString StationName);
    void TrainNumUpDate(bool b);
    void ShowDate(QStringList strList);
    void SetBacklackRow(TableView* mCTableView,int index);

    TableView* mCTableView;
    QVector<TrainNumWnd*> m_ArrayTrainNumWnd;
    QString strStationName="";

    QStringList strList;
private:
    Ui::SeekTrain *ui;
private slots:
    void itemLBtnClicked(const QModelIndex &index);
    void TextChangeSolt(QString str);
};

#endif // SEEKTRAIN_H
