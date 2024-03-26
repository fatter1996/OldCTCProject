#include "searchtrainnowidget.h"
#include "ui_searchtrainnowidget.h"

SearchTrainNoWidget::SearchTrainNoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchTrainNoWidget)
{
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
}

SearchTrainNoWidget::~SearchTrainNoWidget()
{
    delete ui;
}

void SearchTrainNoWidget::on_pushButton_2_clicked()
{
    emit SendTextAnalysisSignal(77, QCursor::pos());
    emit searchTrainNum(ui->lineEdit->text());
}

void SearchTrainNoWidget::on_pushButton_3_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    this->close();
}
