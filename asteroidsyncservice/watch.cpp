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

#include "watch.h"

#include <QDBusArgument>
#include <QDebug>

Watch::Watch(const QDBusObjectPath &path, QObject *parent) : QObject(parent), m_path(path)
{
    m_iface = new QDBusInterface("org.asteroidsyncservice", path.path(), "org.asteroidsyncservice.Watch", QDBusConnection::sessionBus(), this);
    connect(m_iface, SIGNAL(TimeServiceChanged()), this, SLOT(statusTimeService()));
    connect(m_iface, SIGNAL(NotifyServiceChanged()), this, SLOT(statusNotifyService()));
    connect(m_iface, SIGNAL(BatteryServiceReady()), this, SLOT(batteryServiceReady()));
    connect(m_iface, SIGNAL(LevelChanged(quint8)), this, SLOT(batteryLevelRefresh(quint8)));
    
    dataChanged();
}

void Watch::selectWatch()
{
    m_iface->call("SelectWatch");
}

QDBusObjectPath Watch::path()
{
    return m_path;
}

QString Watch::address()
{
    return m_address;
}

QString Watch::name()
{
    return m_name;
}

QString Watch::weatherCityName()
{
    return ""; // TODO
}

void Watch::setWeatherCityName(const QString &c)
{
    m_iface->call("WeatherSetCityName", c);
    emit weatherCityNameChanged();
}

quint8 Watch::batteryLevel() {
    return m_batteryLevel;
}

QVariant Watch::fetchProperty(const QString &propertyName)
{
    QDBusMessage m = m_iface->call(propertyName);
    if (m.type() != QDBusMessage::ErrorMessage && m.arguments().count() == 1)
        return m.arguments().first();

    qDebug() << "error getting property:" << propertyName << m.errorMessage();
    return QVariant();
}

void Watch::dataChanged()
{
    m_name = fetchProperty("Name").toString();
    m_address = fetchProperty("Address").toString();
    m_batteryLevel = fetchProperty("BatteryLevel").toInt();
    statusTimeService();
    statusNotifyService();
}

void Watch::requestScreenshot()
{
    m_iface->call("RequestScreenshot");
}

void Watch::setTime(QDateTime t)
{
    m_iface->call("SetTime", t.currentDateTime());
}

bool Watch::timeServiceReady()
{
    return m_timeServiceReady;
}

void Watch::statusTimeService()
{
    bool timeServiceReady = fetchProperty("StatusTimeService").toBool();
    if(timeServiceReady != m_timeServiceReady) {
        m_timeServiceReady = timeServiceReady;
        emit timeServiceChanged();
    }
}

void Watch::batteryServiceReady()
{
    quint8 batLvl = fetchProperty("BatteryLevel").toInt();
    if(batLvl !=  m_batteryLevel) {
        m_batteryLevel = batLvl;
        emit batteryLevelChanged();
    }
}

void Watch::batteryLevelRefresh(quint8 batLvl)
{
    if(batLvl != m_batteryLevel) {
        m_batteryLevel = batLvl;
        emit batteryLevelChanged();
    }
}

bool Watch::notificationServiceReady()
{
    return m_notificationServiceReady;
}

void Watch::statusNotifyService()
{
    bool notificationServiceReady = fetchProperty("StatusNotifyService").toBool();
    if(notificationServiceReady != m_notificationServiceReady) {
        m_notificationServiceReady = notificationServiceReady;
        emit notificationServiceChanged();
    }
}

void Watch::setVibration(QString v)
{
    m_iface->call("SetVibration", v);
}

void Watch::sendNotify(unsigned int id, QString appName, QString icon, QString body, QString summary)
{
    m_iface->call("SendNotify", id, appName, icon, body, summary);
}
