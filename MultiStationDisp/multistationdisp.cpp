#include "multistationdisp.h"
#include "ui_multistationdisp.h"
#include <QFileInfo>
#include <QSettings>
#include <QtDebug>

MultiStationDisp::MultiStationDisp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MultiStationDisp)
{
    ui->setupUi(this);
}

MultiStationDisp::~MultiStationDisp()
{
    delete ui;
}

void MultiStationDisp::AddWidget(MyStation* station, int index)
{
    QString strFile="Data/MultiStation/MultiStation.ini";
    QFileInfo fileInfo(strFile);
    if (!fileInfo.isFile())
    {
        qCritical()<<QString::fromLocal8Bit("File %1 not existed!").arg(strFile);
        return;
    }

    QSettings* settings = new QSettings(strFile, QSettings::IniFormat);
    QString str = settings->value(QString("Station%1/Range").arg(index + 1)).toString();
    QStringList reStr = str.split(',');

    QRect re;
    re.setX(reStr.at(0).toInt());
    re.setY(reStr.at(1).toInt());
    re.setWidth(reStr.at(2).toInt());
    re.setHeight(reStr.at(3).toInt());

    StationInfo info;
    info.route = re;
    info.pStation = station;
    pVecStationInfo.append(info);

    ReSize(re);


    QWidget* wnd = new QWidget(this);
    wnd->setObjectName(QString("StationWnd_%1").arg(index));
    wnd->installEventFilter(this);
    wnd->move(re.x() * 0.6, re.y() * 0.6);
    wnd->setFixedSize(re.width() * 0.6, re.height() * 0.6);
    wnd->setStyleSheet("background-color: rgb(0, 0, 0 ,0);");
    wnd->show();
}

bool MultiStationDisp::eventFilter(QObject *obj,QEvent *event)
{
    if(obj->objectName().left(10) == "StationWnd")
    {
        if(event->type() == QEvent::Paint)
        {
            QWidget* wnd = (QWidget*)obj;
            QPainter painter(wnd);
            painter.setRenderHint(QPainter::Antialiasing);
            QPen pen; //画笔
            pen.setColor(QColor(0, 0, 0));
            QBrush brush;
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(QColor(255, 0, 0, 0)); //画刷
            painter.setPen(pen); //添加画笔
            painter.setBrush(brush); //添加画刷

            QString wndName = wnd->objectName();
            int i = wndName.mid(wndName.indexOf('_') + 1).toInt();
            double p = *nDiploid;

            pVecStationInfo.at(i).pStation->drawMuti(&painter, 1);
            painter.end();

            if(pVecStationInfo.at(i).diploid != p)
            {
                QRect re = pVecStationInfo.at(i).route;
                wnd->setFixedSize(re.width() * p * 0.6, re.height() * p * 0.6);
                wnd->move(re.x() * p * 0.6, re.y() * p * 0.6);
                qDebug() << obj->objectName() << QString("{ %1,%2,%3,%4 }").arg(re.width() * p * 0.6).arg(re.height()*p * 0.6).arg(re.x()*p).arg(re.y()*p);
                ReSize(wnd->geometry());
                pVecStationInfo[i].diploid = p;
            }
        }
        if(event->type() == QEvent::MouseButtonPress)
        {
            curWidget = (QWidget*)obj;
        }
    }
    if(event->type() == QEvent::Wheel)
    {
        event->accept();
        return true;
    }
    return QWidget::eventFilter(obj,event);
}

void MultiStationDisp::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(curWidget)
        {
            curPoint = curWidget->pos();
            curMousePoint = event->pos();
        }
    }
}

void MultiStationDisp::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(curWidget && isRePos)
        {
            ReSize(curWidget->geometry());
            isRePos = false;

            QString strFile="Data/MultiStation/MultiStation.ini";
            QFileInfo fileInfo(strFile);
            if (!fileInfo.isFile())
            {
                qCritical()<<QString::fromLocal8Bit("File %1 not existed!").arg(strFile);
                return;
            }

            QSettings* settings = new QSettings(strFile, QSettings::IniFormat);

            QString name = curWidget->objectName();
            int i = curWidget->objectName().mid(curWidget->objectName().indexOf('_') + 1).toInt();
            QString key = QString("Station%1/Range").arg(i + 1);
            QRect re = curWidget->geometry();

            double p = 0.6 * (*nDiploid);
            double rx = re.x() / p;
            double ry = re.y() / p;
            int width = pVecStationInfo[i].route.width();
            int height = pVecStationInfo[i].route.height();
            pVecStationInfo[i].route.setX(rx);
            pVecStationInfo[i].route.setY(ry);
            pVecStationInfo[i].route.setWidth(width);
            pVecStationInfo[i].route.setHeight(height);
            QString value = QString("%1,%2,%3,%4").arg((int)rx).arg((int)ry).arg(pVecStationInfo[i].route.width()).arg(pVecStationInfo[i].route.height());
            qDebug() << value;
            settings->setValue(key, value);
        }
    }
}

void MultiStationDisp::mouseMoveEvent(QMouseEvent *event)
{
    if(curWidget)
    {
        isRePos = true;
        QPoint pos = event->pos() - curMousePoint + curPoint;
        if(pos.x() < 0)
            pos.setX(0);
        if(pos.y() < 0)
            pos.setY(0);
        curWidget->move(pos);
    }
}

void MultiStationDisp::ReSize(QRect re)
{
    if(re.x() + re.width() > maxWidth)
    {
        maxWidth = re.x() + re.width();
    }
    if(re.y() + re.height() > maxHeight)
    {
        maxHeight = re.y() + re.height();
    }
    this->setFixedSize(maxWidth, maxHeight);
}
