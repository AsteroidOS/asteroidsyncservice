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

#ifndef SERVICECONTROL_H
#define SERVICECONTROL_H

#include "asteroidsyncservice_config.h"
#include <QDBusInterface>
#include <QObject>

static const QString SYNCSERVICED_SYSTEMD_UNIT(SYSTEMD_SERVICE_NAME);

class ServiceControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool serviceRunning READ serviceRunning WRITE setServiceRunning NOTIFY serviceRunningChanged)

public:
    explicit ServiceControl(QObject *parent = 0);

    bool serviceRunning() const;
    bool setServiceRunning(bool running);
    Q_INVOKABLE bool startService();
    Q_INVOKABLE bool stopService();
    Q_INVOKABLE bool restartService();

private slots:
    void getUnitProperties();
    void onPropertiesChanged(QString interface, QMap<QString, QVariant> changed, QStringList invalidated);

signals:
    void serviceRunningChanged();

private:
    QDBusInterface *systemd;
    QDBusObjectPath unitPath;
    QVariantMap unitProperties;

};

#endif // SERVICECONTROL_H

