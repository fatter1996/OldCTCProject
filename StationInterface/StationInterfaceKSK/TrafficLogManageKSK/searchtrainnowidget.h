#ifndef SEARCHTRAINNOWIDGET_H
#define SEARCHTRAINNOWIDGET_H

#include <QWidget>

namespace Ui {
class SearchTrainNoWidget;
}

class SearchTrainNoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SearchTrainNoWidget(QWidget *parent = nullptr);
    ~SearchTrainNoWidget();
signals:
    void searchTrainNum(QString);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::SearchTrainNoWidget *ui;
};

#endif // SEARCHTRAINNOWIDGET_H
