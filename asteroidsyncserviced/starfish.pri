WATCHFISH_FEATURES = notificationmonitor walltime music

SUBDIRS += libwatchfish

PKGCONFIG += qt5-boostable

SOURCES += platforms/sailfishos/main.cpp \
    platforms/sailfishos/sailfishplatform.cpp

HEADERS += platforms/sailfishos/sailfishplatform.h
