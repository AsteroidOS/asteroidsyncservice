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
    Q_PROPERTY(QDBusInterface* interface READ interface)

public:
    UbuntuPlatform(WatchesManager *wm, QObject *parent = 0);
    ~UbuntuPlatform();
    QDBusInterface* interface() const;

public slots:
    uint Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expire_timeout);
    void NotificationServiceReady();
    void updateMusicStatus();
    void updateMusicTitle();
    void updateMusicAlbum();
    void updateMusicArtist();

private:
    QString encodeIcon(QString appName) const;
    QString encodeAppName(QString appName) const;
    
    QDBusInterface *m_iface;
    MediaService *m_mediaService;
    NotificationService *m_notificationService;
    bool m_notificationServiceReady = false;
};

#endif // UBUNTUPLATFORM_H
