include(../bvp61850.pri)

QT += testlib         #Include QtTest to use SignalSpy, QTest::mouseClick, etc

TARGET = Tests
TEMPLATE = app

#CONFIG += c++14
CONFIG += testcase
CONFIG -= debug_and_release
CONFIG += cmdline

DEFINES += \
    NDEBUG

INCLUDEPATH += \
    googletest \

HEADERS += \

SOURCES += \
    googletest/gtest-all.cc \
    main.cpp \
    tst_extAlarm.cpp \
#    tst_bvpCommon.cpp \



#    $${PIG_PATH}/src/menu/menu.cpp \
#	$${PIG_PATH}/src/protocols/iec101/CIec101.cpp \
#	$${PIG_PATH}/src/protocols/modbus/protocolModbus.cpp \
