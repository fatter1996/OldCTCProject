#include "changeccproperty_dlg.h"
#include "ui_changeccproperty_dlg.h"
#include <QMouseEvent>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

ChangeCCProperty_Dlg::ChangeCCProperty_Dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChangeCCProperty_Dlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无标题栏
    setAttribute(Qt::WA_TranslucentBackground,true);//背景透明
    //ui->lineEdit_3->setEnabled(false);
    ui->lineEdit_4->setEnabled(false);
    CapitalAndLowerCaseLetter = false;
    ui->lineEdit_2->setText("");
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, [=]() {
        on_pushButton_3_clicked();
    });
    onOK = false;
}

ChangeCCProperty_Dlg::~ChangeCCProperty_Dlg()
{
    delete ui;
}

//设置车次号
void ChangeCCProperty_Dlg::SetCCH(QString str,bool bElectricTraction,QString m_strLocomotiveNumber)
{
    ui->lineEdit_4->setText(str);
    ui->checkBox->setChecked(bElectricTraction);
    ui->lineEdit_3->setText(m_strLocomotiveNumber);
}

//设置机车速率
void ChangeCCProperty_Dlg::SetTrainSpeed(QString str)
{
    QString text=GetTrainSpeed();
    if(str=="<")
    {
        ui->lineEdit_3->setText(text.left(text.length()-1));
    }
    else
        ui->lineEdit_3->setText(text+str);
}
//设置机车号
void ChangeCCProperty_Dlg::SetTrainNum(QString str)
{
    QString text=GetTrainNum();
    if(str=="<")
    {
        ui->lineEdit_2->setText(text.left(text.length()-1));
    }
    else
        ui->lineEdit_2->setText(text+str);
}
//获取机车速率
QString ChangeCCProperty_Dlg::GetTrainSpeed()
{
    if(ui->lineEdit_3->text()=="0")
    {
        return "";
    }
    else
    {
        return ui->lineEdit_3->text();
    }
}
//获取机车号
QString ChangeCCProperty_Dlg::GetTrainNum()
{
   return ui->lineEdit_2->text();
}
//设置软键盘输入字符
void ChangeCCProperty_Dlg::SetInputStr(QString str,QString strboj)
{
    if(strboj==objectName())
    {
        if(nlineeditCheck==0)//设置机车速率
        {
            SetTrainSpeed(str);
        }
        else if(nlineeditCheck==1)//设置机车号
        {
            SetTrainNum(str);
        }
    }

}
bool ChangeCCProperty_Dlg::GetElectric()
{
    return ui->checkBox->isChecked();
}



void ChangeCCProperty_Dlg::on_pushButton_4_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    close();
    strAssObjName="";
    ui->lineEdit_2->setText("");
    onOK = false;
}

void ChangeCCProperty_Dlg::on_pushButton_3_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    emit SendTextAnalysisSignal(77, QCursor::pos());
    onOK = true;
    close();
}
void ChangeCCProperty_Dlg::mouseMoveEvent(QMouseEvent *event)//移动工具条窗口//QT5实战项目视频
{
//    QMainWindow::mouseMoveEvent(event);
    QWidget::mouseMoveEvent(event);
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    QPoint x=y-this->z;
    this->move(x);
}

void ChangeCCProperty_Dlg::mousePressEvent(QMouseEvent *event)
{
//    QMainWindow::mousePressEvent(event);
    QWidget::mousePressEvent(event);
    QPoint x=this->geometry().topLeft();//窗口左上角相对于桌面左上角的位置，窗口位置
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    this->z=y-x;//定值//z是鼠标相对于窗口的位置
}

void ChangeCCProperty_Dlg::mouseReleaseEvent(QMouseEvent *event)
{
//    QMainWindow::mouseReleaseEvent(event);
    QWidget::mouseReleaseEvent(event);
    this->z=QPoint();
}
/***************************************************************/

void ChangeCCProperty_Dlg::on_pushButton_name_38_clicked()
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

void ChangeCCProperty_Dlg::on_pushButton_name_37_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    if(strAssObjName != "")
    {
        strAssObjName = strAssObjName.left(strAssObjName.length()-1);
        ui->lineEdit_2->setText(strAssObjName);
    }
}
void ChangeCCProperty_Dlg::SetAssObjName(QString str)
{
    strAssObjName=str;
}
//获取关联对象名称
QString ChangeCCProperty_Dlg::GetAssObjName()
{
    return strAssObjName;
}

void ChangeCCProperty_Dlg::AddNewAssObjName(char ch)
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

void ChangeCCProperty_Dlg::on_pushButton_name_1_clicked()
{
    AddNewAssObjName('1');
}

void ChangeCCProperty_Dlg::on_pushButton_name_2_clicked()
{
    AddNewAssObjName('2');
}

void ChangeCCProperty_Dlg::on_pushButton_name_3_clicked()
{
    AddNewAssObjName('3');
}
void ChangeCCProperty_Dlg::on_pushButton_name_4_clicked()
{
    AddNewAssObjName('4');
}

void ChangeCCProperty_Dlg::on_pushButton_name_5_clicked()
{
    AddNewAssObjName('5');
}

void ChangeCCProperty_Dlg::on_pushButton_name_6_clicked()
{
    AddNewAssObjName('6');
}

void ChangeCCProperty_Dlg::on_pushButton_name_7_clicked()
{
    AddNewAssObjName('7');
}

void ChangeCCProperty_Dlg::on_pushButton_name_8_clicked()
{
    AddNewAssObjName('8');
}

void ChangeCCProperty_Dlg::on_pushButton_name_9_clicked()
{
    AddNewAssObjName('9');
}

void ChangeCCProperty_Dlg::on_pushButton_name_10_clicked()
{
    AddNewAssObjName('0');
}

void ChangeCCProperty_Dlg::on_pushButton_name_Q_clicked()
{
    AddNewAssObjName('Q');
}

void ChangeCCProperty_Dlg::on_pushButton_name_W_clicked()
{
    AddNewAssObjName('W');
}

void ChangeCCProperty_Dlg::on_pushButton_name_E_clicked()
{
    AddNewAssObjName('E');
}

void ChangeCCProperty_Dlg::on_pushButton_name_R_clicked()
{
    AddNewAssObjName('R');
}

void ChangeCCProperty_Dlg::on_pushButton_name_T_clicked()
{
    AddNewAssObjName('T');
}

void ChangeCCProperty_Dlg::on_pushButton_name_Y_clicked()
{
    AddNewAssObjName('Y');
}

void ChangeCCProperty_Dlg::on_pushButton_name_U_clicked()
{
    AddNewAssObjName('U');
}

void ChangeCCProperty_Dlg::on_pushButton_name_I_clicked()
{
    AddNewAssObjName('I');
}

void ChangeCCProperty_Dlg::on_pushButton_name_O_clicked()
{
    AddNewAssObjName('O');
}

void ChangeCCProperty_Dlg::on_pushButton_name_P_clicked()
{
    AddNewAssObjName('P');
}

void ChangeCCProperty_Dlg::on_pushButton_name_A_clicked()
{
    AddNewAssObjName('A');
}

void ChangeCCProperty_Dlg::on_pushButton_name_S_clicked()
{
    AddNewAssObjName('S');
}

void ChangeCCProperty_Dlg::on_pushButton_name_D_clicked()
{
    AddNewAssObjName('D');
}

void ChangeCCProperty_Dlg::on_pushButton_name_F_clicked()
{
    AddNewAssObjName('F');
}

void ChangeCCProperty_Dlg::on_pushButton_name_G_clicked()
{
    AddNewAssObjName('G');
}

void ChangeCCProperty_Dlg::on_pushButton_name_H_clicked()
{
    AddNewAssObjName('H');
}

void ChangeCCProperty_Dlg::on_pushButton_name_J_clicked()
{
    AddNewAssObjName('J');
}

void ChangeCCProperty_Dlg::on_pushButton_name_K_clicked()
{
    AddNewAssObjName('K');
}

void ChangeCCProperty_Dlg::on_pushButton_name_L_clicked()
{
    AddNewAssObjName('L');
}

void ChangeCCProperty_Dlg::on_pushButton_name_Z_clicked()
{
    AddNewAssObjName('Z');
}

void ChangeCCProperty_Dlg::on_pushButton_name_X_clicked()
{
    AddNewAssObjName('X');
}

void ChangeCCProperty_Dlg::on_pushButton_name_C_clicked()
{
    AddNewAssObjName('C');
}

void ChangeCCProperty_Dlg::on_pushButton_name_V_clicked()
{
    AddNewAssObjName('V');
}

void ChangeCCProperty_Dlg::on_pushButton_name_B_clicked()
{
    AddNewAssObjName('B');
}

void ChangeCCProperty_Dlg::on_pushButton_name_N_clicked()
{
    AddNewAssObjName('N');
}

void ChangeCCProperty_Dlg::on_pushButton_name_M_clicked()
{
    AddNewAssObjName('M');
}
