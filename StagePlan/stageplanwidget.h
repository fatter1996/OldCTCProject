#ifndef STAGEPLANWIDGET_H
#define STAGEPLANWIDGET_H

#include <QWidget>
#include "TableView/tableview.h"
#include "MyStation/mystation.h"

namespace Ui {
class StagePlanWidget;
}

class StagePlanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StagePlanWidget(QWidget *parent = nullptr);
    ~StagePlanWidget();

    void InitTableView();
    void StagePlanUpdata();
    void InitStation(MyStation* pMyStation) { m_pMyStation = pMyStation; }

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

signals:
    void signFor();
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private:
    TableView* mCTableView = nullptr;
    MyStation* m_pMyStation = nullptr;
    Ui::StagePlanWidget *ui;
};

#endif // STAGEPLANWIDGET_H
