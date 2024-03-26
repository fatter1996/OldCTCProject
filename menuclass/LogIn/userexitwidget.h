#ifndef USEREXITWIDGET_H
#define USEREXITWIDGET_H

#include <QWidget>
#include "GlobalHeaders/GlobalStructure.h"

namespace Ui {
class UserExitWidget;
}

class UserExitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserExitWidget(QWidget *parent = nullptr);
    ~UserExitWidget();
    void Init();
signals:
    void UserExitSignal(bool b);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private:
    Ui::UserExitWidget *ui;
};

#endif // USEREXITWIDGET_H
