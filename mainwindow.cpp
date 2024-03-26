#include <QString>
#include <QDebug>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QTime>
#include <QScrollBar>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "Log/log.h"

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(SYSTITLE);
    puserexit=new UserExitWidget();
}

void MainWindow::addWidget(QWidget* wnd)
{
    ui->centralwidget->layout()->addWidget(wnd);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::closeEvent (QCloseEvent *e)
{
    //e->accept();
    puserexit->show();
    e->ignore();
    connect(puserexit,&UserExitWidget::UserExitSignal,this,[=](){
       exit(0);
    });
    QLOG_INFO()<<QString("系统退出！");
}
void MainWindow::UserExitSignalSolt(bool b)
{
    if(b)
    {
        this->close();
    }
}
