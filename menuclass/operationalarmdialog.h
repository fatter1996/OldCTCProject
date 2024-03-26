#ifndef OPERATIONALARMDIALOG_H
#define OPERATIONALARMDIALOG_H

#include <QDialog>
#include <QCloseEvent>
namespace Ui {
class OperationAlarmDialog;
}

class OperationAlarmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OperationAlarmDialog(QWidget *parent = nullptr);
    ~OperationAlarmDialog();
    void InitShow(QString str);

private:
    Ui::OperationAlarmDialog *ui;
    void closeEvent (QCloseEvent *e);
};

#endif // OPERATIONALARMDIALOG_H
