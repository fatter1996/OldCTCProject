#include "confirmcch_dlg.h"
#include "ui_confirmcch_dlg.h"
#include <QMouseEvent>
#include <QMessageBox>

ConfirmCCH_Dlg::ConfirmCCH_Dlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfirmCCH_Dlg)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无标题栏
    setAttribute(Qt::WA_TranslucentBackground,true);//背景透明
    CapitalAndLowerCaseLetter = false;
    ui->lineEdit_3->setEnabled(false);
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, [=]() {
        on_pushButton_3_clicked();
    });
    onOK = false;
}

ConfirmCCH_Dlg::~ConfirmCCH_Dlg()
{
    delete ui;
}

void ConfirmCCH_Dlg::Init(QString type,QString strTitel,QString strOld)
{
    SetTitel(strTitel);
    SetOldCCH(strOld);
    ui->lineEdit_2->setText(strOld);
    if(type.contains("车次号确认"))
    {
     // ui->lineEdit_2->setEnabled(false);
     // ui->lineEdit_2->setReadOnly(false);
      ntype=1;
    }
    else
    {
      ntype=0;
    }
}
//设置标题
void ConfirmCCH_Dlg::SetTitel(QString strTitel)
{
    ui->PassWordTittle->setText(strTitel);
}
//设置旧车次号
void ConfirmCCH_Dlg::SetOldCCH(QString strOld)
{
    ui->lineEdit_3->setText(strOld);
}
////设置新车次号
//void ConfirmCCH_Dlg::SetNewCCH(QString strNew,QString strboj)
//{
//    if(strboj==objectName()/*&& ntype!=1*/)
//    {
//        QString text=GetNewCCH();
//        if(strNew=="<")
//        {
//            ui->lineEdit_2->setText(text.left(text.length()-1));
//        }
//        else
//            ui->lineEdit_2->setText(text+strNew);
//    }

//}
//获取旧车次号
QString ConfirmCCH_Dlg::GetOldCCH()
{
    return ui->lineEdit_3->text();
}
//获取新车次号
QString ConfirmCCH_Dlg::GetNewCCH()
{
    return strAssObjName;
}

void ConfirmCCH_Dlg::on_pushButton_4_clicked()
{
    emit SendTextAnalysisSignal(78, QCursor::pos());
    close();
    strAssObjName="";
    ui->lineEdit_2->setText("");
    onOK = false;
}

void ConfirmCCH_Dlg::on_pushButton_3_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    emit SendTextAnalysisSignal(77, QCursor::pos());
    onOK = true;
    close();
}
void ConfirmCCH_Dlg::mouseMoveEvent(QMouseEvent *event)//移动工具条窗口//QT5实战项目视频
{
//    QMainWindow::mouseMoveEvent(event);
    QWidget::mouseMoveEvent(event);
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    QPoint x=y-this->z;
    this->move(x);
}

void ConfirmCCH_Dlg::mousePressEvent(QMouseEvent *event)
{
//    QMainWindow::mousePressEvent(event);
    QWidget::mousePressEvent(event);
    QPoint x=this->geometry().topLeft();//窗口左上角相对于桌面左上角的位置，窗口位置
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    this->z=y-x;//定值//z是鼠标相对于窗口的位置
}

void ConfirmCCH_Dlg::mouseReleaseEvent(QMouseEvent *event)
{
//    QMainWindow::mouseReleaseEvent(event);
    QWidget::mouseReleaseEvent(event);
    this->z=QPoint();
}
/***************************************************************/

void ConfirmCCH_Dlg::on_pushButton_name_38_clicked()
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

void ConfirmCCH_Dlg::on_pushButton_name_37_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    if(strAssObjName != "")
    {
        strAssObjName = strAssObjName.left(strAssObjName.length()-1);
        ui->lineEdit_2->setText(strAssObjName);
    }
}

void ConfirmCCH_Dlg::AddNewAssObjName(char ch)
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

void ConfirmCCH_Dlg::on_pushButton_name_1_clicked()
{
    AddNewAssObjName('1');
}

void ConfirmCCH_Dlg::on_pushButton_name_2_clicked()
{
    AddNewAssObjName('2');
}

void ConfirmCCH_Dlg::on_pushButton_name_3_clicked()
{
    AddNewAssObjName('3');
}
void ConfirmCCH_Dlg::on_pushButton_name_4_clicked()
{
    AddNewAssObjName('4');
}

void ConfirmCCH_Dlg::on_pushButton_name_5_clicked()
{
    AddNewAssObjName('5');
}

void ConfirmCCH_Dlg::on_pushButton_name_6_clicked()
{
    AddNewAssObjName('6');
}

void ConfirmCCH_Dlg::on_pushButton_name_7_clicked()
{
    AddNewAssObjName('7');
}

void ConfirmCCH_Dlg::on_pushButton_name_8_clicked()
{
    AddNewAssObjName('8');
}

void ConfirmCCH_Dlg::on_pushButton_name_9_clicked()
{
    AddNewAssObjName('9');
}

void ConfirmCCH_Dlg::on_pushButton_name_10_clicked()
{
    AddNewAssObjName('0');
}

void ConfirmCCH_Dlg::on_pushButton_name_Q_clicked()
{
    AddNewAssObjName('Q');
}

void ConfirmCCH_Dlg::on_pushButton_name_W_clicked()
{
    AddNewAssObjName('W');
}

void ConfirmCCH_Dlg::on_pushButton_name_E_clicked()
{
    AddNewAssObjName('E');
}

void ConfirmCCH_Dlg::on_pushButton_name_R_clicked()
{
    AddNewAssObjName('R');
}

void ConfirmCCH_Dlg::on_pushButton_name_T_clicked()
{
    AddNewAssObjName('T');
}

void ConfirmCCH_Dlg::on_pushButton_name_Y_clicked()
{
    AddNewAssObjName('Y');
}

void ConfirmCCH_Dlg::on_pushButton_name_U_clicked()
{
    AddNewAssObjName('U');
}

void ConfirmCCH_Dlg::on_pushButton_name_I_clicked()
{
    AddNewAssObjName('I');
}

void ConfirmCCH_Dlg::on_pushButton_name_O_clicked()
{
    AddNewAssObjName('O');
}

void ConfirmCCH_Dlg::on_pushButton_name_P_clicked()
{
    AddNewAssObjName('P');
}

void ConfirmCCH_Dlg::on_pushButton_name_A_clicked()
{
    AddNewAssObjName('A');
}

void ConfirmCCH_Dlg::on_pushButton_name_S_clicked()
{
    AddNewAssObjName('S');
}

void ConfirmCCH_Dlg::on_pushButton_name_D_clicked()
{
    AddNewAssObjName('D');
}

void ConfirmCCH_Dlg::on_pushButton_name_F_clicked()
{
    AddNewAssObjName('F');
}

void ConfirmCCH_Dlg::on_pushButton_name_G_clicked()
{
    AddNewAssObjName('G');
}

void ConfirmCCH_Dlg::on_pushButton_name_H_clicked()
{
    AddNewAssObjName('H');
}

void ConfirmCCH_Dlg::on_pushButton_name_J_clicked()
{
    AddNewAssObjName('J');
}

void ConfirmCCH_Dlg::on_pushButton_name_K_clicked()
{
    AddNewAssObjName('K');
}

void ConfirmCCH_Dlg::on_pushButton_name_L_clicked()
{
    AddNewAssObjName('L');
}

void ConfirmCCH_Dlg::on_pushButton_name_Z_clicked()
{
    AddNewAssObjName('Z');
}

void ConfirmCCH_Dlg::on_pushButton_name_X_clicked()
{
    AddNewAssObjName('X');
}

void ConfirmCCH_Dlg::on_pushButton_name_C_clicked()
{
    AddNewAssObjName('C');
}

void ConfirmCCH_Dlg::on_pushButton_name_V_clicked()
{
    AddNewAssObjName('V');
}

void ConfirmCCH_Dlg::on_pushButton_name_B_clicked()
{
    AddNewAssObjName('B');
}

void ConfirmCCH_Dlg::on_pushButton_name_N_clicked()
{
    AddNewAssObjName('N');
}

void ConfirmCCH_Dlg::on_pushButton_name_M_clicked()
{
    AddNewAssObjName('M');
}
