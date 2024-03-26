#include "dialog_password.h"
#include "ui_dialog_password.h"
#include <QMouseEvent>
#include <QMessageBox>

#pragma execution_character_set("utf-8")

Dialog_PassWord::Dialog_PassWord(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_PassWord)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);//无标题栏
    setAttribute(Qt::WA_TranslucentBackground,true);//背景透明
    strDescrib="";
    strAssObjName="";
    CapitalAndLowerCaseLetter = true;
    ui->lineEdit_2->setText("");
    ui->lineEdit_2->setEchoMode(QLineEdit::EchoMode::Password);//PasswordEchoOnEdit
    connect(ui->lineEdit_2, &QLineEdit::returnPressed, [=]() {
        on_pushButton_3_clicked();
    });
    onOK = false;
}

Dialog_PassWord::~Dialog_PassWord()
{
    delete ui;
}

void Dialog_PassWord::InitPassWord()
{
    ui->label_3->setWordWrap(true);
    ui->label_3->setFixedWidth(220);
    ui->label_3->setText(strDescrib);
    ui->PassWordTittle->setText(strTittle);
    //if(isPassWord)
    setFocus(Qt::FocusReason::PopupFocusReason);
    setFocusProxy(ui->lineEdit_2);
    onOK = false;
    strDescrib="";
    strAssObjName="";
    ui->lineEdit_2->setText("");
}

void Dialog_PassWord::on_pushButton_4_clicked()
{
    strAssObjName="";
    ui->lineEdit_2->setText("");
    onOK = false;
    emit SendTextAnalysisSignal(78, QCursor::pos());
    close();
}

void Dialog_PassWord::on_pushButton_3_clicked()
{
    if(ui->lineEdit_2->text() != "123")
    {
         QMessageBox::information(nullptr,"错误","密码输入错误!",tr("确定"),tr("取消"),"",0);
    }
    else
    {
        onOK = true;
        strAssObjName = ui->lineEdit_2->text();
        emit SendTextAnalysisSignal(77, QCursor::pos());
        close();
    }
}

void Dialog_PassWord::mouseMoveEvent(QMouseEvent *event)//移动工具条窗口//QT5实战项目视频
{
//    QMainWindow::mouseMoveEvent(event);
    QWidget::mouseMoveEvent(event);
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    QPoint x=y-this->z;
    this->move(x);
}

void Dialog_PassWord::mousePressEvent(QMouseEvent *event)
{
//    QMainWindow::mousePressEvent(event);
    QWidget::mousePressEvent(event);
    QPoint x=this->geometry().topLeft();//窗口左上角相对于桌面左上角的位置，窗口位置
    QPoint y=event->globalPos();//鼠标相对于桌面左上角的位置，鼠标的全局位置
    this->z=y-x;//定值//z是鼠标相对于窗口的位置
}

void Dialog_PassWord::mouseReleaseEvent(QMouseEvent *event)
{
//    QMainWindow::mouseReleaseEvent(event);
    QWidget::mouseReleaseEvent(event);
    this->z=QPoint();
}

/***************************************************************/

void Dialog_PassWord::on_pushButton_name_38_clicked()
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

void Dialog_PassWord::on_pushButton_name_37_clicked()
{
    strAssObjName = ui->lineEdit_2->text();
    if(strAssObjName != "")
    {
        strAssObjName = strAssObjName.left(strAssObjName.length()-1);
        ui->lineEdit_2->setText(strAssObjName);
        emit SendTextAnalysisSignal(87, QCursor::pos());
    }
}
void Dialog_PassWord::SetAssObjName(QString str)
{
    strAssObjName=str;
}
//获取关联对象名称
QString Dialog_PassWord::GetAssObjName()
{
    return strAssObjName;
}

void Dialog_PassWord::AddNewAssObjName(char ch)
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

void Dialog_PassWord::on_pushButton_name_1_clicked()
{
    AddNewAssObjName('1');
}

void Dialog_PassWord::on_pushButton_name_2_clicked()
{
    AddNewAssObjName('2');
}

void Dialog_PassWord::on_pushButton_name_3_clicked()
{
    AddNewAssObjName('3');
}
void Dialog_PassWord::on_pushButton_name_4_clicked()
{
    AddNewAssObjName('4');
}

void Dialog_PassWord::on_pushButton_name_5_clicked()
{
    AddNewAssObjName('5');
}

void Dialog_PassWord::on_pushButton_name_6_clicked()
{
    AddNewAssObjName('6');
}

void Dialog_PassWord::on_pushButton_name_7_clicked()
{
    AddNewAssObjName('7');
}

void Dialog_PassWord::on_pushButton_name_8_clicked()
{
    AddNewAssObjName('8');
}

void Dialog_PassWord::on_pushButton_name_9_clicked()
{
    AddNewAssObjName('9');
}

void Dialog_PassWord::on_pushButton_name_10_clicked()
{
    AddNewAssObjName('0');
}

void Dialog_PassWord::on_pushButton_name_Q_clicked()
{
    AddNewAssObjName('Q');
}

void Dialog_PassWord::on_pushButton_name_W_clicked()
{
    AddNewAssObjName('W');
}

void Dialog_PassWord::on_pushButton_name_E_clicked()
{
    AddNewAssObjName('E');
}

void Dialog_PassWord::on_pushButton_name_R_clicked()
{
    AddNewAssObjName('R');
}

void Dialog_PassWord::on_pushButton_name_T_clicked()
{
    AddNewAssObjName('T');
}

void Dialog_PassWord::on_pushButton_name_Y_clicked()
{
    AddNewAssObjName('Y');
}

void Dialog_PassWord::on_pushButton_name_U_clicked()
{
    AddNewAssObjName('U');
}

void Dialog_PassWord::on_pushButton_name_I_clicked()
{
    AddNewAssObjName('I');
}

void Dialog_PassWord::on_pushButton_name_O_clicked()
{
    AddNewAssObjName('O');
}

void Dialog_PassWord::on_pushButton_name_P_clicked()
{
    AddNewAssObjName('P');
}

void Dialog_PassWord::on_pushButton_name_A_clicked()
{
    AddNewAssObjName('A');
}

void Dialog_PassWord::on_pushButton_name_S_clicked()
{
    AddNewAssObjName('S');
}

void Dialog_PassWord::on_pushButton_name_D_clicked()
{
    AddNewAssObjName('D');
}

void Dialog_PassWord::on_pushButton_name_F_clicked()
{
    AddNewAssObjName('F');
}

void Dialog_PassWord::on_pushButton_name_G_clicked()
{
    AddNewAssObjName('G');
}

void Dialog_PassWord::on_pushButton_name_H_clicked()
{
    AddNewAssObjName('H');
}

void Dialog_PassWord::on_pushButton_name_J_clicked()
{
    AddNewAssObjName('J');
}

void Dialog_PassWord::on_pushButton_name_K_clicked()
{
    AddNewAssObjName('K');
}

void Dialog_PassWord::on_pushButton_name_L_clicked()
{
    AddNewAssObjName('L');
}

void Dialog_PassWord::on_pushButton_name_Z_clicked()
{
    AddNewAssObjName('Z');
}

void Dialog_PassWord::on_pushButton_name_X_clicked()
{
    AddNewAssObjName('X');
}

void Dialog_PassWord::on_pushButton_name_C_clicked()
{
    AddNewAssObjName('C');
}

void Dialog_PassWord::on_pushButton_name_V_clicked()
{
    AddNewAssObjName('V');
}

void Dialog_PassWord::on_pushButton_name_B_clicked()
{
    AddNewAssObjName('B');
}

void Dialog_PassWord::on_pushButton_name_N_clicked()
{
    AddNewAssObjName('N');
}

void Dialog_PassWord::on_pushButton_name_M_clicked()
{
    AddNewAssObjName('M');
}
