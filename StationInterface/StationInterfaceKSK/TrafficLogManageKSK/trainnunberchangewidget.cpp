#include "trainnunberchangewidget.h"
#include "ui_trainnunberchangewidget.h"

TrainNunberChangeWidget::TrainNunberChangeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainNunberChangeWidget)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, [=](){
        QString arrival = ui->lineEdit_3->text();
        QString departure = ui->lineEdit_4->text();
        bool bReserve = ui->checkBox->isChecked();
        bool bFinalArrival = ui->checkBox_2->isChecked();
        emit trainNunberChange(arrival, departure, bReserve, bFinalArrival);
        emit SendTextAnalysisSignal(77, QCursor::pos());
        this->close();
    });
    connect(ui->pushButton_2, &QPushButton::clicked, [=](){
        emit SendTextAnalysisSignal(78, QCursor::pos());
        this->close();
    });
}

TrainNunberChangeWidget::~TrainNunberChangeWidget()
{
    delete ui;
}

void TrainNunberChangeWidget::Init(MyStation* pMyStation, TrafficLog* pTraffic)
{
    ui->comboBox->addItem(pMyStation->m_strStationName);
    ui->lineEdit->setText(pTraffic->m_strReachTrainNum);
    ui->lineEdit_2->setText(pTraffic->m_strDepartTrainNum);
    if(pTraffic->m_btBeginOrEndFlg == 0xcc)
    {
        ui->checkBox_2->setCheckState(Qt::Checked);
    }
}
