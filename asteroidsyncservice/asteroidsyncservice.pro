TEMPLATE = lib
TARGET = asteroidsyncserviceplugin
QT += qml quick dbus
CONFIG += qt plugin
include(../version.pri)

TARGET = $$qtLibraryTarget($$TARGET)
uri = org.asteroid.syncservice

contains(CONFIG, telescope) {
    include(telescope.pri)
} 

contains(CONFIG, starfish) {
    include(starfish.pri)
}

HEADERS += \
    syncservice_plugin.h \
    watch.h \
    watches.h

SOURCES += \
    syncservice_plugin.cpp \
    watch.cpp \
    watches.cpp

OTHER_FILES = \
    qmldir

qmldir.files = qmldir

qmlfiles.path = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
qmlfiles.files += $$files(qml/*)

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$OUT_PWD/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir qmlfiles
}
