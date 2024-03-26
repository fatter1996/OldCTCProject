#include "mystation.h"

#include <QTextCodec>
#include <QtDebug>
#include <windows.h>
#include <qstring.h>
#include <QThread>
//#include <QtConcurrent>
//#include <QFuture>
#include "GlobalHeaders/GlobalFuntion.h"
#include "Log/log.h"


//向服务端请求数据（请求类型）
void MyStation::RequestDataFromServer(int syncType)
{
    QByteArray dataArray;
    int nCount = 10;
    //添加nCount个字节并全部置零
    dataArray.append(nCount, char(0));
    //标志位-数据同步
    dataArray[nCount-1] = FUNCTYPE_SYNC;
    //请求数据类型
    nCount++;
    dataArray.append(1, char(0));
    dataArray[nCount-1] = syncType;
    //数据操作类型
    nCount++;
    dataArray.append(1, char(0));
    dataArray[nCount-1] = SYNC_FLAG_REQUEST;//请求
    //总包数
    nCount += 2;
    dataArray.append(2, char(0));
    dataArray[nCount-1] = 0x01;//请求
    //当前包序号
    nCount += 2;
    dataArray.append(1, char(0));
    dataArray[nCount-1] = 0x01;//请求

    //帧尾4+空白4
    nCount+=8;
    dataArray.append(8, char(0));
    //打包数据
    packHeadAndTail(&dataArray, nCount);
    //qDebug()<<"Request Data dataArray="<<ByteArrayToString(dataArray);
    //信号-发送数据给Server
    emit sendDataToCTCServerSignal2(this, dataArray, nCount);
}
//根据id在数组中查找该阶段计划的索引
int MyStation::GetIndexOfStagePlanInArrayById(int id)
{
    for(int i=0; i<m_ArrayStagePlan.size(); i++)
    {
        if(id == m_ArrayStagePlan[i]->detail_id)
            return i;
    }
    return -1;
}
//根据id在数组中查找该列车进路序列的索引
int MyStation::GetIndexOfTrainRouteOrderInArrayById(int id)
{
    for(int i=0; i<m_ArrayRouteOrder.size(); i++)
    {
        if(id == m_ArrayRouteOrder[i]->route_id)
        {
            return m_ArrayRouteOrder[i]->m_nRowIndex;
        }
        if(m_ArrayRouteOrder[i]->m_bZHJL == 1)
        {
            for(int j=0; j<m_ArrayRouteOrder.at(i)->m_vecSonTrainRouteOrder.size(); j++)
            {
                if(id == m_ArrayRouteOrder.at(i)->m_vecSonTrainRouteOrder[j]->route_id)
                {
                    return m_ArrayRouteOrder.at(i)->m_vecSonTrainRouteOrder[j]->m_nRowIndex;
                }
            }
        }
    }
    return -1;
}
//根据id在数组中查找该行车日志的索引
int MyStation::GetIndexOfTrafficLogInArrayById(int id)
{
    for(int i=0; i<m_ArrayTrafficLog.size(); i++)
    {
        if(id == m_ArrayTrafficLog[i]->log_id)
            return i;
    }
    return -1;
}
//根据id在数组中查找该调度命令的索引
int MyStation::GetIndexOfDisOrderInArrayById(int id)
{
    for(int i=0; i<m_ArrayDisOrderRecv.size(); i++)
    {
        if(id == m_ArrayDisOrderRecv[i]->order_id)
            return i;
    }
    return -1;
}
//根据id在数组中查找该调度台调度命令的索引
int MyStation::GetIndexOfDisOrderDDTInArrayById(int id)
{
    for(int i=0; i<m_ArrayDisOrderDisp.size(); i++)
    {
        if(id == m_ArrayDisOrderDisp[i]->order_id)
            return i;
    }
    return -1;
}
//根据id在数组中查找该机车调度命令的索引
int MyStation::GetIndexOfDisOrderJCInArrayById(int id)
{
    for(int i=0; i<m_ArrayDisOrderLocomot.size(); i++)
    {
        if(id == m_ArrayDisOrderLocomot[i]->order_id)
            return i;
    }
    return -1;
}
//更新收到的阶段计划信息（功能码-同步标志）
void MyStation::updateStagePlan(QByteArray recvArray,int syncFlag)
{
    //删除所有
    if(SYNC_FLAG_DELALL == syncFlag)
    {
        m_ArrayStagePlan.clear();
        return;
    }

    int nCount = 16;
    //int id = RecvArrayToInt((unsigned char*)recvArray.data()[nCount], 2);
    int id = ByteArrayToUInt(recvArray.mid(nCount,2));
    StagePlan *stagePlan = nullptr;
    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        stagePlan = new StagePlan();
        m_ArrayStagePlan.append(stagePlan);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        int idx = GetIndexOfStagePlanInArrayById(id);
        if(idx > -1)
        {
            stagePlan = m_ArrayStagePlan[idx];
        }
        else
        {
            return;
        }
    }
    //删除
    else if(SYNC_FLAG_DELETE == syncFlag)
    {
        int idx = GetIndexOfStagePlanInArrayById(id);
        if(idx > -1)
        {
            m_ArrayStagePlan.removeAt(idx);
        }
        return;
    }

    stagePlan->station_id = this->getStationID();
    //阶段计划详情id
    stagePlan->detail_id = id;
    nCount+=2;
    //计划号
    stagePlan->m_nPlanNumber = ByteArrayToUInt(recvArray.mid(nCount,4));
    nCount+=4;
    //到达车次长度
    int lenDDCC = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //到达车次
    QString strDDCC;
    //strDDCC.prepend(recvArray.mid(nCount,lenDDCC));
    strDDCC = ByteArrayToUnicode(recvArray.mid(nCount,lenDDCC));
    stagePlan->m_strReachTrainNum = strDDCC;
    nCount += lenDDCC;
    //出发车次长度
    int lenCFCC = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //出发车次
    QString strCFCC;
    //strCFCC.prepend(recvArray.mid(nCount,lenCFCC));
    strCFCC = ByteArrayToUnicode(recvArray.mid(nCount,lenCFCC));
    stagePlan->m_strDepartTrainNum = strCFCC;
    nCount += lenCFCC;
    //到达股道
    //stagePlan->m_nRecvTrainTrack = recvArray[nCount] | recvArray[nCount+1]<<8;
    stagePlan->m_nRecvTrainTrack = ByteArrayToUInt(recvArray.mid(nCount,2));
    stagePlan->m_strRecvTrainTrack = GetStrNameByCode(stagePlan->m_nRecvTrainTrack);
    nCount+=2;
    //到达时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"到达时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        stagePlan->m_timProvReachStation = dateTime;
    }
    nCount += 7;
    //发车股道
    //stagePlan->m_nDepartTrainTrack = recvArray[nCount] | recvArray[nCount+1]<<8;
    stagePlan->m_nDepartTrainTrack = ByteArrayToUInt(recvArray.mid(nCount,2));
    stagePlan->m_strDepartTrainTrack = GetStrNameByCode(stagePlan->m_nDepartTrainTrack);
    nCount += 2;
    //发车时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"出发时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        stagePlan->m_timProvDepaTrain = dateTime;
    }
    nCount += 7;
    //始发终到标志
    stagePlan->m_btBeginOrEndFlg = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //接车口/进站口
    //stagePlan->m_nCodeReachStaEquip = recvArray[nCount] | recvArray[nCount+1]<<8;
    stagePlan->m_nCodeReachStaEquip = ByteArrayToUInt(recvArray.mid(nCount,2));
    stagePlan->m_strXHD_JZk = GetStrNameByCode(stagePlan->m_nCodeReachStaEquip);
    nCount += 2;
    //发车口/出站口
    //stagePlan->m_nCodeDepartStaEquip = recvArray[nCount] | recvArray[nCount+1]<<8;
    stagePlan->m_nCodeDepartStaEquip = ByteArrayToUInt(recvArray.mid(nCount,2));
    stagePlan->m_strXHD_CZk = GetStrNameByCode(stagePlan->m_nCodeDepartStaEquip);
    nCount += 2;
    //电力机车  //recvArray[nCount] == 0x11
    stagePlan->m_bElectric = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount += 1;
    //超限
    stagePlan->m_nLevelCX = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //列车客货类型
    if((int)(recvArray[nCount]&0xFF) == 0x01)
    {
        stagePlan->m_nLHFlg = LCTYPE_KC;
    }
    else
    {
        stagePlan->m_nLHFlg = LCTYPE_HC;
    }
    nCount += 1;
    //列车类型序号（管内动车组、通勤列车等）
    stagePlan->m_nIndexLCLX = (int)(recvArray[nCount]&0xFF);
    stagePlan->m_strLCLX = GetTrainType(stagePlan->m_nIndexLCLX);
    nCount += 1;
    //运行类型序号（动车组、快速旅客列车等）
    stagePlan->m_nIndexYXLX = (int)(recvArray[nCount]&0xFF);
    stagePlan->m_strYXLX = GetTrainRunType(stagePlan->m_nIndexYXLX);
    nCount += 1;
    //线路所标志
    stagePlan->bXianLuSuo = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //计划接收时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"计划接收时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        stagePlan->m_timJHRcv = dateTime;
    }
    nCount += 7;
    //调度台名称长度
    int lenDDT = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //调度台名称
    QString strDDT;
    //strDDT.prepend(recvArray.mid(nCount,lenDDT));
    strDDT = ByteArrayToUnicode(recvArray.mid(nCount,lenDDT));
    stagePlan->m_strDispatchDesk = strDDT;
    nCount += lenDDT;
    //阶段计划类型（增加或删除）
    stagePlan->m_btStagePlanKind = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //签收状态
    stagePlan->m_nStateSignPlan = (int)(recvArray[nCount]&0xFF);

    //线路所处理
    if(stagePlan->bXianLuSuo)
    {
        stagePlan->m_strRecvTrainTrack = stagePlan->m_strXHD_JZk + "-" + stagePlan->m_strXHD_CZk;
        stagePlan->m_strDepartTrainTrack = stagePlan->m_strRecvTrainTrack;
    }

    //
    if(stagePlan->m_nStateSignPlan >= 2)
    {
        emit RecSeverStagePlanSign();
    }
    nCount += 1;
}
//更新收到的列车进路序列信息（功能码-同步标志）
void MyStation::updateTrainRouteOrder(QByteArray recvArray, int syncFlag)
{
    //删除所有
    if(SYNC_FLAG_DELALL == syncFlag)
    {
        m_ArrayRouteOrder.clear();
        return;
    }

    int nCount = 16;
    //int id = RecvArrayToInt((unsigned char*)recvArray.data()[nCount], 2);
    int id = ByteArrayToUInt(recvArray.mid(nCount,2));
    TrainRouteOrder *pTrainRouteOrder = nullptr;
    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        pTrainRouteOrder = new TrainRouteOrder();
        m_ArrayRouteOrder.append(pTrainRouteOrder);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        pTrainRouteOrder = GetTrainRouteOrderById(id);
        if(!pTrainRouteOrder)
            return;
    }
    //删除
    else if(SYNC_FLAG_DELETE == syncFlag)
    {
        pTrainRouteOrder = GetTrainRouteOrderById(id);
        if(pTrainRouteOrder)
        {
            if(pTrainRouteOrder->father_id != 0)
            {
                TrainRouteOrder* pTrainRouteOrder2 = GetTrainRouteOrderById(pTrainRouteOrder->father_id);
                pTrainRouteOrder2->m_vecSonTrainRouteOrder.removeOne(pTrainRouteOrder);
            }
            else m_ArrayRouteOrder.removeOne(pTrainRouteOrder);
        }
        return;
    }

    pTrainRouteOrder->station_id = this->getStationID();
    //进路序列id
    pTrainRouteOrder->route_id = id;
    nCount+=2;
    //计划号
    pTrainRouteOrder->m_nPlanNumber = ByteArrayToUInt(recvArray.mid(nCount,4));
    nCount+=4;
    //始发终到标志
    pTrainRouteOrder->m_btBeginOrEndFlg = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //到达/出发标志
    pTrainRouteOrder->m_btRecvOrDepart = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //车次长度
    int lenCC = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //车次
    QString strCC;
    strCC.prepend(recvArray.mid(nCount,lenCC));
    pTrainRouteOrder->m_strTrainNum = strCC;
    nCount += lenCC;
    //股道设备号
    pTrainRouteOrder->m_nTrackCode = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrainRouteOrder->m_strTrack = GetStrNameByCode(pTrainRouteOrder->m_nTrackCode);
    nCount+=2;
    //计划时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"计划时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrainRouteOrder->m_timPlanned = dateTime;
    }
    nCount += 7;
    //开始时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"开始时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrainRouteOrder->m_timBegin = dateTime;
    }
    nCount += 7;
    //进站口信号机设备号
    pTrainRouteOrder->m_nCodeReachStaEquip = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrainRouteOrder->m_strXHD_JZk = GetStrNameByCode(pTrainRouteOrder->m_nCodeReachStaEquip);
    nCount += 2;
    //出站口信号机设备号
    pTrainRouteOrder->m_nCodeDepartStaEquip = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrainRouteOrder->m_strXHD_CZk = GetStrNameByCode(pTrainRouteOrder->m_nCodeDepartStaEquip);
    nCount += 2;
    //自触模式
    pTrainRouteOrder->m_nAutoTouch = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount++;
    //电力
    pTrainRouteOrder->m_bElectric = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount++;
    //超限等级
    pTrainRouteOrder->m_nLevelCX = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //列车类型（客车/货车）
    if((int)(recvArray[nCount]&0xFF) == 0x01)
    {
        pTrainRouteOrder->m_nLHFlg = LCTYPE_KC;
    }
    else
    {
        pTrainRouteOrder->m_nLHFlg = LCTYPE_HC;
    }
    nCount++;
    //线路所标志
    pTrainRouteOrder->bXianLuSuo = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount++;
    //进路描述（"-"分割）*************
    //长度
    int lenDescrip = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //内容
    QString strDescrip;
    strDescrip.prepend(recvArray.mid(nCount,lenDescrip));
    pTrainRouteOrder->m_strRouteDescrip = strDescrip;
    nCount += lenDescrip;
    //进路描述（","分割）*************
    //长度
    lenDescrip = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //内容
    strDescrip = "";
    strDescrip.prepend(recvArray.mid(nCount,lenDescrip));
    pTrainRouteOrder->m_strRouteDescripReal = strDescrip;
    nCount += lenDescrip;
    //进路当前状态
    pTrainRouteOrder->m_nOldRouteState = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //进路当前状态描述*************
    //长度
    int lenCurrDes = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //内容
    QString strCurrDes;
    //strCurrDes.prepend(recvArray.mid(nCount,lenCurrDes));
    strCurrDes = ByteArrayToUnicode(recvArray.mid(nCount,lenCurrDes));
    pTrainRouteOrder->m_strRouteState = strCurrDes;
    nCount += lenCurrDes;
    //进路方向*************
    //长度
    int lenDir = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //内容
    QString strDir;
    //strDir.prepend(recvArray.mid(nCount,lenDir));
    //strDir = ByteArrayToUnicode(recvArray.mid(nCount,lenDir));
    strDir = QString::fromLocal8Bit(recvArray.mid(nCount,lenDir));
    pTrainRouteOrder->m_strDirection = strDir;
    nCount += lenDir;
    //是延续进路
    pTrainRouteOrder->m_bYXJL = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount++;
    //是变通进路
    pTrainRouteOrder->m_bBTJL = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount++;
    //变通进路数组，用“|”连接 *************
    //长度
    int lenBTArr = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //内容
    QString strBTjl;
    strBTjl.prepend(recvArray.mid(nCount,lenBTArr));
    pTrainRouteOrder->strArrayBTJL = strBTjl.split("|");
    nCount += lenBTArr;
    //删除标志
    pTrainRouteOrder->m_bDeleteFlag = ((int)recvArray[nCount] != 0x00) ? true : false;
    nCount++;
    QStringList routBtnList = pTrainRouteOrder->m_strRouteDescripReal.split(",");
    int btnCount = routBtnList.size();
    if(btnCount >= 2)
    {
        pTrainRouteOrder->m_strXHDBegin = routBtnList[0];
        pTrainRouteOrder->m_strXHDEnd = routBtnList[btnCount-1];
    }
    //是组合进路
    pTrainRouteOrder->m_bZHJL = ((int)recvArray[nCount] != 0x00) ? 1 : 0;
    nCount++;
    //组合进路父id
    pTrainRouteOrder->father_id = ByteArrayToUInt(recvArray.mid(nCount,2));
    nCount+=2;
    //是否人工办理进路时生成
    pTrainRouteOrder->m_bCreateByMan = ((int)recvArray[nCount] != 0x00) ? 1 : 0;
    nCount++;

    //线路所处理
    if(pTrainRouteOrder->bXianLuSuo)
    {
        pTrainRouteOrder->m_strTrack = pTrainRouteOrder->m_strRouteDescrip;
    }

    initCombinedTrainRouteOrder();
//    qDebug() << syncFlag << pTrainRouteOrder->m_strTrainNum
//             << pTrainRouteOrder->m_btBeginOrEndFlg << pTrainRouteOrder->m_btRecvOrDepart
//             << pTrainRouteOrder->m_nLevelCX;
}
//更新收到的行车日志信息（功能码-同步标志）
void MyStation::updateTrafficLog(QByteArray recvArray, int syncFlag)
{
    //删除所有
    if(SYNC_FLAG_DELALL == syncFlag)
    {
        m_ArrayTrafficLog.clear();
        return;
    }

    int nCount = 16;
    int id = ByteArrayToUInt(recvArray.mid(nCount,2));
    TrafficLog *pTrafficLog = nullptr;
    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        pTrafficLog = new TrafficLog();
        m_ArrayTrafficLog.append(pTrafficLog);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        int idx = GetIndexOfTrafficLogInArrayById(id);
        if(idx > -1)
        {
            pTrafficLog = m_ArrayTrafficLog[idx];
        }
        else
        {
            return;
        }
    }
    //删除
    else if(SYNC_FLAG_DELETE == syncFlag)
    {
        int idx = GetIndexOfTrafficLogInArrayById(id);
        if(idx > -1)
        {
            m_ArrayTrafficLog.removeAt(idx);
        }
        return;
    }

    pTrafficLog->station_id = this->getStationID();
    pTrafficLog->m_strStaName = this->getStationName();
    //进路序列id
    pTrafficLog->log_id = id;
    nCount+=2;
    //计划号
    pTrafficLog->m_nPlanNumber = ByteArrayToUInt(recvArray.mid(nCount,4));
    nCount+=4;
    //始发终到标志
    pTrafficLog->m_btBeginOrEndFlg = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //到达车次长度
    int lenDDCC = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //到达车次
    QString strDDCC;
    strDDCC = ByteArrayToUnicode(recvArray.mid(nCount,lenDDCC));
    pTrafficLog->m_strReachTrainNum = strDDCC;
    pTrafficLog->m_strReachTrainNumOld = pTrafficLog->m_strReachTrainNum;
    nCount += lenDDCC;
    //到达股道
    int recvTrainTrackCode = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrafficLog->m_strRecvTrainTrack = GetStrNameByCode(recvTrainTrackCode);
    nCount+=2;
    //计划到站时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"计划到站时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timProvReachStation = dateTime;
    }
    nCount += 7;
    //实际到站时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"实际到站时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timRealReachStation = dateTime;
    }
    nCount += 7;
    //同意邻站发车时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"同意邻站发车时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timAgrFromAdjtStaDepaTrain = dateTime;
    }
    nCount += 7;
    //邻站发车时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"邻站发车时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timFromAdjtStaDepaTrain = dateTime;
    }
    nCount += 7;
    //进站口信号机设备号
    pTrafficLog->m_nCodeReachStaEquip = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrafficLog->m_strXHD_JZk = GetStrNameByCode(pTrafficLog->m_nCodeReachStaEquip);
    nCount += 2;
    //到达车次是上行车次
    pTrafficLog->m_bReachTrainNumSX = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;

    //出发车次长度
    int lenCFCC = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //出发车次
    QString strCFCC;
    strCFCC = ByteArrayToUnicode(recvArray.mid(nCount,lenCFCC));
    pTrafficLog->m_strDepartTrainNum = strCFCC;
    pTrafficLog->m_strDepartTrainNumOld = pTrafficLog->m_strDepartTrainNum;
    nCount += lenCFCC;
    //出发股道设备号
    int departTrainTrackCode = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrafficLog->m_strDepartTrainTrack = GetStrNameByCode(departTrainTrackCode);
    nCount+=2;
    //计划出发时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"计划出发时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timProvDepaTrain = dateTime;
    }
    nCount += 7;
    //实际出发时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"实际出发时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timRealDepaTrain = dateTime;
    }
    nCount += 7;
    //邻站同意发车时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"邻站同意发车时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timToAdjtStaAgrDepaTrain = dateTime;
    }
    nCount += 7;
    //到达邻站时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        //qDebug()<<"到达邻站时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timtoAdjtStation = dateTime;
    }
    nCount += 7;
    //出站口信号机设备号
    pTrafficLog->m_nCodeDepartStaEquip = ByteArrayToUInt(recvArray.mid(nCount,2));
    pTrafficLog->m_strXHD_CZk = GetStrNameByCode(pTrafficLog->m_nCodeDepartStaEquip);
    nCount += 2;
    //出发车次是上行车次
    pTrafficLog->m_bDepartTrainNumSX = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;

    //删除标志
    pTrafficLog->m_bDeleteFlag = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //电力
    pTrafficLog->m_bElectric = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //超限等级
    pTrafficLog->m_nLevelCX = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //列车类型（客车/货车）
    pTrafficLog->m_nLHFlg = (int)(recvArray[nCount]&0xFF) == 0x01 ? LCTYPE_KC : LCTYPE_HC;
    nCount += 1;
    //列车类型（管内动车组、通勤列车等）
    pTrafficLog->m_nIndexLCLX = (int)(recvArray[nCount]&0xFF);
    pTrafficLog->m_strLCLX = GetTrainType(pTrafficLog->m_nIndexLCLX);
    nCount += 1;
    //运行类型序号（动车组、快速旅客列车等）
    pTrafficLog->m_nIndexYXLX = (int)(recvArray[nCount]&0xFF);
    pTrafficLog->m_strYXLX = GetTrainRunType(pTrafficLog->m_nIndexYXLX);
    nCount += 1;
    //线路所标志
    pTrafficLog->bXianLuSuo = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;

    //计划执行完毕标志
    pTrafficLog->m_nExecuteFlag = (int)(recvArray[nCount]&0xFF);
    nCount += 1;

    //办理客运
    pTrafficLog->m_bBLKY = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //允许股道与固定径路不同
    pTrafficLog->m_bAllowGDNotMatch = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //允许出入口与固定进路不同
    pTrafficLog->m_bAllowCRKNotMatch = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //军运
    pTrafficLog->m_bArmy = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;
    //重点
    pTrafficLog->m_bImportant = ((int)(recvArray[nCount]&0xFF) != 0x00) ? true : false;
    nCount += 1;

    //来向名称长度
    int len = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //来向
    QString strTxt;
    strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
    pTrafficLog->m_strFromAdjtStation = strTxt;
    nCount += len;
    //去向名称长度
    len = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //去向
    strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
    pTrafficLog->m_strToAdjtStation = strTxt;
    nCount += len;

    //记事长度
    len = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //记事
    strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
    pTrafficLog->m_strNotes = strTxt;
    nCount += len;

    //计划类型长度
    len = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //计划类型
    strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
    pTrafficLog->m_strTypeFlag = strTxt;
    nCount += len;

    //计划检查校验状态
    pTrafficLog->m_nCheckState = (int)(recvArray[nCount]&0xFF);
    nCount += 1;

    //交令状态（占线板）
    pTrafficLog->m_btJALStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //列检状态（占线板）
    pTrafficLog->m_btLJStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //上水状态（占线板）
    pTrafficLog->m_btSSStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //吸污状态（占线板）
    pTrafficLog->m_btXWStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //交票状态（占线板）
    pTrafficLog->m_btJPStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //乘降状态（占线板）
    pTrafficLog->m_btCJStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //摘挂状态（占线板）
    pTrafficLog->m_btZGStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //列尾状态（占线板）
    pTrafficLog->m_btLWStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //货检状态（占线板）
    pTrafficLog->m_btHJStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //换乘状态（占线板）
    pTrafficLog->m_btHCStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //装卸状态（占线板）
    pTrafficLog->m_btZXStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //机车状态（占线板）
    pTrafficLog->m_btJCStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //道口状态（占线板）
    pTrafficLog->m_btDKStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //车号状态（占线板）
    pTrafficLog->m_btCHStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //综控状态（占线板）
    pTrafficLog->m_btZKStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //站务状态（占线板）
    pTrafficLog->m_btZWStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //接车进路状态（占线板）
    pTrafficLog->m_btJCJLStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //发车进路状态（占线板）
    pTrafficLog->m_btFCJLStatus = (int)(recvArray[nCount]&0xFF);
    nCount += 1;

    //列车位置信息（占线板）长度
    len = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //列车位置信息（占线板）
    strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
    pTrafficLog->m_strTrainPosStatus = strTxt;
    nCount += len;

    //计划下一个流程（占线板）长度
    len = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //计划下一个流程（占线板）
    strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
    pTrafficLog->m_strProc = strTxt;
    nCount += len;

    //图定到站时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));
        int second = 0;

        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timChartReachStation = dateTime;
    }
    nCount += 7;

    //图定出发时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));
        int second = 0;

        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pTrafficLog->m_timChartDepaTrain = dateTime;
    }
    nCount += 7;

    //其他信息处理
    pTrafficLog->m_strTrainNum = pTrafficLog->m_strReachTrainNum!=""?pTrafficLog->m_strReachTrainNum:pTrafficLog->m_strDepartTrainNum;
    pTrafficLog->m_strTrainTrack = pTrafficLog->m_strReachTrainNum!=""?pTrafficLog->m_strRecvTrainTrack:pTrafficLog->m_strDepartTrainTrack;
    pTrafficLog->m_bNewTrafficLog = true;

    //线路所处理
    if(pTrafficLog->bXianLuSuo)
    {
        pTrafficLog->m_strTrainTrack = pTrafficLog->m_strXHD_JZk + "-" + pTrafficLog->m_strXHD_CZk;
        pTrafficLog->m_strRecvTrainTrack = pTrafficLog->m_strTrainTrack;
        pTrafficLog->m_strDepartTrainTrack = pTrafficLog->m_strTrainTrack;
    }
}
//更新收到的调度命令信息（功能码-同步标志）
void MyStation::updateDisOrder(QByteArray recvArray, int syncFlag)
{
    int nCount = 16;
    int orderType = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //车站调度命令
    if(0x01 == orderType)
    {
        this->updateDisOrderStation(recvArray,syncFlag);
        emit updateDisOrderSignal();
    }
    //调度台调度命令
    else if(0x02 == orderType)
    {
        this->updateDisOrderDispatcher(recvArray,syncFlag);
        emit updateDisOrderSignal();
    }
    //机车调度命令
    else if(0x03 == orderType)
    {
        this->updateDisOrderLocomotive(recvArray,syncFlag);
        emit updateDisOrderSignal();
    }
}
//更新收到的车站调度命令信息（功能码-同步标志）
void MyStation::updateDisOrderStation(QByteArray recvArray, int syncFlag)
{
    //删除所有
    if(SYNC_FLAG_DELALL == syncFlag)
    {
        m_ArrayDisOrderRecv.clear();
        return;
    }

    int nCount = 17;
    int id = ByteArrayToUInt(recvArray.mid(nCount,2));
    nCount+=2;
    DispatchOrderStation *pDisOrderSta = nullptr;
    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        pDisOrderSta = new DispatchOrderStation();
        m_ArrayDisOrderRecv.append(pDisOrderSta);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        int idx = GetIndexOfDisOrderInArrayById(id);
        if(idx > -1)
        {
            pDisOrderSta = m_ArrayDisOrderRecv[idx];
        }
        else
        {
            return;
        }
    }
    //删除
    else if(SYNC_FLAG_DELETE == syncFlag)
    {
        int idx = GetIndexOfDisOrderInArrayById(id);
        if(idx > -1)
        {
            m_ArrayDisOrderRecv.removeAt(idx);
        }
        return;
    }

    pDisOrderSta->station_id = this->getStationID();
    //id
    pDisOrderSta->order_id = id;
    //命令号
    pDisOrderSta->uNumber = ByteArrayToUInt(recvArray.mid(nCount,4));
    pDisOrderSta->strNumber = QString::number(pDisOrderSta->uNumber);
    nCount+=4;
    //接收时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//        qDebug()<<"接收时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pDisOrderSta->timOrder = dateTime;
        nCount += 7;
    }
    //调度中心
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderSta->strDisCenter = strTxt;
        nCount += len;
    }
    //调度员/发令人
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderSta->strDisName = strTxt;
        nCount += len;
    }
    //命令类型
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderSta->strType = strTxt;
        nCount += len;
    }
    //命令内容
    {
        //长度
        //int len = (int)(recvArray[nCount]&0xFF);
        int len = (int)(recvArray[nCount]&0xFF);
        len=len+(int)((recvArray[nCount+1]&0xFF) << 8);
        nCount += 2;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderSta->strContent = strTxt;
        nCount += len;
    }
    //接收单位
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderSta->listRecvPlace = strTxt.split("|");
        nCount += len;
    }
    //签收状态
    pDisOrderSta->nStateDisOrder = (int)(recvArray[nCount]&0xFF);
    //判断该调度命令是否已签收、目前只使用调度命令在占线版签收情况
    if(pDisOrderSta->nStateDisOrder==3)
    {
        emit RecSeverDDMLSign();
    }
    nCount += 1;
    //签收人
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        if(len>0)
        {
            //内容
            QString strTxt;
            strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
            pDisOrderSta->strSignName = strTxt;
            nCount += len;
        }
    }
    //签收时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//        qDebug()<<"签收时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pDisOrderSta->timSign = dateTime;
        nCount += 7;
    }
    //阅读人数组
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        if(len>0)
        {
            //内容
            QString strTxt;
            strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
            pDisOrderSta->listReadName = strTxt.split("|");
            nCount += len;
        }
    }
    int a;
    a=0;
}
//更新收到的调度台调度命令信息（功能码-同步标志）
void MyStation::updateDisOrderDispatcher(QByteArray recvArray, int syncFlag)
{
    //删除所有
    if(SYNC_FLAG_DELALL == syncFlag)
    {
        m_ArrayDisOrderDisp.clear();
        return;
    }

    int nCount = 17;
    int id = ByteArrayToUInt(recvArray.mid(nCount,2));
    nCount+=2;
    DispatchOrderDispatcher *pDisOrderDDT = nullptr;
    bool border=false;
    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        pDisOrderDDT = new DispatchOrderDispatcher();
        m_ArrayDisOrderDisp.append(pDisOrderDDT);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        int idx = GetIndexOfDisOrderDDTInArrayById(id);
        if(idx > -1)
        {
            // pDisOrderDDT = m_ArrayDisOrderDisp[idx];
            m_ArrayDisOrderDisp.move(idx,m_ArrayDisOrderDisp.size()-1);
            pDisOrderDDT = m_ArrayDisOrderDisp[m_ArrayDisOrderDisp.size()-1];
            border=true;
        }
        else
        {
            return;
        }
    }
    //删除
    else if(SYNC_FLAG_DELETE == syncFlag)
    {
        int idx = GetIndexOfDisOrderDDTInArrayById(id);
        if(idx > -1)
        {
            m_ArrayDisOrderDisp.removeAt(idx);
        }
        return;
    }

    pDisOrderDDT->station_id = this->getStationID();
    //id
    pDisOrderDDT->order_id = id;
    //命令号
    pDisOrderDDT->uNumber = ByteArrayToUInt(recvArray.mid(nCount,4));
    nCount+=4;
    //命令类型
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderDDT->strType = strTxt;
        nCount += len;
    }
    //命令内容
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        len=len+(int)((recvArray[nCount+1]&0xFF) << 8);
        nCount += 2;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderDDT->strContent = strTxt;
        nCount += len;
    }
    //车站
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderDDT->strStation = strTxt;
        nCount += len;
    }
    //值班人
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderDDT->strDutyName = strTxt;
        nCount += len;
    }
    //创建时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        //int second = 0;//(int)recvArray[nCount+6];
        int second = (int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//        qDebug()<<"创建时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pDisOrderDDT->timCreate = dateTime;
        nCount += 7;
    }
    //发送状态
    int nSend = (int)(recvArray[nCount]&0xFF);
    pDisOrderDDT->bSend = (nSend==1)?true:false;
    nCount += 1;
    //发送时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        //int second = 0;//(int)recvArray[nCount+6];
        int second = (int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//        qDebug()<<"发送时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pDisOrderDDT->timSend = dateTime;
        nCount += 7;
    }
    //调度台个数
    int ddtCount = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //调度台内容
    for(int i=0; i<ddtCount; i++)
    {
        DispatcherInfo ddtInfo;
        //调度台
        {
            //长度
            int len = (int)(recvArray[nCount]&0xFF);
            nCount += 1;
            if(len>0)
            {
                //内容
                QString strTxt;
                strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
                ddtInfo.strDispatcher = strTxt;
                nCount += len;
            }
        }
        //接收状态
        {
            ddtInfo.nRecvState = (int)(recvArray[nCount]&0xFF);
            nCount += 1;
        }
        //调度台接收时间
        {
            int year  = ByteArrayToUInt(recvArray.mid(nCount,2));
            int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));
            int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));
            int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));
            int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));
            //int second = 0;//(int)recvArray[nCount+6];
            int second = (int)recvArray[nCount+6];
            //"2019-03-31 12:24:36";
            //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
            QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//            qDebug()<<"签收时间="<<strDateTime;
            QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
            ddtInfo.timRecv = dateTime;
            nCount += 7;
        }
        //增加一条
        if(!border)
        {
           pDisOrderDDT->vectDispathInfo.append(ddtInfo);
        }
        else //更新数据
        {
            for(int k=0;k<pDisOrderDDT->vectDispathInfo.size();k++)
            {
                if(pDisOrderDDT->vectDispathInfo[k].strDispatcher==ddtInfo.strDispatcher)
                {
                  pDisOrderDDT->vectDispathInfo[k]=ddtInfo;
                }
            }
        }
    }
}
//更新收到的机车调度命令信息（功能码-同步标志）
void MyStation::updateDisOrderLocomotive(QByteArray recvArray, int syncFlag)
{
    //删除所有
    if(SYNC_FLAG_DELALL == syncFlag)
    {
        m_ArrayDisOrderLocomot.clear();
        return;
    }

    int nCount = 17;
    int id = ByteArrayToUInt(recvArray.mid(nCount,2));
    nCount+=2;
    DispatchOrderLocomotive *pDisOrderJC = nullptr;
    bool border=false;
    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        pDisOrderJC = new DispatchOrderLocomotive();
        m_ArrayDisOrderLocomot.append(pDisOrderJC);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        int idx = GetIndexOfDisOrderJCInArrayById(id);
        if(idx > -1)
        {
            //pDisOrderJC = m_ArrayDisOrderLocomot[idx];
            m_ArrayDisOrderLocomot.move(idx,m_ArrayDisOrderLocomot.size()-1);
            pDisOrderJC = m_ArrayDisOrderLocomot[m_ArrayDisOrderLocomot.size()-1];
            border=true;
        }
        else
        {
            return;
        }
    }
    //删除
    else if(SYNC_FLAG_DELETE == syncFlag)
    {
        int idx = GetIndexOfDisOrderJCInArrayById(id);
        if(idx > -1)
        {
            m_ArrayDisOrderLocomot.removeAt(idx);
        }
        return;
    }

    pDisOrderJC->station_id = this->getStationID();
    //id
    pDisOrderJC->order_id = id;
    //命令种类
    pDisOrderJC->orderType = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //命令号
    pDisOrderJC->uNumber = ByteArrayToUInt(recvArray.mid(nCount,4));
    nCount+=4;
    //命令类型
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        //路票文字特殊处理
        if(pDisOrderJC->orderType==2)
        {
          pDisOrderJC->strType = "路票";
        }
        else
        {
          pDisOrderJC->strType = strTxt;
        }

        nCount += len;
    }
    //命令内容
    {
        //长度
//        int len = (int)(recvArray[nCount]&0xFF);
//        nCount += 1;
        int len = (int)(recvArray[nCount]&0xFF);
        len=len+(int)((recvArray[nCount+1]&0xFF) << 8);
        nCount += 2;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderJC->strContent = strTxt;
        nCount += len;
    }
    //车站
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderJC->strStation = strTxt;
        nCount += len;
    }
    //值班人
    {
        //长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        //内容
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
        pDisOrderJC->strDutyName = strTxt;
        nCount += len;
    }
    //创建时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        //int second = 0;//(int)recvArray[nCount+6];
        int second =(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//        qDebug()<<"创建时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pDisOrderJC->timCreate = dateTime;
        nCount += 7;
    }
    //发送状态
    int nSend = (int)(recvArray[nCount]&0xFF);
    pDisOrderJC->bSend = (nSend==1)?true:false;
    nCount += 1;
    //发送时间
    {
        int year  = ByteArrayToUInt(recvArray.mid(nCount,2));//(int)(recvArray[nCount] | recvArray[nCount+1]<<8);
        int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));//(int)recvArray[nCount+2];
        int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));//(int)recvArray[nCount+3];
        int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));//(int)recvArray[nCount+4];
        int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));//(int)recvArray[nCount+5];
        int second = 0;//(int)recvArray[nCount+6];
        //"2019-03-31 12:24:36";
        //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
        QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//        qDebug()<<"发送时间="<<strDateTime;
        QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
        pDisOrderJC->timSend = dateTime;
        nCount += 7;
    }
    //车次（机车）个数
    int jcCount = (int)(recvArray[nCount]&0xFF);
    nCount += 1;
    //车次（机车）内容
    for(int i=0; i<jcCount; i++)
    {
        LocomotiveInfo jcInfo;
        //车次
        {
            //长度
            int len = (int)(recvArray[nCount]&0xFF);
            nCount += 1;
            if(len>0)
            {
                //内容
                QString strTxt;
                strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
                jcInfo.strCheCi = strTxt;
                nCount += len;
            }
        }
        //机车
        {
            //长度
            int len = (int)(recvArray[nCount]&0xFF);
            nCount += 1;
            if(len>0)
            {
                //内容
                QString strTxt;
                strTxt = ByteArrayToUnicode(recvArray.mid(nCount,len));
                jcInfo.strLocomotive = strTxt;
                nCount += len;
            }
        }
        //签收状态
        {
            jcInfo.nRecvState = (int)(recvArray[nCount]&0xFF);
            nCount += 1;
        }
        //机车接收时间
        {
            int year  = ByteArrayToUInt(recvArray.mid(nCount,2));
            int mouth = ByteArrayToUInt(recvArray.mid(nCount+2,1));
            int day   = ByteArrayToUInt(recvArray.mid(nCount+3,1));
            int hour  = ByteArrayToUInt(recvArray.mid(nCount+4,1));
            int mini  = ByteArrayToUInt(recvArray.mid(nCount+5,1));
            int second = 0;//(int)recvArray[nCount+6];
            //"2019-03-31 12:24:36";
            //QString strDateTime= QString("%1-%2-%3 %4:%5:%6").arg(year).arg(mouth).arg(day).arg(hour).arg(mini).arg(second);
            QString strDateTime= TransfrmFullDateTimeString(year,mouth,day,hour,mini,second);
//            qDebug()<<"签收时间="<<strDateTime;
            QDateTime dateTime = QDateTime::fromString(strDateTime, "yyyy-MM-dd hh:mm:ss");
            jcInfo.timRecv = dateTime;
            nCount += 7;
        }
        //增加一条
        if(!border)
        {
          pDisOrderJC->vectLocmtInfo.append(jcInfo);
        }
        else //更新数据
        {
            for(int k=0;k<pDisOrderJC->vectLocmtInfo.size();k++)
            {
                if(pDisOrderJC->vectLocmtInfo[k].strCheCi==jcInfo.strCheCi
                        &&pDisOrderJC->vectLocmtInfo[k].strLocomotive==jcInfo.strLocomotive)
                {
                  pDisOrderJC->vectLocmtInfo[k]=jcInfo;
                }
            }
        }
    }
}
//更新收到的股道防溜信息（功能码-同步标志）
void MyStation::updateGDAntiSlip(QByteArray recvArray, int syncFlag)
{
    int nCount = 16;
    QVector<CGD*> vectTemp;
    //股道个数
    int size = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //遍历读取
    for(int i=0; i<size; i++)
    {
        CGD* pGdTemp = new CGD();
        //股道设备号
        pGdTemp->m_nCode = ByteArrayToUInt(recvArray.mid(nCount,2));
        pGdTemp->m_strName = this->GetStrNameByCode(pGdTemp->m_nCode);
        nCount+=2;

        //左侧防溜类型（接车口）
        pGdTemp->m_nLAntiSlipType = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //左侧铁鞋号
        pGdTemp->m_nLTxNum = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //左侧紧固器号
        pGdTemp->m_nLJgqNum = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //左侧警内米数
        pGdTemp->m_nLJnMeters = ByteArrayToUInt(recvArray.mid(nCount,2));
        nCount+=2;

        //右侧防溜类型（接车口）
        pGdTemp->m_nRAntiSlipType = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //右侧铁鞋号
        pGdTemp->m_nRTxNum = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //右侧紧固器号
        pGdTemp->m_nRJgqNum = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //右侧警内米数
        pGdTemp->m_nRJnMeters = ByteArrayToUInt(recvArray.mid(nCount,2));
        nCount+=2;

        //存车信息
        pGdTemp->m_nTrainNums = ByteArrayToUInt(recvArray.mid(nCount,2));
        nCount+=2;

        //新版本存车信息  新版本 字符串类型
        //存车信息 股道栏显示 新协议字符串显示 yzr
        int nlen=(int)(recvArray[nCount]&0xFF);
        nCount+=2;
        QString strTxt;
        strTxt = ByteArrayToUnicode(recvArray.mid(nCount,nlen));
        nCount+=nlen;

        vectTemp.append(pGdTemp);
    }

    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        m_ArrayGDAntiSlip.clear();
        m_ArrayGDAntiSlip.append(vectTemp);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        m_ArrayGDAntiSlip.clear();
        m_ArrayGDAntiSlip.append(vectTemp);
    }
    //更新股道防溜信息
    this->SetAllGDAntiSlip();
}

//更新收到的防错办信息（功能码-同步标志）
void MyStation::updateFCBAttr(QByteArray recvArray, int syncFlag)
{
    int nCount = 16;
    int orderType = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //股道属性
    if(0x01 == orderType)
    {
        this->updateGDAttr(recvArray,syncFlag);
    }
    //出入口属性
    else if(0x02 == orderType)
    {
        this->updateGatewayAttr(recvArray,syncFlag);
    }
}
//更新收到的防错办-股道属性信息（功能码-同步标志）
void MyStation::updateGDAttr(QByteArray recvArray, int syncFlag)
{
    int nCount = 16+1;
    QVector<CGD*> vectTemp;
    //股道个数
    int size = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //遍历读取
    for(int i=0; i<size; i++)
    {
        CGD* pGdTemp = new CGD();
        //股道设备号
        pGdTemp->m_nCode = ByteArrayToUInt(recvArray.mid(nCount,2));
        nCount+=2;
        //股道名称长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        pGdTemp->m_strName = ByteArrayToUnicode(recvArray.mid(nCount,len));
        nCount += len;
        //股道id
        pGdTemp->gdId = ByteArrayToUInt(recvArray.mid(nCount,2));
        nCount+=2;
        //线路性质
        pGdTemp->gdAttr = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //接发车方向
        pGdTemp->jfcDir = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //接发车类型
        pGdTemp->jfcAttr = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //超限
        pGdTemp->overLimit = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //站台
        pGdTemp->platform = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //允许动车组
        pGdTemp->isCRH = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //上水设备
        pGdTemp->isWater = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //排污设备
        pGdTemp->isBlowdown = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //军运
        pGdTemp->army = (int)(recvArray[nCount]&0xFF);
        nCount++;

        vectTemp.append(pGdTemp);
    }

    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        this->vectGDAttr.clear();
        this->vectGDAttr.append(vectTemp);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        this->vectGDAttr.clear();
        this->vectGDAttr.append(vectTemp);
    }
}
//更新收到的防错办-出入口属性信息（功能码-同步标志）
void MyStation::updateGatewayAttr(QByteArray recvArray, int syncFlag)
{
    int nCount = 16+1;
    QVector<CXHD*> vectTemp;
    //股道个数
    int size = (int)(recvArray[nCount]&0xFF);
    nCount++;
    //遍历读取
    for(int i=0; i<size; i++)
    {
        CXHD* pXHDTemp = new CXHD();
        //出入口信号机设备号
        pXHDTemp->m_nCode = ByteArrayToUInt(recvArray.mid(nCount,2));
        nCount+=2;
        //出入口信号机名称长度
        int len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        pXHDTemp->m_strName = ByteArrayToUnicode(recvArray.mid(nCount,len));
        nCount += len;
        //出入口名称
        len = (int)(recvArray[nCount]&0xFF);
        nCount += 1;
        pXHDTemp->enexName = ByteArrayToUnicode(recvArray.mid(nCount,len));
        nCount += len;
        //方向
        pXHDTemp->direct = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //允许超限等级
        pXHDTemp->allowOverLimit = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //允许旅客列车
        pXHDTemp->allowPassenger = (int)(recvArray[nCount]&0xFF);
        nCount++;
        //允许货物列车
        pXHDTemp->allowFreight = (int)(recvArray[nCount]&0xFF);
        nCount++;

        vectTemp.append(pXHDTemp);
    }

    //增加
    if(SYNC_FLAG_ADD == syncFlag)
    {
        this->vectGatewayAttr.clear();
        this->vectGatewayAttr.append(vectTemp);
    }
    //更新
    else if(SYNC_FLAG_UPDATE == syncFlag)
    {
        this->vectGatewayAttr.clear();
        this->vectGatewayAttr.append(vectTemp);
    }
}

//初始化组合进路
void MyStation::initCombinedTrainRouteOrder()
{
    QVector<TrainRouteOrder*> m_ArrayRouteOrderTemp;
    for(int i = 0; i < m_ArrayRouteOrder.size(); i++)
    {
        TrainRouteOrder* order = nullptr;
        if(m_ArrayRouteOrder.at(i)->father_id != 0)
        {
            order = GetTrainRouteOrderById(m_ArrayRouteOrder.at(i)->father_id);
            if(order)
                order->m_vecSonTrainRouteOrder.append(m_ArrayRouteOrder.at(i));
            else m_ArrayRouteOrderTemp.append(m_ArrayRouteOrder.at(i));
        }
        else m_ArrayRouteOrderTemp.append(m_ArrayRouteOrder.at(i));
    }
    m_ArrayRouteOrder.swap(m_ArrayRouteOrderTemp);
}

QVector<TrainRouteOrder> MyStation::getRouteOrderByTrainNum(QString trainNum, int type)
{
    QVector<TrainRouteOrder> vecTempRouteOrder;
    for(int i = 0; i < m_ArrayRouteOrder.size(); i++)
    {
        if(m_ArrayRouteOrder.at(i)->m_strTrainNum == trainNum
            && m_ArrayRouteOrder.at(i)->m_btBeginOrEndFlg == type)
        {
            vecTempRouteOrder.append(*m_ArrayRouteOrder.at(i));
        }
    }
    return vecTempRouteOrder;
}

CGDYC* MyStation::getGDYCByIndex(int Type, int index)
{
    int nIndexTemp = 0;
    for(int i = 0; i < vectGDYCArray.size(); i++)
    {
        if(Type == vectGDYCArray[i]->m_nType)
        {
            if(nIndexTemp == index)
                return vectGDYCArray[i];
            nIndexTemp++;
        }
    }
    return nullptr;
}

int MyStation::getTrafficLogIndexByTrainNum(QString trainNum)
{
    for(int i = 0; i < m_ArrayTrafficLog.size(); i++)
    {
        if(m_ArrayTrafficLog[i]->m_strTrainNum == trainNum)
            return i;
    }
    return -1;
}
