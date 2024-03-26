#ifndef STATIONCTRLDISP_H
#define STATIONCTRLDISP_H

#include <QWidget>
#include "MyStation/mystation.h"
#include "menuclass/poorshuntdlg.h"
#include "menuclass/poorshuntdlg_dlg.h"
#include "menuclass/confirmcch.h"
#include "menuclass/confirmcch_dlg.h"
#include "menuclass/changeccproperty.h"
#include "menuclass/changeccproperty_dlg.h"
#include "menuclass/addtrain.h"
#include "menuclass/addtrain_dlg.h"
#include "menuclass/delcheci.h"
#include "menuclass/qftj.h"
#include "menuclass/statechose.h"
#include "menuclass/modechange.h"
#include "menuclass/LeadSealing/keyboard.h"
#include "menuclass/LeadSealing/passwordentrydlg.h"
#include "menuclass/LeadSealing/dialog_password.h"
#include "menuclass/seektrain.h"
#include "menuclass/trainroutehandle.h"
#include "menuclass/trainroutehandle_dlg.h"
#include "menuclass/LogIn/loninwidget.h"
#include "menuclass/LogIn/userexitwidget.h"
#include "menuclass/dcroutetimewidget.h"
#include "menuclass/dcroutetimewidget_dlg.h"
#include "menuclass/operationalarmdialog.h"
#include "StationInterface/StationFunButton/stationfunbuttonwidget.h"
#include "menuclass/textannotationwidget.h"
//单站界面基类
class StationCtrlDisp : public QWidget
{
    Q_OBJECT
public:
    explicit StationCtrlDisp(QWidget *parent = nullptr);
    ~StationCtrlDisp();

    void InitStationInfo(MyStation* pMyStation);
    virtual void StationMenuAuxiliary() = 0;
    virtual void StationViewResize(int nEnlargeNarrow = 0) = 0;
    virtual void TrainNOViewResize(int nEnlargeNarrow = 0) = 0;
    virtual void TrainNumWndShowStateChenge(bool b) = 0;
    //进路预告窗显示状态
    virtual void RoutePreWndShowStateChenge(bool b) = 0;
signals:
    void SendDataToSecerSignal(int nDtatType,int nFunType=-1,int ndevcode=-1,int nDevCode2=-1);
    void SendDataRouteToSecerSignal(int nDtatType,int nFunType,int ndevcode[],QString str="");
    //void SysClosed(bool b);
    void SendTextAnalysisSignal(int index, QPoint pt, QString dev = "");
    void sendRouteOrderMsg(int type, int routeId, int nData = 0, QString strData = "");

public:
    MyStation* pCurrStation = nullptr;
    bool isStationViewReSize = false;
    bool isTrainNumWndReSize = false;
    int nEnlargeNarrow = 0;
};

#endif // STATIONCTRLDISP_H
