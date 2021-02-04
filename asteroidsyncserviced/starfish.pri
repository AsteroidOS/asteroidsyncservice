DEFINES += SAILFISHOS_PLATFORM

WATCHFISH_FEATURES = notificationmonitor walltime music

include(libwatchfish/libwatchfish.pri)

PKGCONFIG += qt5-boostable

SOURCES += platforms/sailfishos/sailfishplatform.cpp

HEADERS += platforms/sailfishos/sailfishplatform.h

DISTFILES += \
    $$PWD/harbour-asteroidsyncserviced.service
