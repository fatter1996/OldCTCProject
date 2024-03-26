#ifndef DCROUTETIMEWIDGET_H
#define DCROUTETIMEWIDGET_H

#include <QWidget>

namespace Ui {
class DCRouteTimeWidget;
}

class DCRouteTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DCRouteTimeWidget(QWidget *parent = nullptr);
    ~DCRouteTimeWidget();
    void Init(QString str);
    QString GetTimeAndType();
signals:
    void ClosedSignal();
    void CancleSignal();
public slots:
    void ButtonOk();//确认按钮
    void ButtonCancle();//取消按钮
private:
    Ui::DCRouteTimeWidget *ui;
};

#endif // DCROUTETIMEWIDGET_H
