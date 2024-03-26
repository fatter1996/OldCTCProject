#ifndef DCROUTETIMEWIDGET_DLG_H
#define DCROUTETIMEWIDGET_DLG_H

#include <QDialog>

namespace Ui {
class DCRouteTimeWidget_Dlg;
}

class DCRouteTimeWidget_Dlg : public QDialog
{
    Q_OBJECT

public:
    explicit DCRouteTimeWidget_Dlg(QWidget *parent = nullptr);
    ~DCRouteTimeWidget_Dlg();
    void Init(QString str);
    QString GetTimeAndType();
    QString GetInputTime();
    bool onOK;

signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DCRouteTimeWidget_Dlg *ui;
};

#endif // DCROUTETIMEWIDGET_DLG_H
