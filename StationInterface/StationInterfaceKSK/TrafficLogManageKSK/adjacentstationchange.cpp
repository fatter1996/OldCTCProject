#include "adjacentstationchange.h"
#include "ui_adjacentstationchange.h"

AdjacentStationChange::AdjacentStationChange(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdjacentStationChange)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
}

AdjacentStationChange::~AdjacentStationChange()
{
    delete ui;
}

void AdjacentStationChange::Init(MyStation* pMyStation, TrafficLog* pTraffic)
{
    ui->label->setText(QString("%1次列车").arg(pMyStation->m_strStationName));
    for(int i = 0; i < pMyStation->StaConfigInfo.JFCKInfo.size(); i++)
    {
        QString direct = pMyStation->StaConfigInfo.JFCKInfo.at(i).strDirectSta;
        ui->comboBox->addItem(direct);
        ui->comboBox_2->addItem(direct);
    }
    ui->comboBox->setCurrentText(pTraffic->m_strFromAdjtStation);
    ui->comboBox_2->setCurrentText(pTraffic->m_strToAdjtStation);
}

void AdjacentStationChange::on_pushButton_clicked()
{
    emit SendTextAnalysisSignal(77, QCursor::pos());
    emit AdjStationChange(ui->comboBox->currentText(), ui->comboBox_2->currentText());
    this->close();
}

void AdjacentStationChange::on_pushButton_2_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    this->close();
}
