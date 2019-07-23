QT += core bluetooth dbus
QT -= gui

include(../version.pri)
include(libasteroid/libasteroid.pri)

TARGET = asteroidsyncserviced

CONFIG += c++11
CONFIG += console
CONFIG += link_pkgconfig

contains(CONFIG, telescope) {
    include(telescope.pri)
} 

contains(CONFIG, starfish) {
    include(starfish.pri)
}

SOURCES += watchesmanager.cpp \
    dbusinterface.cpp \
    bluez/bluezclient.cpp \
    bluez/bluez_agentmanager1.cpp \
    bluez/bluez_adapter1.cpp \
    bluez/bluez_device1.cpp \
    bluez/freedesktop_objectmanager.cpp \
    bluez/freedesktop_properties.cpp

HEADERS += watchesmanager.h \
    dbusinterface.h \
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
