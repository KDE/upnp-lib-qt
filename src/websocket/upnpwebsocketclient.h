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

#ifndef UPNPWEBSOCKETCLIENT_H
#define UPNPWEBSOCKETCLIENT_H

#include "upnpQtWebSocket_export.h"

#include "upnpwebsocketcommon.h"
#include "upnpwebsocketbaseclient.h"

#include <QtCore/QJsonObject>
#include <QtCore/QSharedPointer>

class QWebSocket;
class UpnpWebSocketClientPrivate;
class UpnpWebSocketCertificateConfiguration;
class UpnpDeviceDescription;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketClient : public UpnpWebSocketBaseClient
{
    Q_OBJECT

public:

    explicit UpnpWebSocketClient(QObject *parent = 0);

    virtual ~UpnpWebSocketClient();

    void askDeviceList();

    void askDeviceDetail();

    QSharedPointer<UpnpDeviceDescription> device(const QString &udn) const;

Q_SIGNALS:

    void newDevice(const QString &udn);

    void removedDevice(const QString &udn);

    void actionAck(const QString &action, qint64 sequenceNumber);

public Q_SLOTS:

private Q_SLOTS:

protected:

    bool handleMessage(const QJsonObject &newMessage) override;

    void hasBeenDisconnected() override;

private:

    void handleHelloAck(QJsonObject aObject);

    void handleServiceList(QJsonObject aObject);

    void handleNewService(QJsonObject aObject);

    void handleRemovedService(QJsonObject aObject);

    void handleCallActionAck(QJsonObject aObject);

    UpnpWebSocketClientPrivate *d;

};

#endif // UPNPWEBSOCKETCLIENT_H
