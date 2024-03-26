#ifndef LONINWIDGET_H
#define LONINWIDGET_H

#include <QWidget>
#include "GlobalHeaders/GlobalStructure.h"
namespace Ui {
class LonInWidget;
}

class LonInWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LonInWidget(QWidget *parent = nullptr);
    ~LonInWidget();
    QString mUserCode;//用户代码
    QString mUserName;//用户姓名
    QString mUserPassWord;//密码
    QStringList strlistname;
    void Init(QStringList strlist);
signals:
    void LogInSignal(QString mUserName);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
public slots:
    void onComboBoxChanged(int n);
private:
    Ui::LonInWidget *ui;
};

#endif // LONINWIDGET_H
