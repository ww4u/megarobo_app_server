QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_NO_DEBUG_OUTPUT

# \note the VERSION variable is only valid for WINDOWS
#VERSION = 0.0.0.0

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    mappservice.cpp \
    serverintf.cpp \
    t4/t4para.cpp \
    t4/mrx_t4service.cpp \
    intfserverpara.cpp \
    intfobj.cpp \
    t4/intfseries.cpp \
    mappexec.cpp \
    mtcpserver.cpp \
    t4/mrx_t4tcpserver.cpp \
    mappserver.cpp \
    t4/mrx_t4server.cpp \
    syspara.cpp \
    lte/letpara.cpp \
    lte/let_tcpserver.cpp \
    lte/let_service.cpp \
    lte/let_server.cpp \
    ../../pump/module/axes.cpp \
    ../../pump/module/devicehandle.cpp \
    ../../pump/module/device.cpp

HEADERS += \
    mappservice.h \
    serverintf.h \
    t4/t4para.h \
    t4/mrx_t4service.h \
    intfserverpara.h \
    intfobj.h \
    t4/intfseries.h \
    mappexec.h \
    myjson.h \
    mtcpserver.h \
    t4/mrx_t4tcpserver.h \
    mappserver.h \
    t4/mrx_t4server.h \
    syspara.h \
    lte/letpara.h \
    lte/let_tcpserver.h \
    lte/let_service.h \
    lte/let_server.h \
    lte/let_intfseries.h \
    ../../pump/module/axes.h \
    ../../pump/module/devicehandle.h \
    ../../pump/module/device.h

win32 {
INCLUDEPATH += "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include"
INCLUDEPATH += "G:/work/mct/source/device/MegaGateway_SDK/src"
INCLUDEPATH += "G:/work/pump/module"
}

linux-arm* {
INCLUDEPATH += "/home/w/work/mct-t4/source/device/MegaGateway_SDK/src"
INCLUDEPATH += ../../pump/module
DEFINES += _RASPBERRY
}

LIBS += -L"../code/lib"
LIBS += -lmrx-device

win32:LIBS += -L"C:/Program Files (x86)/IVI Foundation/VISA/WinNT/lib/msc"
win32:LIBS += -lvisa32

win32:LIBS += -L"../code/3rdlib"
win32:LIBS += -llibws2_32 -llibiphlpapi

DISTFILES += \
    ../readme.md
