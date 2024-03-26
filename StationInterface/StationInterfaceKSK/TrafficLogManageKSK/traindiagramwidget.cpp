#include "traindiagramwidget.h"
#include "ui_traindiagramwidget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QDateTime>

TrainDiagramWidget::TrainDiagramWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrainDiagramWidget)
{
    ui->setupUi(this);
    ui->diagramWidget->installEventFilter(this);
    ui->stationWidget->installEventFilter(this);

}

TrainDiagramWidget::~TrainDiagramWidget()
{
    delete ui;
}

void TrainDiagramWidget::Init(MyStation* pMyStation)
{
    curStation = pMyStation;
    ui->diagramWidget->setFixedSize(6 * 24 * curStation->TrainDiagram.miniteDistance + (SPACING_LEFTORRIGHT * 2),
                                    SPACING_TOPORBOTTOM * 2
                                  + curStation->TrainDiagram.vectRailwayLine.size() * (curStation->TrainDiagram.stationDistance * 2)
                                  + (curStation->TrainDiagram.vectRailwayLine.size() - 1) * curStation->TrainDiagram.lintDistance
                                  + SPACING_TIMEAXIS * 2);
    ui->stationWidget->setFixedHeight(ui->diagramWidget->height());

    ui->scrollArea_2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea_2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea_3->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->scrollArea_3->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(ui->scrollArea_2->verticalScrollBar(), &QScrollBar::valueChanged, [=](int value){
        ui->scrollArea_3->verticalScrollBar()->setValue(value);
    });
    connect(ui->scrollArea_3->verticalScrollBar(), &QScrollBar::valueChanged, [=](int value){
        ui->scrollArea_2->verticalScrollBar()->setValue(value);
    });

    Timer_ID_60000 = startTimer(100);
    nowTime = QDateTime::currentDateTime().time();
    nowTimeLinePx = getPointXByTime(nowTime);

    connect(ui->scrollArea_3->horizontalScrollBar(), &QScrollBar::valueChanged, [=](int value){
//        qDebug()<< nowTimeLinePx << value;
    });
}

bool TrainDiagramWidget::eventFilter(QObject* obj,QEvent* event)
{
    if(obj == ui->diagramWidget)
    {
        if(event->type() == QEvent::Paint)
        {
            drawTimeAxis(true);
            drawGrid();
            drawTimeAxis(false);
            drawNowTimeLine();
            drawTrainPlan();
        }
        if(event->type() == QEvent::Show)
        {
            ui->scrollArea_3->horizontalScrollBar()->setValue(nowTimeLinePx - 800);
        }
    }
    if(obj == ui->stationWidget)
    {
        if(event->type() == QEvent::Paint)
        {
            drawStationName();
        }
    }


    return QWidget::eventFilter(obj, event);
}

void TrainDiagramWidget::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == Timer_ID_60000)
    {
        QTime nowTimeT = QDateTime::currentDateTime().time();
        if(nowTimeT.minute() != nowTime.minute())
        {
            nowTimeLinePx = getPointXByTime(QDateTime::currentDateTime().time());
            ui->diagramWidget->update();
        }
    }
}

void TrainDiagramWidget::drawTimeAxis(bool isTop)
{
    QPainter painter(ui->diagramWidget);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPen pen; //画笔
    pen.setColor(LIGHT_GREEN);
    pen.setWidth(2);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(LIGHT_GREEN); //画刷
    painter.setPen(pen); //添加画笔
    painter.setBrush(brush); //添加画刷

    int startY = 0;
    if(isTop)
        startY = SPACING_TOPORBOTTOM;
    else startY = SPACING_TOPORBOTTOM
            + curStation->TrainDiagram.vectRailwayLine.size() * (curStation->TrainDiagram.stationDistance * 2)
            + (curStation->TrainDiagram.vectRailwayLine.size() - 1) * curStation->TrainDiagram.lintDistance
            + SPACING_TIMEAXIS * 2;


    painter.drawLine(SPACING_LEFTORRIGHT, startY, 6 * 24 * curStation->TrainDiagram.miniteDistance + SPACING_LEFTORRIGHT, startY);
    int hours = 3;

    for(int i = 0; i < 6 * 24 + 1; i++)
    {
        if(hours >= 24)
            hours = 0;
        if(i % 6 == 0)//小时
        {
            QPen pen; //画笔
            pen.setColor(LIGHT_GREEN);
            pen.setWidth(2);
            QFont font("微软雅黑", 11, QFont::Bold);
            painter.setFont(font);
            painter.setPen(pen);

            if(isTop)
            {
                painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                                 curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY - 30);

                painter.drawText(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT - 20, startY - 35,
                                 QString("%1:00").arg(hours, 2, 10, QLatin1Char('0')));
            }
            else
            {
                painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                                 curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY + 30);

                painter.drawText(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT - 20, startY + 48,
                                 QString("%1:00").arg(hours, 2, 10, QLatin1Char('0')));
            }


            hours++;
        }
        else if(i % 3 == 0) //半小时
        {
            QPen pen; //画笔
            pen.setColor(LIGHT_GREEN);
            pen.setWidth(1);
            painter.setPen(pen);
            QFont font("微软雅黑", 10, QFont::Bold);
            painter.setFont(font);
            if(isTop)
            {
                painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                                 curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY - 25);
                painter.drawText(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT - 8, startY - 30, "30");
            }
            else
            {
                painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                                 curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY + 25);
                painter.drawText(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT - 8, startY + 42, "30");
            }

        }
        else //十分钟
        {
            QPen pen; //画笔
            pen.setColor(LIGHT_GREEN);
            QVector<qreal>dashes;
            dashes << 3 << 4;
            pen.setDashPattern(dashes);
            pen.setWidth(1);
            painter.setPen(pen);
            if(isTop)
            {
                painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                                 curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY - 20);
            }
            else
            {
                painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                                 curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY + 20);
            }
        }
    }
}

void TrainDiagramWidget::drawGrid()
{
    int startY = SPACING_TOPORBOTTOM + SPACING_TIMEAXIS;
    int endX = 6 * 24 * curStation->TrainDiagram.miniteDistance + SPACING_LEFTORRIGHT;
    QPainter painter(ui->diagramWidget);
    painter.setRenderHint(QPainter::Antialiasing, false);

    for(int i = 0; i < curStation->TrainDiagram.vectRailwayLine.size(); i++)
    {
        QPen pen; //画笔
        pen.setColor(LIGHT_GREEN);
        pen.setWidth(2);
        QBrush brush;
        brush.setStyle(Qt::NoBrush);
        painter.setPen(pen); //添加画笔
        painter.setBrush(brush); //添加画刷
        painter.drawRect(QRect({SPACING_LEFTORRIGHT, startY, endX - SPACING_LEFTORRIGHT, curStation->TrainDiagram.stationDistance * 2}));
        painter.drawLine(SPACING_LEFTORRIGHT, startY + curStation->TrainDiagram.stationDistance, endX, startY + curStation->TrainDiagram.stationDistance);

        for(int i = 0; i < 6 * 24 + 1; i++)
        {
            if(i % 6 == 0)//小时
            {
                QPen pen; //画笔
                pen.setColor(LIGHT_GREEN);
                pen.setWidth(2);
                painter.setPen(pen);
            }
            else if(i % 3 == 0) //半小时
            {
                QPen pen; //画笔
                pen.setColor(LIGHT_GREEN);
                QVector<qreal>dashes;
                dashes << 16 << 8;
                pen.setDashPattern(dashes);
                pen.setWidth(1);
                painter.setPen(pen);
            }
            else //十分钟
            {
                QPen pen; //画笔
                pen.setColor(LIGHT_GREEN);
                pen.setWidth(1);
                painter.setPen(pen);
            }
            painter.drawLine(curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY,
                             curStation->TrainDiagram.miniteDistance * i + SPACING_LEFTORRIGHT, startY + curStation->TrainDiagram.stationDistance * 2);
        }
        startY += curStation->TrainDiagram.stationDistance * 2 + curStation->TrainDiagram.lintDistance;
    }
}

void TrainDiagramWidget::drawStationName()
{
    QPainter painter(ui->stationWidget);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPen pen; //画笔
    pen.setColor(LIGHT_GREEN);
    pen.setWidth(2);
    painter.setPen(pen); //添加画笔
    QFont font("楷体", 13, QFont::Bold);
    painter.setFont(font);
    QFontMetrics metrics(font);
    int startY = SPACING_TOPORBOTTOM + SPACING_TIMEAXIS;
    int strWidth = 0;
    for(int i = 0; i < curStation->TrainDiagram.vectRailwayLine.size(); i++)
    {
        painter.drawText(16, startY + 4, curStation->TrainDiagram.vectRailwayLine.at(i).startLine);
        painter.drawText(16, startY + curStation->TrainDiagram.stationDistance + 4, curStation->m_strStationName);
        painter.drawText(16, startY + curStation->TrainDiagram.stationDistance * 2 + 4, curStation->TrainDiagram.vectRailwayLine.at(i).endLine);
        startY += curStation->TrainDiagram.stationDistance * 2 + curStation->TrainDiagram.lintDistance;

        strWidth = metrics.width(curStation->TrainDiagram.vectRailwayLine.at(i).endLine);
        if(strWidth > maxStationWidgetWidth)
            maxStationWidgetWidth = strWidth;
        strWidth = metrics.width(curStation->m_strStationName);
        if(strWidth > maxStationWidgetWidth)
            maxStationWidgetWidth = strWidth;
        strWidth = metrics.width(curStation->TrainDiagram.vectRailwayLine.at(i).startLine);
        if(strWidth > maxStationWidgetWidth)
            maxStationWidgetWidth = strWidth;
    }

    if(maxStationWidgetWidth > ui->stationWidget->width())
    {
        ui->stationWidget->setFixedWidth(maxStationWidgetWidth);
    }
}

void TrainDiagramWidget::drawNowTimeLine()
{
    QPainter painter(ui->diagramWidget);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QPen pen; //画笔
    pen.setColor(Qt::blue);
    pen.setWidth(2);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::blue); //画刷
    painter.setPen(pen); //添加画笔
    painter.setBrush(brush); //添加画刷
    int startY = SPACING_TOPORBOTTOM + SPACING_TIMEAXIS;
    int endY = startY + curStation->TrainDiagram.vectRailwayLine.size() * (curStation->TrainDiagram.stationDistance * 2)
            + (curStation->TrainDiagram.vectRailwayLine.size() - 1) * curStation->TrainDiagram.lintDistance;
    painter.drawLine(nowTimeLinePx, startY, nowTimeLinePx, endY);
}

void TrainDiagramWidget::drawTrainPlan()
{
    QPainter painter(ui->diagramWidget);
    painter.setRenderHint(QPainter::Antialiasing, false);

    QBrush brush;
    brush.setStyle(Qt::NoBrush);
    painter.setBrush(brush); //添加画刷


    for(int i = 0; i < curStation->m_ArrayTrafficLog.size(); i++)
    {
        TrafficLog* pTrafficLog = curStation->m_ArrayTrafficLog.at(i);
        int index = getDirectionIndex(pTrafficLog);
        int min = 0;
        int startX  = 0;
        int startY = 0;
        int offsetX = 0;
        int offset1 = 0;
        int offset2 = 0;
        int offset3 = curStation->TrainDiagram.miniteDistance;

        startY = SPACING_TOPORBOTTOM +SPACING_TIMEAXIS
                + (index * 2 - 1)  * curStation->TrainDiagram.stationDistance
                + (index - 1) * curStation->TrainDiagram.lintDistance;


        //int min = pTrafficLog->m_timProvReachStation.time().toString("mm").toInt();
        //int min2 = pTrafficLog->m_timProvDepaTrain.time().toString("mm").toInt();
        if(pTrafficLog->m_btBeginOrEndFlg == JFC_TYPE_SF || pTrafficLog->m_btBeginOrEndFlg == JFC_TYPE_TG)
        {
            if(pTrafficLog->m_bDepartTrainNumSX)
            {
                offset1 = 4;
                offset2 = curStation->TrainDiagram.stationDistance / 2;
            }
            else
            {
                offset1 = -4;
                offset2 = -1 * curStation->TrainDiagram.stationDistance / 2;
            }
            if(pTrafficLog->m_timRealDepaTrain.isNull())
            {
                setPenInfoByPoint(&painter, true, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timProvDepaTrain.time().toString("mm").toInt();
                startX = getPointXByTime(pTrafficLog->m_timProvDepaTrain.time());
            }
            else
            {
                setPenInfoByPoint(&painter, false, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timRealDepaTrain.time().toString("mm").toInt();
                startX = getPointXByTime(pTrafficLog->m_timRealDepaTrain.time());
            }
            painter.drawText(startX - offset1, startY - 4, QString("%1").arg(min % 10));
            painter.drawLine(startX - offset3, startY + offset2,
                             startX + offset3, startY - offset2);
            drawTrainNum(&painter, startX - offset3, startY + offset2, pTrafficLog->m_strTrainNum, pTrafficLog->m_bDepartTrainNumSX);
            drawEndFlag(&painter, startX + offset3, startY - offset2, pTrafficLog->m_bDepartTrainNumSX);
        }
        else if(pTrafficLog->m_btBeginOrEndFlg == JFC_TYPE_ZD)
        {
            if(pTrafficLog->m_bReachTrainNumSX)
            {
                offset1 = 4;
                offset2 = curStation->TrainDiagram.stationDistance / 2;
            }
            else
            {
                offset1 = -4;
                offset2 = -1 * curStation->TrainDiagram.stationDistance / 2;
            }
            if(pTrafficLog->m_timRealReachStation.isNull())
            {
                setPenInfoByPoint(&painter, true, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timProvReachStation.time().toString("mm").toInt();
                startX = getPointXByTime(pTrafficLog->m_timProvReachStation.time());
            }
            else
            {
                setPenInfoByPoint(&painter, false, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timRealReachStation.time().toString("mm").toInt();
                startX = getPointXByTime(pTrafficLog->m_timRealReachStation.time());
            }


            painter.drawText(startX - offset1, startY - 4, QString("%1").arg(min % 10));
            painter.drawLine(startX - offset3, startY + offset2, startX, startY);
            drawTrainNum(&painter, startX - offset3, startY + offset2, pTrafficLog->m_strTrainNum, pTrafficLog->m_bReachTrainNumSX);
        }
        else
        {
            if(pTrafficLog->m_bDepartTrainNumSX || pTrafficLog->m_bReachTrainNumSX)
            {
                offset1 = 4;
                offset2 = curStation->TrainDiagram.stationDistance / 2;
            }
            else
            {
                offset1 = -12;
                offset2 = -1 * curStation->TrainDiagram.stationDistance / 2;
            }

            if(pTrafficLog->m_timRealReachStation.isNull())
            {
                setPenInfoByPoint(&painter, true, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timProvReachStation.time().toString("mm").toInt();
                startX = getPointXByTime(pTrafficLog->m_timProvReachStation.time());
            }
            else
            {
                setPenInfoByPoint(&painter, false, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timRealReachStation.time().toString("mm").toInt();
                startX = getPointXByTime(pTrafficLog->m_timRealReachStation.time());
            }
            painter.drawText(startX - offset1, startY - 4, QString("%1").arg(min % 10));
            painter.drawLine(startX - offset3, startY + offset2, startX, startY);
            drawTrainNum(&painter, startX - offset3, startY + offset2, pTrafficLog->m_strTrainNum, pTrafficLog->m_bDepartTrainNumSX);

            if(pTrafficLog->m_timRealDepaTrain.isNull())
            {
                setPenInfoByPoint(&painter, true, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timProvDepaTrain.time().toString("mm").toInt();
                offsetX = getPointXByTime(pTrafficLog->m_timProvDepaTrain.time());
            }
            else
            {
                setPenInfoByPoint(&painter, false, curStation->m_ArrayTrafficLog.at(i)->m_nLHFlg);
                min = pTrafficLog->m_timRealDepaTrain.time().toString("mm").toInt();
                offsetX = getPointXByTime(pTrafficLog->m_timRealDepaTrain.time());
            }
            painter.drawLine(startX, startY, offsetX, startY);
            painter.drawText(offsetX + offset1, startY + 16, QString("%1").arg(min % 10));
            painter.drawLine(offsetX, startY, offsetX + offset3, startY - offset2);
            drawEndFlag(&painter, offsetX + offset3, startY - offset2, pTrafficLog->m_bDepartTrainNumSX);
        }
    }
}

void TrainDiagramWidget::drawTrainNum(QPainter* painter, int X, int Y, QString trainNum, bool isUpgoing)
{

    QFontMetrics metrics(painter->font());
    if(isUpgoing)
    {
        painter->drawLine(X, Y, X - metrics.width(trainNum), Y);
        painter->drawLine(X - metrics.width(trainNum), Y, X - 8 - metrics.width(trainNum), Y + 8);
        painter->drawText(X - metrics.width(trainNum), Y + 16, trainNum);
    }
    else
    {
        painter->drawLine(X, Y, X - metrics.width(trainNum), Y);
        painter->drawLine(X - metrics.width(trainNum), Y, X - 8 - metrics.width(trainNum), Y - 8);
        painter->drawText(X - metrics.width(trainNum), Y - 4, trainNum);
    }
}

void TrainDiagramWidget::drawEndFlag(QPainter* painter, int X, int Y, bool isUpgoing)
{
    /*
    if(isUpgoing)
    {
        painter->drawLine(X, Y, X, Y - 12);
        QPointF pt[3] = {
            QPointF(X, Y - 20),
            QPointF(X - 6, Y - 12),
            QPointF(X + 6, Y - 12)
        };
        painter->drawPolygon(pt, 3);
    }
    else
    {
        painter->drawLine(X, Y, X, Y + 12);
        QPointF pt[3] = {
            QPointF(X, Y + 20),
            QPointF(X - 6, Y + 12),
            QPointF(X + 6, Y + 12)
        };
        painter->drawPolygon(pt, 3);
    }*/
    if(isUpgoing)
    {
        painter->drawLine(X, Y, X + 24, Y);
        painter->drawLine(X + 24, Y, X, Y - 12);
    }
    else
    {
        painter->drawLine(X, Y, X + 24, Y);
        painter->drawLine(X + 24, Y, X, Y + 12);
    }
}

int TrainDiagramWidget::getPointXByTime(QTime time)
{
    int hour = time.hour();
    if(hour < 3)
        hour += 24;
    hour -= 3;

    int minite = time.minute() / 10;
    int min = time.minute() % 10;
    int X = SPACING_LEFTORRIGHT + (hour * 6 + minite) * curStation->TrainDiagram.miniteDistance + min * (curStation->TrainDiagram.miniteDistance / 9);
    return X;
}

int TrainDiagramWidget::getDirectionIndex(TrafficLog* pTrafficLog)
{
    for (int i = 0; i < curStation->TrainDiagram.vectRailwayLine.size(); i++)
    {
        if((curStation->TrainDiagram.vectRailwayLine[i].startLine == pTrafficLog->m_strFromAdjtStation
            || curStation->TrainDiagram.vectRailwayLine[i].endLine == pTrafficLog->m_strFromAdjtStation)
                && (curStation->TrainDiagram.vectRailwayLine[i].endLine == pTrafficLog->m_strToAdjtStation
                    || curStation->TrainDiagram.vectRailwayLine[i].startLine == pTrafficLog->m_strToAdjtStation))
        {
            return i + 1;
        }
    }
    return 1;
}

void TrainDiagramWidget::setPenInfoByPoint(QPainter* painter, bool isNull, int nLHFlg)
{
    QFont font;
    QPen pen; //画笔
    if(isNull)
    {
        pen.setWidth(1);
        if(nLHFlg == 1)
            pen.setColor(PURPLE);
        else pen.setColor(QColor(205, 133, 63));
        font = QFont("微软雅黑", 11);
    }
    else
    {
        pen.setWidth(2);
        if(nLHFlg == 1)
            pen.setColor(Qt::red);
        else pen.setColor(QColor(139, 69, 19));
        font = QFont("微软雅黑", 11, QFont::Bold);
    }
    painter->setFont(font);
    painter->setPen(pen); //添加画笔
}
