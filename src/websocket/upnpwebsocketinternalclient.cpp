/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "upnpwebsocketinternalclient.h"

#include "upnpdevicedescription.h"

#include "upnpwebsocketserversocket.h"

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QDebug>

class UpnpWebSocketClientPrivate
{
public:

    int mIdClient;

    QWebSocket *mSocket;

    UpnpWebSocketServerSocket *mServer;
};

UpnpWebSocketInternalClient::UpnpWebSocketInternalClient(int idClient, QWebSocket *socket, UpnpWebSocketServerSocket *server, QObject *parent)
    : QObject(parent), d(new UpnpWebSocketClientPrivate)
{
    d->mIdClient = idClient;
    d->mSocket = socket;
    d->mServer = server;

    connect(d->mSocket, &QWebSocket::binaryMessageReceived, this, &UpnpWebSocketInternalClient::binaryMessageReceived);
    connect(d->mSocket, &QWebSocket::bytesWritten, this, &UpnpWebSocketInternalClient::bytesWritten);
    connect(d->mSocket, &QWebSocket::disconnected, this, &UpnpWebSocketInternalClient::disconnected);
    connect(d->mSocket, &QWebSocket::pong, this, &UpnpWebSocketInternalClient::pong);
    connect(d->mSocket, &QWebSocket::proxyAuthenticationRequired, this, &UpnpWebSocketInternalClient::proxyAuthenticationRequired);
    connect(d->mSocket, &QWebSocket::readChannelFinished, this, &UpnpWebSocketInternalClient::readChannelFinished);
    connect(d->mSocket, &QWebSocket::sslErrors, this, &UpnpWebSocketInternalClient::sslErrors);
    connect(d->mSocket, &QWebSocket::stateChanged, this, &UpnpWebSocketInternalClient::stateChanged);
    connect(d->mSocket, &QWebSocket::textMessageReceived, this, &UpnpWebSocketInternalClient::textMessageReceived);

    connect(d->mServer, &UpnpWebSocketServerSocket::newDeviceHasBeenPublished, this, &UpnpWebSocketInternalClient::newDeviceHasBeenPublished);
    connect(d->mServer, &UpnpWebSocketServerSocket::deviceHasBeenRemoved, this, &UpnpWebSocketInternalClient::deviceHasBeenRemoved);
}

UpnpWebSocketInternalClient::~UpnpWebSocketInternalClient()
{
    delete d;
}

void UpnpWebSocketInternalClient::binaryMessageReceived(const QByteArray &message)
{    
    qDebug() << "UpnpWebSocketInternalClient::binaryMessageReceived" << message;

    auto newMessage = QJsonDocument::fromBinaryData(message);

    if (newMessage.isNull()) {
        qDebug() << "UpnpWebSocketInternalClient::binaryMessageReceived" << "error 1";
        return;
    }

    if (!newMessage.isObject()) {
        qDebug() << "UpnpWebSocketInternalClient::binaryMessageReceived" << "error 2";
        return;
    }

    auto newMessageObject = newMessage.object();

    switch(getType(newMessageObject))
    {
    case UpnpWebSocketMessageType::Hello:
        handleHello(newMessageObject);
        break;
    case UpnpWebSocketMessageType::PublishDevice:
        handleNewService(newMessageObject);
        break;
    case UpnpWebSocketMessageType::AskDeviceList:
        handleAskServiceList(newMessageObject);
        break;
    default:
        qDebug() << "UpnpWebSocketInternalClient::binaryMessageReceived" << "unknown message" << static_cast<int>(getType(newMessageObject));
        break;
    }
}

void UpnpWebSocketInternalClient::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void UpnpWebSocketInternalClient::disconnected()
{
    Q_EMIT closeClient(d->mIdClient);
}

void UpnpWebSocketInternalClient::error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    Q_EMIT closeClient(d->mIdClient);
}

void UpnpWebSocketInternalClient::pong(quint64 elapsedTime, const QByteArray &payload)
{
    Q_UNUSED(elapsedTime);
    Q_UNUSED(payload);
}

void UpnpWebSocketInternalClient::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Q_UNUSED(proxy);
    Q_UNUSED(authenticator);
}

void UpnpWebSocketInternalClient::readChannelFinished()
{
}

void UpnpWebSocketInternalClient::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);

    Q_EMIT closeClient(d->mIdClient);
}

void UpnpWebSocketInternalClient::stateChanged(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}

void UpnpWebSocketInternalClient::textMessageReceived(const QString &message)
{
    Q_UNUSED(message);
}

void UpnpWebSocketInternalClient::newDeviceHasBeenPublished(const QString &udn)
{
    auto newMessage = createMessage(UpnpWebSocketMessageType::NewDevice);

    auto newDevice = d->mServer->device(udn);

    if (!newDevice) {
        return;
    }

    newMessage.insert(QStringLiteral("device"), UpnpWebSocketProtocol::deviceDescriptionToJson(newDevice.data()));

    sendMessage(newMessage);
}

void UpnpWebSocketInternalClient::deviceHasBeenRemoved(const QString &udn)
{
    auto newMessage = createMessage(UpnpWebSocketMessageType::RemovedDevice);

    auto removedDevice = d->mServer->device(udn);

    if (!removedDevice) {
        return;
    }

    newMessage.insert(QStringLiteral("device"), UpnpWebSocketProtocol::deviceDescriptionToJson(removedDevice.data()));

    sendMessage(newMessage);
}

QJsonObject UpnpWebSocketInternalClient::createMessage(UpnpWebSocketMessageType type)
{
    QJsonObject answerObject;
    answerObject.insert(QStringLiteral("messageType"), QJsonValue(static_cast<int>(type)));

    return answerObject;
}

void UpnpWebSocketInternalClient::sendMessage(const QJsonObject &messageObject)
{
    QJsonDocument newMessage;
    newMessage.setObject(messageObject);

    d->mSocket->sendBinaryMessage(newMessage.toBinaryData());
}

void UpnpWebSocketInternalClient::sendError()
{
    sendMessage(createMessage(UpnpWebSocketMessageType::Error));
}

void UpnpWebSocketInternalClient::handleHello(QJsonObject aObject)
{
    Q_UNUSED(aObject);

    qDebug() << "UpnpWebSocketInternalClient::handleHello";

    sendMessage(createMessage(UpnpWebSocketMessageType::HelloAck));
}

void UpnpWebSocketInternalClient::handleNewService(QJsonObject aObject)
{
    qDebug() << "UpnpWebSocketInternalClient::handleNewService";

    auto deviceKey = aObject.find(QStringLiteral("device"));
    if (deviceKey == aObject.end()) {
        return;
    }

    if (deviceKey.value().isUndefined() || deviceKey.value().isNull() || !deviceKey.value().isObject()) {
        return;
    }

    auto newDevice = deviceKey.value().toObject();

    auto newDeviceDescription = UpnpWebSocketProtocol::deviceDescriptionFromJson(newDevice);

    qDebug() << newDevice;
    if (newDeviceDescription) {
        qDebug() << newDeviceDescription->deviceType();
        auto result = d->mServer->addDevice(newDeviceDescription, d->mIdClient);

        if (!result) {
            auto newObject = createMessage(UpnpWebSocketMessageType::DeviceIsNotPublished);

            newObject.insert(QStringLiteral("UDN"), newDeviceDescription->UDN());

            sendMessage(newObject);
        }

        auto newObject = createMessage(UpnpWebSocketMessageType::DeviceIsPublished);

        newObject.insert(QStringLiteral("UDN"), newDeviceDescription->UDN());

        sendMessage(newObject);
    } else {
        qDebug() << "decode problem";

        auto newObject = createMessage(UpnpWebSocketMessageType::DeviceIsNotPublished);

        sendMessage(newObject);
    }
}

void UpnpWebSocketInternalClient::handleAskServiceList(QJsonObject aObject)
{
    Q_UNUSED(aObject);

    auto newObject = createMessage(UpnpWebSocketMessageType::DeviceList);

    QJsonArray allDeviceUDN;

    for(auto &oneDevice : d->mServer->allDeviceUDN()) {
        auto fullDevice = d->mServer->device(oneDevice);

        if (!fullDevice) {
            return;
        }

        allDeviceUDN.append(UpnpWebSocketProtocol::deviceDescriptionToJson(fullDevice.data()));
    }

    newObject.insert(QStringLiteral("devices"), allDeviceUDN);


    sendMessage(newObject);
}

UpnpWebSocketMessageType UpnpWebSocketInternalClient::getType(QJsonObject aObject)
{
    auto messageTypeKey = aObject.find(QStringLiteral("messageType"));
    if (messageTypeKey == aObject.end()) {
        return UpnpWebSocketMessageType::Undefined;
    }

    if (messageTypeKey.value().isUndefined() || messageTypeKey.value().isNull()) {
        return UpnpWebSocketMessageType::Undefined;
    }

    if (!messageTypeKey.value().isDouble()) {
        return UpnpWebSocketMessageType::Undefined;
    }

    int messageTypeInt = static_cast<int>(messageTypeKey.value().toDouble());

    if (messageTypeInt < static_cast<int>(UpnpWebSocketMessageType::Hello) ||
            messageTypeInt > static_cast<int>(UpnpWebSocketMessageType::ServiceIsSubscribed)) {
        return UpnpWebSocketMessageType::Undefined;
    }

    return static_cast<UpnpWebSocketMessageType>(messageTypeInt);
}


#include "moc_upnpwebsocketinternalclient.cpp"
