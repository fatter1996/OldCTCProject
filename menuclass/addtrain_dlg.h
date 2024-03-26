#ifndef ADDTRAIN_DLG_H
#define ADDTRAIN_DLG_H

#include <QDialog>
#include "GlobalHeaders/GlobalStructure.h"
namespace Ui {
class AddTrain_Dlg;
}

class AddTrain_Dlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddTrain_Dlg(QWidget *parent = nullptr);
    ~AddTrain_Dlg();


    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

    QString strDescrib;//
    QString strAssObjName;//关联对象名称
    bool CapitalAndLowerCaseLetter;
    bool onOK;
    //初始化
    void Init(QString strSta,QString strGD,bool btitle);
    //获取车次号
    QString GetCCH();
    //获取电力机车状态
    bool GetElectric();
    bool GetSimTrain();
    bool bok;
    //设置关联对象名称
    void SetAssObjName(QString str);
    //获取关联对象名称
    QString GetAssObjName();

    void AddNewAssObjName(char ch);
signals:
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
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
    Ui::AddTrain_Dlg *ui;
    QPoint z;
};

#endif // ADDTRAIN_DLG_H


