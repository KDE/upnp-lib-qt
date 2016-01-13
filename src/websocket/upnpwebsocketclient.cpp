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

#include "upnpwebsocketclient.h"

#include "upnpwebsocketcertificateconfiguration.h"

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

class UpnpWebSocketClientPrivate
{
public:

};

UpnpWebSocketClient::UpnpWebSocketClient(QObject *parent)
    : UpnpWebSocketBaseClient(parent), d(new UpnpWebSocketClientPrivate)
{
}

UpnpWebSocketClient::~UpnpWebSocketClient()
{
    delete d;
}

void UpnpWebSocketClient::askDeviceList()
{
    auto newObject = createMessage(UpnpWebSocketMessageType::AskDeviceList);

    sendMessage(newObject);
}

bool UpnpWebSocketClient::handleMessage(const QJsonObject &newMessage)
{
    bool messageHandled = UpnpWebSocketBaseClient::handleMessage(newMessage);

    if (messageHandled) {
        return true;
    }

    switch(getType(newMessage))
    {
    case UpnpWebSocketMessageType::HelloAck:
        handleHelloAck(newMessage);
        messageHandled = true;
        break;
    case UpnpWebSocketMessageType::DeviceList:
        handleServiceList(newMessage);
        messageHandled = true;
        break;
    case UpnpWebSocketMessageType::NewDevice:
        handleNewService(newMessage);
        messageHandled = true;
        break;
    default:
        qDebug() << "unknown message" << static_cast<int>(getType(newMessage));
        break;
    }

    return messageHandled;
}

void UpnpWebSocketClient::hasBeenDisconnected()
{
}

void UpnpWebSocketClient::handleHelloAck(QJsonObject aObject)
{
    Q_UNUSED(aObject);

    askDeviceList();
}

void UpnpWebSocketClient::handleServiceList(QJsonObject aObject)
{
    const auto &listDeviceValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("devices"));
    if (listDeviceValue.isNull() || !listDeviceValue.isArray()) {
        return;
    }

    const auto &listDeviceArray = listDeviceValue.toArray();
    for (auto oneDevice : listDeviceArray) {
        qDebug() << "new device" << oneDevice.toString();
    }
}

void UpnpWebSocketClient::handleNewService(QJsonObject aObject)
{
    const auto &newDeviceValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("device"));
    qDebug() << "new device" << newDeviceValue.toString();
}


#include "moc_upnpwebsocketclient.cpp"
