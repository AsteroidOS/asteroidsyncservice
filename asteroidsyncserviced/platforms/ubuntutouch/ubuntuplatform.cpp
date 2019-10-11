#include "ubuntuplatform.h"

#include <stdlib.h>
#include <time.h>
#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDebug>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

UbuntuPlatform::UbuntuPlatform(WatchesManager *wm, QObject *parent) : QObject(parent)
{
    // initialize random seed
    std::srand (time(NULL));
    // Notifications
    m_notificationService = wm->notificationService();
    connect(m_notificationService, SIGNAL(ready()), this, SLOT(NotificationServiceReady()));
    connect(wm, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    QDBusConnection::sessionBus().registerObject("/org/freedesktop/Notifications", this, QDBusConnection::ExportAllSlots);
    m_iface = new QDBusInterface("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
    m_iface->call("AddMatch", "interface='org.freedesktop.Notifications',member='Notify',type='method_call',eavesdrop='true'");
}

UbuntuPlatform::~UbuntuPlatform()
{
}

QDBusInterface *UbuntuPlatform::interface() const
{
    return m_iface;
}

void UbuntuPlatform::NotificationServiceReady()
{
    m_notificationServiceReady = true;
}

uint UbuntuPlatform::Notify(const QString &app_name, uint replaces_id, const QString &app_icon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expire_timeout)
{
    QStringList hiddenNotifications = {"indicator-sound", "indicator-network"};
    if (!hiddenNotifications.contains(app_name)) {
        if (hints.contains("x-canonical-secondary-icon") && hints.value("x-canonical-secondary-icon").toString() == "incoming-call") {
            qDebug() << __func__ << "Have a phone call notification. Ignoring it..." << app_name << app_icon;
        } else if(m_notificationServiceReady) {
            unsigned int randId = rand() % 100000 + 1;
            m_notificationService->insertNotification("", randId, encodeAppName(app_name), encodeIcon(app_name), summary, body, m_notificationService->getVibration());
        } else {
            qDebug() << __func__ << "Service not ready";
        }
    }
    return 0;
}

QString UbuntuPlatform::encodeIcon(const QString appName) const
{
    if(appName.contains("dekko"))
        return "ios-mail";
    else if(appName.contains("teleports"))
        return "ios-paper-plane";
	else if(appName.contains("indicator-datetime"))
		return "ios-alarm";
    else
        return "logo-tux";
}

QString UbuntuPlatform::encodeAppName(const QString appName) const
{
    if(appName.contains("dekko"))
        return "Dekko 2";
    else if(appName.contains("teleports"))
        return "TELEports";
    else if(appName.contains("fluffychat"))
        return "FluffyChat";
	else if(appName.contains("indicator-datetime"))
		return "Alarm";
    else
        return "unkown";
}

void UbuntuPlatform::updateMusicStatus()
{
}

void UbuntuPlatform::updateMusicTitle()
{
}

void UbuntuPlatform::updateMusicAlbum()
{
}

void UbuntuPlatform::updateMusicArtist()
{
}
