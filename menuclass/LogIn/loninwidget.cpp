#include "loninwidget.h"
#include "ui_loninwidget.h"
#include<QMessageBox>

#pragma execution_character_set("utf-8")
LonInWidget::LonInWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LonInWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::WindowMaximizeButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    //ui->comboBox->addItem("用户1");
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxChanged(int)));
    //确定
    connect(ui->pushButton_3,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(77, QCursor::pos());
        if(ui->lineEdit_3->text()!=""&&ui->lineEdit->text()!="")
        {
            emit LogInSignal(ui->lineEdit_3->text());
            close();
        }
        else
        {
            QMessageBox::information(nullptr,"错误","密码和用户名不能为空!",tr("确定"),tr("取消"),"",0);
        }
    });
    //取消
    connect(ui->pushButton_4,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        close();
    });
}

LonInWidget::~LonInWidget()
{
    delete ui;
}
void LonInWidget::Init(QStringList strlist)
{
    ui->comboBox->clear();
    strlistname=strlist;
    for (int i=0;i<strlist.size();i++)
    {
        ui->comboBox->addItem("用户"+QString::number(i+1));
    }
    if(strlist.size()>0)
    {
        ui->lineEdit_3->setText(strlist[0]);
    }
    else
    {
        ui->lineEdit_3->setText("");
    }
    ui->lineEdit->setText("");
}
void LonInWidget::onComboBoxChanged(int n)
{
    if(n>=0&&n<strlistname.size())
    {
      ui->lineEdit_3->setText(strlistname[n]);
    }
}
