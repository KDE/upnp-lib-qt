/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "upnpwebsocketdevicenotifier.h"

#include "upnpwebsocketabstractservicecontrol.h"
#include "upnpwebsocketclient.h"

#include <QtCore/QSharedPointer>
#include <QtCore/QDebug>

class UpnpWebSocketDeviceNotifierPrivate
{
public:

    UpnpWebSocketClient *mWebSocketClient = nullptr;

    QString mServiceId;

    QString mUdn;

    bool mIsDeviceValid = false;

    UpnpDeviceDescription* mDeviceDescription = nullptr;

};

UpnpWebSocketDeviceNotifier::UpnpWebSocketDeviceNotifier(QObject *parent)
    : QObject(parent), d(new UpnpWebSocketDeviceNotifierPrivate)
{
    qDebug() << "UpnpWebSocketDeviceNotifier::UpnpWebSocketDeviceNotifier";
}

UpnpWebSocketDeviceNotifier::~UpnpWebSocketDeviceNotifier()
{
    delete d;
}

UpnpWebSocketClient *UpnpWebSocketDeviceNotifier::webSocketClient() const
{
    return d->mWebSocketClient;
}

QString UpnpWebSocketDeviceNotifier::serviceId() const
{
    return d->mServiceId;
}

QString UpnpWebSocketDeviceNotifier::udn() const
{
    return d->mUdn;
}

bool UpnpWebSocketDeviceNotifier::isDeviceValid() const
{
    return d->mIsDeviceValid;
}

UpnpDeviceDescription *UpnpWebSocketDeviceNotifier::deviceDescription() const
{
    return d->mDeviceDescription;
}

void UpnpWebSocketDeviceNotifier::setWebSocketClient(UpnpWebSocketClient *value)
{
    if (d->mWebSocketClient == value) {
        return;
    }

    if (d->mWebSocketClient) {
        disconnect(d->mWebSocketClient, &UpnpWebSocketClient::newDevice, this, &UpnpWebSocketDeviceNotifier::newDevice);
        disconnect(d->mWebSocketClient, &UpnpWebSocketClient::removedDevice, this, &UpnpWebSocketDeviceNotifier::removedDevice);
    }

    d->mWebSocketClient = value;

    if (d->mWebSocketClient) {
        connect(d->mWebSocketClient, &UpnpWebSocketClient::newDevice, this, &UpnpWebSocketDeviceNotifier::newDevice);
        connect(d->mWebSocketClient, &UpnpWebSocketClient::removedDevice, this, &UpnpWebSocketDeviceNotifier::removedDevice);
    }

    Q_EMIT webSocketClientChanged();
}

void UpnpWebSocketDeviceNotifier::setServiceId(QString value)
{
    if (d->mServiceId == value) {
        return;
    }

    d->mServiceId = value;
    Q_EMIT serviceIdChanged();
}

void UpnpWebSocketDeviceNotifier::setUdn(QString value)
{
    if (d->mUdn == value) {
        return;
    }

    d->mUdn = value;
    Q_EMIT udnChanged();
}

void UpnpWebSocketDeviceNotifier::newDevice(const QString &udn)
{
    if (udn == d->mUdn) {
        d->mIsDeviceValid = true;
        d->mDeviceDescription = d->mWebSocketClient->rawDevice(d->mUdn);
        Q_EMIT isDeviceValidChanged();
        Q_EMIT deviceDescriptionChanged();
    }
}

void UpnpWebSocketDeviceNotifier::removedDevice(const QString &udn)
{
    if (udn == d->mUdn) {
        d->mIsDeviceValid = false;
        d->mDeviceDescription = nullptr;
        Q_EMIT isDeviceValidChanged();
        Q_EMIT deviceDescriptionChanged();
    }
}


#include "moc_upnpwebsocketdevicenotifier.cpp"
