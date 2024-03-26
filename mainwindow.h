#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GlobalHeaders/Global.h"
#include "menuclass/LogIn/userexitwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addWidget(QWidget* wnd);

private:
    Ui::MainWindow *ui;
    UserExitWidget *puserexit;//用户注销
void closeEvent (QCloseEvent *e);
public slots:
    void UserExitSignalSolt(bool b);
protected:

};
#endif // MAINWINDOW_H
