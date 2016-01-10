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

#include "upnpwebsocketbaseclient.h"

#include "upnpwebsocketcertificateconfiguration.h"

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

class UpnpWebSocketBaseClientPrivate
{
public:

    UpnpWebSocketCertificateConfiguration mCertificateConfiguration;

    QPointer<QWebSocket> mSocket;
};

UpnpWebSocketBaseClient::UpnpWebSocketBaseClient(QObject *parent)
    : QObject(parent), d(new UpnpWebSocketBaseClientPrivate)
{
}

UpnpWebSocketBaseClient::~UpnpWebSocketBaseClient()
{
    delete d;
}

UpnpWebSocketCertificateConfiguration *UpnpWebSocketBaseClient::certificateConfiguration() const
{
    return &d->mCertificateConfiguration;
}

void UpnpWebSocketBaseClient::connectServer(const QUrl &serverUrl)
{
    QSslConfiguration myConfiguration;
    d->mCertificateConfiguration.initialize(&myConfiguration);

    d->mSocket = new QWebSocket();

    connect(d->mSocket.data(), &QWebSocket::aboutToClose, this, &UpnpWebSocketBaseClient::aboutToClose);
    connect(d->mSocket.data(), &QWebSocket::binaryMessageReceived, this, &UpnpWebSocketBaseClient::binaryMessageReceived);
    connect(d->mSocket.data(), &QWebSocket::bytesWritten, this, &UpnpWebSocketBaseClient::bytesWritten);
    connect(d->mSocket.data(), &QWebSocket::connected, this, &UpnpWebSocketBaseClient::connected);
    connect(d->mSocket.data(), &QWebSocket::disconnected, this, &UpnpWebSocketBaseClient::disconnected);
    connect(d->mSocket.data(), &QWebSocket::pong, this, &UpnpWebSocketBaseClient::pong);
    connect(d->mSocket.data(), &QWebSocket::proxyAuthenticationRequired, this, &UpnpWebSocketBaseClient::proxyAuthenticationRequired);
    connect(d->mSocket.data(), &QWebSocket::readChannelFinished, this, &UpnpWebSocketBaseClient::readChannelFinished);
    connect(d->mSocket.data(), &QWebSocket::sslErrors, this, &UpnpWebSocketBaseClient::sslErrors);
    connect(d->mSocket.data(), &QWebSocket::stateChanged, this, &UpnpWebSocketBaseClient::stateChanged);
    connect(d->mSocket.data(), &QWebSocket::textMessageReceived, this, &UpnpWebSocketBaseClient::textMessageReceived);

    d->mSocket->setSslConfiguration(myConfiguration);

    d->mSocket->open(serverUrl);
}

void UpnpWebSocketBaseClient::sendHello()
{
    auto newObject = createMessage(UpnpWebSocketMessageType::Hello);
    sendMessage(newObject);
}

void UpnpWebSocketBaseClient::aboutToClose()
{
}

void UpnpWebSocketBaseClient::binaryMessageReceived(const QByteArray &message)
{
    auto newMessage = QJsonDocument::fromBinaryData(message);

    if (newMessage.isNull()) {
        return;
    }

    if (!newMessage.isObject()) {
        return;
    }

    auto newMessageObject = newMessage.object();
    handleMessage(newMessageObject);
}

void UpnpWebSocketBaseClient::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void UpnpWebSocketBaseClient::connected()
{
    sendHello();
}

void UpnpWebSocketBaseClient::disconnected()
{
}

void UpnpWebSocketBaseClient::error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    qDebug() << "UpnpWebSocketBaseClient::error" << error;
}

void UpnpWebSocketBaseClient::pong(quint64 elapsedTime, const QByteArray &payload)
{
    Q_UNUSED(elapsedTime);
    Q_UNUSED(payload);
}

void UpnpWebSocketBaseClient::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Q_UNUSED(proxy);
    Q_UNUSED(authenticator);
}

void UpnpWebSocketBaseClient::readChannelFinished()
{
}

void UpnpWebSocketBaseClient::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);

    qDebug() << "UpnpWebSocketBaseClient::sslErrors" << errors;
}

void UpnpWebSocketBaseClient::stateChanged(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}

void UpnpWebSocketBaseClient::textMessageReceived(const QString &message)
{
    Q_UNUSED(message);
}

bool UpnpWebSocketBaseClient::handleMessage(const QJsonObject &newMessage)
{
    bool messageHandled = false;

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

UpnpWebSocketMessageType UpnpWebSocketBaseClient::getType(QJsonObject aObject)
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

QJsonObject UpnpWebSocketBaseClient::createMessage(UpnpWebSocketMessageType type)
{
    QJsonObject answerObject;
    answerObject.insert(QStringLiteral("messageType"), QJsonValue(static_cast<int>(type)));

    return answerObject;
}

void UpnpWebSocketBaseClient::sendMessage(const QJsonObject &messageObject)
{
    QJsonDocument newMessage;
    newMessage.setObject(messageObject);

    d->mSocket->sendBinaryMessage(newMessage.toBinaryData());
}


#include "moc_upnpwebsocketbaseclient.cpp"
