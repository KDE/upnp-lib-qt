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

#include "upnpwebsocketclient.h"

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

class UpnpWebSocketClientPrivate
{
public:

    QPointer<QWebSocket> mSocket;
};

UpnpWebSocketClient::UpnpWebSocketClient(QScopedPointer<QWebSocket> &socket, QObject *parent)
    : QObject(parent), d(new UpnpWebSocketClientPrivate)
{
    d->mSocket = socket.take();

    connect(d->mSocket.data(), &QWebSocket::aboutToClose, this, &UpnpWebSocketClient::aboutToClose);
    connect(d->mSocket.data(), &QWebSocket::binaryMessageReceived, this, &UpnpWebSocketClient::binaryMessageReceived);
    connect(d->mSocket.data(), &QWebSocket::bytesWritten, this, &UpnpWebSocketClient::bytesWritten);
    connect(d->mSocket.data(), &QWebSocket::disconnected, this, &UpnpWebSocketClient::disconnected);
    connect(d->mSocket.data(), &QWebSocket::pong, this, &UpnpWebSocketClient::pong);
    connect(d->mSocket.data(), &QWebSocket::proxyAuthenticationRequired, this, &UpnpWebSocketClient::proxyAuthenticationRequired);
    connect(d->mSocket.data(), &QWebSocket::readChannelFinished, this, &UpnpWebSocketClient::readChannelFinished);
    connect(d->mSocket.data(), &QWebSocket::sslErrors, this, &UpnpWebSocketClient::sslErrors);
    connect(d->mSocket.data(), &QWebSocket::stateChanged, this, &UpnpWebSocketClient::stateChanged);
    connect(d->mSocket.data(), &QWebSocket::textMessageReceived, this, &UpnpWebSocketClient::textMessageReceived);
}

UpnpWebSocketClient::~UpnpWebSocketClient()
{
    delete d;
}

void UpnpWebSocketClient::aboutToClose()
{
}

void UpnpWebSocketClient::binaryMessageReceived(const QByteArray &message)
{
    auto newMessage = QJsonDocument::fromBinaryData(message);

    if (newMessage.isNull()) {
        return;
    }

    if (!newMessage.isObject()) {
        return;
    }

    auto newMessageObject = newMessage.object();


}

void UpnpWebSocketClient::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void UpnpWebSocketClient::disconnected()
{
}

void UpnpWebSocketClient::error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
}

void UpnpWebSocketClient::pong(quint64 elapsedTime, const QByteArray &payload)
{
    Q_UNUSED(elapsedTime);
    Q_UNUSED(payload);
}

void UpnpWebSocketClient::proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Q_UNUSED(proxy);
    Q_UNUSED(authenticator);
}

void UpnpWebSocketClient::readChannelFinished()
{
}

void UpnpWebSocketClient::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
}

void UpnpWebSocketClient::stateChanged(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}

void UpnpWebSocketClient::textMessageReceived(const QString &message)
{
    Q_UNUSED(message);
}


#include "moc_upnpwebsocketclient.cpp"
