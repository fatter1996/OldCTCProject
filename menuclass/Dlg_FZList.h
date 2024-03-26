#ifndef DLG_FZLIST_H
#define DLG_FZLIST_H

#include <QDialog>

namespace Ui {
class DLG_FZList;
}

class DLG_FZList : public QDialog
{
    Q_OBJECT

public:
    explicit DLG_FZList(QWidget *parent = nullptr);
    ~DLG_FZList();
    void  SetJGVisable(bool b);//设置尖轨选项显示
    void  SetXGVisable(bool b);//设置心轨选项显示
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();



    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::DLG_FZList *ui;

signals:
    //命令清除 命令下发
    void OrderFZListSignal(int ntype);
    //股道无电
    void GDWDSignal();
    //接触网定位无电
    void JCWDWWDSignal();
    //接触网反位无电
    void JCWFWWDSignal();
    //尖轨故障
    void JGGZSignal();
    //心轨故障
    void XGGZSignal();
    //破封统计
    void PFTJSignal();
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
};

#endif // DLG_FZLIST_H
