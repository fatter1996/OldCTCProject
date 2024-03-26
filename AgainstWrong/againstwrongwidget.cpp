#include "againstwrongwidget.h"
#include "ui_againstwrongwidget.h"

AgainstWrongWidget::AgainstWrongWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgainstWrongWidget)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
}

AgainstWrongWidget::~AgainstWrongWidget()
{
    delete ui;
}

void AgainstWrongWidget::ShowCheckMsg(CheckResult* checkResult)
{
    pCheckResult = checkResult;
    vecCheckResult.append(pCheckResult);
    ui->textEdit->setText(pCheckResult->checkMsg);
    ui->buttonForce->setEnabled(pCheckResult->bEnforced);

    QSettings* settings = new QSettings("Data/Global.ini", QSettings::IniFormat);
    // 指定为中文
    settings->setIniCodec("UTF-8");

    //读取配置-通信信息
    int time = settings->value("CONFIG/AutoCloseTime").toInt();
    if(time == 0)
        time = 5000;
    timerId = startTimer(time);
    this->show();
}

void AgainstWrongWidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timerId)
    {
        vecCheckResult.clear();
        this->close();
    }
}

void AgainstWrongWidget::on_buttonForce_clicked()
{
//    qDebug() << "强制执行";
    ui->textEdit->clear();
    //emit sendAgainstWrongMsg(pCheckResult->id, 0x01);
    for (int i = 0; i < vecCheckResult.size(); i++)
    {
        emit sendAgainstWrongMsg(vecCheckResult.at(i)->id, 0x01);
    }
    vecCheckResult.clear();
    this->close();
}

void AgainstWrongWidget::on_buttonCancel_clicked()
{
//    qDebug() << "放弃";
    ui->textEdit->clear();
    //emit sendAgainstWrongMsg(pCheckResult->id, 0x02);
    for (int i = 0; i < vecCheckResult.size(); i++)
    {
        emit sendAgainstWrongMsg(vecCheckResult.at(i)->id, 0x02);
    }
    vecCheckResult.clear();
    this->close();
}
