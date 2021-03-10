include(../bvp61850.pri)

QT       += core gui serialport widgets

CONFIG += c++11

CONFIG(release, debug|release): DEFINES += NDEBUG

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    wrapper.cpp \
    alarm/alarm.cpp \
    control/control.cpp \
    paramtree/paramtree.cpp \
    serial/serial.cpp \
    serial/serialport.cpp \
    vpanel/readreg.cpp \
    vpanel/vpitem.cpp \
    vpanel/vpreg.cpp \
    widget/combobox.cpp \
    widget/qcolorbutton.cpp \
    widget/qledindicator.cpp \
    widget/qpush1.cpp


HEADERS += \
    mainwindow.h \
    wrapper.h \
    alarm/alarm.h \
    control/control.h \
    paramtree/paramtree.h \
    widget/combobox.h \
    widget/qcolorbutton.h \
    widget/qledindicator.h \
    widget/qpush1.h \
    vpanel/readreg.h \
    vpanel/vpitem.h \
    vpanel/vpreg.h \
    serial/serial.h \
    serial/serialport.h


FORMS += \
    mainwindow.ui \
    alarm/alarm.ui \
    control/control.ui \
    serial/serial.ui \
    vpanel/vpitem.ui \
    vpanel/vpreg.ui

INCLUDEPATH +=


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
