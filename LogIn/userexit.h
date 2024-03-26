#ifndef USEREXIT_H
#define USEREXIT_H

#include <QDialog>

namespace Ui {
class userexit;
}

class userexit : public QDialog
{
    Q_OBJECT

public:
    explicit userexit(QWidget *parent = nullptr);
    ~userexit();

private:
    Ui::userexit *ui;
};

#endif // USEREXIT_H
