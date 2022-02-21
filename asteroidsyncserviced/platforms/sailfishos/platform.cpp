/*
 * Copyright (C) 2018 - Florent Revest <revestflo@gmail.com>
 *               2016 - Andrew Branson <andrew.branson@jollamobile.com>
 *                      Ruslan N. Marchenko <me@ruff.mobi>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "platform.h"

#include "libwatchfish/musiccontroller.h"
#include "libwatchfish/notificationmonitor.h"
#include "libwatchfish/walltimemonitor.h"

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusReply>
#include <QDebug>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

Platform::Platform(WatchesManager *wm, QObject *parent) : QObject(parent)
{
    qDBusRegisterMetaType<QList<QVariantMap>>();

    m_timeService = wm->timeService();
    m_wallTimeMonitor = new watchfish::WallTimeMonitor(this);
    connect(m_wallTimeMonitor, &watchfish::WallTimeMonitor::timeChanged, this, &Platform::onTimeChanged);

    m_notificationService = wm->notificationService();
    m_notificationMonitor = new watchfish::NotificationMonitor(this);
    connect(m_notificationMonitor, &watchfish::NotificationMonitor::notification, this, &Platform::newNotification);

    m_mediaService = wm->mediaService();
    m_musicController = new watchfish::MusicController(this);
    connect(m_musicController, SIGNAL(statusChanged()), SLOT(updateMusicStatus()));
    connect(m_musicController, SIGNAL(titleChanged()), SLOT(updateMusicTitle()));
    connect(m_musicController, SIGNAL(albumChanged()), SLOT(updateMusicAlbum()));
    connect(m_musicController, SIGNAL(artistChanged()), SLOT(updateMusicArtist()));
    connect(m_mediaService, SIGNAL(play()), m_musicController, SLOT(play()));
    connect(m_mediaService, SIGNAL(pause()), m_musicController, SLOT(play()));
    connect(m_mediaService, SIGNAL(next()), m_musicController, SLOT(next()));
    connect(m_mediaService, SIGNAL(previous()), m_musicController, SLOT(previous()));
}

Platform::~Platform()
{
    delete m_musicController;
    delete m_notificationMonitor;
    delete m_wallTimeMonitor;
}

void Platform::onTimeChanged() {
    m_timeService->setTime(QDateTime());
}

void Platform::newNotification(watchfish::Notification *notification)
{
    if (!notification->category().endsWith(".group")) {
        connect(notification, SIGNAL(closed(CloseReason)), this, SLOT(onNotificationClosed(CloseReason)));
        m_notificationService->insertNotification(notification->appId(), notification->id(), notification->appName(), "ios-mail", notification->summary(), notification->body(), NotificationService::Strong);
    }
}

void Platform::onNotificationClosed(watchfish::Notification::CloseReason)
{
    watchfish::Notification *notif = (watchfish::Notification *)sender();
    m_notificationService->removeNotification(notif->id());
}

void Platform::updateMusicStatus()
{
    m_mediaService->setPlaying(m_musicController->status() == watchfish::MusicController::StatusPlaying);
}

void Platform::updateMusicTitle()
{
    m_mediaService->setTitle(m_musicController->title());
}

void Platform::updateMusicAlbum()
{
    m_mediaService->setAlbum(m_musicController->album());
}

void Platform::updateMusicArtist()
{
    m_mediaService->setArtist(m_musicController->artist());
}
