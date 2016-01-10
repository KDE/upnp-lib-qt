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

#ifndef UPNPWEBSOCKETSERVSERSOCKET_H
#define UPNPWEBSOCKETSERVSERSOCKET_H

#include "upnpQtWebSocket_export.h"

#include <QtWebSockets/QWebSocketCorsAuthenticator>
#include <QtWebSockets/qwebsocketprotocol.h>

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

#include <QtCore/QObject>
#include <QtCore/QList>

class UpnpSsdpServerSocketPrivate;
class UpnpWebSocketCertificateConfiguration;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketServerSocket : public QObject
{

    Q_OBJECT

    Q_PROPERTY(UpnpWebSocketCertificateConfiguration* certificateConfiguration
               READ certificateConfiguration)

public:

    explicit UpnpWebSocketServerSocket(QObject *parent = 0);

    virtual ~UpnpWebSocketServerSocket();

    Q_INVOKABLE void init(const QString &serverName);

    UpnpWebSocketCertificateConfiguration* certificateConfiguration() const;

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

