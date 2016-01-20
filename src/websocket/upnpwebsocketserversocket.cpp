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

#include "upnpwebsocketserversocket.h"

#include "upnpwebsocketinternalclient.h"
#include "upnpwebsocketcertificateconfiguration.h"

#include "upnpdevicedescription.h"

#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

#include <QtCore/QPair>
#include <QtCore/QPointer>
#include <QtCore/QDebug>

class UpnpSsdpServerSocketPrivate
{
public:

    QPointer<QWebSocketServer> mServerSocket;

    UpnpWebSocketCertificateConfiguration mCertificateConfiguration;

    int mIdNextClient;

    QMap<int, QSharedPointer<UpnpWebSocketInternalClient>> mAllClients;

    QMap<int, QMap<QString, QSharedPointer<UpnpDeviceDescription>>> mAllDevices;

    QList<QString> mAllDeviceUDN;

    QMap<QString, int> mDeviceUdnToClientId;
};

UpnpWebSocketServerSocket::UpnpWebSocketServerSocket(QObject *parent)
    : QObject(parent), d(new UpnpSsdpServerSocketPrivate)
{
    d->mIdNextClient = 1;
}

UpnpWebSocketServerSocket::~UpnpWebSocketServerSocket()
{
    delete d;
}

void UpnpWebSocketServerSocket::init(const QString &serverName)
{
    QSslConfiguration myConfiguration;
    d->mCertificateConfiguration.initialize(&myConfiguration);

    d->mServerSocket = new QWebSocketServer(serverName, QWebSocketServer::SecureMode);
    d->mServerSocket->setSslConfiguration(myConfiguration);

    connect(d->mServerSocket.data(), &QWebSocketServer::acceptError, this, &UpnpWebSocketServerSocket::acceptError);
    connect(d->mServerSocket.data(), &QWebSocketServer::closed, this, &UpnpWebSocketServerSocket::closed);
    connect(d->mServerSocket.data(), &QWebSocketServer::newConnection, this, &UpnpWebSocketServerSocket::newConnection);
    connect(d->mServerSocket.data(), &QWebSocketServer::originAuthenticationRequired, this, &UpnpWebSocketServerSocket::originAuthenticationRequired);
    connect(d->mServerSocket.data(), &QWebSocketServer::peerVerifyError, this, &UpnpWebSocketServerSocket::peerVerifyError);
    connect(d->mServerSocket.data(), &QWebSocketServer::serverError, this, &UpnpWebSocketServerSocket::serverError);
    connect(d->mServerSocket.data(), &QWebSocketServer::sslErrors, this, &UpnpWebSocketServerSocket::sslErrors);

    d->mServerSocket->listen(QHostAddress::Any, 11443);
}

UpnpWebSocketCertificateConfiguration *UpnpWebSocketServerSocket::certificateConfiguration() const
{
    return &d->mCertificateConfiguration;
}

bool UpnpWebSocketServerSocket::addDevice(QSharedPointer<UpnpDeviceDescription> newDevice, int clientId)
{
    d->mAllDevices[clientId][newDevice->UDN()] = newDevice;
    d->mAllDeviceUDN.push_back(newDevice->UDN());
    d->mDeviceUdnToClientId[newDevice->UDN()] = clientId;

    Q_EMIT newDeviceHasBeenPublished(newDevice->UDN());

    return true;
}

QList<QString> UpnpWebSocketServerSocket::allDeviceUDN()
{
    return d->mAllDeviceUDN;
}

QSharedPointer<UpnpDeviceDescription> UpnpWebSocketServerSocket::device(const QString &udn) const
{
    auto clientId = d->mDeviceUdnToClientId.find(udn);

    if (clientId == d->mDeviceUdnToClientId.end()) {
        return {};
    }

    auto clientDevices = d->mAllDevices.find((clientId.value()));
    if (clientDevices == d->mAllDevices.end()) {
        return {};
    }

    auto oneDevice = clientDevices->find(udn);
    if (oneDevice == clientDevices->end()) {
        return {};
    }

    return oneDevice.value();
}

bool UpnpWebSocketServerSocket::proxy(const QString &udn, const QJsonObject &message)
{
    auto clientId = d->mDeviceUdnToClientId.find(udn);

    if (clientId == d->mDeviceUdnToClientId.end()) {
        return false;
    }

    auto clientDevice = d->mAllClients.find((clientId.value()));
    if (clientDevice == d->mAllClients.end()) {
        return false;
    }

    (*clientDevice)->sendMessage(message);

    return true;
}

void UpnpWebSocketServerSocket::clientHasClosed(int idClient)
{
    qDebug() << "UpnpWebSocketServerSocket::clientHasClosed" << idClient;

    const auto &clientDevices = d->mAllDevices[idClient];

    for(auto oneDevice : clientDevices) {
        Q_EMIT deviceHasBeenRemoved(oneDevice->UDN());
        d->mAllDeviceUDN.removeAll(oneDevice->UDN());
        d->mDeviceUdnToClientId.remove(oneDevice->UDN());
    }

    d->mAllDevices.remove(idClient);
    d->mAllClients.remove(idClient);
}

void UpnpWebSocketServerSocket::acceptError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    qDebug() << "UpnpSsdpServerSocket::acceptError";
}

void UpnpWebSocketServerSocket::closed()
{
    qDebug() << "UpnpSsdpServerSocket::closed";
}

void UpnpWebSocketServerSocket::newConnection()
{
    qDebug() << "UpnpSsdpServerSocket::newConnection";

    QWebSocket* newConnection(d->mServerSocket->nextPendingConnection());

    if (!newConnection) {
        return;
    }

    d->mAllClients[d->mIdNextClient].reset(new UpnpWebSocketInternalClient(d->mIdNextClient, newConnection, this));

    connect(d->mAllClients[d->mIdNextClient].data(), &UpnpWebSocketInternalClient::closeClient, this, &UpnpWebSocketServerSocket::clientHasClosed);

    ++d->mIdNextClient;
}

void UpnpWebSocketServerSocket::originAuthenticationRequired(QWebSocketCorsAuthenticator *authenticator)
{
    authenticator->setAllowed(true);
}

void UpnpWebSocketServerSocket::peerVerifyError(const QSslError &error)
{
    Q_UNUSED(error);
    qDebug() << "UpnpSsdpServerSocket::peerVerifyError";
}

void UpnpWebSocketServerSocket::serverError(QWebSocketProtocol::CloseCode closeCode)
{
    Q_UNUSED(closeCode);
    qDebug() << "UpnpSsdpServerSocket::serverError" << closeCode << d->mServerSocket->errorString();
}

void UpnpWebSocketServerSocket::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
    qDebug() << "UpnpSsdpServerSocket::sslErrors" << errors;
}


#include "moc_upnpwebsocketserversocket.cpp"
