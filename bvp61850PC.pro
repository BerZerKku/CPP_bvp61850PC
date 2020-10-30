QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG(release, debug|release): DEFINES += NDEBUG

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bvpCommon/debug.cpp \
    bvpCommon/modbus.cpp \
    bvpCommon/param.cpp \
    bvpCommon/serialprotocol.cpp \
    control.cpp \
    main.cpp \
    mainwindow.cpp \
    qcolorbutton.cpp \
    widget/qledindicator.cpp \
    widget/qpush1.cpp \
    readreg/readreg.cpp \
    readreg/vpitem.cpp \
    readreg/vpreg.cpp \
    serial.cpp \
    serial/serialport.cpp

HEADERS += \
    bvpCommon/debug.hpp \
    bvpCommon/hardware.hpp \
    bvpCommon/modbus.h \
    bvpCommon/param.h \
    bvpCommon/serialprotocol.h \
    control.h \
    mainwindow.h \
    qcolorbutton.h \
    widget/qledindicator.h \
    widget/qpush1.h \
    readreg/readreg.h \
    readreg/vpitem.h \
    readreg/vpreg.h \
    serial.h \
    serial/serialport.h

FORMS += \
    control.ui \
    mainwindow.ui \
    serial.ui \
    vpitem.ui \
    vpreg.ui

INCLUDEPATH += \
    bvpCommon/


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
