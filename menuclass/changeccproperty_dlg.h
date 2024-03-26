#ifndef CHANGECCPROPERTY_DLG_H
#define CHANGECCPROPERTY_DLG_H

#include <QDialog>

namespace Ui {
class ChangeCCProperty_Dlg;
}

class ChangeCCProperty_Dlg : public QDialog
{
    Q_OBJECT

public:
    explicit ChangeCCProperty_Dlg(QWidget *parent = nullptr);
    ~ChangeCCProperty_Dlg();

    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    QString strDescrib;//
    QString strAssObjName;//关联对象名称
    bool CapitalAndLowerCaseLetter;
    bool onOK;
    //初始化
    int nlineeditCheck;//机车速率 机车号编辑框选中状态
    //设置车次号
    void SetCCH(QString str,bool bElectricTraction,QString m_strLocomotiveNumber);

    //设置机车速率
    void SetTrainSpeed(QString str);
    //设置机车号
    void SetTrainNum(QString str);
    //获取机车速率
    QString GetTrainSpeed();
    //获取机车号
    QString GetTrainNum();
    bool GetElectric();
    bool bok;

    //设置关联对象名称
    void SetAssObjName(QString str);
    //获取关联对象名称
    QString GetAssObjName();

    void AddNewAssObjName(char ch);
signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

public slots:
    //设置软键盘输入字符
    void SetInputStr(QString str,QString strboj);

private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_name_38_clicked();

    void on_pushButton_name_37_clicked();

    void on_pushButton_name_1_clicked();

    void on_pushButton_name_2_clicked();

    void on_pushButton_name_3_clicked();

    void on_pushButton_name_4_clicked();

    void on_pushButton_name_5_clicked();

    void on_pushButton_name_6_clicked();

    void on_pushButton_name_7_clicked();

    void on_pushButton_name_8_clicked();

    void on_pushButton_name_9_clicked();

    void on_pushButton_name_10_clicked();

    void on_pushButton_name_Q_clicked();

    void on_pushButton_name_W_clicked();

    void on_pushButton_name_E_clicked();

    void on_pushButton_name_R_clicked();

    void on_pushButton_name_T_clicked();

    void on_pushButton_name_Y_clicked();

    void on_pushButton_name_U_clicked();

    void on_pushButton_name_I_clicked();

    void on_pushButton_name_O_clicked();

    void on_pushButton_name_P_clicked();

    void on_pushButton_name_A_clicked();

    void on_pushButton_name_S_clicked();

    void on_pushButton_name_D_clicked();

    void on_pushButton_name_F_clicked();

    void on_pushButton_name_G_clicked();

    void on_pushButton_name_H_clicked();

    void on_pushButton_name_J_clicked();

    void on_pushButton_name_K_clicked();

    void on_pushButton_name_L_clicked();

    void on_pushButton_name_Z_clicked();

    void on_pushButton_name_X_clicked();

    void on_pushButton_name_C_clicked();

    void on_pushButton_name_V_clicked();

    void on_pushButton_name_B_clicked();

    void on_pushButton_name_N_clicked();

    void on_pushButton_name_M_clicked();

private:
    Ui::ChangeCCProperty_Dlg *ui;
    QPoint z;
};

#endif // CHANGECCPROPERTY_DLG_H

