#include "userexitwidget.h"
#include "ui_userexitwidget.h"
#include<QMessageBox>
UserExitWidget::UserExitWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UserExitWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    //确定
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(77, QCursor::pos());
//        if(ui->lineEdit_2->text()=="123")
//        {
//            emit UserExitSignal(ui->lineEdit_2->text());
//            close();
//        }
//        else
//        {
//            QMessageBox::information(nullptr,"错误","密码不正确!");
//        }
           emit UserExitSignal(true);
         close();
    });
     //取消
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(121, QCursor::pos());
        //UserExitSignalCancle(false);
        // emit UserExitSignal(false);
       close();
    });
}

UserExitWidget::~UserExitWidget()
{
    delete ui;
}
void UserExitWidget::Init()
{
    ui->lineEdit_2->setText("");
}
