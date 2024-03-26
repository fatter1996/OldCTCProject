QT       += core gui
QT       += network
QT       += texttospeech
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS   += -lDbgHelp

CONFIG += c++11

RC_FILE=CTCPro_resource.rc

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include(LogIn/LogIn.pri)
include(StationStorage/StationStorage.pri)
include(TrainMarshal/TrainMarshal.pri)
include(SystemAlarm/SystemAlarm.pri)
include(QJlogic/QJlogic.pri)
include(Option/Option.pri)
include(yt46/yt46.pri)
include(tsrs/tsrs.pri)
include(menuclass/menuclass.pri)
include(DDOrder/DDOrder.pri)
include(StagePlan/StagePlan.pri)
include(TableView/TableView.pri)
include(RouteOrder/RouteOrder.pri)
include(TableForm/TableForm.pri)
#全局变量模块
include(GlobalHeaders/GlobalHeaders.pri)
#车站基类模块
include(BaseDataPro/BaseDataPro.pri)
#自定义车站模块
include(MyStation/MyStation.pri)
#通信模块
include(SocketPro/SocketPro.pri)
#主业务逻辑模块
include(MyDoc/MyDoc.pri)
#轻量级日志库
include(QsLog/QsLog.pri)
#自定义日志使用
include(Log/Log.pri)

#站场界面
include(StationInterface/StationInterface.pri)
#防错办窗口
include(AgainstWrong/AgainstWrong.pri)

include(TextAnalysis/TextAnalysis.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/DDOrder/DDOrderManage
INCLUDEPATH += $$PWD/DDOrder/DDOrderSign
#INCLUDEPATH += $$PWD/MultistationDisp
INCLUDEPATH += $$PWD/TrafficLog
INCLUDEPATH += $$PWD/StationInterface/StationInterfaceKSK
RESOURCES += \
    Res.qrc
# win64 {
#           CONFIG(debug, debug|release) {
#           #        DEFINES += _DEBUG
#           VLD_PATH = C:/Program Files (x86)/Visual Leak Detector
#           INCLUDEPATH += $VLD_PATH/include
#           LIBS += -L$VLD_PATH/lib/Win64 -lvld
#            }
#       }

DISTFILES += \
    DevelopLog.txt



