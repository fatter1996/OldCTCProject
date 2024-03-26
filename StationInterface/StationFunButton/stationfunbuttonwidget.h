#ifndef STATIONFUNBUTTONWIDGET_H
#define STATIONFUNBUTTONWIDGET_H

#include <QWidget>
#include<QPushButton>
#include<QVector>
#include<QButtonGroup>
#include<QAction>
#include<QMenu>
#include "BaseDataPro/station.h"
#include "menuclass/LeadSealing/dialog_password.h"
#include "menuclass/Dlg_FZList.h"

//底部功能按钮窗口
namespace Ui {
class StationFunButtonWidget;
}
#pragma execution_character_set("utf-8")

class StationFunButtonWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StationFunButtonWidget(QWidget *parent = nullptr);
    ~StationFunButtonWidget();
    //系统窗口类型-1卡斯柯，2卡斯柯集控，3铁科，4通号...
    int wndType;
    QButtonGroup *pFunButtonGroup;
    int nFunButtonPressState;//功能按钮按下标志
    QString strButtonName;//功能按钮名称
    QMenu *menuAuxiliary;//辅助菜单
    QMenu *menuModeChange;//方式转换
    Dialog_PassWord *dPassWord;
    DLG_FZList *FZList;
    bool bFCZKFlag=true;//非常站控状态
    bool bStationDCFlag=true;//车站调车状态
public:
    //初始化界面
    void Init(int ntype);
    void CreatFunButtonKSK();
    void InitMouseRightMenu();
    // 设置功能按钮栏状态
    void SetFunButtonEn(bool b);
    //设置命令下达，清除按钮使能
    void SetFunButtonEn(int n,bool b);
    void SetFunButtonEn_id(int id,bool b);
    //设置功能按钮可见
    void SetFunButtonVisable(int id,bool b);
    //获取命令下达、清除按钮使能状态
    bool GetFunButtonEn(int n);
    bool GetFunButtonEn_id(int id);
    //设置辅助菜单尖轨心轨显示
    void SetMenuAuxiliary(bool b);

signals:
    void FunBtnStateChangeSignal();
    //铅封窗口显示信号
    void LeadSwalingShowSignal();
    //辅助菜单选择结果信号
    void MenuAuxiliaryChoseSignal(QString str);
    //状态选择按下信号
    void StateChoseSignal();
    //方式转化按下信号
    void ModeChangeSignal(QString str);
    //命令清除 命令下发
    void OrderButtonSignal(int ntype);

    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
private slots:
    void slot_btnGroupClicked(int nId);

    void on_pushButton_18_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_17_clicked();

    void on_pushButton_20_clicked();

    void on_pushButton_21_clicked();

    void on_pushButton_22_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_24_clicked();

private:
    Ui::StationFunButtonWidget *ui;
    virtual bool eventFilter(QObject *obj,QEvent *event) override;

public:
    bool pushButtonDDMDFlag;
};

#endif // STATIONFUNBUTTONWIDGET_H
