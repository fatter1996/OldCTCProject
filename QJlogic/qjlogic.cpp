#include "qjlogic.h"
#include "ui_qjlogic.h"

QJlogic::QJlogic(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QJlogic)
{
    ui->setupUi(this);

    mQTabWidget=ui->tabWidget;
    ui->tabWidget->setCurrentIndex(0);
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    //mQTabWidget->widget
    initSoltsFunction();
}

QJlogic::~QJlogic()
{
    delete ui;
}

void QJlogic::Init(MyStation* pStation)
{
    ui->comboBox->addItem(pStation->getStationName());
    ui->comboBox_6->addItem(pStation->getStationName());
    ui->comboBox_8->addItem(pStation->getStationName());
    ui->comboBox_19->addItem(pStation->getStationName());

    //for(int i = 0; i < pStation->DevArray.size(); i++)
    //{
    //    if(pStation->DevArray[i]->getDevType() == Dev_GD)
    //    {
    //        CGD* pGD=(CGD*)(pStation->DevArray[i]);
    //        if(pGD)
    //        {
    //            if(pGD->pCenter.x() < 0 || pGD->pCenter.y() < 0)
    //            {
    //                continue;
    //            }
    //            if(pGD->getGDType() == JJ_QD)
    //            {
    //                ui->comboBox_2->addItem(pGD->getName());
    //            }
    //        }
    //    }
    //}

    for(int i = 0; i < pStation->ZDBSArray.size(); i++)
    {
        ui->comboBox_5->addItem(pStation->ZDBSArray.at(i)->getName());
        ui->comboBox_7->addItem(pStation->ZDBSArray.at(i)->getName());
        ui->comboBox_20->addItem(pStation->ZDBSArray.at(i)->getName());

        QStringList strGDList;
        CXHD* pXHD = pStation->GetXHDByCode(pStation->GetCodeByStrName(pStation->ZDBSArray.at(i)->getName()));
        if(pXHD && (pXHD->getXHDType() == CZ_XHJ || pXHD->getXHDType() == JZ_XHJ))
        {
            for (int i = 0; i < pStation->StaConfigInfo.JCKCount; i++)
            {
                if(pStation->StaConfigInfo.JFCKInfo.at(i).strJCKName == pXHD->m_strName)
                {
                    strGDList << pStation->StaConfigInfo.JFCKInfo.at(i).strArrJckJJQD;
                }
                if(pStation->StaConfigInfo.JFCKInfo.at(i).strFCKName == pXHD->m_strName)
                {
                    strGDList << pStation->StaConfigInfo.JFCKInfo.at(i).strArrFckLQQD;
                }
            }

            for(int j = 0; j < strGDList.size(); j++)
            {
                ui->comboBox_2->addItem(strGDList.at(j));
            }
        }
    }
}

void QJlogic::initSoltsFunction()
{

    connect(ui->tabWidget, &QTabWidget::currentChanged, [=](int index){
        this->setWindowTitle(ui->tabWidget->tabText(index));
    });

    connect(ui->pushButton, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x11, ui->comboBox_2->currentText());
        ui->tabWidget->setEnabled(false);
        nTimerId = startTimer(10 * 1000);
    });
    connect(ui->pushButton_2, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x12, ui->comboBox_2->currentText());
    });

    connect(ui->pushButton_6, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x21, ui->comboBox_5->currentText());
        ui->tabWidget->setEnabled(false);
        nTimerId = startTimer(10 * 1000);
    });
    connect(ui->pushButton_5, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x22, ui->comboBox_5->currentText());
    });

    connect(ui->pushButton_8, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x31, ui->comboBox_7->currentText());
        ui->tabWidget->setEnabled(false);
        nTimerId = startTimer(10 * 1000);
    });
    connect(ui->pushButton_7, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x32, ui->comboBox_7->currentText());
    });

    connect(ui->pushButton_10, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x41, ui->comboBox_20->currentText());
        ui->tabWidget->setEnabled(false);
        nTimerId = startTimer(10 * 1000);
    });
    connect(ui->pushButton_9, &QPushButton::clicked, [=](){
        emit SendLogicCheck(0x42, ui->comboBox_20->currentText());
    });
}

void QJlogic::Clear()
{
    ui->textEdit_8->clear();
    ui->textEdit_9->clear();
    ui->textEdit_10->clear();
    ui->textEdit_11->clear();
}

void QJlogic::onRecvLogicCheck(int type, int result, QString msg)
{
    if(type == 0x11 || type == 0x12)
        ui->textEdit_8->setText(msg);
    if(type == 0x21 || type == 0x22)
        ui->textEdit_9->setText(msg);
    if(type == 0x31 || type == 0x32)
        ui->textEdit_10->setText(msg);
    if(type == 0x41 || type == 0x42)
        ui->textEdit_11->setText(msg);
    ui->tabWidget->setEnabled(true);
    killTimer(nTimerId);
}

void QJlogic::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == nTimerId)
    {
        ui->textEdit_8->setText("超时:未收到自律机的列控反馈信息(闭塞分区无车占用验证)");
        ui->tabWidget->setEnabled(true);
        killTimer(nTimerId);
    }
}
