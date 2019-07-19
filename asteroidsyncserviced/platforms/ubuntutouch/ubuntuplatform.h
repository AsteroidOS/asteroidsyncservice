#ifndef UBUNTUPLATFORM_H
#define UBUNTUPLATFORM_H

#include <QDBusContext>

#include "../../libasteroid/services/mediaservice.h"
#include "../../libasteroid/services/notificationservice.h"
#include "../../libasteroid/services/timeservice.h"
#include "../../watchesmanager.h"

class UbuntuPlatform : public QObject, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
    UbuntuPlatform(WatchesManager *wm, QObject *parent = 0);
    ~UbuntuPlatform();

public slots:
    void onTimeChanged();

    void updateMusicStatus();
    void updateMusicTitle();
    void updateMusicAlbum();
    void updateMusicArtist();

private:

    TimeService *m_timeService;
    MediaService *m_mediaService;
    NotificationService *m_notificationService;
};

#endif // UBUNTUPLATFORM_H
