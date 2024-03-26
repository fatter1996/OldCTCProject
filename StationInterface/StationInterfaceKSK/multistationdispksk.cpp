#include "multistationdispksk.h"
#include "ui_multistationdispksk.h"
#include <QScrollBar>
#pragma execution_character_set("utf-8")

MultiStationDispKSK::MultiStationDispKSK(QWidget *parent) :
    MultiStationDisp(parent),
    ui(new Ui::MultiStationDispKSK)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
    ui->scrollArea->setMouseTracking(true);
    ui->widget->setMouseTracking(true);
    ui->widget_2->setMouseTracking(true);
    this->installEventFilter(this);
    ui->widget->installEventFilter(this);
    ui->widget_2->installEventFilter(this);
    Timer_ID_500 = startTimer(500);
}

MultiStationDispKSK::~MultiStationDispKSK()
{
    delete ui;
}

void MultiStationDispKSK::AddMultiStationInfo(MyStation* station, int index)
{
    QString strFile="Data/MultiStation/MultiStation.ini";
    QFileInfo fileInfo(strFile);
    if (!fileInfo.isFile())
    {
        qCritical()<<QString::fromLocal8Bit("File %1 not existed!").arg(strFile);
        return;
    }

    QSettings* settings = new QSettings(strFile, QSettings::IniFormat);
    settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    StationInfo info;
    info.route = settings->value(QString("Station%1/Range").arg(index + 1)).toRect();
    info.isIsOverturn = settings->value(QString("Station%1/IsOverturn").arg(index + 1)).toUInt();
    info.offsetPt = settings->value(QString("Station%1/OffSetPt").arg(index + 1)).toPoint();
    info.factor = settings->value(QString("Station%1/Factor").arg(index + 1)).toDouble();
    QWidget* wnd = new QWidget(ui->widget_2);
    wnd->setMouseTracking(true);
    wnd->installEventFilter(this);
    wnd->setObjectName(QString("StationWnd_%1").arg(index));
    wnd->installEventFilter(this);
    wnd->move(info.route.x(), info.route.y());
    wnd->setFixedSize(info.route.width(), info.route.height());
    wnd->setStyleSheet("QWidget{background-color: rgba(0, 0, 0 ,0);}");
    if(info.isIsOverturn == 1)
    {
        station->isOverturn = true;

        station->volloverPro();
    }
    station->SetOffsetPt(info.offsetPt);
    station->InitTrainWnd();
    info.pStation = station;
    info.wnd = wnd;
    pVecStationInfo.append(info);
}
void MultiStationDispKSK::StationViewResize(int nEnlargeNarrow)
{
    for (int i = 0; i < pVecStationInfo.size(); i++)
    {
        pVecStationInfo.at(i).pStation->SetDiploid(nEnlargeNarrow, pVecStationInfo.at(i).pStation->nDiploidMuti);
        diploid = pVecStationInfo.at(i).pStation->nDiploidMuti;
        QRect re = pVecStationInfo.at(i).route;
        pVecStationInfo.at(i).wnd->setFixedSize(re.width() * diploid, re.height() * diploid);
        pVecStationInfo.at(i).wnd->move(re.x() * diploid, re.y() * diploid);
        isStationViewReSize = !(pVecStationInfo.at(i).pStation->nDiploidMuti == 1.0f);
    }
    WidgetReSize();
}

void MultiStationDispKSK::TrainNOViewResize(int nEnlargeNarrow)
{

    for (int i = 0; i < pVecStationInfo.size(); i++)
    {
        pVecStationInfo.at(i).pStation->SetDiploid(nEnlargeNarrow, pVecStationInfo.at(i).pStation->nDiploidCheCiMuti);
        isTrainNumWndReSize = !(pVecStationInfo.at(i).pStation->nDiploidCheCiMuti == 1.0f);
    }
}

bool MultiStationDispKSK::eventFilter(QObject *obj,QEvent *event)
{
    if(event->type() == QEvent::Paint)
    {
        ui->widget_2->setFixedSize(initialSize.width(), initialSize.height());
        if(obj->objectName().left(10) == "StationWnd")
        {
            QPainter painter((QWidget*)obj);
            painter.setRenderHint(QPainter::Antialiasing);
            QPen pen; //画笔
            pen.setColor(QColor(0, 0, 0));
            QBrush brush;
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(QColor(255, 0, 0, 0)); //画刷
            painter.setPen(pen); //添加画笔
            painter.setBrush(brush); //添加画刷

            int i = obj->objectName().mid(obj->objectName().indexOf('_') + 1).toInt();
            pVecStationInfo.at(i).pStation->draw(&painter, nElapsed, pVecStationInfo.at(i).factor, true);
            painter.end();
        }
    }
    if(obj->objectName().left(10) == "StationWnd")
    {
        if(event->type() == QEvent::MouseButtonDblClick && !isRePos)
        {
            if(curWidget)
            {
                curWidget->setStyleSheet("QWidget{background-color: rgba(0, 0, 0 ,0);}");
                curWidget = nullptr;
            }
            curWidget = (QWidget*)obj;
            curWidget->setStyleSheet("QWidget{background-color: rgba(0, 0, 0 ,0);border:1px dashed white;}");
        }
    }
    return MultiStationDisp::eventFilter(obj,event);
}

void MultiStationDispKSK::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(curWidget)
        {
            QPoint pt;
            pt.setX(event->pos().x() + ui->scrollArea->horizontalScrollBar()->sliderPosition());
            pt.setY(event->pos().y() + ui->scrollArea->verticalScrollBar()->sliderPosition());

            if(curWidget->geometry().contains(pt))
            {
                curPoint = curWidget->pos();
                curMousePoint = event->pos();
                isRePos = true;
            }
        }
    }

    return MultiStationDisp::mousePressEvent(event);
}

void MultiStationDispKSK::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if(curWidget && curWidget->isActiveWindow() && isRePos)
        {
            isRePos = false;
            QString strFile="Data/MultiStation/MultiStation.ini";
            QFileInfo fileInfo(strFile);
            if (!fileInfo.isFile())
            {
                qCritical()<<QString::fromLocal8Bit("File %1 not existed!").arg(strFile);
                return;
            }

            QSettings* settings = new QSettings(strFile, QSettings::IniFormat);
            settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
            QString name = curWidget->objectName();
            int i = curWidget->objectName().mid(curWidget->objectName().indexOf('_') + 1).toInt();
            QString key = QString("Station%1/Range").arg(i + 1);
            QRect re = curWidget->geometry();

            diploid = pVecStationInfo.at(i).pStation->nDiploidMuti;
            double rx = re.x() / diploid;
            double ry = re.y() / diploid;
            int width = pVecStationInfo[i].route.width();
            int height = pVecStationInfo[i].route.height();
            pVecStationInfo[i].route.setX(rx);
            pVecStationInfo[i].route.setY(ry);
            pVecStationInfo[i].route.setWidth(width);
            pVecStationInfo[i].route.setHeight(height);
            WidgetReSize();
            QString value = QString("%1,%2,%3,%4").arg((int)rx).arg((int)ry).arg(pVecStationInfo[i].route.width()).arg(pVecStationInfo[i].route.height());
            settings->setValue(key, pVecStationInfo[i].route);
        }
    }
    return MultiStationDisp::mouseReleaseEvent(event);
}

void MultiStationDispKSK::mouseMoveEvent(QMouseEvent* event)
{
    if(curWidget && isRePos)
    {
        QPoint pos = event->pos() - curMousePoint + curPoint;
        if(pos.x() < 0)
            pos.setX(0);
        if(pos.y() < 0)
            pos.setY(0);
        curWidget->move(pos);
    }
    return MultiStationDisp::mouseMoveEvent(event);
}

void MultiStationDispKSK::keyReleaseEvent(QKeyEvent *event)
{

    if(event->key() == Qt::Key::Key_Escape)
    {
        if(curWidget)
        {
            curWidget->setStyleSheet("QWidget{background-color: rgba(0, 0, 0 ,0);}");
            curWidget = nullptr;
            isRePos = false;
        }
    }
    return MultiStationDisp::keyReleaseEvent(event);
}

void MultiStationDispKSK::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == Timer_ID_500)
    {
        if (nElapsed>1000)
        {
            nElapsed=0;
        }
        nElapsed++;

        //接通光带
        nLightband_15++;
        nLightband_30++;
        if(nLightband_30>60)
        {
           BLightband_30 = false;
           nLightband_30=0;
        }
        if(nLightband_15>30)
        {
           BLightband_15=false;
           nLightband_15=0;
        }
    }
}


void MultiStationDispKSK::WidgetReSize()
{
    maxWidth = 0;
    maxHeight = 0;
    int rx = 0;
    int ry = 0;

    for (int i = 0; i < pVecStationInfo.size(); i++)
    {
        rx = (pVecStationInfo.at(i).route.x() + pVecStationInfo.at(i).route.width()) * diploid;
        ry = (pVecStationInfo.at(i).route.y() + pVecStationInfo.at(i).route.height()) * diploid;
        if(rx > maxWidth)
        {
            maxWidth = rx;
        }
        if(ry * diploid > maxHeight)
        {
            maxHeight = ry;
        }
    }
    if(maxWidth < this->width())
        maxWidth = this->width();

    if(maxHeight < this->height())
        maxHeight = this->height();

    initialSize = { maxWidth,maxHeight };
}

void MultiStationDispKSK::TrainNumWndShowStateChenge(bool b)
{
    bShowCheCi = b;
    for (int i = 0; i < pVecStationInfo.size(); i++)
    {
        pVecStationInfo.at(i).pStation->SetTrainNumWndShow(bShowCheCi);
    }

}
void MultiStationDispKSK::RoutePreWndShowStateChenge(bool b)
{
    bRoutePreWndShowState= b;
    for (int i = 0; i < pVecStationInfo.size(); i++)
    {
        pVecStationInfo.at(i).pStation->SetRoutePreWndShow(bRoutePreWndShowState);
    }
}

void MultiStationDispKSK::TextShowSolt(QString name,bool b)
{
    for (int i = 0; i < pVecStationInfo.size(); i++)
    {
        if(name=="进站信号机")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(3,b);
        }
        else if(name=="出站信号机")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(4,b);
        }
        else if(name=="调车信号机")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(5,b);
        }
        else if(name=="区间信号机")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(6,b);
        }
        else if(name=="站名")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(7,b);
        }
        else if(name=="方向")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(8,b);
        }
        else if(name=="进路窗外框")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(9,b);
        }
        else if(name=="股道")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(10,b);
        }
        else if(name=="无岔区段")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(11,b);
        }
        else if(name=="区间轨道")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(12,b);
        }
        else if(name=="道岔")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(13,b);
        }
        else if(name=="道岔区段")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(14,b);
        }
        else if(name=="挤岔报警灯")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(15,b);
        }
        else if(name=="报警灯")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(16,b);
        }
        else if(name=="半自动闭塞")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(17,b);
        }
        else if(name=="一般文字")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(18,b);
        }
        else if(name=="绝缘节")
        {
            pVecStationInfo.at(i).pStation->SetShowJYJ(b);
            this->bShowJYJ = b;
        }
        else if(name=="按钮")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(19,b);
        }
        else if(name=="计数器")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(20,b);
        }
        else if(name=="定时器")
        {
            pVecStationInfo.at(i).pStation->SetXHJANShow(21,b);
        }
    }
}
