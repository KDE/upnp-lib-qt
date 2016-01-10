/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "upnpwebsocketcertificateconfiguration.h"

#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>

#include <QtCore/QFile>
#include <QtCore/QList>

class UpnpSsdpCertificateConfigurationPrivate
{
public:

    QString mCertificateAuthorityFileName;

    QString mCertificateFileName;

    QList<QSslCertificate> mCertificateAuthority;

};

UpnpWebSocketCertificateConfiguration::UpnpWebSocketCertificateConfiguration(QObject *parent)
    : QObject(parent), d(new UpnpSsdpCertificateConfigurationPrivate)
{
}

UpnpWebSocketCertificateConfiguration::~UpnpWebSocketCertificateConfiguration()
{
    delete d;
}

const QString &UpnpWebSocketCertificateConfiguration::certificateAuthorityFileName() const
{
    return d->mCertificateAuthorityFileName;
}

void UpnpWebSocketCertificateConfiguration::setCertificateAuthorityFileName(const QString &value)
{
    d->mCertificateAuthorityFileName = value;
    Q_EMIT certificateAuthorityFileNameChanged();
}

const QString &UpnpWebSocketCertificateConfiguration::certificateFileName() const
{
    return d->mCertificateFileName;
}

void UpnpWebSocketCertificateConfiguration::setCertificateFileName(const QString &value)
{
    d->mCertificateFileName = value;
    Q_EMIT certificateFileNameChanged();
}

void UpnpWebSocketCertificateConfiguration::initialize(QSslConfiguration *configuration)
{
    d->mCertificateAuthority = QSslCertificate::fromPath(d->mCertificateAuthorityFileName);
    QSslSocket::addDefaultCaCertificates(d->mCertificateAuthority);

    auto readCertificates = QSslCertificate::fromPath(d->mCertificateFileName);
    if (readCertificates.size() != 1) {
        return;
    }

    QFile myCertificate(d->mCertificateFileName);
    myCertificate.open(QIODevice::ReadOnly);
    QSslKey myPrivateKey(&myCertificate, QSsl::Rsa, QSsl::Pem, QSsl::PrivateKey, "");

    configuration->setCaCertificates(d->mCertificateAuthority);
    configuration->setLocalCertificate(readCertificates[0]);
    configuration->setPrivateKey(myPrivateKey);
    configuration->setPeerVerifyMode(QSslSocket::VerifyPeer);
}


#include "moc_upnpwebsocketcertificateconfiguration.cpp"
