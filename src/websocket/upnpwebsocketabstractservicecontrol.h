/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#ifndef UPNPWEBSOCKETABSTRACTDEVICECONTROL_H
#define UPNPWEBSOCKETABSTRACTDEVICECONTROL_H

#include "upnpQtWebSocket_export.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QList>

class UpnpWebSocketAbstractDeviceControlPrivate;
class UpnpDeviceDescription;
class UpnpWebSocketClient;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketAbstractServiceControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription* deviceDescription
               READ deviceDescription
               WRITE setDeviceDescription
               NOTIFY deviceDescriptionChanged)

    Q_PROPERTY(QString serviceId
               READ serviceId
               WRITE setServiceId
               NOTIFY serviceIdChanged)

    Q_PROPERTY(UpnpWebSocketClient* webSocketClient
               READ webSocketClient
               WRITE setWebSocketClient
               NOTIFY webSocketClientChanged)

public:

    static const qint64 INVALID_SEQUENCE_NUMBER = -1;

    explicit UpnpWebSocketAbstractServiceControl(QObject *parent = 0);

    virtual ~UpnpWebSocketAbstractServiceControl();

    Q_INVOKABLE qint64 callAction(const QString &action, const QList<QVariant> &arguments);

    Q_INVOKABLE void subscribeEvents();

    void handleEventNotification();

    void setDeviceDescription(UpnpDeviceDescription *value);

    UpnpDeviceDescription *deviceDescription() const;

    void setServiceId(const QString &value);

    const QString &serviceId() const;

    void setWebSocketClient(UpnpWebSocketClient *value);

    UpnpWebSocketClient *webSocketClient() const;

Q_SIGNALS:

    void deviceDescriptionChanged();

    void serviceIdChanged();

    void webSocketClientChanged();

public Q_SLOTS:

    void actionCompleted(const QString &action, qint64 sequenceNumber);

private Q_SLOTS:

protected:

    virtual void parseEventNotification(const QString &eventName, const QString &eventValue);

private:

    UpnpWebSocketAbstractDeviceControlPrivate *d;
};

#endif // UPNPWEBSOCKETABSTRACTdeviceCONTROL_H
