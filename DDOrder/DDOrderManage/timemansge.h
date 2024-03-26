#ifndef TIMEMANSGE_H
#define TIMEMANSGE_H

#include <QWidget>

namespace Ui {
class TimeMansge;
}

class TimeMansge : public QWidget
{
    Q_OBJECT

public:
    explicit TimeMansge(QWidget *parent = nullptr);
    ~TimeMansge();

private:
    Ui::TimeMansge *ui;
};

#endif // TIMEMANSGE_H
