#include "alonxhbtn.h"

AlonXHBtn::AlonXHBtn()
{
    m_nANTYPE = 0;//信号按钮类型（调车按钮/列车按钮）
    m_pCenter = QPoint(0,0);//按钮中心点
    m_rectBtn = QRect(-1,-1,-1,-1);//按钮轮廓坐标
    m_textRect = QRect(-1,-1,-1,-1);
    m_RangeVisible = false;
    m_nBtnIsDown = false;
    m_nBtnFlash = false;
    m_nFuncLockState = false;
    m_nCode = -1;
    m_bNameUp = false;
    isDown_Button = 0;
    ButtonForm=0;
    m_RangeVisible = false;
    m_RangeVisible_XHD = false;
    bZDFlash=true;
}

void AlonXHBtn::Draw(QPainter *painter,long nElapsed, double nDiploid,QPoint offset,int type)
{
    if(type==0x55)
    {
        Draw_Th_kb(painter,nElapsed, nDiploid,offset);
    }
}
void AlonXHBtn::Draw_Th_kb(QPainter *painter,long nElapsed, double nDiploid,QPoint offset)
{
    QFont font;
    if(m_nANTYPE==DCAN)
    {
        ButtonForm=0;
    }
    else if(m_nANTYPE==LCAN)
    {
        ButtonForm=1;
    }
    if(ButtonForm == 1)
    {
        rect_Button.setRect(m_pCenter.x() * nDiploid + offset.x() * nDiploid,m_pCenter.y() * nDiploid + offset.y() * nDiploid,10*nDiploid,10*nDiploid);
    }
    else
    {
        rect_Button.setRect(m_pCenter.x() * nDiploid + offset.x() * nDiploid,m_pCenter.y() * nDiploid + offset.y() * nDiploid,14*nDiploid,14*nDiploid);
    }

    //反走样,防止出现锯齿状线条
    painter->setRenderHint(QPainter::Antialiasing, true);

    font.setFamily("宋体");
    font.setPointSize(10 * nDiploid);//字号
    font.setItalic(false);//斜体
    painter->setFont(font);//设置字体
    painter->setPen(Qt::white);//设置画笔颜色


    if(ButtonForm == 1)
    {
        Draw_Rect_Button(painter,//画笔
                         rect_Button,//按钮绘制区域//需要修改按钮区域大小为10
                         bFCZKState,//非常站控状态
                         getIsDown_Button(),//按钮是否按下
                         m_nFuncLockState,//封锁状态
                         m_nBtnFlash,//终端闪烁状态
                         0,//倒计时时间
                         THIN_GRAY,//按钮颜色
                         Button_Name, //按钮文字显示
                         nElapsed,//闪烁读秒
                         nDiploid,//放大倍数
                         offset);//绘制KSK方形按钮
    }
    else
    {
        Draw_circle_Button(painter,//画笔
                           rect_Button,//按钮绘制区域//需要修改按钮区域大小为14
                           bFCZKState,//非常站控状态
                           getIsDown_Button(),//按钮是否按下
                           m_nFuncLockState,//封锁状态
                           m_nBtnFlash,//终端闪烁状态
                           0,//倒计时时间
                           Button_Name, //按钮文字显示
                           m_RangeVisible,//按钮是否高亮
                           nElapsed, //闪烁读秒
                           nDiploid, //放大倍数
                           offset);//绘制圆形按钮
    }
}

void AlonXHBtn::setVollover(QPoint pt_Base)
{

}
unsigned int AlonXHBtn::getDevType()
{
    return Dev_DLAN;
}

void AlonXHBtn::setButtonPt(QPoint pt)
{
    p_ButtonPt=pt;
    p_ButtonPtConst=pt;
}
QPoint AlonXHBtn::getButtonPt()
{
    return p_ButtonPt;
}

void AlonXHBtn::setIsDown_Button(int flag)
{
    isDown_Button=flag;
}
int AlonXHBtn::getIsDown_Button()
{
    return isDown_Button;
}



void AlonXHBtn::Draw_circle_Button(QPainter *painter,//画笔
                              QRectF m_ButtonRect,//按钮绘制区域
                              bool m_FCZKState,//非常站控状态
                              int m_ButtonIsDownState,//按钮是否按下
                              bool m_FSState,//封锁状态
                              bool m_bZDANFlash,//终端闪烁状态
                              int mTimeCount,//倒计时时间
                              QString rectTextShow, //按钮文字显示
                              bool RangeVisible,//按钮是否高亮
                              long nElapsed, //闪烁读秒
                              double nDiploid, //放大倍数
                              QPoint offset)//绘制圆形按钮
{
#pragma region//>..调车按钮绘制
    //反走样,防止出现锯齿状线条
    painter->setRenderHint(QPainter::Antialiasing, true);

    //按钮边框默认颜色
    QColor btnClrTopLeft = WHITE;//按钮边框-TL
    QColor btnClrBottomRight = GRAY;//按钮边框-BR

    if(!m_FCZKState)
    {
        if (m_ButtonIsDownState==1)
        {
            if (nElapsed % 2 == 0)
            {
                btnClrTopLeft = WHITE;//按钮边框-TL
                btnClrBottomRight = THIN_GRAY;//按钮边框-BR
            }
            else
            {
                btnClrTopLeft = THIN_GRAY;//按钮边框-TL
                btnClrBottomRight = WHITE;//按钮边框-BR
            }
        }
        if (m_ButtonIsDownState==2)
        {
            btnClrTopLeft = THIN_GRAY;//按钮边框-TL
            btnClrBottomRight = WHITE;//按钮边框-BR
        }
        if((m_RangeVisible_XHD)||(m_RangeVisible))
        {
            //反走样,防止出现锯齿状线条
            painter->setRenderHint(QPainter::Antialiasing, false);

            painter->setPen(QPen(Qt::white, 1,Qt::DashLine));
            painter->setBrush(Qt::transparent);
            painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.bottom()+2);
            painter->drawLine(m_ButtonRect.right()+2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

            painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.left()-2,m_ButtonRect.top()-2);
            painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.top()-2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

            //反走样,防止出现锯齿状线条
            painter->setRenderHint(QPainter::Antialiasing, true);
        }
    }

    //绘制下半圆
    QPen pen_BK;
    pen_BK.setColor(btnClrBottomRight);
    pen_BK.setWidth(2);
    painter->setPen(pen_BK);
    painter->drawArc(m_ButtonRect, 180*16, 180*16);
    //绘制上半圆
    pen_BK.setColor(btnClrTopLeft);
    painter->setPen(pen_BK);
    painter->drawArc(m_ButtonRect, 0, 180*16);

    QBrush newBrushBK;
    newBrushBK.setStyle(Qt::SolidPattern);
    QPen pen;
    pen.setWidth(1);

    //绘制调车按钮圆体
    if(m_FCZKState)//非常站控模式下
    {
        newBrushBK.setColor(THIN_THIN_GRAY);
        painter->setBrush(newBrushBK);

        pen.setColor(THIN_THIN_GRAY);
        painter->setPen(pen);
        if(m_RangeVisible_XHD)
        {
            newBrushBK.setColor(THIN_BLUE_DOWN);
            painter->setBrush(newBrushBK);

            pen.setColor(THIN_BLUE_DOWN);
            painter->setPen(pen);
        }
    }
    else if(0/*m_FSState*/)//按钮封锁-粉闪
    {
        if(nElapsed % XHDBTN_FLASTIME == 0) //控制时间间隔
        {
            if(m_ButtonIsDownState == 1)
            {
                newBrushBK.setColor(BLUE);
                painter->setBrush(newBrushBK);

                pen.setColor(BLUE);
                painter->setPen(pen);
            }
            else
            {
                newBrushBK.setColor(GRAY);
                painter->setBrush(newBrushBK);

                pen.setColor(GRAY);
                painter->setPen(pen);
            }
        }
        else
        {
            newBrushBK.setColor(BROWN);
            painter->setBrush(newBrushBK);

            pen.setColor(BROWN);
            painter->setPen(pen);
        }
    }
    else if(m_RangeVisible)
    {
        newBrushBK.setColor(THIN_BLUE_DOWN);
        painter->setBrush(newBrushBK);

        pen.setColor(THIN_BLUE_DOWN);
        painter->setPen(pen);
    }
    else if((m_RangeVisible_XHD)/*||(m_RangeVisible_DCAN)*/)
    {
        newBrushBK.setColor(THIN_BLUE_DOWN);
        painter->setBrush(newBrushBK);

        pen.setColor(THIN_BLUE_DOWN);
        painter->setPen(pen);
    }
    else
    {
        switch (m_ButtonIsDownState)
        {
        case 0:
            {
                //newBrushBK.setColor(THIN_GRAY);//GRAY
                newBrushBK.setColor(GRAY);
                painter->setBrush(newBrushBK);

                //pen.setColor(THIN_GRAY);
                pen.setColor(GRAY);
                painter->setPen(pen);
                break;
            }
        case 1:
            {
                if(nElapsed % XHDBTN_FLASTIME == 0) //控制时间间隔
                {
//                    newBrushBK.setColor(THIN_GRAY);
//                    painter->setBrush(newBrushBK);

//                    pen.setColor(THIN_GRAY);
//                    painter->setPen(pen);
                    newBrushBK.setColor(WHITE);
                    painter->setBrush(newBrushBK);

                    pen.setColor(WHITE);
                    painter->setPen(pen);
                }
                else
                {
                    newBrushBK.setColor(BLUE);
                    painter->setBrush(newBrushBK);

                    pen.setColor(BLUE);
                    painter->setPen(pen);
                }
                break;
            }
        case 2:
            {
                newBrushBK.setColor(RED);
                painter->setBrush(newBrushBK);

                pen.setColor(RED);
                painter->setPen(pen);
                break;
            }
        case 4://可按下，终端闪烁
            {
                if(m_bZDANFlash && bZDFlash && !m_nBtnFlash)
                {
                    if(nElapsed % XHDBTN_FLASTIME == 0) //控制时间间隔
                    {
                        newBrushBK.setColor(YELLOW);
                        painter->setBrush(newBrushBK);

                        pen.setColor(YELLOW);
                        painter->setPen(pen);
                    }
                    else
                    {
                        newBrushBK.setColor(GRAY);
                        painter->setBrush(newBrushBK);

                        pen.setColor(GRAY);
                        painter->setPen(pen);
                    }
                }
                else
                {
                    newBrushBK.setColor(GRAY);
                    painter->setBrush(newBrushBK);

                    pen.setColor(GRAY);
                    painter->setPen(pen);
                }
                break;
            }
        }
    }
    QPointF point = QPointF(m_ButtonRect.left()+7*nDiploid,m_ButtonRect.top()+7*nDiploid);
    painter->drawEllipse(point,6 * nDiploid,6 * nDiploid);

    //封锁X
    if(m_FSState)
    {
        QPen penX;
        penX.setColor(RED);
        penX.setWidth(2);
        painter->setPen(penX);

        QRectF m_rectDC_X;       //调车按钮区域
        m_rectDC_X.setLeft(m_ButtonRect.left()+2*nDiploid);
        m_rectDC_X.setTop(m_ButtonRect.top()+2*nDiploid);
        m_rectDC_X.setRight(m_ButtonRect.right()-2*nDiploid);
        m_rectDC_X.setBottom(m_ButtonRect.bottom()-2*nDiploid);

        painter->drawLine(m_rectDC_X.left(), m_rectDC_X.top(),m_rectDC_X.right(), m_rectDC_X.bottom());
        painter->drawLine(m_rectDC_X.right(), m_rectDC_X.top(),m_rectDC_X.left(), m_rectDC_X.bottom());
    }
    //反走样,防止出现锯齿状线条
    painter->setRenderHint(QPainter::Antialiasing, false);

    //倒计时
    if((0<mTimeCount)&&(255>mTimeCount))
    {
        QString strTimeCount;
        strTimeCount=QString::number(mTimeCount);

        QPen penX;
        penX.setColor(RED);
        penX.setWidth(1);
        painter->setPen(penX);
        QRectF m_rectDC_X;
        m_rectDC_X.setLeft(m_ButtonRect.left()-20 * nDiploid);
        m_rectDC_X.setTop(m_ButtonRect.top()-20 * nDiploid);
        m_rectDC_X.setRight(m_ButtonRect.right()+20 * nDiploid);
        m_rectDC_X.setBottom(m_ButtonRect.bottom()-20 * nDiploid);

        painter->drawText(m_rectDC_X,Qt::AlignHCenter|Qt::AlignVCenter,strTimeCount);
    }
    //绘制按钮的文字
    if(rectTextShow != "")
    {
        pen.setColor(getLightState(wordcolor,nElapsed));
        QFont font;
        font.setWeight(50);//字体宽度,0~99
        font.setPointSize((wordsize) * nDiploid);//字体高度,10表示高度为10个像素点
        painter->setPen(pen);
        painter->setFont(font);
        if(m_bNameUp)
        {
            m_textRect.setLeft(m_ButtonRect.left()-5 * nDiploid);
            m_textRect.setTop(m_ButtonRect.top()-5 * nDiploid);
            m_textRect.setRight(m_ButtonRect.right() +0* nDiploid);
            m_textRect.setBottom(m_ButtonRect.bottom() +0* nDiploid);
        }
        else
        {
            m_textRect.setLeft(m_ButtonRect.left()-5 * nDiploid);
            m_textRect.setTop(m_ButtonRect.top()+30 * nDiploid);
            m_textRect.setRight(m_ButtonRect.right() +0* nDiploid);
            m_textRect.setBottom(m_ButtonRect.bottom() +0* nDiploid);
        }
        painter->drawText(m_textRect.left(),m_textRect.top(),rectTextShow);
    }
#pragma endregion
}

void AlonXHBtn::Draw_Rect_Button(QPainter *painter,//画笔
                              QRectF m_ButtonRect,//按钮绘制区域
                              bool m_FCZKState,//非常站控状态
                              int m_ButtonIsDownState,//按钮是否按下
                              bool m_FSState,//封锁状态
                              bool m_bZDANFlash,//终端闪烁状态
                              int mTimeCount,//倒计时时间
                              QColor color,//按钮颜色
                              QString rectTextShow, //按钮文字显示
                              long nElapsed,//闪烁读秒
                              double nDiploid,//放大倍数
                              QPoint offset)//绘制KSK方形按钮
{
#pragma region//>..列车按钮绘制
    //反走样,防止出现锯齿状线条
    painter->setRenderHint(QPainter::Antialiasing, false);

    //按钮边框默认颜色
    QColor btnClrTopLeft = WHITE;//按钮边框-TL
    QColor btnClrBottomRight = GRAY ;//按钮边框-BR

    //绘制按钮边框
    QPen pen_BK;
    pen_BK.setColor(btnClrBottomRight);
    pen_BK.setWidth(2);
    painter->setPen(pen_BK);
    painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.bottom()+2);
    painter->drawLine(m_ButtonRect.right()+2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

    pen_BK.setColor(btnClrTopLeft);
    pen_BK.setWidth(2);
    painter->setPen(pen_BK);
    painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.left()-2,m_ButtonRect.top()-2);
    painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.top()-2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

    if(!m_FCZKState)
    {
        if (m_ButtonIsDownState==1)
        {
            if (nElapsed % 2 == 0)
            {
                btnClrTopLeft = WHITE;//按钮边框-TL
                btnClrBottomRight = GRAY;//按钮边框-BR

                pen_BK.setColor(btnClrBottomRight);
                pen_BK.setWidth(2);
                painter->setPen(pen_BK);
                painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.bottom()+2);
                painter->drawLine(m_ButtonRect.right()+2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

                pen_BK.setColor(btnClrTopLeft);
                pen_BK.setWidth(2);
                painter->setPen(pen_BK);
                painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.left()-2,m_ButtonRect.top()-2);
                painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.top()-2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

            }
            else
            {
                btnClrTopLeft = GRAY;//按钮边框-TL
                btnClrBottomRight = WHITE;//按钮边框-BR

                pen_BK.setColor(btnClrTopLeft);
                pen_BK.setWidth(2);
                painter->setPen(pen_BK);
                painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.left()-2,m_ButtonRect.top()-2);
                painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.top()-2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

                pen_BK.setColor(btnClrBottomRight);
                pen_BK.setWidth(2);
                painter->setPen(pen_BK);
                painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.bottom()+2);
                painter->drawLine(m_ButtonRect.right()+2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);
            }
        }
        if (m_ButtonIsDownState==2)
        {
            btnClrTopLeft = GRAY;//按钮边框-TL
            btnClrBottomRight = WHITE;//按钮边框-BR

            pen_BK.setColor(btnClrTopLeft);
            pen_BK.setWidth(2);
            painter->setPen(pen_BK);
            painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.left()-2,m_ButtonRect.top()-2);
            painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.top()-2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);

            pen_BK.setColor(btnClrBottomRight);
            pen_BK.setWidth(2);
            painter->setPen(pen_BK);
            painter->drawLine(m_ButtonRect.left()-2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.bottom()+2);
            painter->drawLine(m_ButtonRect.right()+2,m_ButtonRect.bottom()+2,m_ButtonRect.right()+2,m_ButtonRect.top()-2);
        }
        if((m_RangeVisible_XHD)||(m_RangeVisible)/*||((rectBtnTG==m_ButtonRect)&&(m_RangeVisible_TGAN))||((m_rectLC==m_ButtonRect)&&(m_RangeVisible_LCAN))||((m_rectYD==m_ButtonRect)&&(m_RangeVisible_XHD_YDAN))*/)
        {
            //反走样,防止出现锯齿状线条
            painter->setRenderHint(QPainter::Antialiasing, false);

            painter->setPen(QPen(Qt::white, 1,Qt::DashLine));
            painter->setBrush(Qt::transparent);
            painter->drawLine(m_ButtonRect.left()-4,m_ButtonRect.bottom()+4,m_ButtonRect.right()+4,m_ButtonRect.bottom()+4);
            painter->drawLine(m_ButtonRect.right()+4,m_ButtonRect.bottom()+4,m_ButtonRect.right()+4,m_ButtonRect.top()-4);

            painter->drawLine(m_ButtonRect.left()-4,m_ButtonRect.bottom()+4,m_ButtonRect.left()-4,m_ButtonRect.top()-4);
            painter->drawLine(m_ButtonRect.left()-4,m_ButtonRect.top()-4,m_ButtonRect.right()+4,m_ButtonRect.top()-4);

            //反走样,防止出现锯齿状线条
            painter->setRenderHint(QPainter::Antialiasing, true);
        }
    }



    //绘制按钮主体
    QPen pen;
    pen.setJoinStyle(Qt::PenJoinStyle ::MiterJoin);
    pen.setWidth(2);
    QBrush newBrushBK;
    newBrushBK.setStyle(Qt::SolidPattern);
    if(m_FCZKState)//非常站控模式下
    {
        newBrushBK.setColor(THIN_GRAY);
        painter->setBrush(newBrushBK);

        pen.setColor(THIN_GRAY);
        painter->setPen(pen);
        if(m_RangeVisible_XHD)
        {
            newBrushBK.setColor(THIN_BLUE_DOWN);
            painter->setBrush(newBrushBK);

            pen.setColor(THIN_BLUE_DOWN);
            painter->setPen(pen);
        }
    }
    else if(0/*m_FSState*/)//按钮封锁-粉闪
    {
        if(nElapsed % XHDBTN_FLASTIME == 0) //控制时间间隔
        {
            if(m_ButtonIsDownState == 1)
            {
                newBrushBK.setColor(BLUE);
                painter->setBrush(newBrushBK);

                pen.setColor(BLUE);
                painter->setPen(pen);
            }
            else
            {
                newBrushBK.setColor(THIN_GRAY);
                painter->setBrush(newBrushBK);

                pen.setColor(THIN_GRAY);
                painter->setPen(pen);
            }
        }
        else
        {
            newBrushBK.setColor(BROWN);
            painter->setBrush(newBrushBK);

            pen.setColor(BROWN);
            painter->setPen(pen);
        }
    }
    else if(m_RangeVisible)
    {
        newBrushBK.setColor(THIN_BLUE_DOWN);
        painter->setBrush(newBrushBK);

        pen.setColor(THIN_BLUE_DOWN);
        painter->setPen(pen);
    }
    else if(m_RangeVisible_XHD)
    {
        newBrushBK.setColor(THIN_BLUE_DOWN);
        painter->setBrush(newBrushBK);

        pen.setColor(THIN_BLUE_DOWN);
        painter->setPen(pen);
    }
//    else if((m_rectLC==m_ButtonRect)&&(m_RangeVisible_LCAN))
//    {
//        newBrushBK.setColor(THIN_BLUE_DOWN);
//        painter->setBrush(newBrushBK);

//        pen.setColor(THIN_BLUE_DOWN);
//        painter->setPen(pen);
//    }
//    else if((rectBtnTG==m_ButtonRect)&&(m_RangeVisible_TGAN))
//    {
//        newBrushBK.setColor(THIN_BLUE_DOWN);
//        painter->setBrush(newBrushBK);

//        pen.setColor(THIN_BLUE_DOWN);
//        painter->setPen(pen);
//    }
//    else if((m_rectYD==m_ButtonRect)&&(m_RangeVisible_XHD_YDAN))
//    {
//        newBrushBK.setColor(THIN_BLUE_DOWN);
//        painter->setBrush(newBrushBK);

//        pen.setColor(THIN_BLUE_DOWN);
//        painter->setPen(pen);
//    }
    else
    {
        switch (m_ButtonIsDownState)
        {
        case 0:
            {
                newBrushBK.setColor(color);
                painter->setBrush(newBrushBK);

                pen.setColor(color);
                painter->setPen(pen);
                break;
            }
        case 1:
            {
                if(nElapsed % XHDBTN_FLASTIME == 0) //控制时间间隔
                {
//                    newBrushBK.setColor(color);
//                    painter->setBrush(newBrushBK);

//                    pen.setColor(color);
//                    painter->setPen(pen);

                    newBrushBK.setColor(WHITE);
                    painter->setBrush(newBrushBK);

                    pen.setColor(WHITE);
                    painter->setPen(pen);
                }
                else
                {
                    newBrushBK.setColor(BLUE);
                    painter->setBrush(newBrushBK);

                    pen.setColor(BLUE);
                    painter->setPen(pen);
                }
                break;
            }
        case 2:
            {
                newBrushBK.setColor(RED);
                painter->setBrush(newBrushBK);

                pen.setColor(RED);
                painter->setPen(pen);
                break;
            }
        case 4://可按下，终端闪烁
            {
                if(m_bZDANFlash && bZDFlash && !m_nBtnFlash)
                {
                    if(nElapsed % XHDBTN_FLASTIME == 0) //控制时间间隔
                    {
                        newBrushBK.setColor(YELLOW);
                        painter->setBrush(newBrushBK);

                        pen.setColor(YELLOW);
                        painter->setPen(pen);
                    }
                    else
                    {
                        newBrushBK.setColor(color);
                        painter->setBrush(newBrushBK);

                        pen.setColor(color);
                        painter->setPen(pen);
                    }
                }
                else
                {
                    newBrushBK.setColor(color);
                    painter->setBrush(newBrushBK);

                    pen.setColor(color);
                    painter->setPen(pen);
                }
                break;
            }
        }
    }
    painter->drawRect(m_ButtonRect);

    //封锁X
    if(m_FSState)
    {
        QPen penX;
        penX.setColor(RED);
        penX.setWidth(2);
        painter->setPen(penX);

        QRectF m_rectDC_X;       //调车按钮区域
        m_rectDC_X.setLeft(m_ButtonRect.left());
        m_rectDC_X.setTop(m_ButtonRect.top());
        m_rectDC_X.setRight(m_ButtonRect.right());
        m_rectDC_X.setBottom(m_ButtonRect.bottom());

        painter->drawLine(m_rectDC_X.left(), m_rectDC_X.top(),m_rectDC_X.right(), m_rectDC_X.bottom());
        painter->drawLine(m_rectDC_X.right(), m_rectDC_X.top(),m_rectDC_X.left(), m_rectDC_X.bottom());
    }

    //绘制按钮的文字
    if(rectTextShow != ""&&bBtnNameShow)
    {
        pen.setColor(getLightState(wordcolor,nElapsed));
        QFont font;
        font.setWeight(50);//字体宽度,0~99
        font.setPointSize((wordsize) * nDiploid);//字体高度,10表示高度为10个像素点
        painter->setPen(pen);
        painter->setFont(font);
        if(m_bNameUp)
        {
            m_textRect.setLeft(m_ButtonRect.left()-5 * nDiploid);
            m_textRect.setTop(m_ButtonRect.top()-5 * nDiploid);
            m_textRect.setRight(m_ButtonRect.right() +0* nDiploid);
            m_textRect.setBottom(m_ButtonRect.bottom() +0* nDiploid);
        }
        else
        {
            m_textRect.setLeft(m_ButtonRect.left()-5 * nDiploid);
            m_textRect.setTop(m_ButtonRect.top()+30 * nDiploid);
            m_textRect.setRight(m_ButtonRect.right() +0* nDiploid);
            m_textRect.setBottom(m_ButtonRect.bottom() +0* nDiploid);
        }
        painter->drawText(m_textRect.left(),m_textRect.top(),rectTextShow);

    }
    //反走样,防止出现锯齿状线条
    painter->setRenderHint(QPainter::Antialiasing, false);
#pragma endregion
}

QColor AlonXHBtn::getLightState(int nType,int nElapsed)//（颜色：0灭灯,1红,2绿,3白,4黄,5红闪,6绿闪,7白闪,8黄闪,9蓝）
{
    QColor color;
    color = BLACK;
    switch(nType)
    {
    case 0:
        {
            color = BLACK;
            return color;
            break;
        }
    case 1:
        {
            color = RED;
            return color;
            break;
        }
    case 2:
        {
            color = GREEN;
            return color;
            break;
        }
    case 3:
        {
            color = WHITE;
            return color;
            break;
        }
    case 4:
        {
            color = YELLOW;
            return color;
            break;
        }
    case 5:
        {
            if(nElapsed%2 == 0)
                color = RED;
            else
                color = BLACK;
            return color;
            break;
        }
    case 6:
        {
            if(nElapsed%2 == 0)
                color = GREEN;
            else
                color = BLACK;
            return color;
            break;
        }
    case 7:
        {
            if(nElapsed%2 == 0)
                color = WHITE;
            else
                color = BLACK;
            return color;
            break;
        }
    case 8:
        {
            if(nElapsed%2 == 0)
                color = YELLOW;
            else
                color = BLACK;
            return color;
            break;
        }
    case 9:
        {
            color = BLUE;
            return color;
            break;
        }
    case 10:
        {
            color = THIN_GRAY;
            return color;
            break;
        }

    }
    return color;
}

bool AlonXHBtn::GetMousePoint(QPoint p)
{
    if (rect_Button.contains(p))
    {
        return true;
    }
    else
    {
        return false;
    }
}

