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

#include "upnpservicedescription.h"

#include <KDSoapClient/KDSoapClientInterface.h>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

class UpnpServiceDescriptionPrivate
{
public:

    UpnpServiceDescriptionPrivate()
        : mNetworkAccess(), mServiceType(), mServiceId()
    {
    }

    QNetworkAccessManager mNetworkAccess;

    QVariant mServiceType;

    QVariant mServiceId;
};

UpnpServiceDescription::UpnpServiceDescription(QObject *parent)
    : QObject(parent), d(new UpnpServiceDescriptionPrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpServiceDescription::finishedDownload);
}
UpnpServiceDescription::~UpnpServiceDescription()
{
    delete d;
}

void UpnpServiceDescription::setServiceType(const QVariant &newServiceType)
{
    d->mServiceType = newServiceType;
    Q_EMIT serviceTypeChanged();
}

const QVariant &UpnpServiceDescription::serviceType() const
{
    return d->mServiceType;
}

void UpnpServiceDescription::setServiceId(const QVariant &newServiceId)
{
    d->mServiceId = newServiceId;
    Q_EMIT serviceIdChanged();
}

const QVariant &UpnpServiceDescription::serviceId() const
{
    return d->mServiceId;
}

void UpnpServiceDescription::downloadAndParseServiceDescription(const QUrl &serviceUrl)
{
    qDebug() << "UpnpServiceCaller::downloadAndParseServiceDescription" << serviceUrl;
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpServiceDescription::finishedDownload(QNetworkReply *reply)
{
    qDebug() << "UpnpServiceCaller::finishedDownload" << reply->url();
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QDomDocument serviceDescriptionDocument;
        serviceDescriptionDocument.setContent(reply);
        qDebug() << serviceDescriptionDocument.toString();
    }
}

#include "moc_upnpservicedescription.cpp"
