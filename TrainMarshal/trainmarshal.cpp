#include "trainmarshal.h"
#include "ui_trainmarshal.h"
#include <QDateTime>
#pragma execution_character_set("utf-8")
TrainMarshal::TrainMarshal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainMarshal)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    //setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    Init();
}

TrainMarshal::~TrainMarshal()
{
    delete ui;
}
void TrainMarshal::Init()
{
    QDateTime timOrder=QDateTime::currentDateTime();
    ui->comboBox->addItem(timOrder.toString("yyyy年MM月dd日"));
    ui->comboBox_2->addItem("模拟调度中心");
    ui->comboBox_3->addItem("模拟调度中心");
    InitTableView();
    mCTableView->SetTableHeadHideV(false);
    mCTableView->SetTableHeadHideH(true);
    mCTableView->CreatTableView();
    ui->widget_5->layout()->addWidget(mCTableView);
}
void TrainMarshal::InitTableView()
{
    mCTableView= new TableView();
    mCTableView->mTableViewInfo.Init();

    mCTableView->mTableViewInfo.Vect_HeaderData.append("列车车次");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("所属调度台");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(120);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("发送/收到");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("牵引机车");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(100);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("超限");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("司机");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("辆数");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);

    mCTableView->mTableViewInfo.Vect_HeaderData.append("记长");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("总重");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("车长");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);
    mCTableView->mTableViewInfo.Vect_HeaderData.append("附挂机车");
    mCTableView->mTableViewInfo.Vect_HeaderDataWidth.append(80);

//    mCTableView->SetSectionText("序号", 48);
    mCTableView->SetTableHeadHeight(0, 24);
    mCTableView->SetTableHeadHeight(1, 16);
    mCTableView->IintTableColAndRow(0, 11);
    mCTableView->SetTableHeadsetStyleSheet(1,
        "QHeaderView{"
            "background:rgb(240, 240, 240);"
        "}"
        "QHeaderView::section:horizontal {"
            "background:rgb(240, 240, 240);"
            "font-size: 12px;"
            "font-weight: normal;"
        "}");

}
