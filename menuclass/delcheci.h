#ifndef DELCHECI_H
#define DELCHECI_H

#include <QWidget>

namespace Ui {
class DelCheCi;
}

class DelCheCi : public QWidget
{
    Q_OBJECT

public:
    explicit DelCheCi(QWidget *parent = nullptr);
    ~DelCheCi();
    void Init(QString str);
    bool bok;
signals:
    void ReturnResultSignal();
private:
    Ui::DelCheCi *ui;
};

#endif // DELCHECI_H
