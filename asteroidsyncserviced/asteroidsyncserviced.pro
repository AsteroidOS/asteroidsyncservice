QT += core bluetooth dbus
QT -= gui

WATCHFISH_FEATURES = notificationmonitor walltime music

SUBDIRS += libwatchfish

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
