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

#include "asteroidsyncservice_config.h"
#include "dbusinterface.h"
#include "watchesmanager.h"

#include "watch.h"

#include <QDBusConnection>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

#include <algorithm>
#include <utility>

/*!
 * look up weather icon code, given WMO weather code
 *
 * sources:
 * WMO weather codes:  https://www.nodc.noaa.gov/archive/arc0021/0002199/1.1/data/0-data/HTML/WMO-CODE/WMO4677.HTM
 * weather icon codes: https://openweathermap.org/weather-conditions
 */
[[nodiscard]] static int iconlookup(int wxcode) {
    int iconcode{800};  // every day is sunny!
    switch(wxcode) {
        case  0: iconcode = 800; break; // sunny
        case  1: iconcode = 801; break; // mainly sunny
        case  2: iconcode = 802; break; // partly cloudy
        case  3: iconcode = 803; break; // mostly cloudy
        case 45: iconcode = 741; break; // foggy
        case 48: iconcode = 741; break; // rime fog
        case 51: iconcode = 300; break; // light drizzle
        case 53: iconcode = 301; break; // drizzle
        case 55: iconcode = 302; break; // heavy drizzle
        case 56: iconcode = 612; break; // light freezing drizzle
        case 57: iconcode = 613; break; // freezing drizzle
        case 61: iconcode = 500; break; // light rain
        case 63: iconcode = 501; break; // rain
        case 65: iconcode = 502; break; // heavy rain
        case 66: iconcode = 511; break; // light freezing rain
        case 67: iconcode = 511; break; // freezing rain
        case 71: iconcode = 600; break; // light snow
        case 73: iconcode = 601; break; // snow
        case 75: iconcode = 602; break; // heavy snow
        case 77: iconcode = 601; break; // snow grains
        case 80: iconcode = 500; break; // light showers
        case 81: iconcode = 501; break; // showers
        case 82: iconcode = 502; break; // heavy showers
        case 85: iconcode = 600; break; // light snow showers
        case 86: iconcode = 601; break; // snow showers
        case 95: iconcode = 211; break; // thunderstorm
        case 96: iconcode = 200; break; // light thunderstorms with hail (no hail designation in codes, so just use t'storm)
        case 99: iconcode = 211; break; // thunderstorm with hail (no hail designation in codes, so just use t'storm)

        default: {
            iconcode = 800;
            qDebug() << "unknown weather code passed to iconlookup:" << wxcode;
            break;
        }
    }
    return iconcode;
}
/*!
 * \brief Convert JSON weather string to settings for asteroid-weather
 *
 * \param weatherJson String containing weather JSON.  As and example,
 * if we request the weather data for Cape Town, South Africa, the request
 * URL would be
 * "https://api.open-meteo.com/v1/forecast?latitude=35.858&longitude=-79.1032&timezone=auto&daily=weather_code,temperature_2m_max,temperature_2m_min"
 * and the response might be:
 *   "latitude":35.850216,"longitude":-79.097015,"generationtime_ms":0.102996826171875,"utc_offset_seconds":-14400,"timezone":"America/New_York","timezone_abbreviation":"EDT","elevation":188.0,"daily_units":{"time":"iso8601","weather_code":"wmo code","temperature_2m_max":"°C","temperature_2m_min":"°C"},"daily":{"time":["2024-04-25","2024-04-26","2024-04-27","2024-04-28","2024-04-29","2024-04-30","2024-05-01"],"weather_code":[3,3,3,3,2,51,2],"temperature_2m_max":[23.2,21.8,20.2,25.9,27.7,28.8,29.6],"temperature_2m_min":[8.8,8.6,11.5,11.2,13.3,14.9,14.2]}}
 *
 * Note that by default, the temperatures are in degrees C which we must convert to Kelvin for the weather app
 *  see https://open-meteo.com/
 */
[[nodiscard]] static QList<WeatherDay> weatherJsonToVector(const QString &weatherJson)
{
    /* This looks complex, but it's really just a way to compensate for the fact
     * that with Qt5, size() returned an int, and with Qt6, it returns a qsizetype.
     * This automatically determines the type so the comparison within std::min()
     * below does not trigger a compiler warning.
     */
    static constexpr decltype(std::declval<QJsonArray>().size()) maxWeatherDays{5};
    QList<WeatherDay> days;
    constexpr double CtoKwithRounding{272.15 + 0.5};
    QJsonParseError parseError;
    qDebug() << "weather string: " << weatherJson.toUtf8();
    auto json = QJsonDocument::fromJson(weatherJson.toUtf8(), &parseError);
    if (json.isNull()) {
        qWarning() << "JSON parse error:  " << parseError.errorString();
        return days;
    }
    auto min = json["daily"]["temperature_2m_min"].toArray();
    auto max = json["daily"]["temperature_2m_max"].toArray();
    auto icon = json["daily"]["weather_code"].toArray();
    int count = std::min(maxWeatherDays, min.count());
    for (int i{0}; i < count; ++i) {
        days.push_back({ static_cast<short int>(iconlookup(icon[i].toInt())),
                         static_cast<short int>(min[i].toDouble() + CtoKwithRounding),
                         static_cast<short int>(max[i].toDouble() + CtoKwithRounding)
                       });
        qDebug() << "Day " << i << " of " << count << " = [ " << days[i].m_wxIcon 
            << ", " << days[i].m_loTemp
            << ", " << days[i].m_hiTemp
            << " ]";
    }
    return days;
}

/* Watch Interface */

DBusWatch::DBusWatch(Watch *watch, WatchesManager* wm, QObject *parent): QObject(parent), m_watch(watch), m_wm(wm)
{
    m_screenshotService = wm->screenshotService();
    m_weatherService = wm->weatherService();
    m_batteryService = wm->batteryService();
    m_timeService = wm->timeService();
    m_notificationService = wm->notificationService();

    connect(m_batteryService, SIGNAL(ready()), this, SIGNAL(BatteryServiceReady()));
    connect(m_batteryService, SIGNAL(levelChanged(quint8)), this, SIGNAL(LevelChanged(quint8)));
    connect(m_timeService, SIGNAL(ready()), this, SLOT(onTimeServiceReady()));
    connect(m_notificationService, SIGNAL(ready()), this, SLOT(onNotifyServiceReady()));
    connect(m_screenshotService, SIGNAL(ready()), this, SLOT(onScreenshotServiceReady()));
    connect(m_screenshotService, SIGNAL(progressChanged(unsigned int)), this, SIGNAL(ProgressChanged(unsigned int)));
    connect(m_screenshotService, SIGNAL(screenshotReceived(QByteArray)), this, SIGNAL(ScreenshotReceived(QByteArray)));
    connect(m_weatherService, SIGNAL(ready()), this, SLOT(onWeatherServiceReady()));
    connect(wm, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

void DBusWatch::onDisconnected()
{
    m_timeServiceReady = false;
    emit TimeServiceChanged();

    m_notifyServiceReady = false;
    emit NotifyServiceChanged();

    m_screenshotServiceReady = false;
    emit ScreenshotServiceChanged();

    m_weatherServiceReady = false;
    emit WeatherServiceChanged();
}

void DBusWatch::SelectWatch()
{
    m_wm->setDevice(m_watch);
}

QString DBusWatch::Address() const
{
    return m_watch->getAddress().toString();
}

QString DBusWatch::Name() const
{
    return m_watch->getName();
}

quint8 DBusWatch::BatteryLevel()
{
    return m_batteryService->level();
}

void DBusWatch::RequestScreenshot()
{
    m_screenshotService->requestScreenshot();
}

void DBusWatch::WeatherSetCityName(QString cityName)
{
    m_weatherService->setCity(cityName);
}

void DBusWatch::WeatherSetWeather(QString weatherJson)
{
    QList<WeatherDay> weatherDays = weatherJsonToVector(weatherJson);
    m_weatherService->setWeatherDays(weatherDays);
}

void DBusWatch::onTimeServiceReady()
{
    m_timeServiceReady = true;
    emit TimeServiceChanged();
}

bool DBusWatch::StatusTimeService()
{
    return m_timeServiceReady;
}

void DBusWatch::onNotifyServiceReady()
{
    m_notifyServiceReady = true;
    emit NotifyServiceChanged();
}

bool DBusWatch::StatusNotifyService()
{
    return m_notifyServiceReady;
}

void DBusWatch::SetTime(QDateTime t)
{
    m_timeService->setTime(t);
}

void DBusWatch::SendNotify(unsigned int id, QString appName, QString icon, QString body, QString summary, QString vibration)
{
    m_notificationService->insertNotification("", id, appName, icon, body, summary, vibration);
}

void DBusWatch::onScreenshotServiceReady()
{
    m_screenshotServiceReady = true;
    emit ScreenshotServiceChanged();
}

bool DBusWatch::StatusScreenshotService()
{
    return m_screenshotServiceReady;
}

void DBusWatch::onWeatherServiceReady()
{
    m_weatherServiceReady = true;
    emit WeatherServiceChanged();
}

bool DBusWatch::StatusWeatherService()
{
    return m_weatherServiceReady;
}

/* Manager Interface */

DBusInterface::DBusInterface(WatchesManager *wm, QObject *parent) : QObject(parent)
{
    m_watchesManager = wm;

    QDBusConnection::sessionBus().registerService("org.asteroidsyncservice");
    QDBusConnection::sessionBus().registerObject("/org/asteroidsyncservice/Manager", this, QDBusConnection::ExportScriptableSlots|QDBusConnection::ExportScriptableSignals);

    foreach (Watch *watch, m_watchesManager->watches())
        watchAdded(watch);

    connect(m_watchesManager, &WatchesManager::watchAdded, this, &DBusInterface::watchAdded);
    connect(m_watchesManager, &WatchesManager::watchRemoved, this, &DBusInterface::watchRemoved);
    connect(m_watchesManager, SIGNAL(currentWatchChanged()), this, SIGNAL(SelectedWatchChanged()));
    connect(m_watchesManager, SIGNAL(connected()), this, SIGNAL(SelectedWatchConnectedChanged()));
    connect(m_watchesManager, SIGNAL(disconnected()), this, SIGNAL(SelectedWatchConnectedChanged()));
}

QString DBusInterface::Version()
{
    return QStringLiteral(VERSION);
}

QList<QDBusObjectPath> DBusInterface::ListWatches()
{
    QList<QDBusObjectPath> ret;

    foreach (const QString &address, m_dbusWatches.keys())
        ret.append(QDBusObjectPath("/org/asteroidsyncservice/" + address));

    return ret;
}

QDBusObjectPath DBusInterface::SelectedWatch()
{
    Watch *cur = m_watchesManager->currentWatch();
    if(cur) {
        QString address = cur->getAddress().toString().replace(":", "_");
        return QDBusObjectPath("/org/asteroidsyncservice/" + address);
    } else
        return QDBusObjectPath("/");
}

bool DBusInterface::SelectedWatchConnected()
{
    return m_watchesManager->isConnected();
}

void DBusInterface::watchAdded(Watch *watch)
{
    QString address = watch->getAddress().toString().replace(":", "_");
    if (m_dbusWatches.contains(address))
        return;

    DBusWatch *dbusWatch = new DBusWatch(watch, m_watchesManager, this);
    m_dbusWatches.insert(address, dbusWatch);
    QDBusConnection::sessionBus().registerObject("/org/asteroidsyncservice/" + address, dbusWatch, QDBusConnection::ExportAllContents);

    emit WatchesChanged();
}

void DBusInterface::watchRemoved(Watch *watch)
{
    QString address = watch->getAddress().toString().replace(":", "_");

    QDBusConnection::sessionBus().unregisterObject("/org/asteroidsyncservice/" + address);
    m_dbusWatches.remove(address);

    emit WatchesChanged();
}

