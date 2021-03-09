BVP_PATH = $$system(pwd)/bvpCommon

INCLUDEPATH += \
    $$system(pwd) \
    $${BVP_PATH}

HEADERS += \
    $${BVP_PATH}/bvpCommon.hpp \
    $${BVP_PATH}/debug.hpp \
    $${BVP_PATH}/extAlarm.hpp \
    $${BVP_PATH}/global.hpp \
    $${BVP_PATH}/hardware.hpp \
    $${BVP_PATH}/param.h \
    $${BVP_PATH}/paramDB.h\
    $${BVP_PATH}/serial/avantpc.h \
    $${BVP_PATH}/serial/avantpi.h \
    $${BVP_PATH}/serial/modbusVp.h \
    $${BVP_PATH}/serial/protocolAvant.h \
    $${BVP_PATH}/serial/serialprotocol.h \
    $${BVP_PATH}/serial/ringArray.hpp

SOURCES += \
    $${BVP_PATH}/bvpCommon.cpp \
    $${BVP_PATH}/debug.cpp \
    $${BVP_PATH}/param.cpp \
    $${BVP_PATH}/serial/avantpc.cpp \
    $${BVP_PATH}/serial/avantpi.cpp \
    $${BVP_PATH}/serial/modbusVp.cpp \
    $${BVP_PATH}/serial/protocolAvant.cpp \
    $${BVP_PATH}/serial/serialprotocol.cpp

