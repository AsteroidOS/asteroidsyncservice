#include "ubuntuplatform.h"

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
    qDBusRegisterMetaType<QList<QVariantMap>>();

    m_timeService = wm->timeService();
}

UbuntuPlatform::~UbuntuPlatform()
{
}

void UbuntuPlatform::onTimeChanged() {
    m_timeService->setTime(QDateTime());
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
