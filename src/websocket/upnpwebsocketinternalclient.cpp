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

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QDebug>

class UpnpWebSocketClientPrivate
{
public:

    QWebSocket *mSocket;
};

UpnpWebSocketInternalClient::UpnpWebSocketInternalClient(QWebSocket *socket, QObject *parent)
    : QObject(parent), d(new UpnpWebSocketClientPrivate)
{
    d->mSocket = socket;

    connect(d->mSocket, &QWebSocket::aboutToClose, this, &UpnpWebSocketInternalClient::aboutToClose);
    connect(d->mSocket, &QWebSocket::binaryMessageReceived, this, &UpnpWebSocketInternalClient::binaryMessageReceived);
    connect(d->mSocket, &QWebSocket::bytesWritten, this, &UpnpWebSocketInternalClient::bytesWritten);
    connect(d->mSocket, &QWebSocket::disconnected, this, &UpnpWebSocketInternalClient::disconnected);
    connect(d->mSocket, &QWebSocket::pong, this, &UpnpWebSocketInternalClient::pong);
    connect(d->mSocket, &QWebSocket::proxyAuthenticationRequired, this, &UpnpWebSocketInternalClient::proxyAuthenticationRequired);
    connect(d->mSocket, &QWebSocket::readChannelFinished, this, &UpnpWebSocketInternalClient::readChannelFinished);
    connect(d->mSocket, &QWebSocket::sslErrors, this, &UpnpWebSocketInternalClient::sslErrors);
    connect(d->mSocket, &QWebSocket::stateChanged, this, &UpnpWebSocketInternalClient::stateChanged);
    connect(d->mSocket, &QWebSocket::textMessageReceived, this, &UpnpWebSocketInternalClient::textMessageReceived);
}

UpnpWebSocketInternalClient::~UpnpWebSocketInternalClient()
{
    delete d;
}

void UpnpWebSocketInternalClient::aboutToClose()
{
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
}

void UpnpWebSocketInternalClient::error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
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
}

void UpnpWebSocketInternalClient::stateChanged(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}

void UpnpWebSocketInternalClient::textMessageReceived(const QString &message)
{
    Q_UNUSED(message);
}

QJsonDocument UpnpWebSocketInternalClient::createMessage(UpnpWebSocketMessageType type)
{
    QJsonObject answerObject;
    answerObject.insert(QStringLiteral("messageType"), QJsonValue(static_cast<int>(type)));

    QJsonDocument answer;
    answer.setObject(answerObject);

    return answer;
}

void UpnpWebSocketInternalClient::sendError()
{
    d->mSocket->sendBinaryMessage(createMessage(UpnpWebSocketMessageType::Error).toBinaryData());
}

void UpnpWebSocketInternalClient::handleHello(QJsonObject aObject)
{
    Q_UNUSED(aObject);

    qDebug() << "UpnpWebSocketInternalClient::handleHello";

    d->mSocket->sendBinaryMessage(createMessage(UpnpWebSocketMessageType::HelloAck).toBinaryData());
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
            messageTypeInt > static_cast<int>(UpnpWebSocketMessageType::HelloAck)) {
        return UpnpWebSocketMessageType::Undefined;
    }

    return static_cast<UpnpWebSocketMessageType>(messageTypeInt);
}


#include "moc_upnpwebsocketinternalclient.cpp"
