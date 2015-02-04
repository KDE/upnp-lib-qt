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

#include "upnpservicecaller.h"

#include <KDSoapClient/KDSoapClientInterface.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

class UpnpServiceCallerPrivate
{
public:

    UpnpServiceCallerPrivate(const QString &endPoint, const QString &messageNamespace)
        : mClientInterface(new KDSoapClientInterface(endPoint, messageNamespace)), mMessageNamespace(messageNamespace), mNetworkAccess()
    {
    }

    UpnpServiceCallerPrivate()
        : mClientInterface(nullptr), mMessageNamespace(), mEndPoint(), mNetworkAccess()
    {
    }

    KDSoapClientInterface *mClientInterface;

    QString mMessageNamespace;

    QString mEndPoint;

    QNetworkAccessManager mNetworkAccess;
};

UpnpServiceCaller::UpnpServiceCaller(const QString &endPoint, const QString &messageNamespace, QObject *parent)
    : QObject(parent), d(new UpnpServiceCallerPrivate(endPoint, messageNamespace))
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpServiceCaller::finishedDownload);
}

UpnpServiceCaller::UpnpServiceCaller(QObject *parent)
    : QObject(parent), d(new UpnpServiceCallerPrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpServiceCaller::finishedDownload);
}
UpnpServiceCaller::~UpnpServiceCaller()
{
    delete d;
}

void UpnpServiceCaller::setEndPoint(const QString &newEndPoint)
{
    if (d->mClientInterface) {
        d->mClientInterface->setEndPoint(newEndPoint);
    }
    d->mEndPoint = newEndPoint;
    Q_EMIT endPointChanged();
}

const QString &UpnpServiceCaller::endPoint() const
{
    return d->mEndPoint;
}

void UpnpServiceCaller::setMessageNamespace(const QString &newMessageNamespace)
{
    if (d->mClientInterface) {
        delete d->mClientInterface;
    }

    d->mMessageNamespace = newMessageNamespace;
    d->mClientInterface = new KDSoapClientInterface(d->mEndPoint, d->mMessageNamespace);
    Q_EMIT messageNamespaceChanged();
}

const QString &UpnpServiceCaller::messageNamespace() const
{
    return d->mMessageNamespace;
}

void UpnpServiceCaller::downloadAndParseServiceDescription(const QUrl &serviceUrl)
{
    qDebug() << "UpnpServiceCaller::downloadAndParseServiceDescription" << serviceUrl;
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpServiceCaller::asyncCallService(const QString &name, const QVariantList &parameters)
{
    if (d->mClientInterface) {
        d->mClientInterface->setSoapVersion(KDSoapClientInterface::SOAP1_1);
        d->mClientInterface->setStyle(KDSoapClientInterface::RPCStyle);

        KDSoapMessage message;
        message.addArgument(QStringLiteral("InstanceID"), QStringLiteral("0"));
        message.addArgument(QStringLiteral("Speed"), QStringLiteral("1"));
        d->mClientInterface->call(name, message, d->mMessageNamespace + QStringLiteral("#") + name);
    }
}

void UpnpServiceCaller::finishedDownload(QNetworkReply *reply)
{
}

#include "moc_upnpservicecaller.cpp"
