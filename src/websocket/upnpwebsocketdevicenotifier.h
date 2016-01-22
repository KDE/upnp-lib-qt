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

#ifndef UPNPWEBSOCKETDEVICENOTIFIER_H
#define UPNPWEBSOCKETDEVICENOTIFIER_H

#include "upnpQtWebSocket_export.h"

#include <QtCore/QObject>
#include <QtCore/QString>

class UpnpWebSocketDeviceNotifierPrivate;
class UpnpWebSocketClient;
class UpnpDeviceDescription;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketDeviceNotifier : public QObject
{

    Q_OBJECT

    Q_PROPERTY(UpnpWebSocketClient* webSocketClient
               READ webSocketClient
               WRITE setWebSocketClient
               NOTIFY webSocketClientChanged)

    Q_PROPERTY(QString udn
               READ udn
               WRITE setUdn
               NOTIFY udnChanged)

    Q_PROPERTY(QString serviceId
               READ serviceId
               WRITE setServiceId
               NOTIFY serviceIdChanged)

    Q_PROPERTY(bool isDeviceValid
               READ isDeviceValid
               NOTIFY isDeviceValidChanged)

    Q_PROPERTY(QSharedPointer<UpnpDeviceDescription> deviceDescription
               READ deviceDescription
               NOTIFY deviceDescriptionChanged)

public:

    explicit UpnpWebSocketDeviceNotifier(QObject *parent = 0);

    ~UpnpWebSocketDeviceNotifier();

    UpnpWebSocketClient *webSocketClient() const;

    QString serviceId() const;

    QString udn() const;

    bool isDeviceValid() const;

    QSharedPointer<UpnpDeviceDescription> deviceDescription() const;

Q_SIGNALS:

    void webSocketClientChanged();

    void serviceIdChanged();

    void udnChanged();

    void isDeviceValidChanged();

    void deviceDescriptionChanged();

public Q_SLOTS:

    void setWebSocketClient(UpnpWebSocketClient* value);

    void setServiceId(QString value);

    void setUdn(QString udn);

    void newDevice(const QString &udn);

    void removedDevice(const QString &udn);

private:

    UpnpWebSocketDeviceNotifierPrivate *d;

};

#endif // UPNPWEBSOCKETDEVICENOTIFIER_H
