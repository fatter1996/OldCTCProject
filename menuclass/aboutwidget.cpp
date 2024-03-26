#include "aboutwidget.h"
#include "ui_aboutwidget.h"

AboutWidget::AboutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutWidget)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
    //隐藏最大化、最小化
    this->setWindowFlags(Qt::WindowCloseButtonHint /*| Qt::WindowMinMaxButtonsHint*/);
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        emit SendTextAnalysisSignal(77, QCursor::pos());
        close();
    });
}

AboutWidget::~AboutWidget()
{
    delete ui;
}
