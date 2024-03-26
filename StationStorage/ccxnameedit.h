#ifndef CCXNAMEEDIT_H
#define CCXNAMEEDIT_H

#include <QWidget>

namespace Ui {
class ccxnameedit;
}

class ccxnameedit : public QWidget
{
    Q_OBJECT

public:
    explicit ccxnameedit(QWidget *parent = nullptr);
    ~ccxnameedit();

private:
    Ui::ccxnameedit *ui;
};

#endif // CCXNAMEEDIT_H
