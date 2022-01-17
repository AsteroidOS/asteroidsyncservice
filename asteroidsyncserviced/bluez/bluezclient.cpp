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

#include "bluezclient.h"
#include "dbus-shared.h"
#include "../libasteroid/services/common.h"

#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>

BluezClient::BluezClient(QObject *parent):
    QObject(parent),
    m_dbus(QDBusConnection::systemBus()),
    m_bluezManager("org.bluez", "/", m_dbus)
{
    qDBusRegisterMetaType<InterfaceList>();
    qDBusRegisterMetaType<ManagedObjectList>();

    if (m_bluezManager.isValid()) {
        connect(&m_bluezManager, SIGNAL(InterfacesAdded(QDBusObjectPath,InterfaceList)),
                this, SLOT(slotInterfacesAdded(QDBusObjectPath,InterfaceList)));

        connect(&m_bluezManager, SIGNAL(InterfacesRemoved(QDBusObjectPath,QStringList)),
                this, SLOT(slotInterfacesRemoved(QDBusObjectPath,QStringList)));

        auto objectList = m_bluezManager.GetManagedObjects().argumentAt<0>();
        auto iterator = objectList.constBegin();
        while (iterator != objectList.constEnd()) {
            InterfaceList ifaces = iterator.value();

            if (ifaces.contains(BLUEZ_DEVICE_IFACE)) {
                auto path = iterator.key();
                auto properties = ifaces.value(BLUEZ_DEVICE_IFACE);
                addDevice(path, properties);
            }
            ++iterator;
        }

        if (m_devices.isEmpty()) {
            // Try with bluez 4
            QDBusConnection system = QDBusConnection::systemBus();

            QDBusReply<QList<QDBusObjectPath> > listAdaptersReply = system.call(
                        QDBusMessage::createMethodCall("org.bluez", "/", "org.bluez.Manager",
                                                       "ListAdapters"));
            if (!listAdaptersReply.isValid()) {
                qWarning() << listAdaptersReply.error().message();
                return;
            }

            QList<QDBusObjectPath> adapters = listAdaptersReply.value();

            if (adapters.isEmpty()) {
                qWarning() << "No BT adapters found";
                return;
            }

            QDBusReply<QVariantMap> adapterPropertiesReply = system.call(
                        QDBusMessage::createMethodCall("org.bluez", adapters[0].path(), "org.bluez.Adapter",
                                                       "GetProperties"));
            if (!adapterPropertiesReply.isValid()) {
                qWarning() << adapterPropertiesReply.error().message();
                return;
            }

            QList<QDBusObjectPath> devices;
            adapterPropertiesReply.value()["Devices"].value<QDBusArgument>() >> devices;

            foreach (QDBusObjectPath path, devices) {
                QDBusReply<QVariantMap> devicePropertiesReply = system.call(
                            QDBusMessage::createMethodCall("org.bluez", path.path(), "org.bluez.Device",
                                                           "GetProperties"));
                if (!devicePropertiesReply.isValid()) {
                    qCritical() << devicePropertiesReply.error().message();
                    continue;
                }

                const QVariantMap &dict = devicePropertiesReply.value();

                QString name = dict["Name"].toString();
                if (true) { // TODO: Filter
                    addDevice(path, dict);
                }
            }
        }
    }
}

QList<Device> BluezClient::pairedWatches() const
{
    QList<Device> ret;
    if (m_bluezManager.isValid()) {
        foreach (const Device &dev, m_devices) {
            ret << dev;
        }
    }
    return ret;
}

void BluezClient::addDevice(const QDBusObjectPath &path, const QVariantMap &properties)
{
    QString address = properties.value("Address").toString();
    QString name = properties.value("Name").toString();
    QStringList uuids = properties.value("UUIDs").toStringList();
    
    if (!m_devices.contains(address) && isAsteroidOSWatch(uuids)) {
        Device device;
        device.address = QBluetoothAddress(address);
        device.name = name;
        device.path = path.path();
        m_devices.insert(path.path(), device);
        emit devicesChanged();
    }
}

void BluezClient::slotInterfacesAdded(const QDBusObjectPath &path, InterfaceList ifaces)
{
    if (ifaces.contains(BLUEZ_DEVICE_IFACE)) {
        auto properties = ifaces.value(BLUEZ_DEVICE_IFACE);
        addDevice(path, properties);
    }
}

void BluezClient::slotInterfacesRemoved(const QDBusObjectPath &path, const QStringList &ifaces)
{
    if (!ifaces.contains(BLUEZ_DEVICE_IFACE)) {
        return;
    }
    if (m_devices.contains(path.path())) {
        m_devices.take(path.path());
        emit devicesChanged();
    }
}

bool BluezClient::isAsteroidOSWatch(const QStringList uuids) const
{
    auto supportedWatch = uuids.contains(NOTIF_UUID, Qt::CaseInsensitive);
    return supportedWatch;
}
