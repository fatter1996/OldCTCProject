#include "dcroutetimewidget_dlg.h"
#include "ui_dcroutetimewidget_dlg.h"
#pragma execution_character_set("utf-8")

DCRouteTimeWidget_Dlg::DCRouteTimeWidget_Dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DCRouteTimeWidget_Dlg)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
    this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->setWindowTitle(" 调车进路参数");
    onOK = false;
}

DCRouteTimeWidget_Dlg::~DCRouteTimeWidget_Dlg()
{
    delete ui;
}
void DCRouteTimeWidget_Dlg::Init(QString str)
{
    ui->lineEdit->setText("0");
    ui->label->setWordWrap(true);
    ui->label->setFixedWidth(220);
    ui->label->setText(QString("%1调车进路预计占用时间").arg(str));
    ui->radioButton->setChecked(true);
}
QString DCRouteTimeWidget_Dlg::GetTimeAndType()
{
    QString str="";
    if(ui->radioButton->isChecked())
    {
        str="电力";
    }
    else
    {
        str="内燃";
    }
    str=str+ui->lineEdit->text();
    return str;
}

QString DCRouteTimeWidget_Dlg::GetInputTime()
{
    QString str="";
    str=ui->lineEdit->text();
    return str;
}

void DCRouteTimeWidget_Dlg::on_pushButton_clicked()
{
    //emit SendTextAnalysisSignal(77, QCursor::pos());
    this->close();
    onOK = true;
}

void DCRouteTimeWidget_Dlg::on_pushButton_2_clicked()
{
    //emit SendTextAnalysisSignal(78, QCursor::pos());
    this->close();
    onOK = false;
}
