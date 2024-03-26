#include "gdyc.h"


CGDYC::CGDYC(QObject *parent)
{

}

CGDYC::~CGDYC()
{

}

void CGDYC::Init(int type, QPointF pCenter, QString strGDName, QVector<CBaseData*> vecBaseData, bool bShowFrame, CGD* pShowGD)
{
    m_nType = type;
    m_pPointConst = pCenter;
    if(m_nType == 1) //腰岔封锁
    {
        for(int i = 0; i < vecBaseData.size(); i++)
        {
            m_vecGDDCArray.append((CGDDC*)vecBaseData[i]);
        }
        m_vecGDArray.append(pShowGD);
    }
    else
    {
        for(int i = 0; i < vecBaseData.size(); i++)
        {
            m_vecGDArray.append((CGD*)vecBaseData[i]);
        }
    }

    m_strGD = strGDName;
    m_bShowFrame = bShowFrame;
}

void CGDYC::Draw(QPainter *painter, long nElapsed, double nDiploid, QPoint offset)
{

    m_pPoint.setX(m_pPointConst.x() + offset.x());
    m_pPoint.setY(m_pPointConst.y() + offset.y());


    QPen pen;
    QFont font;
    font.setFamily("宋体");
    font.setPointSize(10 * nDiploid);//字号
    painter->setFont(font);//设置字体
    painter->setRenderHint(QPainter::Antialiasing, true);

    if(m_nType == 1)
    {
        for(int i = 0; i < m_vecGDDCArray.size(); i++)
        {
            if(m_vecGDDCArray[i])
                m_nState |= m_vecGDDCArray.at(i)->isYCFS;
        }

        QRect rectWnd(m_pPoint.x(), m_pPoint.y(), 192, 64);
        QRect rectLamp;

        if(m_bShowFrame)
        {
            //绘制外边框
            pen.setWidth(1);
            pen.setColor(Qt::white);
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);

            painter->drawRect(rectWnd.x() * nDiploid,
                              rectWnd.y() * nDiploid,
                              rectWnd.width() * nDiploid,
                              rectWnd.height() * nDiploid);

            //绘制道岔闭锁灯文字1
            pen.setWidth(1 * nDiploid);
            pen.setColor(Qt::white);
            painter->setPen(pen);
            QRect rectText1(rectWnd.x() + 100, rectWnd.y() + 4, 80, 16);
            rectText1.setRect(rectText1.x() * nDiploid,
                              rectText1.y() * nDiploid,
                              rectText1.width() * nDiploid,
                              rectText1.height() * nDiploid);
            painter->drawText(rectText1, Qt::AlignCenter, QString("%1腰岔").arg(m_strGD));

            //绘制道岔闭锁灯文字2
            QRect rectText2(rectWnd.x() + 100, rectWnd.y() + 44, 80, 16);
            rectText2.setRect(rectText2.x() * nDiploid,
                              rectText2.y() * nDiploid,
                              rectText2.width() * nDiploid,
                              rectText2.height() * nDiploid);
            painter->drawText(rectText2, Qt::AlignCenter, QString("%1腰岔锁闭").arg(m_strGD));

            //绘制按钮文字
            pen.setColor(Qt::yellow);
            painter->setPen(pen);
            QRect rectText3(rectWnd.x() + 12, rectWnd.y() + 44, 80, 16);
            rectText3.setRect(rectText3.x() * nDiploid,
                              rectText3.y() * nDiploid,
                              rectText3.width() * nDiploid,
                              rectText3.height() * nDiploid);
            painter->drawText(rectText3, Qt::AlignCenter, QString("%1腰岔解锁").arg(m_strGD));
            rectLamp = QRect(rectWnd.x() + 136, rectWnd.y() + 24, 16, 16);
            m_RectBtn.setRect(rectWnd.x() + 44, rectWnd.y() + 24, 16, 16);
        }
        else
        {
            m_RectBtn.setRect(m_pPoint.x(), m_pPoint.y(), 16, 16);
            rectLamp = QRect(m_pPoint.x() + 24, m_pPoint.y(), 16, 16);
        }

        //绘制道岔闭锁灯
        pen.setWidth(2);
        pen.setColor(THIN_GRAY);
        painter->setPen(pen);
        if(m_nState)
        {
            QBrush brush;
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(Qt::white);
            painter->setBrush(brush);
        }
        else painter->setBrush(Qt::NoBrush);

        painter->drawEllipse(rectLamp.x() * nDiploid,
                             rectLamp.y() * nDiploid,
                             rectLamp.width() * nDiploid,
                             rectLamp.height() * nDiploid);
        //绘制按钮
        DrawRect(painter, nElapsed, nDiploid, offset);
    }
    else if(m_nType == 2)
    {
        QRect rectWnd(m_pPoint.x(), m_pPoint.y(), 72, 36);
        //绘制按钮
        m_RectBtn.setRect(m_pPoint.x() + 28, m_pPoint.y(), 16, 16);
        DrawRect(painter, nElapsed, nDiploid, offset);

        //绘制股道解锁确认文字
        pen.setColor(Qt::yellow);
        painter->setPen(pen);
        QRect rectText1(rectWnd.x(), rectWnd.y() + 20, 72, 16);
        rectText1.setRect(rectText1.x() * nDiploid,
                          rectText1.y() * nDiploid,
                          rectText1.width() * nDiploid,
                          rectText1.height() * nDiploid);
        painter->drawText(rectText1, Qt::AlignCenter, QString("%1确认").arg(m_strGD));


    }

    if(m_vecGDArray.size() > 0 && m_nTime > 0)
    {

        QRectF textRect = m_vecGDArray.at(0)->m_textRect;
        if(m_nType == 1)
        {
            textRect.setLeft(textRect.left());
            textRect.setTop(textRect.top() - 12);
            textRect.setRight(textRect.right());
            textRect.setBottom(textRect.top() + 10);
        }
        else if(m_nType == 2)
        {
            textRect.setLeft(textRect.right() + 4);
            textRect.setTop(textRect.top());
            textRect.setRight(textRect.right() + 24);
            textRect.setBottom(textRect.bottom());
        }
        pen.setColor(Qt::red);
        painter->setPen(pen);
        painter->drawText(textRect, Qt::AlignCenter, QString("%1").arg(m_nTime));
    }


//    if(m_nTime > 0)
//        qDebug() << m_strGD << "倒计时时间 : " << m_nTime;
}

bool CGDYC::GetMousePoint(QPoint p)
{
    return m_RectBtn.contains(p);
}

int CGDYC::getGLXH(int index)
{
    if(m_nType == 1 && index< m_vecGDDCArray.size())
        return m_vecGDDCArray[index]->getCode();
    else if(m_nType == 2 && index< m_vecGDArray.size())
        return m_vecGDArray[index]->getCode();
    return -1;
}

void CGDYC::DrawRect(QPainter *painter, long nElapsed, double nDiploid, QPoint offset)
{
    m_RectBtn.setRect(m_RectBtn.x() * nDiploid,
                      m_RectBtn.y() * nDiploid,
                      m_RectBtn.width() * nDiploid,
                      m_RectBtn.height() * nDiploid);

    //按钮边框默认颜色
    QColor btnClrTopLeft = WHITE;//按钮边框-TL
    QColor btnClrBottomRight = GRAY;//按钮边框-BR

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    QPen pen;
    pen.setWidth(2);
    pen.setJoinStyle(Qt::PenJoinStyle ::MiterJoin);

    QColor color;
    if(m_bBtnIsDown)
    {
        if(nElapsed % 2 == 0)
            color = THIN_GRAY;
        else color = BLUE;
        btnClrTopLeft = GRAY;
        btnClrBottomRight = WHITE;
    }
    else color = THIN_GRAY;

    //绘制按钮边框
    QPen pen_BK;
    pen_BK.setColor(btnClrBottomRight);
    pen_BK.setWidth(3);
    painter->setPen(pen_BK);
    painter->drawLine(m_RectBtn.left(), m_RectBtn.bottom(), m_RectBtn.right(), m_RectBtn.bottom());
    painter->drawLine(m_RectBtn.right(), m_RectBtn.bottom(), m_RectBtn.right(), m_RectBtn.top());

    pen_BK.setColor(btnClrTopLeft);
    pen_BK.setWidth(2);
    painter->setPen(pen_BK);
    painter->drawLine(m_RectBtn.left() ,m_RectBtn.bottom(), m_RectBtn.left(), m_RectBtn.top());
    painter->drawLine(m_RectBtn.left(), m_RectBtn.top(), m_RectBtn.right(), m_RectBtn.top());

    pen.setColor(color);
    brush.setColor(color);
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(m_RectBtn.x() + 2,
                      m_RectBtn.y() + 2,
                      m_RectBtn.width() - 4,
                      m_RectBtn.height() - 4);
}

void CGDYC::timerEvent(QTimerEvent *event)
{

}
