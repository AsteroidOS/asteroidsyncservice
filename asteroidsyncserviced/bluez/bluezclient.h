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

#ifndef BLUEZCLIENT_H
#define BLUEZCLIENT_H

#include "bluez_helper.h"

#include <QList>
#include <QBluetoothAddress>
#include <memory>

class BluezClientPrivate;

class Device {
public:
    QBluetoothAddress address;
    QString name;
    QString path;
};

class BluezClient: public QObject
{
    Q_OBJECT

public:
    BluezClient(QObject *parent = 0);
    // declare destructor explicitly to prevent inlining 
    virtual ~BluezClient();

    QList<Device> pairedWatches() const;

private slots:
    void addDevice(const QDBusObjectPath &path, const QVariantMap &properties);

    void slotInterfacesAdded(const QDBusObjectPath &path, InterfaceList ifaces);
    void slotInterfacesRemoved(const QDBusObjectPath &path, const QStringList &ifaces);

signals:
    void devicesChanged();

private:
    bool isAsteroidOSWatch(const QStringList uuids) const;
#if Q_DECLARE_PRIVATE_SUPPORTS_UNIQUE_PTR != 1
template <typename Wrapper> static inline typename Wrapper::pointer qGetPtrHelper(const Wrapper &p) { return p.operator->(); }
#endif
    Q_DECLARE_PRIVATE(BluezClient);
    std::unique_ptr<BluezClientPrivate> d_ptr;
    QHash<QString, Device> m_devices;
};

#endif // BLUEZCLIENT_H
