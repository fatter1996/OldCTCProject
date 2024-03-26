﻿#include "mydoc.h"
#include <QtDebug>
#include <QSettings>
#include <QMessageBox>
#include <windows.h>
#include "GlobalHeaders/GlobalFuntion.h"

//解析CTCServer通道数据
void MyDoc::receiveCTCServerDataSlot(QByteArray dataArray, QString clientAdd, int clientPort)
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    unsigned char RecvArray[2048]={0};
    int nLength=0;
    if((dataArray.size() < 6) || (dataArray.size() >= 2048))
    {
        return;   //防止越界
    }
    memset(RecvArray,0,2048);
    memcpy(RecvArray,dataArray,dataArray.size());  //转换数据类型
    nLength=(int)(RecvArray[4] | (RecvArray[5]<<8));
    if(nLength != dataArray.size())
    {
//        qDebug()<<" 接收长度与数据解析长度不符！ "<<nLength<<dataArray.size();
        return;
    }
    if((RecvArray[0] == 0xEF) && (RecvArray[1] == 0xEF) && (RecvArray[2] == 0xEF) && (RecvArray[3] == 0xEF)
       && (RecvArray[nLength-1] == 0xFE) && (RecvArray[nLength-2] == 0xFE) && (RecvArray[nLength-3] == 0xFE) && (RecvArray[nLength-4] == 0xFE))//帧头帧尾校验成功
    {
        int dataType = (int)RecvArray[8];
        if(dataType == DATATYPE_TRAIN)
        {
            if(pStation->StaConfigInfo.isHavePXRJ == true)  //2020.7.9-BJT-是否有培训软件判别
            {
                pTrainAndTest->CheckUdpData(RecvArray, nLength);
            }
        }
        else
        {
            recvServerData(dataArray, nLength, clientAdd, clientPort);
            recvServerDataJSJFLBL(dataArray, nLength, clientAdd, clientPort);
        }
    }
}
//发送更新数据消息（给所有连接终端-CTC、占线板）
void MyDoc::sendUpdateDataMsg(MyStation *pStation, int _type, int currCtcIndex, int currZxbIndex)
{
    int nLength = 60;
    QByteArray byArrayUDPDate;
    byArrayUDPDate.append(nLength, char(0));//添加60个字节并全部置零
    for(int i = 0; i < 4; i++)
    {
        byArrayUDPDate[i] = 0xEF;
        byArrayUDPDate[nLength - i -1] = 0xFE;
    }
    byArrayUDPDate[4] = nLength;//帧长度
    byArrayUDPDate[5] = nLength<<8;//帧长度
    byArrayUDPDate[6] = pStation->getStationID();
    byArrayUDPDate[8] = DATATYPE_CTC;
    byArrayUDPDate[9] = FUNCTYPE_UPDATE;//分类信息码
    byArrayUDPDate[10] = _type;//子分类信息码
    sendDataToCTCLogic(byArrayUDPDate, nLength);
}
//接收服务终端数据处理
void MyDoc::recvServerData(QByteArray recvArray, int recvlen, QString clientAdd, int clientPort)
{
    //车站id
    int nStaid = (int)recvArray[6];
    //多站数据
    MyStation* pStationMuti = getMultiMyStationByStaIDInStaArray(nStaid);
    //判断id是否已配置
    MyStation* pStation = getMyStationByStaIDInStaArray(nStaid);
//    if (!pStation)
//    {
//        return;
//    }


    int revcType = (int)(recvArray[9]&0xFF);
    //qDebug()<<QString("接收到【Server终端%1】的数据").arg(nStaid);

    //保存接收到数据的socket信息，用于向对方发送数据
    //pStation->lsAddr = clientAdd;
    //pStation->lsPort = clientPort;
    //pStation->IsCommunicationsNormal = true;  //有效通信数据解析后置相关标志量为真
    ////pStation->IrregularCycle = 0;  //有效通信数据解析后置相关非正常通信周期计数为零
    //qDebug()<<QString("接收到【联锁终端%1】的数据").arg(nStaid);
//    if(revcType != 0x00)
//    {
//        //sendStatusToAllStationCTC(recvArray, recvlen);
//    }

    //Mutex.lock();
    //站场平面图信息
    if(revcType == 0x11)
    {
        if (pStation)
        {
            pStation->updateDevStateOfLS((unsigned char *)recvArray.data());
        }
        if (pStationMuti)
        {
            pStationMuti->updateDevStateOfLS((unsigned char *)recvArray.data());
        }
    }
    //行车日志列车报点信息
    else if(revcType == 0x55)
    {
        //RecvLSData_ReportTime(pStation, (unsigned char *)recvArray.data(), recvlen);
    }
    //联锁列车实时位置报点
    else if(revcType == 0x66)
    {
        //RecvLSData_TrainPos(pStation, (unsigned char *)recvArray.data(), recvlen);
        if (pStation)
        {
            pStation->recvServerData_TrainPos((unsigned char *)recvArray.data(), recvlen);
        }
        if (pStationMuti)
        {
            pStationMuti->recvServerData_TrainPos((unsigned char *)recvArray.data(), recvlen);
        }
    }
    //非常站控
    else if(revcType == 0x77)
    {
        //RecvLSData_FCZK(pStation, (unsigned char *)recvArray.data(), recvlen);
    }
    //调度命令 0x0c
    else if(revcType == 0x99)
    {
         emit DDOrderManageUpData();
        //RecvLSData_DDML(pStation, (unsigned char *)recvArray.data(), recvlen);
    }
    //阶段计划
    else if(revcType == 0x33)
    {
         emit StagePlanUpData(false);
        //RecvLSData_JDJH(pStation, (unsigned char *)recvArray.data(), recvlen);
    }
//    //邻站预告
//    else if(revcType == 0x0d)
//    {
//        //RecvLSData_LZYG(pStation, (unsigned char *)recvArray.data(), recvlen);
//    }
    //发送计划
    else if(revcType == 0x35)
    {

    }
    //邻站预告，行车日志操作
    else if(revcType == FUNCTYPE_TRAFFIC)
    {
        //RecvLSData_LZYG(pStation, (unsigned char *)recvArray.data(), recvlen);
        //行车日志报点操作
        if(0x01 == (int)(recvArray[10]&0xFF))
        {
            //日志id
            int idLog = (int)(recvArray[11] | (recvArray[12]<<8));
            //
            if(PLAN_CMD_FCYG == (int)recvArray[13])//预告（发预）
            {
                //上一个站操作预告，同意接车单元格显红
                pStationView->GetTrafficLogManage()->AdjacentStationReportingPoint(1, idLog);
                //调试
//                qDebug()<<"收到邻站-发车预告";

            }
            else if(PLAN_CMD_TYYG == (int)recvArray[13])//同意（接预）
            {
                //下一个站操作同意预告，邻站同意发车单元格取消红色
                pStationView->GetTrafficLogManage()->AdjacentStationReportingPoint(2, idLog);
                //调试
//                qDebug()<<"收到邻站-同意（接预）";
            }
            else if(0x08 == (int)recvArray[13])//取消闭塞
            {
                //上一个站操作取消闭塞，同意邻站发车单元格取消红色
                pStationView->GetTrafficLogManage()->AdjacentStationReportingPoint(3, idLog);
                //调试
//                qDebug()<<"收到邻站-取消闭塞";
            }
        }
    }
    //进路办理回执
    else if(revcType == 0x1B)
    {
        //RecvLSData_RouteReturn(pStation, (unsigned char *)recvArray.data(), recvlen);
    }
    //控制模式
    else if(revcType == 0x3a)
    {
        if (pStation)
        {
            emit RecvServerModeChange(recvArray,recvlen);
        }
    }
    //系统报警信息
    else if(revcType == 0x40)
    {
        if (pStation)
        {
            emit RecvServerAlarm(recvArray,recvlen);
        }
    }
    //语音播报信息
    else if(revcType == FUNCTYPE_SPEACH)
    {
        if (pStation)
        {
            pStation->recvServerData_Speach(recvArray,recvlen);
        }
    }
    else if(revcType==FUNCTYPE_CMDCLEAR)
    {
        if (pStation)
        {
//            qDebug()<<"收到[服务端]数据   清除";
            emit ClearFunButtonStateSignal();
        }
    }
    //站场重置
    else if(revcType==0xEA)
    {
        if (pStation)
        {
//            qDebug()<<"收到[服务端]数据   站场重置";
            pStation->ResetStationInfo();
            emit TrainRouteOrderUpData();
            emit TrafficLogUpData();
            emit ResetStationSignal();
            emit StagePlanUpData();
        }
    }
    //防错办反馈信息
    else if(revcType==FUNCTYPE_CHECK)
    {
        if (pStation)
        {
//            qDebug()<<"收到[服务端]数据   防错办";
            CheckResult* checkResult = pStation->recvServerData_RouteCheck(recvArray);
            pAgainstWrongWidget->ShowCheckMsg(checkResult);
        }
    }
    //用户登录注销操作
    else if(revcType==FUNCTYPE_USERLOGINEXIT)
    {
        if (pStation)
        {
//            qDebug()<<"收到[服务端]数据   用户登录注销操作";
            if((int)recvArray[10]==0x02)
            {
                int namelen=(int)recvArray[11];
                //登录结果
                int nresult=(int)recvArray[12+namelen];
                if(nresult==0x02||nresult==0x03)
                {
                    emit UserAlarmSignal(nresult);
                }
            }
        }
    }
    //时钟同步
    else if(revcType == 0xAA)
    {
        int nCount = 10;
        SYSTEMTIME sysTime;
        GetSystemTime(&sysTime);
        sysTime.wYear = (recvArray[nCount] | recvArray[nCount+1]<<8);
        sysTime.wMonth = recvArray[nCount+2];
        sysTime.wDay  = recvArray[nCount+3];
        sysTime.wHour = recvArray[nCount+4];
        sysTime.wMinute = recvArray[nCount+5];
        sysTime.wSecond = recvArray[nCount+6];
        sysTime.wDayOfWeek = -1;
        sysTime.wMilliseconds = 0;
        SetLocalTime(&sysTime);
    }
    //一键关机
    else if(revcType == 0xAB)
    {
        system("shutdown -s -t 00");
    }
    else if(revcType == FUNCTYPE_LOGICCHECK_RECV) //区间逻辑检查
    {
        emit RecvLogicCheck(recvArray[10], recvArray[11], QString::fromLocal8Bit(recvArray.mid(13, (int)recvArray[12])));
    }

    //Mutex.unlock();
}
void MyDoc::recvServerDataJSJFLBL(QByteArray recvArray, int recvlen, QString clientAdd, int clientPort)
{
    //车站id
    int nStaid = (int)recvArray[6];
    //多站数据
    MyStation* pStationMuti = getMultiMyStationByStaIDInStaArray(nStaid);
    //判断id是否已配置
    MyStation* pStation = getMyStationByStaIDInStaArray(nStaid);

    //int revcType = (int)(recvArray[9]&0xFF);
    if (pStation)
    {
        SetJSJFLBL(pStation,recvArray);
    }
    if(pStationMuti)
    {
        SetJSJFLBL(pStationMuti,recvArray);
    }
}
//设置教师机发送分路不良标识
void MyDoc::SetJSJFLBL( MyStation* pStation,QByteArray recvArray)
{
    //故障设置
    int GDcode=0;
    CGD *gd=nullptr;
    CGDDC *gddc=nullptr;
    if((int)recvArray[10]==0x23&&(int)recvArray[11]==0x00)//无岔区段
    {
        GDcode=(unsigned char)recvArray[12]+(((unsigned char)recvArray[13])<<8);
        gd=pStation->GetGDByCode(GDcode);
        if(gd!=nullptr)
        {
            gd->setGDFLBL(true);
            gd->m_nGDFLBLKX = false;
            gd->bJSJSetFLBLFlag=true;
        }
    }
    else if((int)recvArray[10]==0x17&&(int)recvArray[11]==0x00)//岔前
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gddc=pStation->GetDCByCode(GDcode);
        if(gddc!=nullptr)
        {
            gddc->flblStatusCQ=1;
            gddc->bJSJSetFLBLFlagCQ=true;
        }
       // qDebug()<<"收到[教师机]数据   分路不良设置岔前"<<GDcode<<pStation->GetDCByCode(GDcode)->getName();
    }
    else if((int)recvArray[10]==0x15&&(int)recvArray[11]==0x00)//定位
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gddc=pStation->GetDCByCode(GDcode);
        if(gddc!=nullptr)
        {
            gddc->flblStatusDW=1;
            gddc->bJSJSetFLBLFlagDW=true;
        }
        //qDebug()<<"收到[教师机]数据   分路不良设置定位"<<GDcode;
    }
    else if((int)recvArray[10]==0x16&&(int)recvArray[11]==0x00)//反位
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gddc=pStation->GetDCByCode(GDcode);
        if(gddc!=nullptr)
        {
            gddc->flblStatusFW=1;
            gddc->bJSJSetFLBLFlagFW=true;
        }
//        qDebug()<<"收到[教师机]数据   分路不良设置反位"<<GDcode;
    }
    //故障恢复
    else if((int)recvArray[10]==0x24&&(int)recvArray[11]==0x00)//无岔区段
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gd=pStation->GetGDByCode(GDcode);
        if(gd!=nullptr)
        {
            gd->setGDFLBL(false);
            gd->m_nGDFLBLKX = true;
            gd->bJSJSetFLBLFlag=false;
        }
//        qDebug()<<"收到[教师机]数据   分路不良设置无岔区段"<<GDcode;
    }
    else if((int)recvArray[10]==0x1a&&(int)recvArray[11]==0x00)//岔前
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gddc=pStation->GetDCByCode(GDcode);
        if(gddc!=nullptr)
        {
            //gddc->flblStatusCQ=1;
            gddc->bJSJSetFLBLFlagCQ=false;
        }
        //qDebug()<<"收到[教师机]数据   分路不良设置岔前"<<GDcode;
    }
    else if((int)recvArray[10]==0x18&&(int)recvArray[11]==0x00)//定位
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gddc=pStation->GetDCByCode(GDcode);
        if(gddc!=nullptr)
        {
           // gddc->flblStatusDW=1;
            gddc->bJSJSetFLBLFlagDW=false;
        }
        //qDebug()<<"收到[教师机]数据   分路不良设置定位"<<GDcode;
    }
    else if((int)recvArray[10]==0x19&&(int)recvArray[11]==0x00)//反位
    {
        GDcode=(int)recvArray[12]+(((int)recvArray[13])<<8);
        gddc=pStation->GetDCByCode(GDcode);
        if(gddc!=nullptr)
        {
            //gddc->flblStatusFW=1;
            gddc->bJSJSetFLBLFlagFW=false;
        }
        //qDebug()<<"收到[教师机]数据   分路不良设置反位"<<GDcode;
    }

}
//发送心跳信息给后台服务
void MyDoc::SendHeartBeatToTCCService()
{
    int nLength = 30;//60;
    //unsigned int stationID=0x00;
    QByteArray byArrayUDPDate;
    byArrayUDPDate.append(nLength, char(0));//添加20个字节并全部置零
//    for (int i = 0; i < 4; i++)
//    {
//        byArrayUDPDate[i] = 0xEF;
//        byArrayUDPDate[nLength - i - 1] = 0xFE;
//    }
    //TODO :byArrayUDPDate[7] 表示ctc终端本站编号 后续须从配置文件中获取
    //byArrayUDPDate[7]=0x01;
    byArrayUDPDate[8] = DATATYPE_CTC;//CTC制式
    byArrayUDPDate[9] = 0x23;//分类信息码

    //发送本站心跳
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        if(i == currStaIndex)
        {
            MyStation* pStation = vectMyStation.at(i);
            if (pStation)
            {
                //stationID = pStation->getStationID();
                ////memcpy(&byArrayUDPDate[6], &stationID, 1);//车站id
                //byArrayUDPDate[6] = stationID;
                //byArrayUDPDate[7] = pStation->ABNUM;
                pStation->packHeadAndTail(&byArrayUDPDate, nLength);
                sendDataToCTCLogic(byArrayUDPDate, nLength);
                sendDataToCTCLogic2(byArrayUDPDate, nLength);
            }
        }
    }
}
//发送数据给CTC逻辑服务端
void MyDoc::sendDataToCTCLogic(QByteArray pSendDate, int nLength)
{
   emit sendDataToMainSignal(pSendDate,m_AddrCTCToService.addrService, m_AddrCTCToService.nPortService1, nLength);
}
//获取当前车站id
int MyDoc::GetStationID()
{
    int stationID=-1;
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        if(i == currStaIndex)
        {
            MyStation* pStation = vectMyStation.at(i);
            if (pStation)
            {
                stationID = pStation->getStationID();
                break;
            }
        }
    }
    return stationID;
}
void MyDoc::SendFunButtonDataToCTCService(int nFunType,int ndevcode,int nDevCode2)
{
    int nLength = 30;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0xEF, 0xEF, 0xEF, 0xEF,	//帧头
                                                0x12,	//帧长低位*/
                                                0x00,	//帧长高位*/
                                                0x00,	//目标地址码
                                                0x00,	//本机地址码
                                                0x00,	//车站标志
                                                0x88,	//信息分类码 （CTC车务终端----->联锁仿真机）
                                                0x00,	//功能按钮类型
                                                0x00,	//设备号
                                                0x00,	//设备号
                                                0x00,	//设备号
                                                0x00, 0x00, 0x00, 0x00};	//帧尾

    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
    byArrayUDPJLOrderDate[10]=nFunType;
    if(nFunType==0x01||nFunType==0x02||nFunType==0x03)
    {
        nLength=30;
    }
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    if(nFunType==0x01||nFunType==0x02||nFunType==0x03)
    {
        /*按钮类型两位-00表示列车按钮；
        按钮类型两位-01表示调车按钮；
        按钮类型两位-10表示通过按钮；*/
        int n=0;
        if(nFunType==0x01)
            n=0x0000;
        else if(nFunType==0x02)
            n=0x4000;
        else if(nFunType==0x03)
            n=0x8000;
        ndevcode=ndevcode|n;
        if(nDevCode2!=-1)
        {
            if(n==0x8000)
            {
               nDevCode2=nDevCode2;
            }
            else
            {
               nDevCode2=nDevCode2|n;
            }

        }

        byArrayUDPJLOrderDate[11] = ndevcode;
        byArrayUDPJLOrderDate[12] = ndevcode>>8;
    }
    else
    {
        byArrayUDPJLOrderDate[11] = ndevcode;
        byArrayUDPJLOrderDate[12] = ndevcode>>8;
    }
    if(nDevCode2!=-1)
    {
        byArrayUDPJLOrderDate[13] = nDevCode2;
        byArrayUDPJLOrderDate[14] = nDevCode2>>8;
    }

    byArrayUDPJLOrderDate[nLength-4]=0xfe;
    byArrayUDPJLOrderDate[nLength-3]=0xfe;
    byArrayUDPJLOrderDate[nLength-2]=0xfe;
    byArrayUDPJLOrderDate[nLength-1]=0xfe;

    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);

}
void MyDoc::SendFunButtonDataToCTCService(int nFunType,int ndevcode[],QString str)
{
    int nLength;
    int nstrlen=str.size();
    nLength=32+nstrlen;

    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0xEF, 0xEF, 0xEF, 0xEF,	//帧头
                                                0x12,	//帧长低位*/
                                                0x00,	//帧长高位*/
                                                0x00,	//目标地址码
                                                0x00,	//本机地址码
                                                0x00,	//车站标志
                                                0x88,	//信息分类码 （CTC车务终端----->联锁仿真机）
                                                0x00,	//功能按钮类型
                                                0x00,	//设备号
                                                0x00,	//设备号
                                                0x00,	//设备号
                                                0x00, 0x00, 0x00, 0x00};	//帧尾

    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
    byArrayUDPJLOrderDate[10]=nFunType;
//    if(nFunType==0x01||nFunType==0x02||nFunType==0x03)
//    {
//        nLength=30;
//    }
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    if(nFunType==0x01||nFunType==0x02||nFunType==0x03)
    {
        /*按钮类型两位-00表示列车按钮；
        按钮类型两位-01表示调车按钮；
        按钮类型两位-10表示通过按钮；*/
        int n=0;
        if(nFunType==0x01)
            n=0x00;
        else if(nFunType==0x02)
            n=0x40;
        else if(nFunType==0x03)
            n=0x80;

        CXHD* pXhd = nullptr;
        for(int i=0;i<8;i++)
        {
            if(ndevcode[i]!=-1)
            {
                pXhd = pStation->GetXHDByCode(ndevcode[i]);

                byArrayUDPJLOrderDate[11+i*2] = ndevcode[i];
                byArrayUDPJLOrderDate[12+i*2] = ndevcode[i]>>8;

                //虚信号必须发送调按
                if(pXhd && pXhd->isSignalType)
                {
//                    for(int p=0;p<pStation->mXXHDButtonInfo.XhdNameList.size();p++)
//                    {
//                        if(pStation->mXXHDButtonInfo.XhdNameList[p]==pXhd->m_strName)
//                        {
//                            if(pStation->mXXHDButtonInfo.nDevSendState==0)
//                            {
//                                byArrayUDPJLOrderDate[12+i*2] |= 0x00;
//                            }
//                            else if(pStation->mXXHDButtonInfo.nDevSendState==1)
//                            {
//                                byArrayUDPJLOrderDate[12+i*2] |= 0x40;
//                            }
//                        }
//                    }

                    if(XXHDStateSend(pStation, pXhd))
                    {
                        byArrayUDPJLOrderDate[12+i*2] |= 0x00;
                    }
                    else
                    {
                        byArrayUDPJLOrderDate[12+i*2] |= 0x40;
                    }
                }
//                else
//                {
//                    byArrayUDPJLOrderDate[12+i*2] |= n;
//                    if(i > 0 && nFunType == 0x03)
//                    {
//                        byArrayUDPJLOrderDate[12+i*2] &= 0x3F;
//                    }
//                }
            }
        }
    }
    if(str.contains("电力")||str.contains("内燃"))
    {
        nLength=33;
        int ntime=str.right(str.length()-2).toInt();
        //占用分钟时间
        byArrayUDPJLOrderDate[27]=ntime;
        //调车机车属性
        if(str.contains("电力"))
        {
            byArrayUDPJLOrderDate[28]=0x01;
        }
        else
        {
            byArrayUDPJLOrderDate[28]=0x02;
        }
    }
    else
    {
        QByteArray byteArray1 = str.toLocal8Bit();//toLatin1();
         int nUserNameLen = byteArray1.count();
         nLength=32+nUserNameLen;
        //车次号长度
        byArrayUDPJLOrderDate[27]=nUserNameLen;
        //车次号
        //nLength=11+2+nUserNameLen+nPassWordlen+4;
        for(int u=0; u<nUserNameLen; u++)
        {
            byArrayUDPJLOrderDate[28+u] = byteArray1[u];
        }
    }
    byArrayUDPJLOrderDate[nLength-4]=0xfe;
    byArrayUDPJLOrderDate[nLength-3]=0xfe;
    byArrayUDPJLOrderDate[nLength-2]=0xfe;
    byArrayUDPJLOrderDate[nLength-1]=0xfe;

    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);

}

void MyDoc::SendXHJPressType(int nFunType,int ndevcode)
{
    int nLength = 18;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};	//帧尾
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
    byArrayUDPJLOrderDate[9]=FUNCTYPE_BTNCLICK;
    //赋值设备号
    byArrayUDPJLOrderDate[10] = ndevcode;
    byArrayUDPJLOrderDate[11] = ndevcode>>8;
    //进路按钮类型
    byArrayUDPJLOrderDate[12]=nFunType;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
void MyDoc::SendDevPressInfo()
{
    int nLength = 18;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};	//帧尾
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
    byArrayUDPJLOrderDate[9]=FUNCTYPE_DEVOPERA;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
void MyDoc::SendClearBtnInfo()
{
    int nLength = 18;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};	//帧尾
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
    byArrayUDPJLOrderDate[9]=FUNCTYPE_CMDCLEAR;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}

void MyDoc::SendSignFor()
{
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);

    int nLength = 18;
    for(int i = 0; i < pStation->m_ArrayStagePlan.size(); i++)
    {
        QByteArray dataArray;
        dataArray.resize(nLength);
        //帧长度
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        //本机地址
        dataArray[6]=GetStationID();
        //目的地址
        dataArray[7]=pStation->ABNUM;
        //车站标志 软件类型
        dataArray[8]=DATATYPE_CTC;
        dataArray[9]=FUNCTYPE_STAGEPL;
        dataArray[10]=0x01;
        dataArray[11]=pStation->m_ArrayStagePlan.at(i)->plan_id;
        dataArray[12]=pStation->m_ArrayStagePlan.at(i)->plan_id >> 8;
        dataArray[13]=0xaa;
        sendDataToCTCLogic(dataArray, nLength);
    }
}
void MyDoc::SendSendPlan()
{
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    int nLength = 15;
    QByteArray dataArray;
    dataArray.resize(nLength);
    //帧长度
    dataArray[4]=nLength;
    dataArray[5]=nLength >> 8;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;
    dataArray[9]=FUNCTYPE_STAGEPL;
    dataArray[10]=0x03;
    sendDataToCTCLogic(dataArray, nLength);
}

void MyDoc::SendRouteOrder(int type, int routeId, int nData, QString strData)
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    QByteArray dataArray;
    dataArray.resize(20);
    int nLength = 0;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;
    dataArray[9]=FUNCTYPE_ROUTE;

    if(type == MSGTYPE_MANTRIGGER)
    {
        nLength = 18;
        dataArray.resize(nLength);
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        dataArray[10]=0x01;
        //进路序列ID
        dataArray[11]=routeId;
        dataArray[12]=routeId >> 8;

        dataArray[13]=nData;
    }
    else if(type == MSGTYPE_SETTRIGGERTYPE)
    {
        //帧长度
        nLength = 18;
        dataArray.resize(nLength);
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        dataArray[10]=0x02;
        //进路序列ID
        dataArray[11]=routeId;
        dataArray[12]=routeId >> 8;

        dataArray[13]=nData;
    }
    else if(type == MSGTYPE_TRACKINFO)
    {
        //帧长度
        nLength = 19;
        dataArray.resize(nLength);
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        dataArray[10]=0x03;
        //进路序列ID
        dataArray[11]=routeId;
        dataArray[12]=routeId >> 8;

        dataArray[13]=nData;
        dataArray[14]=nData >> 8;
    }
    else if(type == MSGTYPE_ROUTEDESCRIP)
    {
        nLength = 18 + nData;
        dataArray.resize(nLength);
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        dataArray[10]=0x04;
        //进路序列ID
        dataArray[11]=routeId;
        dataArray[12]=routeId >> 8;

        dataArray[13]=nData;
        dataArray.replace(14, nData, strData.toStdString().c_str());
    }
    else if(type == MSGTYPE_PERMISSION)
    {
        nLength = 16;
        dataArray.resize(nLength);
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        dataArray[10]=0x05;
        dataArray[11]=nData;
        dataArray[12]=0;
    }
    else if(type == MSGTYPE_DELETE)
    {
        nLength = 18;
        dataArray.resize(nLength);
        dataArray[4]=nLength;
        dataArray[5]=nLength >> 8;
        dataArray[10]=0x06;
        dataArray[11]=routeId;
        dataArray[12]=routeId >> 8;
        dataArray[13]=nData;
    }

    sendDataToCTCLogic(dataArray, nLength);
}

void MyDoc::SendTrafficLog(int id, int type, QDateTime date)
{
    if(type == 0x1c)
        pStationView->setSendPlanFlash();
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    QByteArray dataArray;
    dataArray.resize(25);

    dataArray[4]=25;
    dataArray[5]=25 >> 8;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;

    dataArray[9]=FUNCTYPE_TRAFFIC;
    dataArray[10]=0x01;
    dataArray[11]=id;
    dataArray[12]=id >> 8;
    dataArray[13]=type;

    QByteArray dateTimeArr;
    dateTimeArr.resize(7);
    dateTimeArr[0] = date.date().year();
    dateTimeArr[1] = date.date().year() >> 8;
    dateTimeArr[2] = date.date().month();
    dateTimeArr[3] = date.date().day();
    dateTimeArr[4] = date.time().hour();
    dateTimeArr[5] = date.time().minute();
    dateTimeArr[6] = date.time().second();

    dataArray.replace(14, 7, dateTimeArr);
    sendDataToCTCLogic(dataArray, 25);
}

void MyDoc::SendTrafficLogUpdata(int type, TrafficLog pTrafficLog)
{
    pStationView->setSendPlanFlash();
    if(pTrafficLog.m_btBeginOrEndFlg == JFC_TYPE_SF)
    {
        pTrafficLog.m_strReachTrainNum = "";
        pTrafficLog.m_strReachTrainNumOld = "";
    }
    else if(pTrafficLog.m_btBeginOrEndFlg == JFC_TYPE_ZD)
    {
        pTrafficLog.m_strDepartTrainNum = "";
        pTrafficLog.m_strDepartTrainNumOld = "";
    }

    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    QByteArray dataArray;
    int lenght = 68 + pTrafficLog.m_strReachTrainNumOld.size()
            + pTrafficLog.m_strReachTrainNum.size()
            + pTrafficLog.m_strDepartTrainNumOld.size()
            + pTrafficLog.m_strDepartTrainNum.size();
    dataArray.resize(lenght);

    dataArray[4]=lenght;
    dataArray[5]=lenght >> 8;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;

    int flag = 9;
    dataArray[flag++]=FUNCTYPE_CHGCC;
    dataArray[flag++]=type;
    dataArray[flag++]=pTrafficLog.m_btBeginOrEndFlg;

    dataArray[flag++]=pTrafficLog.m_strReachTrainNumOld.size();
    memcpy(dataArray.data()+flag,pTrafficLog.m_strReachTrainNumOld.toStdString().c_str(),pTrafficLog.m_strReachTrainNumOld.size());
    flag += pTrafficLog.m_strReachTrainNumOld.size();

    dataArray[flag++]=pTrafficLog.m_strReachTrainNum.size();
    memcpy(dataArray.data()+flag,pTrafficLog.m_strReachTrainNum.toStdString().c_str(),pTrafficLog.m_strReachTrainNum.size());
    flag += pTrafficLog.m_strReachTrainNum.size();

    int JCCode = getCurrMyStationInStaArray()->GetCodeByStrName(pTrafficLog.m_strXHD_JZk);
    dataArray[flag++]=JCCode;
    dataArray[flag++]=JCCode >> 8;
    int JCTrack = getCurrMyStationInStaArray()->GetCodeByStrName(pTrafficLog.m_strRecvTrainTrack);
    dataArray[flag++]=JCTrack;
    dataArray[flag++]=JCTrack >> 8;

    QByteArray dateTimeArr;
    dateTimeArr.resize(7);
    dateTimeArr[0] = pTrafficLog.m_timProvReachStation.date().year();
    dateTimeArr[1] = pTrafficLog.m_timProvReachStation.date().year() >> 8;
    dateTimeArr[2] = pTrafficLog.m_timProvReachStation.date().month();
    dateTimeArr[3] = pTrafficLog.m_timProvReachStation.date().day();
    dateTimeArr[4] = pTrafficLog.m_timProvReachStation.time().hour();
    dateTimeArr[5] = pTrafficLog.m_timProvReachStation.time().minute();
    dateTimeArr[6] = pTrafficLog.m_timProvReachStation.time().second();

    dataArray.replace(flag, 7, dateTimeArr);
    flag += 7;

    dataArray[flag++]=pTrafficLog.m_strDepartTrainNumOld.size();
    memcpy(dataArray.data()+flag,pTrafficLog.m_strDepartTrainNumOld.toStdString().c_str(),pTrafficLog.m_strDepartTrainNumOld.size());
    flag += pTrafficLog.m_strDepartTrainNumOld.size();

    dataArray[flag++]=pTrafficLog.m_strDepartTrainNum.size();
    memcpy(dataArray.data()+flag,pTrafficLog.m_strDepartTrainNum.toStdString().c_str(),pTrafficLog.m_strDepartTrainNum.size());
    flag += pTrafficLog.m_strDepartTrainNum.size();

    int FCCode = getCurrMyStationInStaArray()->GetCodeByStrName(pTrafficLog.m_strXHD_CZk);
    dataArray[flag++]=FCCode;
    dataArray[flag++]=FCCode >> 8;
    int FCTrack = getCurrMyStationInStaArray()->GetCodeByStrName(pTrafficLog.m_strDepartTrainTrack);
    dataArray[flag++]=FCTrack;
    dataArray[flag++]=FCTrack >> 8;

    QByteArray dateTimeArr2;
    dateTimeArr2.resize(7);
    dateTimeArr2[0] = pTrafficLog.m_timProvDepaTrain.date().year();
    dateTimeArr2[1] = pTrafficLog.m_timProvDepaTrain.date().year() >> 8;
    dateTimeArr2[2] = pTrafficLog.m_timProvDepaTrain.date().month();
    dateTimeArr2[3] = pTrafficLog.m_timProvDepaTrain.date().day();
    dateTimeArr2[4] = pTrafficLog.m_timProvDepaTrain.time().hour();
    dateTimeArr2[5] = pTrafficLog.m_timProvDepaTrain.time().minute();
    dateTimeArr2[6] = pTrafficLog.m_timProvDepaTrain.time().second();

    dataArray.replace(flag, 7, dateTimeArr2);
    flag += 7;

    dataArray[flag++]=pTrafficLog.m_bElectric;
    dataArray[flag++]=pTrafficLog.m_nLevelCX;
    dataArray[flag++]=pTrafficLog.m_nIndexLCLX;
    dataArray[flag++]=pTrafficLog.m_nIndexYXLX;
    dataArray[flag++]=pTrafficLog.m_bDeleteFlag > 0 ? 0x11 : 0x00;
    dataArray[flag++]=pTrafficLog.m_bBLKY;
    dataArray[flag++]=pTrafficLog.m_bImportant;
    dataArray[flag++]=pTrafficLog.m_bArmy;
    dataArray[flag++]=pTrafficLog.m_bAllowGDNotMatch;
    dataArray[flag++]=pTrafficLog.m_bAllowCRKNotMatch;

    dataArray[flag++] = pTrafficLog.m_btLJStatus;
    dataArray[flag++] = pTrafficLog.m_btJALStatus;
    dataArray[flag++] = pTrafficLog.m_btJPStatus;
    dataArray[flag++] = pTrafficLog.m_btLWStatus;
    dataArray[flag++] = pTrafficLog.m_btJCStatus;
    dataArray[flag++] = pTrafficLog.m_btHJStatus;
    dataArray[flag++] = pTrafficLog.m_btCJStatus;
    dataArray[flag++] = pTrafficLog.m_btSSStatus;
    dataArray[flag++] = pTrafficLog.m_btZGStatus;
    dataArray[flag++] = pTrafficLog.m_btHCStatus;
    dataArray[flag++] = pTrafficLog.m_btZXStatus;
    dataArray[flag++] = pTrafficLog.m_btXWStatus;
    dataArray[flag++] = pTrafficLog.m_btDKStatus;
    dataArray[flag++] = pTrafficLog.m_btCHStatus;
    dataArray[flag++] = pTrafficLog.m_btZWStatus;
    dataArray[flag++] = pTrafficLog.m_btZKStatus;
//    qDebug() << "send" << dataArray.toHex();
    sendDataToCTCLogic(dataArray, lenght);
}

void MyDoc::SendTrafficLogFlowUpdata(int logId, int type, bool flag)
{
    pStationView->setSendPlanFlash();
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    QByteArray dataArray;

    int len = 26;
    dataArray.resize(len);

    dataArray[4]=len;
    dataArray[5]=len >> 8;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;
    dataArray[9]=FUNCTYPE_FLOWS;
    dataArray[10]=0x02;
    dataArray[11]=logId;
    dataArray[12]=logId >> 8;
    dataArray[13]=type;
    dataArray[14]=flag?0xaa:0xee;

    QByteArray dateTimeArr;
    QDateTime date = QDateTime::currentDateTime();
    dateTimeArr.resize(7);
    dateTimeArr[0] = date.date().year();
    dateTimeArr[1] = date.date().year() >> 8;
    dateTimeArr[2] = date.date().month();
    dateTimeArr[3] = date.date().day();
    dateTimeArr[4] = date.time().hour();
    dateTimeArr[5] = date.time().minute();
    dateTimeArr[6] = date.time().second();

    dataArray.replace(15, 7, dateTimeArr);

    //dataArray[15]=type;
    sendDataToCTCLogic(dataArray, len);
}

void MyDoc::onUpdataNotes(TrafficLog pTrafficLog)
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    QByteArray dataArray;
    QByteArray tempArr = pTrafficLog.m_strNotes.toLocal8Bit();
    int len = 18 + tempArr.length();
    dataArray.resize(len);

    dataArray[4]=len;
    dataArray[5]=len >> 8;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;
    dataArray[9]=FUNCTYPE_TRAFFIC;
    dataArray[10]=0x02;
    dataArray[11]=pTrafficLog.log_id;
    dataArray[12]=pTrafficLog.log_id >> 8;
    int flag = 13;


    dataArray[flag++]=tempArr.length();
    dataArray.replace(flag, tempArr.length(), tempArr);
    //memcpy(dataArray.data()+flag,pTrafficLog.m_strNotes.toUtf8(),pTrafficLog.m_strNotes.size());
    flag += pTrafficLog.m_strNotes.size();
    sendDataToCTCLogic(dataArray, len);
}

void MyDoc::SendAgainstWrong(int id, int type)
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    QByteArray dataArray;

    int len = 20;
    dataArray.resize(len);

    dataArray[4]=len;
    dataArray[5]=len >> 8;
    //本机地址
    dataArray[6]=GetStationID();
    //目的地址
    dataArray[7]=pStation->ABNUM;
    //车站标志 软件类型
    dataArray[8]=DATATYPE_CTC;
    dataArray[9]=FUNCTYPE_CHECK;
    dataArray[10]=0x02;
    memcpy(dataArray.data()+11,&id,4);
    dataArray[15]=type;
    sendDataToCTCLogic(dataArray, len);
}

//向服务断发送用户登录 注销信息
void MyDoc::SendUserLongInExitSlot(int ntype,QString strUserName,QString strPassWord)
{
    int nLength = 30;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
    byArrayUDPJLOrderDate[9]=FUNCTYPE_USERLOGINEXIT;

    QByteArray dataArray;
    if(ntype==0)//登录
    {
        QByteArray byteArray1 = strUserName.toLocal8Bit();//toLatin1();
        int nUserNameLen = byteArray1.count();
        QByteArray byteArray2 = strPassWord.toLocal8Bit();//toLatin1();
        int nPassWordlen = byteArray2.count();
        nLength=11+2+nUserNameLen+nPassWordlen+4;

        byArrayUDPJLOrderDate[10]=0x01;
        byArrayUDPJLOrderDate[11]=nUserNameLen;
        for(int u=0; u<nUserNameLen; u++)
        {
            byArrayUDPJLOrderDate[12+u] = byteArray1[u];
        }
        byArrayUDPJLOrderDate[12+nUserNameLen]=nPassWordlen;
        for(int u=0; u<nPassWordlen; u++)
        {
            byArrayUDPJLOrderDate[13+nUserNameLen+u] = byteArray2[u];
        }
    }
    else if(ntype==1)//注销
    {
        byArrayUDPJLOrderDate[10]=0x03;
        QByteArray byteArray1 = strUserName.toLocal8Bit();//toLatin1();
        int nUserNameLen = byteArray1.count();
        nLength=11+1+nUserNameLen+4;

        //byArrayUDPJLOrderDate[10]=0x01;
        byArrayUDPJLOrderDate[11]=nUserNameLen;
        for(int u=0; u<nUserNameLen; u++)
        {
            byArrayUDPJLOrderDate[12+u] = byteArray1[u];
        }
    }
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
//调度命令签收槽
void MyDoc::DispatchOrderStationSignSolt(int norder,bool b)
{
    int nLength = 18;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};	//帧尾
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;

    byArrayUDPJLOrderDate[9]=FUNCTYPE_DISPTCH;
    byArrayUDPJLOrderDate[10]=0x01;
    byArrayUDPJLOrderDate[11]= norder;
    byArrayUDPJLOrderDate[12]= norder>>8;
    if(b)
    {
       byArrayUDPJLOrderDate[13]= 0xaa;
    }
    else
    {
       byArrayUDPJLOrderDate[13]= 0xbb;
    }

    //帧尾
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}

void MyDoc::ForWardDDOrderToTrainSolt(DispatchOrderStation mDispatchOrderStation)
{
    int nLength = 500;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[500] = {0x00};	//帧尾
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
//    //帧长度
//    byArrayUDPJLOrderDate[4]=nLength;
//    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
//    信息分类码-调度命令
    byArrayUDPJLOrderDate[9]=FUNCTYPE_DISPTCH;
    //调度命令转发机车
    byArrayUDPJLOrderDate[10]=0x02;
    //调度命令ID（数据库中自增）
    byArrayUDPJLOrderDate[11]= mDispatchOrderStation.order_id;
    byArrayUDPJLOrderDate[12]= mDispatchOrderStation.order_id>>8;
    //车次（机车）个数 strTrainnum
    int m=0;
    QStringList strtrainlist = mDispatchOrderStation.strTrainnum.split(",");
    QStringList strchecilist = mDispatchOrderStation.strCheci.split(",");
    m=strtrainlist.size();
    byArrayUDPJLOrderDate[13]=m;
    int nindex=13;
    for(int i=0;i<m;i++)
    {
        //车次号1长度 字符串长度m
        //车次号1 ASCII码
        QByteArray byteArray1 = strchecilist[i].toLocal8Bit();//toLatin1();
        int nncheciLen = byteArray1.count();
        QByteArray byteArray2 = strtrainlist[i].toLocal8Bit();//toLatin1();
        int ntrainlen = byteArray2.count();
        nindex++;
        byArrayUDPJLOrderDate[nindex]=nncheciLen;
        for(int u=0; u<nncheciLen; u++)
        {
            nindex++;
            byArrayUDPJLOrderDate[nindex] = byteArray1[u];
        }
        nindex++;
        byArrayUDPJLOrderDate[nindex]=ntrainlen;
        for(int u=0; u<ntrainlen; u++)
        {
            nindex++;
            byArrayUDPJLOrderDate[nindex] = byteArray2[u];
        }
    }
//    nLength=nindex+3;
   nLength=nindex+5;
   //帧长度
   byArrayUDPJLOrderDate[4]=nLength;
   byArrayUDPJLOrderDate[5]=nLength>>8;
    //帧尾
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
//调度命令 发送至机车
void MyDoc::SendOrderLocomotiveSolt(DispatchOrderLocomotive* mDispatchOrderLocomotive)
{
    int nLength = 1000;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[1000] = {0x00};	//帧尾
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
//    //帧长度
//    byArrayUDPJLOrderDate[4]=nLength;
//    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
//    信息分类码-调度命令
    byArrayUDPJLOrderDate[9]=FUNCTYPE_DISPTCH;
    //新建机车调度命令
    byArrayUDPJLOrderDate[10]=0x05;
    //    当前操作 1保存，2发送

    if(mDispatchOrderLocomotive->bSend)
    {
        byArrayUDPJLOrderDate[11]=2;

    }
    else
    {
       //byArrayUDPJLOrderDate[11]=1;
        if(mDispatchOrderLocomotive->bDel)
        {
            byArrayUDPJLOrderDate[11]=3;
        }
        else
        {
            byArrayUDPJLOrderDate[11]=1;
        }
    }

    //调度命令ID（数据库中自增）	2	首次新建时为0
    byArrayUDPJLOrderDate[12]=mDispatchOrderLocomotive->order_id;
    byArrayUDPJLOrderDate[13]=mDispatchOrderLocomotive->order_id>>8;
    //命令种类
    byArrayUDPJLOrderDate[14]=mDispatchOrderLocomotive->orderType;
    //命令号 4
    byArrayUDPJLOrderDate[15]=mDispatchOrderLocomotive->uNumber;
    byArrayUDPJLOrderDate[16]=mDispatchOrderLocomotive->uNumber>>8;
    byArrayUDPJLOrderDate[17]=mDispatchOrderLocomotive->uNumber>>16;
    byArrayUDPJLOrderDate[18]=mDispatchOrderLocomotive->uNumber>>24;

    QByteArray bstrType = mDispatchOrderLocomotive->strType.toLocal8Bit();//toLatin1();
    int nstrType = bstrType.count();
    //命令类型长度 1	字符串长度m
    byArrayUDPJLOrderDate[19]=nstrType;
    //命令类型内容	m	ASCII码
    int nindex=19;
    for(int u=0; u<nstrType; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrType[u];
    }
    //命令内容长度	2	字符串长度m
    QByteArray bstrContent = mDispatchOrderLocomotive->strContent.toLocal8Bit();//toLatin1();
    int nstrContent = bstrContent.count();
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrContent;
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrContent>>8;
    //命令内容	m	ASCII码
    for(int u=0; u<nstrContent; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrContent[u];
    }
    //车站长度	1	字符串长度m
    QByteArray bstrStation = mDispatchOrderLocomotive->strStation.toLocal8Bit();//toLatin1();
    int nstrStation = bstrStation.count();
    //车站	m	ASCII码
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrStation;
    for(int u=0; u<nstrStation; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrStation[u];
    }
    //值班人长度	1	字符串长度m
    //值班人	m	ASCII码
    QByteArray bstrDutyName = mDispatchOrderLocomotive->strDutyName.toLocal8Bit();//toLatin1();
    int nstrDutyName = bstrDutyName.count();
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrDutyName;
    for(int u=0; u<nstrDutyName; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrDutyName[u];
    }
    //车次（机车）个数 strTrainnum
//    int m=0;
//    QStringList strtrainlist = mDispatchOrderLocomotive.strTrainnum.split(",");
//    QStringList strchecilist = mDispatchOrderLocomotive.strCheci.split(",");
//    m=strtrainlist.size();
     nindex++;
    byArrayUDPJLOrderDate[nindex]=mDispatchOrderLocomotive->vectLocmtInfo.size();

    for(int i=0;i<mDispatchOrderLocomotive->vectLocmtInfo.size();i++)
    {
        //车次号1长度 字符串长度m
        //车次号1 ASCII码
        QByteArray byteArray1 = mDispatchOrderLocomotive->vectLocmtInfo[i].strCheCi.toLocal8Bit();//toLatin1();
        int nncheciLen = byteArray1.count();
        QByteArray byteArray2 = mDispatchOrderLocomotive->vectLocmtInfo[i].strLocomotive.toLocal8Bit();//toLatin1();
        int ntrainlen = byteArray2.count();
        nindex++;
        byArrayUDPJLOrderDate[nindex]=nncheciLen;
        for(int u=0; u<nncheciLen; u++)
        {
            nindex++;
            byArrayUDPJLOrderDate[nindex] = byteArray1[u];
        }
        nindex++;
        byArrayUDPJLOrderDate[nindex]=ntrainlen;
        for(int u=0; u<ntrainlen; u++)
        {
            nindex++;
            byArrayUDPJLOrderDate[nindex] = byteArray2[u];
        }
    }
    nLength=nindex+5;
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //帧尾
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
//调度命令 发送至调度台
void MyDoc::SendOrderDispatcherSolt(DispatchOrderDispatcher *mDispatchOrderDispatcher)
{
    int nLength = 500;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[500] = {0x00};	//帧尾
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
//    //帧长度
//    byArrayUDPJLOrderDate[4]=nLength;
//    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;
//    信息分类码-调度命令
    byArrayUDPJLOrderDate[9]=FUNCTYPE_DISPTCH;
    //新建调度台调度命令
    byArrayUDPJLOrderDate[10]=0x04;
    //    当前操作 1保存，2发送
    if(mDispatchOrderDispatcher->bSend)
    {
        byArrayUDPJLOrderDate[11]=2;

    }
    else
    {
       //byArrayUDPJLOrderDate[11]=1;
        if(mDispatchOrderDispatcher->bDel)
        {
             byArrayUDPJLOrderDate[11]=3;
        }
        else
        {
          byArrayUDPJLOrderDate[11]=1;
        }
    }


    //调度命令ID（数据库中自增）	2	首次新建时为0
    byArrayUDPJLOrderDate[12]=mDispatchOrderDispatcher->order_id;
    byArrayUDPJLOrderDate[13]=mDispatchOrderDispatcher->order_id>>8;
    //命令号 4
    byArrayUDPJLOrderDate[14]=mDispatchOrderDispatcher->uNumber;
    byArrayUDPJLOrderDate[15]=mDispatchOrderDispatcher->uNumber>>8;
    byArrayUDPJLOrderDate[16]=mDispatchOrderDispatcher->uNumber>>16;
    byArrayUDPJLOrderDate[17]=mDispatchOrderDispatcher->uNumber>>24;

    QByteArray bstrType = mDispatchOrderDispatcher->strType.toLocal8Bit();//toLatin1();
    int nstrType = bstrType.count();
    //命令类型长度 1	字符串长度m
    byArrayUDPJLOrderDate[18]=nstrType;
    //命令类型内容	m	ASCII码
    int nindex=18;
    for(int u=0; u<nstrType; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrType[u];
    }
    //命令内容长度	2	字符串长度m
    QByteArray bstrContent = mDispatchOrderDispatcher->strContent.toLocal8Bit();//toLatin1();
    int nstrContent = bstrContent.count();
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrContent;
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrContent>>8;
    //命令内容	m	ASCII码
    for(int u=0; u<nstrContent; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrContent[u];
    }
    //车站长度	1	字符串长度m
    QByteArray bstrStation = mDispatchOrderDispatcher->strStation.toLocal8Bit();//toLatin1();
    int nstrStation = bstrStation.count();
    //车站	m	ASCII码
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrStation;
    for(int u=0; u<nstrStation; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrStation[u];
    }
    //值班人长度	1	字符串长度m
    //值班人	m	ASCII码
    QByteArray bstrDutyName = mDispatchOrderDispatcher->strDutyName.toLocal8Bit();//toLatin1();
    int nstrDutyName = bstrDutyName.count();
    nindex++;
    byArrayUDPJLOrderDate[nindex]=nstrDutyName;
    for(int u=0; u<nstrDutyName; u++)
    {
        nindex++;
        byArrayUDPJLOrderDate[nindex] = bstrDutyName[u];
    }
    //创建时间 7	2年1月1日1时1分1秒
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.date().year();
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.date().year()>>8;
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.date().month();
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.date().day();
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.time().hour();
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.time().minute();
      nindex++;
      byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->timCreate.time().second();
    //调度台个数个数 strTrainnum
     nindex++;
     byArrayUDPJLOrderDate[nindex]=mDispatchOrderDispatcher->vectDispathInfo.size();

    for(int i=0;i<mDispatchOrderDispatcher->vectDispathInfo.size();i++)
    {
        //车次号1长度 字符串长度m
        //车次号1 ASCII码
        QByteArray byteArray1 = mDispatchOrderDispatcher->vectDispathInfo[i].strDispatcher.toLocal8Bit();//toLatin1();
        int nncheciLen = byteArray1.count();
        nindex++;
        byArrayUDPJLOrderDate[nindex]=nncheciLen;
        for(int u=0; u<nncheciLen; u++)
        {
            nindex++;
            byArrayUDPJLOrderDate[nindex] = byteArray1[u];
        }
    }
//    nLength=nindex+3;
     nLength=nindex+5;
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //帧尾
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
void MyDoc::SendAutoSignStateSolt(bool bsign)
{
    int nLength = 18;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};	//帧尾
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;

    byArrayUDPJLOrderDate[9]=FUNCTYPE_CHGMODE;
    byArrayUDPJLOrderDate[10]=0x03;
    byArrayUDPJLOrderDate[11]=bsign;

    //帧尾
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}
void MyDoc::SendStationCtrModeChangeSlot(int mode,int type1,int type2)
{
    int nLength = 18;
    unsigned int stationID=0x00;
    unsigned char byArrayUDPJLOrderDate[100] = {0x00};	//帧尾
    byArrayUDPJLOrderDate[0]=0xEF;
    byArrayUDPJLOrderDate[1]=0xEF;
    byArrayUDPJLOrderDate[2]=0xEF;
    byArrayUDPJLOrderDate[3]=0xEF;
    //帧长度
    byArrayUDPJLOrderDate[4]=nLength;
    byArrayUDPJLOrderDate[5]=nLength>>8;
    //本机地址
    byArrayUDPJLOrderDate[6]=GetStationID();
    //目的地址
    MyStation* pStation=nullptr;
    pStation=getMyStationByIndexInStaArray(currStaIndex);
    byArrayUDPJLOrderDate[7]=pStation->ABNUM;
    //车站标志 软件类型
    byArrayUDPJLOrderDate[8]=DATATYPE_CTC;

    byArrayUDPJLOrderDate[9]=FUNCTYPE_CHGMODE;
    byArrayUDPJLOrderDate[10]=mode;
    if(mode==2)
    {
        byArrayUDPJLOrderDate[11]= type1;
        byArrayUDPJLOrderDate[12]= type2;
    }
    else
    {
        byArrayUDPJLOrderDate[11]= type1;
    }

    //帧尾
    byArrayUDPJLOrderDate[nLength-4]=0xFE;
    byArrayUDPJLOrderDate[nLength-3]=0xFE;
    byArrayUDPJLOrderDate[nLength-2]=0xFE;
    byArrayUDPJLOrderDate[nLength-1]=0xFE;
    QByteArray dataArray;
    dataArray.resize(nLength);
    memcpy(dataArray.data(),byArrayUDPJLOrderDate,nLength);  //转换数据类型
    sendDataToCTCLogic(dataArray, nLength);
}

void MyDoc::SendTextAnalysis(TextFrame frame)
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    int nLength = 29 + frame.textLenght;
    QByteArray dataArray;
    dataArray.resize(nLength);
    int flag = 0;
    dataArray.replace(flag, 4, QByteArray::fromHex("efefefef"));
    flag += 4;
    dataArray[flag++]=nLength;
    dataArray[flag++]=nLength >> 8;
    //本机地址
    dataArray[flag++]=GetStationID();
    //目的地址
    dataArray[flag++]=pStation->ABNUM;

    dataArray[flag++]=DATATYPE_CTC;
    dataArray[flag++]=frame.captureState;
    dataArray[flag++]=frame.textDisplayState;

    QDateTime time = QDateTime::currentDateTime();
    QByteArray dateTimeArr;
    dateTimeArr.resize(7);
    dateTimeArr[0] = time.date().year();
    dateTimeArr[1] = time.date().year() >> 8;
    dateTimeArr[2] = time.date().month();
    dateTimeArr[3] = time.date().day();
    dateTimeArr[4] = time.time().hour();
    dateTimeArr[5] = time.time().minute();
    dateTimeArr[6] = time.time().second();

    dataArray.replace(flag, 7, dateTimeArr);
    flag += 7;

    dataArray[flag++]=frame.point.x();
    dataArray[flag++]=frame.point.x() >> 8;
    dataArray[flag++]=frame.point.y();
    dataArray[flag++]=frame.point.y() >> 8;

    dataArray[flag++]=frame.duration;
    dataArray[flag++]=frame.textLenght;
    dataArray[flag++]=frame.textColor;

    dataArray.replace(flag, frame.textLenght, frame.text);
    flag += frame.textLenght;
    dataArray.replace(flag, 4, QByteArray::fromHex("fefefefe"));
    flag += 4;
    sendDataToCTCLogic2(dataArray, nLength);
}
//槽-发送数据给CTC后台服务逻辑
void MyDoc::sendDataToCTCServerSlot(MyStation *pMyStation, QByteArray dataArray, int nLength)
{
    sendDataToCTCLogic(dataArray, nLength);
}
//虚信号灯状态处理
bool MyDoc::XXHDStateSend(MyStation *pMyStation, CXHD *XXHD)
{
    for(int p=0;p<pMyStation->mXXHDButtonInfo.XhdNameList.size();p++)
    {
        if(pMyStation->mXXHDButtonInfo.XhdNameList[p]==XXHD->m_strName)
        {
            if(pMyStation->mXXHDButtonInfo.nDevSendState==0)
            {
                return true;
            }
            else if(pMyStation->mXXHDButtonInfo.nDevSendState==1)
            {
                return false;
            }
        }
    }
    return false;
}

void MyDoc::SendIntervalLogicCheck(int nType, QString strName)
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    int nLength = 16 + strName.length();
    QByteArray dataArray;
    dataArray.resize(nLength);
    int flag = 0;
    dataArray.replace(flag, 4, QByteArray::fromHex("efefefef"));
    flag += 4;
    dataArray[flag++]=nLength;
    dataArray[flag++]=nLength >> 8;
    dataArray[flag++]=GetStationID();
    dataArray[flag++]=pStation->ABNUM;
    dataArray[flag++]=DATATYPE_CTC;

    dataArray[flag++]=FUNCTYPE_LOGICCHECK;

    dataArray[flag++]=nType;
    dataArray[flag++]=strName.length();
    dataArray.replace(flag, strName.length(), strName.toLocal8Bit());
    flag += strName.length();
    dataArray.replace(flag, 4, QByteArray::fromHex("fefefefe"));
    flag += 4;
    sendDataToCTCLogic(dataArray, nLength);
}
