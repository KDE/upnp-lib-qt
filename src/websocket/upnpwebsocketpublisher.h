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

#ifndef UPNPWEBSOCKETPUBLISHER_H
#define UPNPWEBSOCKETPUBLISHER_H

#include "upnpQtWebSocket_export.h"

#include "upnpwebsocketcommon.h"
#include "upnpwebsocketbaseclient.h"

#include <QtCore/QJsonObject>

class UpnpWebSocketPublisherPrivate;
class UpnpDeviceDescription;
class UpnpServiceDescription;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketPublisher : public UpnpWebSocketBaseClient
{
    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription* description
               READ description
               WRITE setDescription
               NOTIFY descriptionChanged)

public:

    explicit UpnpWebSocketPublisher(QObject *parent = 0);

    virtual ~UpnpWebSocketPublisher();

    void setDescription(UpnpDeviceDescription *value);

    UpnpDeviceDescription* description();

    const UpnpDeviceDescription* description() const;

    static QJsonObject serviceDescriptionToJson(const UpnpServiceDescription *serviceDescription);

    static QJsonObject deviceDescriptionToJson(const UpnpDeviceDescription *deviceDescription);

Q_SIGNALS:

    void descriptionChanged();

public Q_SLOTS:

    void publish();

private Q_SLOTS:

protected:

    bool handleMessage(const QJsonObject &newMessage) override;

private:

    void addDeviceDescription(QJsonObject &newMessage, const UpnpDeviceDescription *deviceDescription);

    void handleHelloAck(QJsonObject aObject);

    UpnpWebSocketPublisherPrivate *d;

};

#endif // UPNPWEBSOCKETPUBLISHER_H
