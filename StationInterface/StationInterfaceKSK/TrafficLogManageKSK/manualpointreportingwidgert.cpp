#include "manualpointreportingwidgert.h"
#include "ui_manualpointreportingwidgert.h"

ManualPointReportingWidgert::ManualPointReportingWidgert(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualPointReportingWidgert)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
}

ManualPointReportingWidgert::~ManualPointReportingWidgert()
{
    delete ui;
}

void ManualPointReportingWidgert::Init(MyStation* pStation, TrafficLog* pTrafficLog, int type)
{
    ui->stationNameCombo->addItem(pStation->m_strStationName);

    ui->planArriveTimeEdit->setDateTime(QDateTime());
    ui->planArriveTimeEdit->setEnabled(false);
    ui->planDepartTimeEdit->setDateTime(QDateTime());
    ui->planDepartTimeEdit->setEnabled(false);
    ui->factArriveTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->factdepartTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->planArriveTimeEdit->setDateTime(pTrafficLog->m_timProvReachStation);
    ui->planDepartTimeEdit->setDateTime(pTrafficLog->m_timProvDepaTrain);
    ui->comboBox_2->addItem(pTrafficLog->m_strFromAdjtStation);
    ui->comboBox_3->addItem(pTrafficLog->m_strToAdjtStation);
    if(type == 1) //到达
    {
        ui->factdepartTimeEdit->setEnabled(false);
    }
    else if(type == 2) //出发
    {
        ui->factArriveTimeEdit->setEnabled(false);
    }

    connect(ui->pushButton, &QPushButton::clicked, [=](){
        emit reportingPoints(ui->factArriveTimeEdit->dateTime(), ui->factdepartTimeEdit->dateTime());
        emit SendTextAnalysisSignal(77, QCursor::pos());
        this->close();
    });

    connect(ui->pushButton_2, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        this->close();
    });
}
