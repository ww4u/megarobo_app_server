QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    mappserver.cpp \
    mappservice.cpp \
    serverintf.cpp \
    t4/t4para.cpp \
    t4/mrx_t4service.cpp \
    t4/mrx_t4server.cpp \
    intfserverpara.cpp

HEADERS += \
    mappserver.h \
    mappservice.h \
    serverintf.h \
    t4/t4para.h \
    t4/mrx_t4service.h \
    t4/mrx_t4server.h \
    intfserverpara.h

INCLUDEPATH += "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include"

LIBS += -L"../code/lib"
LIBS += -lmrx-device

win32:LIBS += -L"C:/Program Files (x86)/IVI Foundation/VISA/WinNT/lib/msc"
win32:LIBS += -lvisa32

win32:LIBS += -L"../code/3rdlib"
win32:LIBS += -llibws2_32 -llibiphlpapi

