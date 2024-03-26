#ifndef DDORDERNOSIGNWIDGET_H
#define DDORDERNOSIGNWIDGET_H

#include <QDialog>

namespace Ui {
class DDOrderNoSignWidget;
}

class DDOrderNoSignWidget : public QDialog
{
    Q_OBJECT

public:
    explicit DDOrderNoSignWidget(QWidget *parent = nullptr);
    ~DDOrderNoSignWidget();
    void Init();
    bool GetPassWord();

    bool bok;
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DDOrderNoSignWidget *ui;
};

#endif // DDORDERNOSIGNWIDGET_H
