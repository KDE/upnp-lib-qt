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

#ifndef UPNPWEBSOCKETCLIENT_H
#define UPNPWEBSOCKETCLIENT_H

#include "upnpQtWebSocket_export.h"

#include "upnpwebsocketcommon.h"

#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

#include <QtCore/QObject>
#include <QtCore/QJsonObject>

class QWebSocket;
class UpnpWebSocketClientPrivate;
class UpnpSsdpCertificateConfiguration;

class UPNPQTWEBSOCKET_EXPORT UpnpWebSocketClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpSsdpCertificateConfiguration* certificateConfiguration
               READ certificateConfiguration)

public:

    explicit UpnpWebSocketClient(QObject *parent = 0);

    virtual ~UpnpWebSocketClient();

    UpnpSsdpCertificateConfiguration* certificateConfiguration() const;

Q_SIGNALS:

    void certificateConfigurationChanged();

public Q_SLOTS:

    void connectServer(const QUrl &serverUrl);

    void sendHello();

private Q_SLOTS:

    void aboutToClose();
    void binaryMessageReceived(const QByteArray &message);
    void bytesWritten(qint64 bytes);
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError error);
    void pong(quint64 elapsedTime, const QByteArray &payload);
    void proxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    void readChannelFinished();
    void sslErrors(const QList<QSslError> &errors);
    void stateChanged(QAbstractSocket::SocketState state);
    void textMessageReceived(const QString &message);

private:

    void handleHelloAck(QJsonObject aObject);

    static UpnpWebSocketMessageType getType(QJsonObject aObject);

    QJsonDocument createMessage(UpnpWebSocketMessageType type);

    UpnpWebSocketClientPrivate *d;

};

#endif // UPNPWEBSOCKETCLIENT_H
