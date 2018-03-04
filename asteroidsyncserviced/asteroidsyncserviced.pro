QT += core bluetooth dbus
QT -= gui

WATCHFISH_FEATURES = notificationmonitor walltime music

include(../version.pri)
include(libwatchfish/libwatchfish.pri)
include(libasteroid/libasteroid.pri)

TARGET = asteroidsyncserviced

CONFIG += c++11
CONFIG += console
CONFIG += link_pkgconfig

PKGCONFIG += qt5-boostable

SOURCES += main.cpp \
    watchesmanager.cpp \
    dbusinterface.cpp \
    platforms/sailfishos/sailfishplatform.cpp \
    bluez/bluezclient.cpp \
    bluez/bluez_agentmanager1.cpp \
    bluez/bluez_adapter1.cpp \
    bluez/bluez_device1.cpp \
    bluez/freedesktop_objectmanager.cpp \
    bluez/freedesktop_properties.cpp

HEADERS += \
    watchesmanager.h \
    dbusinterface.h \
    platforms/sailfishos/sailfishplatform.h \
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
