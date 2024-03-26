#include "mainwindow.h"
#include <QApplication>
#include "MyDoc/mydoc.h"
#include <QTranslator>
#include "Log/log.h"
#include <Windows.h>
#include <DbgHelp.h>
#include <QMessageBox>
#include <QApplication>
#include <QCoreApplication>
//#include <Windows.h>
//#include <WinBase.h>
//#include <QSystemSemaphore>
//#include <QSharedMemory>
#include <QLockFile>

//#include "vld.h"
//#pragma comment(lib, "vld.lib")

LONG CreateCrashHandler(EXCEPTION_POINTERS *pException){
    //创建 Dump 文件
    QDateTime CurDTime = QDateTime::currentDateTime();
    QString current_date = CurDTime.toString("yyyy_MM_dd_hh_mm_ss");
    //dmp文件的命名
    QString dumpText = "Dump_"+current_date+".dmp";
    EXCEPTION_RECORD *record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode, 16));
    QString errAddr(QString::number((uint)record->ExceptionAddress, 16));
    QString errFlag(QString::number(record->ExceptionFlags, 16));
    QString errPara(QString::number(record->NumberParameters, 16));
    HANDLE DumpHandle = CreateFile((LPCWSTR)dumpText.utf16(),
             GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(DumpHandle != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;
        //将dump信息写入dmp文件
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),DumpHandle, MiniDumpNormal, &dumpInfo, NULL, NULL);
        CloseHandle(DumpHandle);
    }
    //创建消息提示
    QMessageBox::warning(NULL,"Dump",QString("ErrorCode%1  ErrorAddr：%2  ErrorFlag:%3 ErrorPara:%4").arg(errCode).arg(errAddr).arg(errFlag).arg(errPara),
        QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}

//程序是否在运行
bool processIsRunning()
{
    bool isRunning = false;
    //方法：文件锁判定程序是否已启动
    //判定同目录、同名称的程序是否在运行
    QString appFilePath = QCoreApplication::applicationFilePath();//qApp->applicationFilePath();
//    qDebug()<<appFilePath;
    QString appName = qApp->applicationName();//+".exe";
//    qDebug()<<appName;
    QString lockFilePath;
    lockFilePath = appFilePath+".singleApp.lock";
//    qDebug() << "lockFilePath: " + lockFilePath;
    QLockFile *lockFile = new QLockFile(lockFilePath);
    if(!lockFile->tryLock(1000))
    {
//        qDebug() << "上锁失败，不能启动！"+appName;
        QMessageBox::warning(NULL, "警告", "程序已经在运行！"+appName);
        isRunning = true;
    }
    else
    {
//        qDebug() << "上锁成功，可以启动！"+appName;
        isRunning = false;
    }
    return isRunning;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //重复启动判断
    if(processIsRunning())
    {
        return -1;
    }

    //获取系统编码
    //QTextCodec *codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    //注冊异常捕获函数
    //SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)CreateCrashHandler);

    //加载Qt标准对话框的中文翻译文件
    QTranslator tran;
    QString originText = GetWorkDirRoot();
    QString searchText = "/";
    QString replaceText = "\\";
    QString filePath = originText.replace(QRegExp(searchText), replaceText);
    bool ok = tran.load("qt_zh_CN.qm", filePath+"Data\\");//注,这里使用的是绝对路径,建议把.qm拷贝到工程目录下,然后使用相对路径.
    a.installTranslator(&tran);

    MainWindow w;
    w.showMaximized();
    //日志
    QMyLog log;
    //日志初始化
    log.initLogger();

    MyDoc* pDoc = new MyDoc();
    if(pDoc->vectMyStation.size()<=0)
    {
        QString strMsg = "没有读取到正确的车站信息！";
        QLOG_ERROR() << strMsg;
        QMessageBox::warning(NULL, "错误", strMsg);
        //没有车站数据则退出
        a.exit();
        return 0;
    }
    //界面初始化
    pDoc->InitMainWnd(&w);
    pDoc->InitMultiStationDisp();
    w.show();
    pDoc->bRequstData = true;
    return a.exec();
}


