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
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>

#include <QtCore/QFile>
#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

class UpnpWebSocketClientPrivate
{
public:

    QString mCertificateAuthorityFileName;

    QString mCertificateFileName;

    QList<QSslCertificate> mCertificateAuthority;

    QPointer<QWebSocket> mSocket;
};

UpnpWebSocketClient::UpnpWebSocketClient(QObject *parent)
    : QObject(parent), d(new UpnpWebSocketClientPrivate)
{
}

UpnpWebSocketClient::~UpnpWebSocketClient()
{
    delete d;
}

const QString &UpnpWebSocketClient::certificateAuthorityFileName() const
{
    return d->mCertificateAuthorityFileName;
}

void UpnpWebSocketClient::setCertificateAuthorityFileName(const QString &value)
{
    d->mCertificateAuthorityFileName = value;
    Q_EMIT certificateAuthorityFileNameChanged();
}

const QString &UpnpWebSocketClient::certificateFileName() const
{
    return d->mCertificateFileName;
}

void UpnpWebSocketClient::setCertificateFileName(const QString &value)
{
    d->mCertificateFileName = value;
    Q_EMIT certificateFileNameChanged();
}

void UpnpWebSocketClient::connectServer(const QUrl &serverUrl)
{
    d->mCertificateAuthority = QSslCertificate::fromPath(d->mCertificateAuthorityFileName);
    QSslSocket::addDefaultCaCertificates(d->mCertificateAuthority);

    d->mSocket = new QWebSocket();

    connect(d->mSocket.data(), &QWebSocket::aboutToClose, this, &UpnpWebSocketClient::aboutToClose);
    connect(d->mSocket.data(), &QWebSocket::binaryMessageReceived, this, &UpnpWebSocketClient::binaryMessageReceived);
    connect(d->mSocket.data(), &QWebSocket::bytesWritten, this, &UpnpWebSocketClient::bytesWritten);
    connect(d->mSocket.data(), &QWebSocket::connected, this, &UpnpWebSocketClient::connected);
    connect(d->mSocket.data(), &QWebSocket::disconnected, this, &UpnpWebSocketClient::disconnected);
    connect(d->mSocket.data(), &QWebSocket::pong, this, &UpnpWebSocketClient::pong);
    connect(d->mSocket.data(), &QWebSocket::proxyAuthenticationRequired, this, &UpnpWebSocketClient::proxyAuthenticationRequired);
    connect(d->mSocket.data(), &QWebSocket::readChannelFinished, this, &UpnpWebSocketClient::readChannelFinished);
    connect(d->mSocket.data(), &QWebSocket::sslErrors, this, &UpnpWebSocketClient::sslErrors);
    connect(d->mSocket.data(), &QWebSocket::stateChanged, this, &UpnpWebSocketClient::stateChanged);
    connect(d->mSocket.data(), &QWebSocket::textMessageReceived, this, &UpnpWebSocketClient::textMessageReceived);

    auto readCertificates = QSslCertificate::fromPath(d->mCertificateFileName);
    if (readCertificates.size() != 1) {
        return;
    }

    QFile myCertificate(d->mCertificateFileName);
    myCertificate.open(QIODevice::ReadOnly);
    QSslKey myPrivateKey(&myCertificate, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "");

    QSslConfiguration myConfiguration;
    myConfiguration.setCaCertificates(d->mCertificateAuthority);
    myConfiguration.setLocalCertificate(readCertificates[0]);
    myConfiguration.setPrivateKey(myPrivateKey);
    myConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer);

    d->mSocket->setSslConfiguration(myConfiguration);

    d->mSocket->open(serverUrl);
}

void UpnpWebSocketClient::sendHello()
{
    d->mSocket->sendBinaryMessage(createMessage(UpnpWebSocketMessageType::Hello).toBinaryData());
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

    switch(getType(newMessageObject))
    {
    case UpnpWebSocketMessageType::HelloAck:
        handleHelloAck(newMessageObject);
        break;
    default:
        break;
    }
}

void UpnpWebSocketClient::bytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
}

void UpnpWebSocketClient::connected()
{
    sendHello();
}

void UpnpWebSocketClient::disconnected()
{
}

void UpnpWebSocketClient::error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);

    qDebug() << "UpnpWebSocketClient::error" << error;
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

    qDebug() << "UpnpWebSocketClient::sslErrors" << errors;
}

void UpnpWebSocketClient::stateChanged(QAbstractSocket::SocketState state)
{
    Q_UNUSED(state);
}

void UpnpWebSocketClient::textMessageReceived(const QString &message)
{
    Q_UNUSED(message);
}

void UpnpWebSocketClient::handleHelloAck(QJsonObject aObject)
{
    Q_UNUSED(aObject);

    qDebug() << "UpnpWebSocketClient::handleHelloAck";

    d->mSocket->sendBinaryMessage(createMessage(UpnpWebSocketMessageType::ServiceList).toBinaryData());
}

UpnpWebSocketMessageType UpnpWebSocketClient::getType(QJsonObject aObject)
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

QJsonDocument UpnpWebSocketClient::createMessage(UpnpWebSocketMessageType type)
{
    QJsonObject answerObject;
    answerObject.insert(QStringLiteral("messageType"), QJsonValue(static_cast<int>(type)));

    QJsonDocument answer;
    answer.setObject(answerObject);

    return answer;
}


#include "moc_upnpwebsocketclient.cpp"
