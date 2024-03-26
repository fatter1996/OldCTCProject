#include "routeprewnd.h"

RoutePreWnd::RoutePreWnd(QObject *parent) : QObject(parent)
{
    juncXHDName = "";
    juncXHDCode = -1;
    pBasePointConst = QPoint(-500,0);
    pBasePoint = pBasePointConst;
    bUpSide = true;//默认上面
    m_bVisible = true;//是否可见
    m_bVisible_m = true;//是否可见
    for(int i=0; i<3; i++)
    {
        preWnd[i] = QRect(-500,0,0,0);
    }
    //for debug
//    for(int i=0; i<2; i++)
//    {
//        RouteInfo route;
//        route.CheCi = "K100"+QString::number(i+1);
//        route.GDName = "IVG";
//        route.m_nKHType = 1;
//        route.routeType = 1;
//        route.routeState = 2;
//        vectRouteInfo.append(route);
//    }
}
//绘制(画笔，计时器，缩放倍数，偏移量)
void RoutePreWnd::Draw(QPainter *painter, long nElapsed, double nDiploid, QPoint offset)
{
    if(!m_bVisible)
        return;

    //范围
    int width = 110;//宽
    int heigh = 25;//高
    int fontHigh = 12;//字体大小

    //区间逻辑检查状态(指示灯)，0隐藏/未知，1启用-绿色，2停用-红色，3异常-黄色
    if(statusQJLJJC)
    {
        if(bUpSide)
            rcLogicLemp = QRect((pBasePointConst.x() + 16) * nDiploid, (pBasePointConst.y() - 96) * nDiploid, 16 * nDiploid, 16 * nDiploid);
        else rcLogicLemp = QRect((pBasePointConst.x() + 16) * nDiploid, (pBasePointConst.y() + 80) * nDiploid, 16 * nDiploid, 16 * nDiploid);

        painter->setPen(Qt::NoPen);
        //不设置渲染器，否则绘制1px会显示2px！
        painter->setRenderHint(QPainter::Antialiasing,false);
        //设置背景
        if(statusQJLJJC == 1)
            painter->setBrush(Qt::green);
        else if(statusQJLJJC == 2)
            painter->setBrush(Qt::red);
        else if(statusQJLJJC == 3)
            painter->setBrush(Qt::yellow);
        //绘制矩形
        painter->drawEllipse(rcLogicLemp);

        if(m_bShowLogicLempText)
        {
            rcLogicLempText = QRect((rcLogicLemp.x() - 72) * nDiploid, (bUpSide ? (rcLogicLemp.y() - 16) : (rcLogicLemp.y() + 16)) * nDiploid, 160 * nDiploid, 16 * nDiploid);
            QFont font;
            font.setFamily("宋体");
            font.setPointSize(10 * nDiploid);//字号
            painter->setFont(font);//设置字体
            painter->setPen(QPen(Qt::yellow, 1));
            if(statusQJLJJC == 1)
                painter->drawText(rcLogicLempText, Qt::AlignCenter, "列控区间占用逻辑检查启用");
            else if(statusQJLJJC == 2)
                painter->drawText(rcLogicLempText, Qt::AlignCenter, "列控区间占用逻辑检查停用");
            else if(statusQJLJJC == 3)
                painter->drawText(rcLogicLempText, Qt::AlignCenter, "列控区间占用逻辑检查异常");
        }
    }

    for(int i=0; i<3; i++)
    {
        if(m_bVisible_m)
        {
            if(bUpSide)
            {
                preWnd[i].setRect(pBasePointConst.x()*nDiploid, (pBasePointConst.y()-heigh*(i+1))*nDiploid, width*nDiploid, heigh*nDiploid);
            }
            else
            {
                preWnd[i].setRect(pBasePointConst.x()*nDiploid, (pBasePointConst.y()+heigh*i)*nDiploid, width*nDiploid, heigh*nDiploid);
            }
            //画笔
            QPen pen1;
            pen1.setWidth(1*nDiploid);
            pen1.setColor(Qt::white);
            pen1.setStyle(Qt::SolidLine);
            painter->setPen(pen1);
            //不设置渲染器，否则绘制1px会显示2px！
            painter->setRenderHint(QPainter::Antialiasing,false);
            //设置背景为透明
            painter->setBrush(Qt::NoBrush);
            //绘制矩形
            painter->drawRect(preWnd[i]);
        }
        //显示车次信息
        if(i<vectRouteInfo.size())
        {
            RouteInfo route = vectRouteInfo[i];
            //字体
            QFont font;
            font.setFamily("宋体");
            font.setPointSize(fontHigh*nDiploid);//字号
            font.setItalic(false);//斜体
            font.setBold(false);//加粗
            painter->setFont(font);//设置字体

            //客车
            if(route.m_nKHType == 1)
            {
                painter->setPen(QPen(LIGHTRED/*Qt::red*/, 1));//设置画笔颜色-红色
            }
            //货车
            else
            {
                painter->setPen(QPen(LIGHTBULE/*Qt::cyan*/, 1));//设置画笔颜色-青色
            }
            QString checi = route.CheCi;
            if(i==0)
            {
                checi.append("*");
            }
            QRect rectLeft = QRect(preWnd[i].x()+1*nDiploid, preWnd[i].y(), preWnd[i].width()*3/5, preWnd[i].height());
            painter->drawText(rectLeft,Qt::AlignLeft|Qt::AlignVCenter,checi);

            //进路状态：0非自触，1自触等待，2进路办理完成，3占用
            QColor color1 = Qt::red;
            switch (route.routeState) {
            case 1: color1 = Qt::yellow; break;
            case 2: color1 = Qt::green; break;
            case 3: color1 = Qt::green; break;
            case 0: color1 = Qt::red; break;
            default: color1 = Qt::red; break;
            }
            painter->setPen(QPen(color1, 1));//设置画笔颜色

            QString gdname = route.GDName;
            //进路类型：1接车，2发车，3通过
            switch (route.routeType) {
            case 1: gdname.prepend("J"); break;
            case 2: gdname.prepend("F"); break;
            case 3: gdname.prepend("T"); break;
            default: break;
            }
            QRect rectRight = QRect(preWnd[i].x()-1*nDiploid+preWnd[i].width()/2, preWnd[i].y(), preWnd[i].width()/2, preWnd[i].height());
            painter->drawText(rectRight,Qt::AlignRight|Qt::AlignVCenter,gdname);
        }
    }
}

bool RoutePreWnd::GetMousePoint(QPoint p)
{
    QRect m_RectWnd;
    if(bUpSide)
    {
        m_RectWnd.setX(preWnd[2].x());
        m_RectWnd.setY(preWnd[2].y());
        m_RectWnd.setWidth(preWnd[2].right() - preWnd[2].left());
        m_RectWnd.setHeight(preWnd[0].bottom() - preWnd[2].top());
    }
    else
    {
        m_RectWnd.setX(preWnd[0].x());
        m_RectWnd.setY(preWnd[0].y());
        m_RectWnd.setWidth(preWnd[0].right() - preWnd[0].left());
        m_RectWnd.setHeight(preWnd[2].bottom() - preWnd[0].top());
    }

    return m_RectWnd.contains(p);
}
