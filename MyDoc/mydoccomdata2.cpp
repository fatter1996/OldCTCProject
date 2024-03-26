#include "mydoc.h"
#include <QtDebug>
#include <QSettings>
#include <QMessageBox>
#include "GlobalHeaders/GlobalFuntion.h"

//发送数据给CTC逻辑服务端
void MyDoc::sendDataToCTCLogic2(QByteArray pSendDate, int nLength)
{
   emit sendDataToMainSignal2(pSendDate,nLength);
}
//槽-发送数据给CTC后台服务逻辑-TCP通道
void MyDoc::sendDataToCTCServerSlot2(MyStation *pMyStation, QByteArray dataArray, int nLength)
{
    sendDataToCTCLogic2(dataArray,nLength);
}
//TCP连接槽
void MyDoc::tcpClientConnectedSlot(QString clientAdd, int clientPort)
{
    QLOG_INFO()<<QString("TCP连接成功Server[%1:%2]").arg(clientAdd).arg(clientPort);
    //重新请求数据，后续定时器会请求数据
    this->bRequstData = true;
}
//TCP断开槽
void MyDoc::tcpClientDisconnectedSlot(QString clientAdd, int clientPort)
{
    QLOG_WARN()<<QString("TCP连接断开Server[%1:%2]").arg(clientAdd).arg(clientPort);
    //清空所有数据
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        MyStation* pStation = vectMyStation.at(i);
        //debug
//        qDebug()<<QString("[%1]清空所有数据").arg(pStation->getStationName());
        //站场重置
        pStation->commToServer=10;
        pStation->ResetStationInfo();
        emit TrainRouteOrderUpData();
        emit TrafficLogUpData();
        emit ResetStationSignal();
        emit StagePlanUpData();
    }
    //QMessageBox::warning(NULL, "提示", COMMUNICATION_LOST_MSG);
}
//解析服务端socket通道数据-TCP
void MyDoc::receiveCTCServerDataSlot2(QByteArray dataArray,QString clientAdd, int clientPort)
{
//    qDebug()<<QString("TCP收到Server数据[%1:%2]").arg(clientAdd).arg(clientPort);
    int recvLength=dataArray.size();
    if(recvLength < 14)
    {
        return;   //无效数据
    }
    //校验和分割收到的数据
    QVector<QByteArray> recvSpitArr = SplitReceiveData_SameHeadTail(dataArray);
    //遍历数据
    for(int i=0; i<recvSpitArr.size(); i++)
    {
        QByteArray dataArray = recvSpitArr[i];
        //车站id
        int nStaid = (int)(dataArray[6]&0xFF);
        //多站数据
        MyStation* pStationMuti = getMultiMyStationByStaIDInStaArray(nStaid);
        //判断id是否已配置
        MyStation* pStation = getMyStationByStaIDInStaArray(nStaid);
//        if (!pStation)
//        {
//            return;
//        }
        //功能分类码
        int dataType = (int)(dataArray[9]&0xFF);
        //数据同步
        if(dataType == FUNCTYPE_SYNC)
        {
            //同步类型
            int syncType = (int)(dataArray[10]&0xFF);
            //同步标志
            int syncFlag = (int)(dataArray[11]&0xFF);
            //总包数
            int totalPk = (int)((dataArray[12]&0xFF)|((dataArray[13]&0xFF)<<8));
            //当前包号
            int currPkNum = (int)((dataArray[14]&0xFF)|((dataArray[15]&0xFF)<<8));
            //阶段计划
            if(syncType == SYNC_JDJH)
            {
                if (pStation)
                {
                    pStation->updateStagePlan(dataArray, syncFlag);
//                    qDebug() << "阶段计划" << dataArray.toHex();
                    emit StagePlanUpData();
                   // emit StagePlanUpData();
                }
            }
            //列车进路序列
            else if(syncType == SYNC_JLXL)
            {
                if (pStation)
                {
                    pStation->updateTrainRouteOrder(dataArray, syncFlag);
                    emit TrainRouteOrderUpData();
//                    qDebug() << "列车进路序列" << dataArray.toHex();
                }
            }
            //行车日志
            else if(syncType == SYNC_XCRZ)
            {
                if (pStation)
                {
                    pStation->updateTrafficLog(dataArray, syncFlag);
                    emit TrafficLogUpData();
//                    qDebug() << "行车日志" << dataArray.toHex();
                }
            }
            //调度命令
            else if(syncType == SYNC_DDML)
            {
                if (pStation)
                {
                    pStation->updateDisOrder(dataArray, syncFlag);
                    //emit DDOrderManageUpData();
//                    qDebug() << "调度命令" << dataArray.toHex();
                }
            }
            //股道防溜
            else if(syncType == SYNC_GDFL)
            {
                if (pStation)
                {
                    pStation->updateGDAntiSlip(dataArray, syncFlag);
                }
                if (pStationMuti)
                {
                    pStationMuti->updateGDAntiSlip(dataArray, syncFlag);
                }
//                qDebug() << "股道防溜" << dataArray.toHex();
            }
            //防错办
            else if(syncType == SYNC_FCB)
            {
                if (pStation)
                {
                    pStation->updateFCBAttr(dataArray, syncFlag);
                    //qDebug() << "收到防错办数据";
//                    qDebug() << "防错办" << dataArray.toHex();
                }
            }
        }
    }
}

