﻿#include "mydoc.h"
#include <QtDebug>
#include <QSettings>
#include <QMessageBox>
#include <QTextCodec>
#include "GlobalHeaders/GlobalFuntion.h"


MyDoc::MyDoc(QObject *parent) : QObject(parent)
{
    //变量初始化
    SysLifeSeconds = 0;
    currStaIndex = 0;
    bUdpMode = true;
    socketUDP = nullptr;
    tcpClient = nullptr;
    databasePort = 0;

    wndType = 1;

    pTimer = new QTimer(this);
    connect(pTimer, SIGNAL(timeout()), this, SLOT(timerTime()));
    pTimer->start(1000);
    initAllData();

    qInfo()<<"init MyDoc";
}
MyDoc::~MyDoc()
{
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        MyStation* pStation = vectMyStation.at(i);
        delete pStation;
    }
    for (int i = 0; i < vectMultiStation.size(); i++)
    {
        MyStation* pStation = vectMultiStation.at(i);
        delete pStation;
    }
    for (int i = 0; i < vectThread.size(); i++)
    {
        QThread* pThread = vectThread.at(i);
        if(pThread->isRunning())
        {
            pThread->quit();
        }
        delete pThread;
    }
    pTimer->stop();
    delete pTimer;
}

//初始化所有数据
void MyDoc::initAllData()
{
    readGlobalData();
    ReadTrainNumTypeTXT();
    ReadDDOrderTemplate();
    readMultiStationDispData();
    initNetCom();
    //最后初始化车站逻辑线程
    initGlobalLogic();
    //记录当前时间
    SysStartedDataTime = QDateTime::currentDateTime();
}
//读取全局数据
void MyDoc::readGlobalData()
{
    QString strFile="Data/Global.ini";
    QString ErrMsg = QString("%1配置数据格式错误：").arg(strFile);
    QFileInfo fileInfo(strFile);
    if (!fileInfo.isFile())
    {
        QLOG_ERROR()<<QString::fromLocal8Bit("File %1 not existed!").arg(strFile);
        return;
    }

    QSettings* settings = new QSettings(strFile, QSettings::IniFormat);
    // 指定为中文
    settings->setIniCodec("UTF-8");

    //读取配置-通信信息
//    bUdpMode = settings->value("Address/UdpMode").toInt() == 1 ? true:false;
//    QString localPorts = settings->value("Address/LocalPort").toString();
//    QStringList localPortList;
//    int c = StringSplit(localPorts, ",", localPortList);
//    if(c<=0)
//    {
//        QLOG_ERROR()<<QString::fromLocal8Bit("本地端口配置错误（LocalPort）！").arg(strFile);
//        return;
//    }
    //m_AddrCTCToService.addrCTC = "";//绑定本机所有地址
    m_AddrCTCToService.nPortCTC = settings->value("Address/LocalPort").toInt();
    m_AddrCTCToService.addrService = settings->value("Address/ServerAddr").toString();
    m_AddrCTCToService.nPortService1 = settings->value("Address/ServerPort1").toInt();
    m_AddrCTCToService.nPortService2 = settings->value("Address/ServerPort2").toInt();

    //读取配置-车站信息
    //int num = settings->value("Stations/Num").toInt();
    //QString str = settings->value("Stations/Num").toString();
    //str = str.toLower();//转小写，防错
    //int num = StringToHex(str);
    //int num = str.contains("0x")?str.toInt(0,16):str.toInt(0,10);
    int num = StringToHex(settings->value("Stations/Num").toString());
    for(int i=0; i<num; i++)
    {
        QString strNum = QString("Stations/Num%1").arg(i+1);
//        qDebug()<<strNum;
        QString info = settings->value(strNum).toString();
        QStringList strArr;
        int count = StringSplit(info, "|", strArr);
        if (2 == count)
        {
            MyStation *pMyStation = new MyStation();
            //pMyStation->setStationID(strArr[0].toInt());
            strArr[0] = strArr[0].toLower();//转小写，防错
            pMyStation->setStationID(StringToHex(strArr[0]));
//            qDebug() << "stationID=" << pMyStation->getStationID();
            pMyStation->setStationName(strArr[1]);
//            qDebug() << "stationName=" << pMyStation->getStationName();
            QString staFile = QString("Data/data%1/Station.txt").arg(i+1);
            QFileInfo fileInfo2(staFile);
            if (!fileInfo2.isFile())
            {
                QLOG_ERROR()<<QString::fromLocal8Bit("File %1 not existed!").arg(staFile);
                break;
            }
            if(pMyStation->readStationDev(staFile, pMyStation))
            {
                //匹配通过按钮与对应信号机关系
                pMyStation->SetXHJTGBtnState();
                staFile = QString("Data/data%1/StationConfig.txt").arg(i+1);
                pMyStation->readStationConfig(staFile, pMyStation);
                pMyStation->SetXHJPDJSState();
                staFile = QString("Data/data%1/BSConfig.txt").arg(i+1);
                pMyStation->readBSConfig(staFile, pMyStation);

//                staFile = QString(staFile_Path+"Data/data%1/LAMPConfig.txt").arg(i+1);
                staFile = QString("Data/data%1/LampFuncBConfig.txt").arg(i+1);
                pMyStation->readLampFuncBConfig(staFile, pMyStation);

                if(pMyStation->StaConfigInfo.isHavePXRJ == true)  //2020.7.9-BJT-是否有培训软件判别
                {
                    pTrainAndTest = new CTrainAndTest();   //培训新增 20200403
                    pTrainAndTest->pDoc = this;            //培训新增 20200403
                }

//                pMyStation->ServerIP = m_AddrCTCToService.addrService;
//                pMyStation->ServerPort1 = m_AddrCTCToService.nPortService1;
//                pMyStation->ServerPort2 = m_AddrCTCToService.nPortService2;
//                pMyStation->LocalPort = m_AddrCTCToService.nPortCTC;
                //pMyStation->LocalPort = localPortList[i].toInt();

                //发送数据绑定
                connect(pMyStation, &MyStation::sendDataToCTCServerSignal, this, &MyDoc::sendDataToCTCServerSlot);
                //发送数据绑定
                connect(pMyStation, &MyStation::sendDataToCTCServerSignal2, this, &MyDoc::sendDataToCTCServerSlot2);

                //初始化
                pMyStation->InitData();
                vectMyStation.append(pMyStation);

//                //工作线程
//                QThread *thread = new QThread(this);
//                vectThread.append(thread);
//                pMyStation->isThreadStart = true;
//                pMyStation->moveToThread(thread);
//                connect(thread, &QThread::started, pMyStation, &MyStation::startWorkSlot);
//                connect(pMyStation, &MyStation::endWorkSignal, thread, &QThread::quit);
//                //thread->start();//此处不启动，初始化结束后统一启动

//                //发送数据绑定
//                connect(pMyStation, &MyStation::sendDataToCTCServerSignal, this, &MyDoc::sendDataToCTCServerSlot);
//                //发送数据绑定
//                connect(pMyStation, &MyStation::sendDataToCTCServerSignal2, this, &MyDoc::sendDataToCTCServerSlot2);

            }
        }
        else
        {
            QLOG_ERROR()<<ErrMsg<<strNum;
        }
    }

//    //数据库地址账号信息
//    databaseIP = settings->value("DataBase/HostIP").toInt();
//    databaseName = settings->value("DataBase/DataBase").toInt();
//    databaseUser = settings->value("DataBase/UserName").toInt();
//    databasePassWord = settings->value("DataBase/PassWord").toInt();
//    databasePort = settings->value("DataBase/Port").toInt();
    bRouteOrderShow = settings->value("CONFIG/LCJLXLWNDSHOW").toBool();
    bSignRouteSyn = settings->value("CONFIG/ManSignRouteSynType").toBool();
    delete settings;
}

void MyDoc::readMultiStationDispData()
{
    QString strFile="Data/MultiStation/MultiStation.ini";
    QFileInfo fileInfo(strFile);
    if (!fileInfo.isFile())
    {
        QLOG_ERROR()<<QString::fromLocal8Bit("File %1 not existed!").arg(strFile);
        return;
    }
    QSettings* settings = new QSettings(strFile, QSettings::IniFormat);
    // 指定为中文
    settings->setIniCodec("UTF-8");

    //读取配置-车站信息
    int num = settings->value("MultiStationDisp/Num").toInt();
//    qDebug()<<num;
    for(int i=0; i<num; i++)
    {
        QString strNum = QString("Station%1").arg(i+1);
        int id = settings->value(QString("%1/ID").arg(strNum)).toInt();
        QString name = settings->value(QString("%1/Name").arg(strNum)).toString();

        MyStation *pMyStation = new MyStation;
        pMyStation->setStationID(id);
        pMyStation->setStationName(name);

        QString staFile = QString("Data/MultiStation/data%1/Station.txt").arg(i+1);
        QFileInfo fileInfo2(staFile);
        if (!fileInfo2.isFile())
        {
            QLOG_ERROR()<<QString::fromLocal8Bit("File %1 not existed!").arg(staFile);
            break;
        }
        if(pMyStation->readStationDev(staFile, pMyStation))
        {
            pMyStation->SetXHJTGBtnState();
            staFile = QString("Data/MultiStation/data%1/StationConfig.txt").arg(i+1);
            pMyStation->readStationConfig(staFile, pMyStation);
            pMyStation->SetXHJPDJSState();

            staFile = QString("Data/MultiStation/data%1/BSConfig.txt").arg(i+1);
            pMyStation->readBSConfig(staFile, pMyStation);

            staFile = QString("Data/MultiStation/data%1/LampFuncBConfig.txt").arg(i+1);
            pMyStation->readLampFuncBConfig(staFile, pMyStation);

            //初始化
            pMyStation->InitData(true);
            vectMultiStation.append(pMyStation);

            //工作线程
            QThread *thread = new QThread(this);
            vectThread.append(thread);
            pMyStation->isThreadStart = true;
            pMyStation->moveToThread(thread);
            connect(thread, &QThread::started, pMyStation, &MyStation::startWorkSlot);
            connect(pMyStation, &MyStation::endWorkSignal, thread, &QThread::quit);
            //thread->start();//此处不启动，初始化结束后统一启动

            //发送数据绑定
            connect(pMyStation, &MyStation::sendDataToCTCServerSignal, this, &MyDoc::sendDataToCTCServerSlot);

        }
    }
}
//初始化通信
void MyDoc::initNetCom()
{
    //主通信方式UDP模式
    //if(bUdpMode)
    {
        socketUDP = new SocketUDP;
        if(socketUDP->initAnyIP(m_AddrCTCToService.nPortCTC))
        {
            connect(socketUDP,SIGNAL(recvDataSignal(QByteArray, QString, int)),this,SLOT(receiveCTCServerDataSlot(QByteArray, QString, int)));
            connect(this,SIGNAL(sendDataToMainSignal(QByteArray,QString,int,int)),socketUDP,SLOT(sendDataSlot(QByteArray,QString,int,int)));
            QLOG_INFO()<<QString("本地端口Any:%1初始化成功！").arg(m_AddrCTCToService.nPortCTC);
        }
    }
    QLOG_INFO()<<QString("与服务端主通信1-%1:%2读取成功！").arg(m_AddrCTCToService.addrService).arg(m_AddrCTCToService.nPortService1);
    //主通信方式TCP模式
    //else
    {
//        serverTcp = new ServerTCP;
//        serverTcp->initAnyIP(m_AddrCTCToService.nPortCTC);
        //if(m_AddrCTCToService.nPortService2 != 0)
        {
            tcpClient = new SocketTCP;
            if(tcpClient->initByIP(m_AddrCTCToService.addrService, m_AddrCTCToService.nPortService2))
            {
                connect(tcpClient,SIGNAL(onConnectedSignal(QString, int)),this,SLOT(tcpClientConnectedSlot(QString, int)));
                connect(tcpClient,SIGNAL(onDisconnectedSignal(QString, int)),this,SLOT(tcpClientDisconnectedSlot(QString, int)));
                connect(tcpClient,SIGNAL(recvDataSignal(QByteArray,QString, int)),this,SLOT(receiveCTCServerDataSlot2(QByteArray,QString,int)));
                connect(this,SIGNAL(sendDataToMainSignal2(QByteArray,int)),tcpClient,SLOT(sendDataSlot(QByteArray,int)));
                QLOG_INFO()<<QString("与服务端主通信2-%1:%2初始化成功！").arg(m_AddrCTCToService.addrService).arg(m_AddrCTCToService.nPortService2);
            }
        }
    }
}
//初始化全局逻辑
void MyDoc::initGlobalLogic()
{
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        MyStation* pStation = vectMyStation.at(i);
        //工作线程
        QThread *thread = new QThread(this);
        vectThread.append(thread);
        pStation->isThreadStart = true;
        pStation->moveToThread(thread);
        connect(thread, &QThread::started, pStation, &MyStation::startWorkSlot);
        connect(pStation, &MyStation::endWorkSignal, thread, &QThread::quit);
        //thread->start();//此处不启动，初始化结束后统一启动
    }
    //统一启动车站线程
    for (int i = 0; i < vectThread.size(); i++)
    {
        QThread* pThread = vectThread.at(i);
        pThread->start();
    }
}
//定时器-1秒定时器
void MyDoc::timerTime()
{
    //统计运行时间
    SysLifeSeconds = SysStartedDataTime.msecsTo(QDateTime::currentDateTime())/1000;
    //qDebug()<<"[SysLifeSeconds]="<<SysLifeSeconds;

    //发送心跳
    SendHeartBeatToTCCService();
    //update();

    //系统启动后4秒时发送请求数据
    //if(4 == SysLifeSeconds)
    if(bRequstData)
    {
        bRequstData = false;
        SysInitSeconds = SysLifeSeconds;
//        qDebug()<<"[SysLifeSeconds]="<<SysLifeSeconds;
    }
    //再过1秒执行
    if((SysInitSeconds+1) == SysLifeSeconds)
    {
        for (int i = 0; i < vectMyStation.size(); i++)
        {
            MyStation* pStation = vectMyStation.at(i);
            //debug
//            qDebug()<<QString("[%1]请求所有数据").arg(pStation->getStationName());
            //请求所有数据
            pStation->RequestDataFromServer(SYNC_ALL);
        }
    }
}
//根据站名获取索引
int MyDoc::getIndexByStaNameInStaArray(QString strStation)
{
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        MyStation* pStation = vectMyStation.at(i);
        if (strStation == pStation->getStationName())
        {
            return i;
        }
    }
    return -1;
}

//根据站名在车站数组中获取车站指针
MyStation* MyDoc::getMyStationByStaNameInStaArray(QString strStation)
{
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        MyStation* pStation = vectMyStation.at(i);
        if (strStation == pStation->getStationName())
        {
            return pStation;
        }
    }
    return NULL;
}

//根据id在车站数组中获取车站指针
MyStation* MyDoc::getMyStationByStaIDInStaArray(int id)
{
    for (int i = 0; i < vectMyStation.size(); i++)
    {
        MyStation* pStation = vectMyStation.at(i);
        if (id == pStation->getStationID())
        {
            return pStation;
        }
    }
    return NULL;
}

//根据索引在车站数组中获取车站指针
MyStation* MyDoc::getMyStationByIndexInStaArray(int idx)
{
    if (0 <= idx && idx <vectMyStation.size())
    {
        MyStation* pStation = vectMyStation.at(idx);
        return pStation;
    }
    return NULL;
}
//根据索引在车站数组中获取车站指针-多站
MyStation* MyDoc::getMultiStationByIndexInStaArray(int idx)
{
    if (0 <= idx && idx <vectMultiStation.size())
    {
        MyStation* pStation = vectMultiStation.at(idx);
        return pStation;
    }
    return NULL;
}
MyStation* MyDoc::getMultiMyStationByStaIDInStaArray(int id)
{
    for (int i = 0; i < vectMultiStation.size(); i++)
    {
        MyStation* pStation = vectMultiStation.at(i);
        int staId = (int)(pStation->getStationID());
        //qDebug()<<staId;
        if (id == staId)
        {
            return pStation;
        }
    }
    return NULL;
}
//在车站数组中获取当前车站指针
MyStation *MyDoc::getCurrMyStationInStaArray()
{
    MyStation* pStation = getMyStationByIndexInStaArray(currStaIndex);
    return pStation;
}
int MyDoc::setCurrIndexByStaName(QString strStation)
{
    //设置车站索引
    currStaIndex = getIndexByStaNameInStaArray(strStation);
    if (currStaIndex > -1)
    {
        //用于控制命令栏的状态更新
        //GetMyStationByStaNameInStaArray(strStation)->m_bModeChanged = TRUE;
    }
    return currStaIndex;
}
//读取列车运行类型配置文件
void MyDoc::ReadTrainNumTypeTXT()
{
    QString filePath = GetWorkDirRoot()+"Data/TrainNumType.txt";
    QFile file(filePath);
    // 指定为GBK
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QLOG_ERROR()<<"打开TrainNumType.txt数据文件失败!";
        return;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        //QString m_str(line);
        QString m_str = codec->toUnicode(line);
        QString getString;
        if(m_str.left(4) == "####")
        {
            break;
        }
        else if(m_str.left(2) == "//")
        {
            continue;
        }
        else if(m_str.indexOf("|") >= 0)
        {
            QStringList strArr;
            int c = StringSplit(m_str, "|", strArr);
            if(c == 2)
            {
                TrainNumType *trainNum_type = new TrainNumType;
                trainNum_type->strType = strArr[0].trimmed();
                trainNum_type->strTypeName = strArr[1].trimmed();
                v_TrainNumType.append(trainNum_type);
            }
        }
        else
        {
            v_TrainRunType.push_back(m_str.trimmed());
        }
    }
    file.close();
    //各站同步
    InitStationTrainNumType();
}
//读取调度命令类型模板
void MyDoc::ReadDDOrderTemplate()
{
    DisOrderTemplate mDisOrderTemplate;
    //读取调度命令模板
    QString filePath = GetWorkDirRoot()+"Data/DispatchOrder.conf";
    QFile file(filePath);
    // 指定为GBK
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QLOG_ERROR()<<"DispatchOrder.conf数据文件失败!";
        return;
    }
    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        QString m_str = codec->toUnicode(line);
        QString getString;
        m_str=m_str.remove("\n");
        if(m_str == "####")
        {
            break;
        }
        else if(m_str=="##")	//开始读取模板
        {
            line = file.readLine();
            m_str = codec->toUnicode(line);
            m_str=m_str.remove("\n");
            mDisOrderTemplate.Init();
            if(m_str.left(18) == "strDisOrderHeader:")
            {
                line = file.readLine();
                m_str = codec->toUnicode(line);
                m_str=m_str.remove("\n");
                mDisOrderTemplate.strDisOrderHeader=m_str;
                //qDebug()<<"strDisOrderHeader:"<<m_str;
            }
            line = file.readLine();
            m_str = codec->toUnicode(line);
            m_str=m_str.remove("\n");
            if(m_str.left(19) == "strDisOrderContent:")
            {
                line = file.readLine();
                m_str = codec->toUnicode(line);
                m_str=m_str.remove("\n");
                mDisOrderTemplate.strDisOrderContent=m_str;
                //qDebug()<<"strDisOrderContent:"<<m_str;
            }
            mVectDisOrderTemplate.append(mDisOrderTemplate);
        }
    }
    file.close();
}
//各站列车运行类型数据同步初始化
void MyDoc::InitStationTrainNumType()
{
    for (int s = 0; s < vectMyStation.count(); s++)
    {
        MyStation* myStation = vectMyStation[s];
        for(int a=0; a<v_TrainNumType.count(); a++)
        {
            myStation->v_TrainNumType.append(v_TrainNumType[a]);
        }
        for(int a=0; a<v_TrainRunType.count(); a++)
        {
            myStation->v_TrainRunType.append(v_TrainRunType[a]);
        }
    }
}

//将接收到的数据根据帧头帧位进行分割并返回分割后的二维数组
QVector<QByteArray> MyDoc::SplitReceiveData_SameHeadTail(QByteArray recvArray)
{
    QVector<QByteArray> dataArray;//用于存放分割后的数据
    QByteArray tempArray;//临时数组

    //长度不够则退出
    if(recvArray.size()<14)
    {
        return dataArray;
    }

    int head = 0xEF;//帧头
    int tail = 0xFE;//帧尾
    QVector<int> header;
    header.append(head);
    header.append(head);
    header.append(head);
    header.append(head);
    QVector<int> ender;
    ender.append(tail);
    ender.append(tail);
    ender.append(tail);
    ender.append(tail);
    int index_Header=0;
    int dataLen=0;//数据长度包含帧头和帧尾

    //从头开始遍历数据帧
    for(int i=0; i<recvArray.size(); i++)
    {
        //比对帧头
        bool headFlag=true;
        for(int j=0;j<header.size();j++)
        {
            if(i+j+ender.size() >= recvArray.size())//越界防护
            {
                headFlag=false;
                break;
            }
            //qDebug()<<"(int)header[j]="<<(int)header[j]<<",(int)recvArray[i+j]="<<(int)(recvArray[i+j]&0xFF);
            if((int)header[j] != (int)(recvArray[i+j]&0xFF))
            {
                headFlag=false;
                break;
            }
        }
        if(!headFlag)
        {
            continue;
        }

        //记录帧头开始位置
        index_Header = i;
        //数据长度包含帧头和帧尾
        dataLen = (int)((recvArray[i+header.size()]&0xFF) | ((recvArray[i+header.size()+1]&0xFF)<<8));
        //帧头+内容+帧尾
        if(i+dataLen>recvArray.size())//越界防护
        {
            break;
        }
        //将i指向帧尾开始的位置，避免不必要的循环
        i+= (dataLen-ender.size());

        //比对帧尾
        bool endFlag=true;
        for(int j=0; j<ender.size(); j++)
        {
            if(i+j>=recvArray.size())  //简单防护
            {
                endFlag=false;
                break;
            }
            if(ender[j] != (int)(recvArray[i+j]&0xFF))
            {
                endFlag=false;
                break;
            }
        }
        if(endFlag == false)
        {
            continue;//帧尾校验失败则不作为帧尾处理
        }

        //加入整包数据
        tempArray.append(recvArray.data()+index_Header, dataLen);
        dataArray.append(tempArray);
        tempArray.clear();

        i+= (ender.size());//到帧尾结束的位置
        i--;
    }

    return dataArray;
}

//培训软件-根据设备编号获取设备名称
QString MyDoc::getNameOfDevNode(MyStation* pStation, unsigned int devnode)
{
    for(auto ement:pStation->DevArray)
    {
        if(ement->getDevType() == Dev_DC)
        {
            CGDDC *gddc=(CGDDC *)ement;
            if(devnode == (int)gddc->getCode())
            {
                return gddc->getName();
            }
        }
        else if(ement->getDevType() == Dev_XH)
        {
            CXHD *xhd=(CXHD *)ement;
            if(devnode == (int)xhd->getCode())
            {
                return xhd->getName();
            }
        }
        else if(ement->getDevType() == Dev_GD)
        {
            CGD *gd=(CGD*)ement;
            if(devnode == (int)gd->getCode())
            {
                return gd->getName();
            }
        }
    }
    return "";
}
//培训软件-根据设备名称获取设备编号
unsigned int MyDoc::getDevNodeOfName(MyStation* pStation, QString strName)
{
    for(auto ement:pStation->DevArray)
    {
        if(ement->getDevType() == Dev_DC)
        {
            CGDDC *gddc=(CGDDC *)ement;
            if(strName == gddc->getName())
            {
                return gddc->getCode();
            }
        }
        else if(ement->getDevType() == Dev_XH)
        {
            CXHD *xhd=(CXHD *)ement;
            if(strName == xhd->getName())
            {
                return xhd->getCode();
            }
        }
        else if(ement->getDevType() == Dev_GD)
        {
            CGD *gd=(CGD*)ement;
            if(strName == gd->getName())
            {
                return gd->getCode();
            }
        }
    }
    return 65535;
}
