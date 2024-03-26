#include "addtrain_dlg.h"
#include "ui_addtrain_dlg.h"
#include <QMouseEvent>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

AddTrain_Dlg::AddTrain_Dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTrain_Dlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无标题栏
    setAttribute(Qt::WA_TranslucentBackground,true);//背景透明
    ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
//    ui->lineEdit_3->setText("");
//    ui->lineEdit_4->setText("");
//    strDescrib="";
//    strAssObjName="";
    CapitalAndLowerCaseLetter = false;
    ui->lineEdit_2->setText("");
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, [=]() {
        on_pushButton_3_clicked();
    });
    onOK = false;
}

AddTrain_Dlg::~AddTrain_Dlg()
{
    delete ui;
}

//初始化车站 股道编辑框
void AddTrain_Dlg::Init(QString strSta,QString strGD,bool btitle)
{
    // ui->label_3
    if(btitle)
    {
       ui->PassWordTittle->setText("  区间加车");
    }
    else
    {
       ui->PassWordTittle->setText("  站内加车");
    }
    ui->lineEdit_3->setText(strSta);
    ui->lineEdit_4->setText(strGD);
}
//获取车次号
QString AddTrain_Dlg::GetCCH()
{
    return strAssObjName;
}
bool AddTrain_Dlg::GetElectric()
{
    return ui->checkBox->isChecked();
}
bool AddTrain_Dlg::GetSimTrain()
{
    bool b=ui->checkBox_2->isChecked();
    return !b;
}
void AddTrain_Dlg::on_pushButton_4_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    close();
    strAssObjName="";
    ui->lineEdit_2->setText("");
    onOK = false;
}

void AddTrain_Dlg::on_pushButton_3_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    emit SendTextAnalysisSignal(77, QCursor::pos());
    onOK = true;
    close();
}
void AddTrain_Dlg::mouseMoveEvent(QMouseEvent *event)//移动工具条窗口//QT5实战项目视频
{
//    QMainWindow::mouseMoveEvent(event);
    QWidget::mouseMoveEvent(event);
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    QPoint x=y-this->z;
    this->move(x);
}

void AddTrain_Dlg::mousePressEvent(QMouseEvent *event)
{
//    QMainWindow::mousePressEvent(event);
    QWidget::mousePressEvent(event);
    QPoint x=this->geometry().topLeft();//窗口左上角相对于桌面左上角的位置，窗口位置
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    this->z=y-x;//定值//z是鼠标相对于窗口的位置
}

void AddTrain_Dlg::mouseReleaseEvent(QMouseEvent *event)
{
//    QMainWindow::mouseReleaseEvent(event);
    QWidget::mouseReleaseEvent(event);
    this->z=QPoint();
}
/***************************************************************/

void AddTrain_Dlg::on_pushButton_name_38_clicked()
{
    //键盘英文变为大写
    QList<QAbstractButton *> buttonList = ui->buttonGroup->buttons();
    QString text;
    for(int i = 0; i < buttonList.size(); ++i)
    {
        text=buttonList[i]->text();
        if (text >= 'a' && text <= 'z')
        {
            text = text.toUpper();
            buttonList[i]->setText(text);
            CapitalAndLowerCaseLetter = false;
        }
        else if(text >= 'A' && text <= 'Z')
        {
            text = text.toLower();
              buttonList[i]->setText(text);
              CapitalAndLowerCaseLetter = true;
        }
    }
}

void AddTrain_Dlg::on_pushButton_name_37_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    if(strAssObjName != "")
    {
        strAssObjName = strAssObjName.left(strAssObjName.length()-1);
        ui->lineEdit_2->setText(strAssObjName);
        emit SendTextAnalysisSignal(87, QCursor::pos());
    }
}
void AddTrain_Dlg::SetAssObjName(QString str)
{
    strAssObjName=str;
}
//获取关联对象名称
QString AddTrain_Dlg::GetAssObjName()
{
    return strAssObjName;
}

void AddTrain_Dlg::AddNewAssObjName(char ch)
{
    strAssObjName = ui->lineEdit_2->text();
    char tempCh = ch;
    if(ch >= 'A' && ch <= 'Z')
    {
        if(CapitalAndLowerCaseLetter)
        {
            tempCh = ch + 32;
        }
    }

    strAssObjName += tempCh;
    ui->lineEdit_2->setText(strAssObjName);
    ui->lineEdit_2->setFocus();
    emit SendTextAnalysisSignal(76, QCursor::pos(), QString(tempCh));
}

void AddTrain_Dlg::on_pushButton_name_1_clicked()
{
    AddNewAssObjName('1');
}

void AddTrain_Dlg::on_pushButton_name_2_clicked()
{
    AddNewAssObjName('2');
}

void AddTrain_Dlg::on_pushButton_name_3_clicked()
{
    AddNewAssObjName('3');
}
void AddTrain_Dlg::on_pushButton_name_4_clicked()
{
    AddNewAssObjName('4');
}

void AddTrain_Dlg::on_pushButton_name_5_clicked()
{
    AddNewAssObjName('5');
}

void AddTrain_Dlg::on_pushButton_name_6_clicked()
{
    AddNewAssObjName('6');
}

void AddTrain_Dlg::on_pushButton_name_7_clicked()
{
    AddNewAssObjName('7');
}

void AddTrain_Dlg::on_pushButton_name_8_clicked()
{
    AddNewAssObjName('8');
}

void AddTrain_Dlg::on_pushButton_name_9_clicked()
{
    AddNewAssObjName('9');
}

void AddTrain_Dlg::on_pushButton_name_10_clicked()
{
    AddNewAssObjName('0');
}

void AddTrain_Dlg::on_pushButton_name_Q_clicked()
{
    AddNewAssObjName('Q');
}

void AddTrain_Dlg::on_pushButton_name_W_clicked()
{
    AddNewAssObjName('W');
}

void AddTrain_Dlg::on_pushButton_name_E_clicked()
{
    AddNewAssObjName('E');
}

void AddTrain_Dlg::on_pushButton_name_R_clicked()
{
    AddNewAssObjName('R');
}

void AddTrain_Dlg::on_pushButton_name_T_clicked()
{
    AddNewAssObjName('T');
}

void AddTrain_Dlg::on_pushButton_name_Y_clicked()
{
    AddNewAssObjName('Y');
}

void AddTrain_Dlg::on_pushButton_name_U_clicked()
{
    AddNewAssObjName('U');
}

void AddTrain_Dlg::on_pushButton_name_I_clicked()
{
    AddNewAssObjName('I');
}

void AddTrain_Dlg::on_pushButton_name_O_clicked()
{
    AddNewAssObjName('O');
}

void AddTrain_Dlg::on_pushButton_name_P_clicked()
{
    AddNewAssObjName('P');
}

void AddTrain_Dlg::on_pushButton_name_A_clicked()
{
    AddNewAssObjName('A');
}

void AddTrain_Dlg::on_pushButton_name_S_clicked()
{
    AddNewAssObjName('S');
}

void AddTrain_Dlg::on_pushButton_name_D_clicked()
{
    AddNewAssObjName('D');
}

void AddTrain_Dlg::on_pushButton_name_F_clicked()
{
    AddNewAssObjName('F');
}

void AddTrain_Dlg::on_pushButton_name_G_clicked()
{
    AddNewAssObjName('G');
}

void AddTrain_Dlg::on_pushButton_name_H_clicked()
{
    AddNewAssObjName('H');
}

void AddTrain_Dlg::on_pushButton_name_J_clicked()
{
    AddNewAssObjName('J');
}

void AddTrain_Dlg::on_pushButton_name_K_clicked()
{
    AddNewAssObjName('K');
}

void AddTrain_Dlg::on_pushButton_name_L_clicked()
{
    AddNewAssObjName('L');
}

void AddTrain_Dlg::on_pushButton_name_Z_clicked()
{
    AddNewAssObjName('Z');
}

void AddTrain_Dlg::on_pushButton_name_X_clicked()
{
    AddNewAssObjName('X');
}

void AddTrain_Dlg::on_pushButton_name_C_clicked()
{
    AddNewAssObjName('C');
}

void AddTrain_Dlg::on_pushButton_name_V_clicked()
{
    AddNewAssObjName('V');
}

void AddTrain_Dlg::on_pushButton_name_B_clicked()
{
    AddNewAssObjName('B');
}

void AddTrain_Dlg::on_pushButton_name_N_clicked()
{
    AddNewAssObjName('N');
}

void AddTrain_Dlg::on_pushButton_name_M_clicked()
{
    AddNewAssObjName('M');
}
