#include "option.h"
#include "ui_option.h"

option::option(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::option)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    connect(ui->checkBox,&QCheckBox::stateChanged,this,[=](){
         emit SendTextAnalysisSignal(203, QCursor::pos());
         emit AutoSignState(ui->checkBox->isChecked());
    });

}

option::~option()
{
    delete ui;
}
void option::Init(int ntype, bool bAutoSignStage)
{
    if(ntype==0)
    {
        ui->checkBox->setVisible(true);
        ui->checkBox->setChecked(bAutoSignStage);
    }
    /*如果没有配置自动签收阶段计划功能（仅人工签收），checkBox不显示*/
    else if(ntype==1)
    {
        ui->checkBox->setVisible(false);
    }
}
