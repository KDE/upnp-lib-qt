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

#include "upnpproxywebsocket.h"

#include <QtWebSockets/QWebSocketServer>

#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtCore/QScopedPointer>

class UpnpProxyWebSocketPrivate
{
public:

    UpnpProxyWebSocketPrivate()
        : mSocket(), mClientSockets()
    {
    }

    QString mServerName;

    QScopedPointer<QWebSocketServer> mSocket;

    QList<QSharedPointer<QWebSocket> > mClientSockets;
};

UpnpProxyWebSocket::UpnpProxyWebSocket(QObject *parent)
    : QObject(parent), d(new UpnpProxyWebSocketPrivate)
{
}

UpnpProxyWebSocket::~UpnpProxyWebSocket()
{
    delete d;
}

const QString &UpnpProxyWebSocket::serverName() const
{
    return d->mServerName;
}

void UpnpProxyWebSocket::setServerName(const QString &value)
{
    d->mServerName = value;
    Q_EMIT serverNameChanged();
}

void UpnpProxyWebSocket::init()
{
    d->mSocket.reset(new QWebSocketServer(d->mServerName, QWebSocketServer::SecureMode));
    connect(d->mSocket.data(), &QWebSocketServer::newConnection, this, &UpnpProxyWebSocket::newClientConnection);
    d->mSocket->listen(QHostAddress::Any, 443);
}

void UpnpProxyWebSocket::newClientConnection()
{
    qDebug() << "UpnpProxyWebSocket::newClientConnection";

    while(d->mSocket->hasPendingConnections()) {
        d->mClientSockets.push_back(QSharedPointer<QWebSocket>(d->mSocket->nextPendingConnection()));
    }
}


#include "moc_upnpproxywebsocket.cpp"
