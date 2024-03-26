#include "stationctrldisp.h"

StationCtrlDisp::StationCtrlDisp(QWidget *parent) :
    QWidget(parent)
{

}

StationCtrlDisp::~StationCtrlDisp()
{

}

void StationCtrlDisp::InitStationInfo(MyStation* pMyStation)
{
    pCurrStation = pMyStation;
    if(pCurrStation)
    {
        pCurrStation->GetQJNameList();
    }
}
