#ifndef TEXTSHOWATTRIBUTE_H
#define TEXTSHOWATTRIBUTE_H

#include <QWidget>
#include <GlobalHeaders/Global.h>
namespace Ui {
class TextShowAttribute;
}

class TextShowAttribute : public QWidget
{
    Q_OBJECT

public:
    explicit TextShowAttribute(QWidget *parent = nullptr);
    ~TextShowAttribute();
    struct TextShowState
    {
        QString name;//设备名称
        int type;//设备类型
        bool state;//设备显示状态

        TextShowState(QString _name, bool _state)
        {
            Init();
            name = _name;
            state = _state;
        }


        void Init()
        {
            name="";
            type=-1;
            state=false;
        }
    };

    QVector<TextShowState>mTextShowStateVect;
    QVector<TextShowState>mTextShowStateVectMult;
    void Init();
    bool setJSQShowFlag;
    void DealFunction();
    void ShowWidgetByType(int wndFlag);
    void ChangeStatus(int wndFlag,QString name,bool b);

signals:
    void SignalOk(QString name,bool b);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");

private slots:
        void slot_btnGroupClicked(int nId);
private:
    Ui::TextShowAttribute *ui;
};

#endif // TEXTSHOWATTRIBUTE_H
