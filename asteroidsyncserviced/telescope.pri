
SOURCES += platforms/ubuntutouch/main.cpp \
    watchesmanager.cpp \
    dbusinterface.cpp \
    platforms/ubuntutouch/ubuntuplatform.cpp \
    bluez/bluezclient.cpp \
    bluez/bluez_agentmanager1.cpp \
    bluez/bluez_adapter1.cpp \
    bluez/bluez_device1.cpp \
    bluez/freedesktop_objectmanager.cpp \
    bluez/freedesktop_properties.cpp

HEADERS += \
    watchesmanager.h \
    dbusinterface.h \
    platforms/ubuntutouch/ubuntuplatform.h \
    bluez/bluezclient.h \
    bluez/bluez_agentmanager1.h \
    bluez/bluez_adapter1.h \
    bluez/bluez_device1.h \
    bluez/freedesktop_objectmanager.h \
    bluez/freedesktop_properties.h

INSTALLS += target systemd

systemd.files = $${TARGET}.service
systemd.path = /usr/lib/systemd/user

target.path = /usr/bin
