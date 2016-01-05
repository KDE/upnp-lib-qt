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

#include "upnpserverwebsocket.h"
#include "upnpwebsocketinternalclient.h"

#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>

#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QList>
#include <QtCore/QFile>
#include <QtCore/QDebug>

class UpnpSsdpServerSocketPrivate
{
public:

    QPointer<QWebSocketServer> mServerSocket;

    QString mCertificateAuthorityFileName;

    QString mCertificateServerFileName;

    QList<QSslCertificate> mCertificateAuthority;

    QSslCertificate mCertificateServer;
};

UpnpSsdpServerSocket::UpnpSsdpServerSocket(QObject *parent)
    : QObject(parent), d(new UpnpSsdpServerSocketPrivate)
{
}

UpnpSsdpServerSocket::~UpnpSsdpServerSocket()
{
    delete d;
}

void UpnpSsdpServerSocket::init(const QString &serverName)
{
    d->mCertificateAuthority = QSslCertificate::fromPath(d->mCertificateAuthorityFileName);
    QSslSocket::addDefaultCaCertificates(d->mCertificateAuthority);

    auto readCertificates = QSslCertificate::fromPath(d->mCertificateServerFileName);
    if (readCertificates.size() != 1) {
        return;
    }

    QFile myCertificate(d->mCertificateServerFileName);
    myCertificate.open(QIODevice::ReadOnly);
    QSslKey myPrivateKey(&myCertificate, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "");

    QSslConfiguration myConfiguration;
    myConfiguration.setCaCertificates(d->mCertificateAuthority);
    myConfiguration.setLocalCertificate(readCertificates[0]);
    myConfiguration.setPrivateKey(myPrivateKey);
    myConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer);

    d->mServerSocket = new QWebSocketServer(serverName, QWebSocketServer::SecureMode);
    d->mServerSocket->setSslConfiguration(myConfiguration);

    connect(d->mServerSocket.data(), &QWebSocketServer::acceptError, this, &UpnpSsdpServerSocket::acceptError);
    connect(d->mServerSocket.data(), &QWebSocketServer::closed, this, &UpnpSsdpServerSocket::closed);
    connect(d->mServerSocket.data(), &QWebSocketServer::newConnection, this, &UpnpSsdpServerSocket::newConnection);
    connect(d->mServerSocket.data(), &QWebSocketServer::originAuthenticationRequired, this, &UpnpSsdpServerSocket::originAuthenticationRequired);
    connect(d->mServerSocket.data(), &QWebSocketServer::peerVerifyError, this, &UpnpSsdpServerSocket::peerVerifyError);
    connect(d->mServerSocket.data(), &QWebSocketServer::serverError, this, &UpnpSsdpServerSocket::serverError);
    connect(d->mServerSocket.data(), &QWebSocketServer::sslErrors, this, &UpnpSsdpServerSocket::sslErrors);

    d->mServerSocket->listen(QHostAddress::Any, 11443);
}

const QString &UpnpSsdpServerSocket::certificateAuthorityFileName() const
{
    return d->mCertificateAuthorityFileName;
}

void UpnpSsdpServerSocket::setCertificateAuthorityFileName(const QString &value)
{
    d->mCertificateAuthorityFileName = value;
    Q_EMIT certificateAuthorityFileNameChanged();
}

const QString &UpnpSsdpServerSocket::certificateServerFileName() const
{
    return d->mCertificateServerFileName;
}

void UpnpSsdpServerSocket::setCertificateServerFileName(const QString &value)
{
    d->mCertificateServerFileName = value;
    Q_EMIT certificateServerFileNameChanged();
}

void UpnpSsdpServerSocket::acceptError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);

    qDebug() << "UpnpSsdpServerSocket::acceptError";
}

void UpnpSsdpServerSocket::closed()
{
    qDebug() << "UpnpSsdpServerSocket::closed";
}

void UpnpSsdpServerSocket::newConnection()
{
    qDebug() << "UpnpSsdpServerSocket::newConnection";

    QWebSocket* newConnection(d->mServerSocket->nextPendingConnection());

    if (!newConnection) {
        return;
    }

    new UpnpWebSocketInternalClient(newConnection);
}

void UpnpSsdpServerSocket::originAuthenticationRequired(QWebSocketCorsAuthenticator *authenticator)
{
    authenticator->setAllowed(true);
}

void UpnpSsdpServerSocket::peerVerifyError(const QSslError &error)
{
    Q_UNUSED(error);
    qDebug() << "UpnpSsdpServerSocket::peerVerifyError";
}

void UpnpSsdpServerSocket::serverError(QWebSocketProtocol::CloseCode closeCode)
{
    Q_UNUSED(closeCode);
    qDebug() << "UpnpSsdpServerSocket::serverError" << closeCode << d->mServerSocket->errorString();
}

void UpnpSsdpServerSocket::sslErrors(const QList<QSslError> &errors)
{
    Q_UNUSED(errors);
    qDebug() << "UpnpSsdpServerSocket::sslErrors" << errors;
}


#include "moc_upnpserverwebsocket.cpp"
