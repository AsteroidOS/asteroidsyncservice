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

#ifndef WATCH_H
#define WATCH_H

#include <QObject>
#include <QDateTime>
#include <QDBusInterface>

class Watch : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString weatherCityName READ weatherCityName WRITE setWeatherCityName NOTIFY weatherCityNameChanged)
    Q_PROPERTY(quint8 batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)
    Q_PROPERTY(bool timeServiceReady READ timeServiceReady NOTIFY timeServiceChanged)
    Q_PROPERTY(bool notificationServiceReady READ notificationServiceReady NOTIFY notificationServiceChanged)

public:
    explicit Watch(const QDBusObjectPath &path, QObject *parent = 0);

    QDBusObjectPath path();

    void selectWatch();
    QString address();
    QString name();

    QString weatherCityName();
    quint8 batteryLevel();
    bool timeServiceReady();
    Q_INVOKABLE void setTime(QDateTime t);
    bool notificationServiceReady();
    Q_INVOKABLE void setVibration(QString v);
    Q_INVOKABLE void sendNotify(unsigned int id, QString appName, QString icon, QString body, QString summary);

public slots:
    void requestScreenshot();
    void setWeatherCityName(const QString &in);

signals:
    void weatherCityNameChanged();
    void batteryLevelChanged();
    void timeServiceChanged();
    void notificationServiceChanged();

private:
    QVariant fetchProperty(const QString &propertyName);

private slots:
    void dataChanged();
    void batteryServiceReady();
    void batteryLevelRefresh(quint8 batLvl);

private:
    QDBusObjectPath m_path;

    QString m_address;
    QString m_name;
    QDBusInterface *m_iface;
    qint8 m_batteryLevel = 0;
};

#endif // WATCH_H

