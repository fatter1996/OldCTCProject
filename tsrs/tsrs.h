#ifndef TSRS_H
#define TSRS_H

#include <QWidget>

namespace Ui {
class tsrs;
}

class tsrs : public QWidget
{
    Q_OBJECT

public:
    explicit tsrs(QWidget *parent = nullptr);
    ~tsrs();

private:
    Ui::tsrs *ui;
};

#endif // TSRS_H
