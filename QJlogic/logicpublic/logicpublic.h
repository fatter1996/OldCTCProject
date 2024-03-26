#ifndef LOGICPUBLIC_H
#define LOGICPUBLIC_H

#include <QWidget>

namespace Ui {
class logicpublic;
}

class logicpublic : public QWidget
{
    Q_OBJECT

public:
    explicit logicpublic(QWidget *parent = nullptr);
    ~logicpublic();

private:
    Ui::logicpublic *ui;
};

#endif // LOGICPUBLIC_H
