#ifndef OPTION_H
#define OPTION_H

#include <QWidget>

namespace Ui {
class option;
}

class option : public QWidget
{
    Q_OBJECT

public:
    explicit option(QWidget *parent = nullptr);
    ~option();
    void Init(int ntype, bool bAutoSignStage);
signals:
    void AutoSignState(bool bsign);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private:
    Ui::option *ui;
};

#endif // OPTION_H
