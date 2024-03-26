#include "textshowattribute.h"
#include "ui_textshowattribute.h"

TextShowAttribute::TextShowAttribute(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextShowAttribute)
{
    ui->setupUi(this);
    setFixedSize(this->width(),this->height());
    setWindowFlags(Qt::Window|Qt::WindowCloseButtonHint);
    setJSQShowFlag=true;
    Init();
    ui->checkBox_16->setEnabled(false);
    connect(ui->pushButton,&QPushButton::clicked,[=](){
      emit SendTextAnalysisSignal(77, QCursor::pos());
      close();
    });
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slot_btnGroupClicked(int)));

}

TextShowAttribute::~TextShowAttribute()
{
    delete ui;
}
void TextShowAttribute::Init()
{
    mTextShowStateVect.append(TextShowState("进站信号机", true));
    mTextShowStateVectMult.append(TextShowState("进站信号机", true));

    mTextShowStateVect.append(TextShowState("出站信号机", true));
    mTextShowStateVectMult.append(TextShowState("出站信号机", true));

    mTextShowStateVect.append(TextShowState("调车信号机", true));
    mTextShowStateVectMult.append(TextShowState("调车信号机", true));

    mTextShowStateVect.append(TextShowState("区间信号机", true));
    mTextShowStateVectMult.append(TextShowState("区间信号机", true));

    mTextShowStateVect.append(TextShowState("站名", true));
    mTextShowStateVectMult.append(TextShowState("站名", true));

    mTextShowStateVect.append(TextShowState("方向", true));
    mTextShowStateVectMult.append(TextShowState("方向", true));

    mTextShowStateVect.append(TextShowState("进路窗外框", true));
    mTextShowStateVectMult.append(TextShowState("进路窗外框", true));

    mTextShowStateVect.append(TextShowState("股道", true));
    mTextShowStateVectMult.append(TextShowState("股道", true));

    mTextShowStateVect.append(TextShowState("无岔区段", true));
    mTextShowStateVectMult.append(TextShowState("无岔区段", true));

    mTextShowStateVect.append(TextShowState("区间轨道", true));
    mTextShowStateVectMult.append(TextShowState("区间轨道", true));

    mTextShowStateVect.append(TextShowState("道岔", true));
    mTextShowStateVectMult.append(TextShowState("道岔", true));

    mTextShowStateVect.append(TextShowState("道岔区段", false));
    mTextShowStateVectMult.append(TextShowState("道岔区段", false));

    mTextShowStateVect.append(TextShowState("挤岔报警灯", false));
    mTextShowStateVectMult.append(TextShowState("挤岔报警灯", false));

    mTextShowStateVect.append(TextShowState("报警灯", true));
    mTextShowStateVectMult.append(TextShowState("报警灯", true));

    mTextShowStateVect.append(TextShowState("半自动闭塞", false));
    mTextShowStateVectMult.append(TextShowState("半自动闭塞", false));

    mTextShowStateVect.append(TextShowState("一般文字", true));
    mTextShowStateVectMult.append(TextShowState("一般文字", true));

    mTextShowStateVect.append(TextShowState("绝缘节", true));
    mTextShowStateVectMult.append(TextShowState("绝缘节", true));

    mTextShowStateVect.append(TextShowState("按钮", true));
    mTextShowStateVectMult.append(TextShowState("按钮", true));

    mTextShowStateVect.append(TextShowState("计数器", true));
    mTextShowStateVectMult.append(TextShowState("计数器", true));
}
void TextShowAttribute:: slot_btnGroupClicked(int nId)
{
    QString name=ui->buttonGroup->button(nId)->text();
    bool b=ui->buttonGroup->button(nId)->isChecked();
    if(!(setJSQShowFlag == false&&name=="计数器"))
    {
        emit SignalOk(name,b);
    }
}

void TextShowAttribute:: DealFunction()
{
    if(setJSQShowFlag == false)
    {
        //ui->checkBox_20->setEnabled(false);
    }
}

void TextShowAttribute::ShowWidgetByType(int wndFlag)
{
    QList<QCheckBox*> checkBoxList = findChildren<QCheckBox*>();
    if(wndFlag == 1)
    {
        for(int i = 0; i < mTextShowStateVect.size(); i++)
        {
            for(int j = 0; j < checkBoxList.size(); j++)
            {
                if(checkBoxList.at(j)->text() == mTextShowStateVect.at(i).name)
                {
                    checkBoxList.at(j)->setCheckState(mTextShowStateVect.at(i).state ? Qt::Checked : Qt::Unchecked);
                    break;
                }
            }

        }
    }
    else if(wndFlag == 2)
    {
        for(int i = 0; i < mTextShowStateVectMult.size(); i++)
        {
            for(int j = 0; j < checkBoxList.size(); j++)
            {
                if(checkBoxList.at(j)->text() == mTextShowStateVectMult.at(i).name)
                {
                    checkBoxList.at(j)->setCheckState(mTextShowStateVectMult.at(i).state ? Qt::Checked : Qt::Unchecked);
                    break;
                }
            }

        }
    }
    if(setJSQShowFlag == false)
    {
        ChangeStatus(wndFlag, "计数器", false);
    }
    this->show();
}

void TextShowAttribute::ChangeStatus(int wndFlag, QString name, bool b)
{
    if(wndFlag == 1)
    {
        for(int i = 0; i < mTextShowStateVect.size(); i++)
        {
            if(mTextShowStateVect.at(i).name == name)
            {
                mTextShowStateVect[i].state = b;
                return;
            }
        }
    }
    else if(wndFlag == 2)
    {
        for(int i = 0; i < mTextShowStateVectMult.size(); i++)
        {
            if(mTextShowStateVectMult.at(i).name == name)
            {
                mTextShowStateVectMult[i].state = b;
                return;
            }
        }
    }
}
