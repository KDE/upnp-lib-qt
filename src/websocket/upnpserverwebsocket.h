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

#ifndef UPNPSSDPSERVERSOCKET_H
#define UPNPSSDPSERVERSOCKET_H

#include "upnpQtWebSocket_export.h"

#include <QtWebSockets/QWebSocketCorsAuthenticator>
#include <QtWebSockets/qwebsocketprotocol.h>

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

#include <QtCore/QObject>

class UpnpSsdpServerSocketPrivate;

class UPNPQTWEBSOCKET_EXPORT UpnpSsdpServerSocket : public QObject
{

    Q_OBJECT

public:

    explicit UpnpSsdpServerSocket(QObject *parent = 0);

    virtual ~UpnpSsdpServerSocket();

    Q_INVOKABLE void init(const QString &serverName);

Q_SIGNALS:

private Q_SLOTS:

    void acceptError(QAbstractSocket::SocketError socketError);
    void closed();
    void newConnection();
    void originAuthenticationRequired(QWebSocketCorsAuthenticator *authenticator);
    void peerVerifyError(const QSslError &error);
    void serverError(QWebSocketProtocol::CloseCode closeCode);
    void sslErrors(const QList<QSslError> &errors);

private:

    UpnpSsdpServerSocketPrivate *d;

};

#endif

