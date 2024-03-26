#include "newtraffclogwidget.h"
#include "ui_newtraffclogwidget.h"
#include "QMessageBox"

NewTraffclogWidget::NewTraffclogWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewTraffclogWidget)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);


}

NewTraffclogWidget::~NewTraffclogWidget()
{
    delete ui;
}

void NewTraffclogWidget::Init(MyStation* pStation, bool isChenge)
{
    pCurStation = pStation;
    m_bIsChenge = isChenge;
    //车站名
    ui->stationNameCombo->addItem(pStation->m_strStationName);
    //接车口/发车口
    ui->wayInCombo->addItem(" ");
    ui->wayOutCombo->addItem(" ");
    for(int i = 0; i < pStation->DevArray.size(); i++)
    {
        if(pStation->DevArray[i]->getDevType() == Dev_XH)
        {
            CXHD* pXHD=(CXHD*)(pStation->DevArray[i]);
            if(pXHD->getXHDType() == JZ_XHJ)
            {
                ui->wayInCombo->addItem(pXHD->m_strName);
                ui->wayOutCombo->addItem(pXHD->m_strName);
            }
        }
    }
    for(int i = 0; i < pStation->v_TrainNumType.size(); i++)
    {
        ui->trainTypeCombo->addItem(pStation->v_TrainNumType[i]->strTypeName);
    }
    for(int i = 0; i < pStation->v_TrainRunType.size(); i++)
    {
        ui->operationTypeCombo->addItem(pStation->v_TrainRunType[i]);
    }
    //接车股道/发车股道
    ui->receivingTrackCombo->addItem(" ");
    ui->departureTrackCombo->addItem(" ");
    for(int i = 0; i < pStation->DevArray.size(); i++)
    {
        if(pStation->DevArray[i]->getDevType() == Dev_GD)
        {
            CGD* pGD=(CGD*)(pStation->DevArray[i]);
            if(pGD->m_nGDType == GD_QD)
            {
                ui->receivingTrackCombo->addItem(pGD->m_strName);
                ui->departureTrackCombo->addItem(pGD->m_strName);
            }
        }
    }
    //来向/去向
    ui->previousStationCombo->addItem(" ");
    ui->nextStationCombo->addItem(" ");
    for(int i = 0; i < pStation->StaConfigInfo.JFCKInfo.size(); i++)
    {
        QString direct = pStation->StaConfigInfo.JFCKInfo.at(i).strDirectSta;
        ui->previousStationCombo->addItem(direct);
        ui->nextStationCombo->addItem(direct);
    }

    //到达时间/出发时间
    ui->arrivalTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->departureTimeEdit->setDateTime(QDateTime::currentDateTime());

    connect(ui->receivingTrackCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        ui->departureTrackCombo->setCurrentIndex(index);
    });

    connect(ui->departureTrackCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        //ui->receivingTrackCombo->setCurrentIndex(index);
    });

    connect(ui->originationCheck, &QCheckBox::stateChanged, [=](int state){
        if(state == Qt::Checked) //选中
        {
            int index= ui->departureTrackCombo->currentIndex();
            //ui->finalArrivalCheck->setCheckState(Qt::Unchecked);
            ui->arriveTrainNoLabel->setEnabled(false);
            ui->arriveTrainNoLabel->clear();
            ui->arrivalOverdueComboBox->setEnabled(false);
            ui->arrivalOverdueComboBox->setCurrentIndex(0);
            ui->wayInCombo->setEnabled(false);
            ui->wayInCombo->setCurrentIndex(0);
            ui->receivingTrackCombo->setEnabled(false);
            ui->receivingTrackCombo->setCurrentIndex(0);
            ui->arrivalTimeEdit->setEnabled(false);
            //ui->arrivalTimeEdit->clear();
            ui->departureTrackCombo->setCurrentIndex(index);
        }
        else
        {
            ui->arriveTrainNoLabel->setEnabled(true);
            ui->arrivalOverdueComboBox->setEnabled(true);
            ui->wayInCombo->setEnabled(true);
            ui->receivingTrackCombo->setEnabled(true);
            ui->arrivalTimeEdit->setEnabled(true);
        }
    });

    connect(ui->finalArrivalCheck, &QCheckBox::stateChanged, [=](int state){
        if(state == Qt::Checked) //选中
        {
            //ui->originationCheck->setCheckState(Qt::Unchecked);
            ui->setOutTrainNoLabel->setEnabled(false);
            ui->setOutTrainNoLabel->clear();
            ui->departOverdueComboBox->setEnabled(false);
            ui->departOverdueComboBox->setCurrentIndex(0);
            ui->wayOutCombo->setEnabled(false);
            ui->wayOutCombo->setCurrentIndex(0);
            ui->departureTrackCombo->setEnabled(false);
            ui->departureTrackCombo->setCurrentIndex(0);
            ui->departureTimeEdit->setEnabled(false);
            //ui->departureTimeEdit->clear();
        }
        else
        {
            ui->setOutTrainNoLabel->setEnabled(true);
            ui->departOverdueComboBox->setEnabled(true);
            ui->wayOutCombo->setEnabled(true);
            ui->departureTrackCombo->setEnabled(true);
            ui->departureTimeEdit->setEnabled(true);
        }
    });

    connect(ui->arrivalOverdueComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        ui->departOverdueComboBox->setCurrentIndex(index);
    });

    connect(ui->wayInCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        QString wayIn = ui->wayInCombo->currentText();
        if(wayIn == " ")
        {
            ui->previousStationCombo->setCurrentIndex(0);
        }

        for(int i = 0; i < pStation->StaConfigInfo.JFCKInfo.size(); i++)
        {
            MyStation::JFCK_T jfc = pStation->StaConfigInfo.JFCKInfo.at(i);
            if(wayIn == jfc.strFCKName || wayIn == jfc.strJCKName)
            {
                ui->previousStationCombo->setCurrentText(jfc.strDirectSta);
                break;
            }
        }
    });

    connect(ui->wayOutCombo, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [=](int index){
        QString wayOut = ui->wayOutCombo->currentText();
        if(wayOut == " ")
        {
            ui->nextStationCombo->setCurrentIndex(0);
        }

        for(int i = 0; i < pStation->StaConfigInfo.JFCKInfo.size(); i++)
        {
            MyStation::JFCK_T jfc = pStation->StaConfigInfo.JFCKInfo.at(i);
            if(wayOut == jfc.strFCKName || wayOut == jfc.strJCKName)
            {
                ui->nextStationCombo->setCurrentText(jfc.strDirectSta);
                break;
            }
        }
    });

}

void NewTraffclogWidget::InitTraffclog(TrafficLog* pTrafficLog)
{
    m_pTrafficLog = pTrafficLog;
    ui->trainTypeCombo->setCurrentIndex(pTrafficLog->m_nIndexLCLX);
    ui->operationTypeCombo->setCurrentIndex(pTrafficLog->m_nIndexYXLX);

    if(pTrafficLog->m_btBeginOrEndFlg == JFC_TYPE_ZD)//终到
    {
        if(m_bIsChenge)
        {
            m_strReachTrainNumOld = pTrafficLog->m_strReachTrainNum;
        }
        ui->finalArrivalCheck->setCheckState(Qt::Checked);
        ui->arriveTrainNoLabel->setText(pTrafficLog->m_strReachTrainNum);
        ui->arrivalOverdueComboBox->setCurrentIndex(pTrafficLog->m_nLevelCX);
        //ui->arrivalOverdueComboBox->setCurrentText(pTrafficLog->m_strDDCX);
        ui->wayInCombo->setCurrentText(pTrafficLog->m_strXHD_JZk);
        ui->previousStationCombo->setCurrentText(pTrafficLog->m_strFromAdjtStation);
        ui->receivingTrackCombo->setCurrentText(pTrafficLog->m_strRecvTrainTrack);
        ui->arrivalTimeEdit->setDateTime(pTrafficLog->m_timProvReachStation);
        ui->departOverdueComboBox->setCurrentIndex(0);
        ui->departOverdueComboBox->setEnabled(false);
        ui->wayOutCombo->setEnabled(false);
        ui->departureTrackCombo->setEnabled(false);
        ui->departureTimeEdit->setEnabled(false);
    }
    else if(pTrafficLog->m_btBeginOrEndFlg == JFC_TYPE_SF)//始发
    {
        if(m_bIsChenge)
        {
            m_strDepartTrainNumOld = pTrafficLog->m_strDepartTrainNum;
        }
        ui->originationCheck->setCheckState(Qt::Checked);
        ui->setOutTrainNoLabel->setText(pTrafficLog->m_strDepartTrainNum);
        ui->departOverdueComboBox->setCurrentIndex(pTrafficLog->m_nLevelCX);
        //ui->departOverdueComboBox->setCurrentText(pTrafficLog->m_strCFCX);
        ui->wayOutCombo->setCurrentText(pTrafficLog->m_strXHD_CZk);
        ui->nextStationCombo->setCurrentText(pTrafficLog->m_strToAdjtStation);
        ui->departureTrackCombo->setCurrentText(pTrafficLog->m_strDepartTrainTrack);
        ui->departureTimeEdit->setDateTime(pTrafficLog->m_timProvDepaTrain);

        ui->arrivalOverdueComboBox->setEnabled(false);
        ui->wayInCombo->setEnabled(false);
        ui->receivingTrackCombo->setEnabled(false);
        ui->arrivalTimeEdit->setEnabled(false);
    }
    else
    {
        if(m_bIsChenge)
        {
            m_strReachTrainNumOld = pTrafficLog->m_strReachTrainNum;
            m_strDepartTrainNumOld = pTrafficLog->m_strDepartTrainNum;
        }
        ui->finalArrivalCheck->setCheckState(Qt::Unchecked);
        ui->arriveTrainNoLabel->setText(pTrafficLog->m_strReachTrainNum);
        ui->arrivalOverdueComboBox->setCurrentIndex(pTrafficLog->m_nLevelCX);
        //ui->arrivalOverdueComboBox->setCurrentText(pTrafficLog->m_strDDCX);
        ui->wayInCombo->setCurrentText(pTrafficLog->m_strXHD_JZk);
        ui->previousStationCombo->setCurrentText(pTrafficLog->m_strFromAdjtStation);
        ui->receivingTrackCombo->setCurrentText(pTrafficLog->m_strRecvTrainTrack);
        ui->arrivalTimeEdit->setDateTime(pTrafficLog->m_timProvReachStation);

        ui->originationCheck->setCheckState(Qt::Unchecked);
        ui->setOutTrainNoLabel->setText(pTrafficLog->m_strDepartTrainNum);
        ui->departOverdueComboBox->setCurrentIndex(pTrafficLog->m_nLevelCX);
        //ui->departOverdueComboBox->setCurrentText(pTrafficLog->m_strCFCX);
        ui->wayOutCombo->setCurrentText(pTrafficLog->m_strXHD_CZk);
        ui->nextStationCombo->setCurrentText(pTrafficLog->m_strToAdjtStation);
        ui->departureTrackCombo->setCurrentText(pTrafficLog->m_strDepartTrainTrack);
        ui->departureTimeEdit->setDateTime(pTrafficLog->m_timProvDepaTrain);
    }

    //ui->departOverdueComboBox->setCurrentIndex(pTrafficLog->m_nLevelCX);
    if(pTrafficLog->m_bElectric)
        ui->electricCheck->setCheckState(Qt::Checked);
    if(pTrafficLog->m_bBLKY)
        ui->passengerCheck->setCheckState(Qt::Checked);
    if(pTrafficLog->m_bImportant)
        ui->importantCheck->setCheckState(Qt::Checked);
    if(pTrafficLog->m_bArmy)
        ui->armyCheck->setCheckState(Qt::Checked);
    if(pTrafficLog->m_bAllowGDNotMatch)
        ui->agreeTrackCheck->setCheckState(Qt::Checked);
    if(pTrafficLog->m_bAllowCRKNotMatch)
        ui->agreeWayCheck->setCheckState(Qt::Checked);
}

void NewTraffclogWidget::on_pushButton_2_clicked()
{
    if(ui->originationCheck->checkState() == Qt::Checked && ui->finalArrivalCheck->checkState() == Qt::Checked)
    {
        QMessageBox::warning(this, tr("SPTC"), tr("不能同时选择始发终到！"), tr("确定"), "", 0);
        emit SendTextAnalysisSignal(77, QCursor::pos());
        return;
    }
    emit SendTextAnalysisSignal(77, QCursor::pos());

    TrafficLog* trafficLog = m_pTrafficLog;
    if(!trafficLog)
        trafficLog = new TrafficLog;

    if(ui->arrivalTimeEdit->dateTime() == ui->departureTimeEdit->dateTime())
        trafficLog->m_btBeginOrEndFlg = JFC_TYPE_TG;
    else trafficLog->m_btBeginOrEndFlg = JFC_TYPE_JF;

    if(ui->finalArrivalCheck->checkState() == Qt::Unchecked)//非终到
    {
        if(m_bIsChenge)
        {
            trafficLog->m_strDepartTrainNumOld = m_strDepartTrainNumOld;
        }
        else trafficLog->m_strDepartTrainNumOld = ui->setOutTrainNoLabel->text();
        trafficLog->m_strDepartTrainNum = ui->setOutTrainNoLabel->text();
        trafficLog->m_strCFCX = ui->departOverdueComboBox->currentText();
        trafficLog->m_strXHD_CZk = ui->wayOutCombo->currentText();
        trafficLog->m_strToAdjtStation = ui->nextStationCombo->currentText();
        trafficLog->m_strDepartTrainTrack = ui->departureTrackCombo->currentText();
        trafficLog->m_timProvDepaTrain = ui->departureTimeEdit->dateTime();
    }
    else trafficLog->m_btBeginOrEndFlg = JFC_TYPE_ZD;

    if(ui->originationCheck->checkState() == Qt::Unchecked) //非始发
    {
        if(m_bIsChenge)
        {
            trafficLog->m_strReachTrainNumOld = m_strReachTrainNumOld;
        }
        else trafficLog->m_strReachTrainNumOld = ui->arriveTrainNoLabel->text();
        trafficLog->m_strReachTrainNum = ui->arriveTrainNoLabel->text();
        trafficLog->m_strDDCX = ui->arrivalOverdueComboBox->currentText();
        trafficLog->m_strXHD_JZk = ui->wayInCombo->currentText();
        trafficLog->m_strFromAdjtStation = ui->previousStationCombo->currentText();
        trafficLog->m_strRecvTrainTrack = ui->receivingTrackCombo->currentText();
        trafficLog->m_timProvReachStation = ui->arrivalTimeEdit->dateTime();
    }
    else trafficLog->m_btBeginOrEndFlg = JFC_TYPE_SF;

    //if(trafficLog->m_timProvDepaTrain == ui->arrivalTimeEdit->dateTime())
    //    trafficLog->m_btBeginOrEndFlg = JFC_TYPE_TG;

    for(int i = 0; i < pCurStation->v_TrainNumType.size(); i++)
    {
        if(pCurStation->v_TrainNumType[i]->strType == "货车")
        {
            trafficLog->m_nLHFlg = LCTYPE_HC;
        }
        else trafficLog->m_nLHFlg = LCTYPE_KC;
    }

    trafficLog->m_nIndexLCLX = ui->trainTypeCombo->currentIndex();
    trafficLog->m_nIndexYXLX = ui->operationTypeCombo->currentIndex();
    trafficLog->m_nLevelCX = ui->departOverdueComboBox->currentIndex();

    trafficLog->m_bElectric = (ui->electricCheck->checkState() == Qt::Checked) ? 0x11 : 0x00;
    trafficLog->m_bBLKY = (ui->passengerCheck->checkState() == Qt::Checked);
    trafficLog->m_bImportant = (ui->importantCheck->checkState() == Qt::Checked);
    trafficLog->m_bArmy = (ui->armyCheck->checkState() == Qt::Checked);
    trafficLog->m_bAllowGDNotMatch = (ui->agreeTrackCheck->checkState() == Qt::Checked);
    trafficLog->m_bAllowCRKNotMatch = (ui->agreeWayCheck->checkState() == Qt::Checked);

    trafficLog->m_strTrainNum = trafficLog->m_strReachTrainNum!=""?trafficLog->m_strReachTrainNum:trafficLog->m_strDepartTrainNum;
    trafficLog->m_bNewTrafficLog = true;
    emit addNewTraffclog(*trafficLog);
    if(!m_pTrafficLog)
        delete trafficLog;
    m_pTrafficLog = nullptr;
    this->close();
}

void NewTraffclogWidget::on_pushButton_3_clicked()
{
    m_pTrafficLog = nullptr;
    emit SendTextAnalysisSignal(78, QCursor::pos());
    this->close();
}

void NewTraffclogWidget::on_pushButton_clicked()
{
    emit SendTextAnalysisSignal(181, QCursor::pos());
    if(ui->arriveTrainNoLabel->text() != "")
    {
        ui->setOutTrainNoLabel->setText(ui->arriveTrainNoLabel->text());
    }
}
