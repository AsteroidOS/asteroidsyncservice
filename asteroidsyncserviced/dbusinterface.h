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

#ifndef DBUSINTERFACE_H
#define DBUSINTERFACE_H

#include "watchesmanager.h"

#include <QObject>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class Watch;

class DBusWatch: public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.asteroidsyncservice.Watch")
public:
    DBusWatch(Watch *watch, WatchesManager* wm, QObject *parent);

signals:
    void Connected();

    void LevelChanged(quint8);
    void TimeServiceChanged();
    void BatteryServiceReady();
    void NotifyServiceChanged();
    void ScreenshotServiceChanged();
    void ProgressChanged(unsigned int);
    void ScreenshotReceived(QByteArray);
    void WeatherServiceChanged();

public slots:
    void SelectWatch();

    QString Address() const;
    QString Name() const;

    quint8 BatteryLevel();
    bool StatusTimeService();
    bool StatusNotifyService();
    bool StatusScreenshotService();
    bool StatusWeatherService();
    void RequestScreenshot();
    void WeatherSetCityName(QString cityName);
    void WeatherSetWeather(QString weatherJson);
    void SetTime(QDateTime t);
    void SendNotify(unsigned int id, QString appName, QString icon, QString body, QString summary, QString vibration);

private slots:
    void onTimeServiceReady();
    void onNotifyServiceReady();
    void onScreenshotServiceReady();
    void onWeatherServiceReady();
    void onDisconnected();

private:
    Watch *m_watch;

    WatchesManager* m_wm;

    BatteryService *m_batteryService;
    ScreenshotService *m_screenshotService;
    WeatherService *m_weatherService;
    TimeService *m_timeService;
    NotificationService *m_notificationService;
    bool m_timeServiceReady = false;
    bool m_notifyServiceReady = false;
    bool m_screenshotServiceReady = false;
    bool m_weatherServiceReady = false;
};

class DBusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.asteroidsyncservice.Manager")

public:
    explicit DBusInterface(WatchesManager *wm, QObject *parent = 0);

public slots:
    Q_SCRIPTABLE QString Version();
    Q_SCRIPTABLE QList<QDBusObjectPath> ListWatches();
    Q_SCRIPTABLE QDBusObjectPath SelectedWatch();
    Q_SCRIPTABLE bool SelectedWatchConnected();

signals:
    Q_SCRIPTABLE void WatchesChanged();
    Q_SCRIPTABLE void NameChanged();
    Q_SCRIPTABLE void SelectedWatchChanged();
    Q_SCRIPTABLE void SelectedWatchConnectedChanged();

private slots:
    void watchAdded(Watch *watch);
    void watchRemoved(Watch *watch);

private:
    QHash<QString, DBusWatch*> m_dbusWatches;
    WatchesManager *m_watchesManager;
};

#endif // DBUSINTERFACE_H

