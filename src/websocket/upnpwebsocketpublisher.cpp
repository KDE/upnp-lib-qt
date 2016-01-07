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

#include "upnpwebsocketpublisher.h"

#include "upnpwebsocketcertificateconfiguration.h"
#include "upnpdevicedescription.h"

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

class UpnpWebSocketPublisherPrivate
{
public:

    QSharedPointer<UpnpDeviceDescription> mDevice;

};

UpnpWebSocketPublisher::UpnpWebSocketPublisher(QObject *parent)
    : UpnpWebSocketBaseClient(parent), d(new UpnpWebSocketPublisherPrivate)
{
}

UpnpWebSocketPublisher::~UpnpWebSocketPublisher()
{
    delete d;
}

void UpnpWebSocketPublisher::setDescription(UpnpDeviceDescription *value)
{
    d->mDevice.reset(value);
    Q_EMIT descriptionChanged();
}

UpnpDeviceDescription *UpnpWebSocketPublisher::description()
{
    return d->mDevice.data();
}

const UpnpDeviceDescription *UpnpWebSocketPublisher::description() const
{
    return d->mDevice.data();
}

void UpnpWebSocketPublisher::publish()
{
}

bool UpnpWebSocketPublisher::handleMessage(const QJsonObject &newMessage)
{
    bool messageHandled = UpnpWebSocketBaseClient::handleMessage(newMessage);

    if (messageHandled) {
        return true;
    }

    switch(getType(newMessage))
    {
    /*case UpnpWebSocketMessageType::HelloAck:
        handleHelloAck(newMessage);
        messageHandled = true;
        break;*/
    default:
        break;
    }

    return messageHandled;
}


#include "moc_upnpwebsocketpublisher.cpp"
