#ifndef MULTISTATIONDISP_H
#define MULTISTATIONDISP_H

#include <QWidget>
#include <QHBoxLayout>
#include <QRect>
#include "MyStation/mystation.h"
namespace Ui {
class MultiStationDisp;
}

struct StationInfo
{
    QString name;
    int stationId = 0;
    QRect route;
    MyStation* pStation = nullptr;
    double diploid = 1;
};

class MultiStationDisp : public QWidget
{
    Q_OBJECT

public:
    explicit MultiStationDisp(QWidget *parent = nullptr);
    ~MultiStationDisp();
    void setDiploid(double* diploid)
    {
        nDiploid = diploid;
        diploidFlag = *nDiploid;
    }
    void AddWidget(MyStation* station, int index);
    bool eventFilter(QObject *obj,QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void ReSize(QRect re);

private:
    Ui::MultiStationDisp *ui;
    QVector<StationInfo> pVecStationInfo;
    int maxWidth = 1920;
    int maxHeight = 1024;
    double* nDiploid = nullptr;
    QWidget* curWidget = nullptr;
    bool isRePos = false;
    bool isReSize = false;
    double diploidFlag = 0;
    QPoint curPoint = { 0,0 };
    QPoint curMousePoint = { 0,0 };
};

#endif // MULTISTATIONDISP_H
