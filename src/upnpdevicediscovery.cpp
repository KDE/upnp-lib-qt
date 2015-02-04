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

#include "upnpdevicediscovery.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

#include <QtCore/QList>
#include <QtCore/QVariantMap>

class UpnpDeviceDiscoveryPrivate
{
public:

    UpnpDeviceDiscoveryPrivate()
        : mNetworkAccess(), mDiscoveryResults(), mDeviceDescription()
    {
    }

    QNetworkAccessManager mNetworkAccess;

    QList<QVariantMap> mDiscoveryResults;

    QVariantMap mDeviceDescription;
};

UpnpDeviceDiscovery::UpnpDeviceDiscovery(QObject *parent) : QObject(parent), d(new UpnpDeviceDiscoveryPrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpDeviceDiscovery::finishedDownload);
}

UpnpDeviceDiscovery::~UpnpDeviceDiscovery()
{
    delete d;
}

void UpnpDeviceDiscovery::downloadAndParseDeviceDescription(const QUrl &serviceUrl)
{
    qDebug() << "UpnpDeviceDiscovery::downloadAndParseServiceDescription" << serviceUrl;
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpDeviceDiscovery::finishedDownload(QNetworkReply *reply)
{
    qDebug() << "UpnpDeviceDiscovery::finishedDownload" << reply->url();
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QDomDocument serviceDescriptionDocument;
        serviceDescriptionDocument.setContent(reply);
        qDebug() << serviceDescriptionDocument.toString();

        const QDomElement &documentRoot = serviceDescriptionDocument.documentElement();

        QDomNode currentChild = documentRoot.firstChild();
        while (!currentChild.isNull()) {
            if (currentChild.isElement() && !currentChild.hasChildNodes()) {
                d->mDeviceDescription[currentChild.nodeName()] = currentChild.toElement().text();
            }
            currentChild = currentChild.nextSibling();
        }

        const QDomElement &deviceRoot = documentRoot.firstChildElement(QStringLiteral("device"));

        currentChild = deviceRoot.firstChild();
        while (!currentChild.isNull()) {
            if (currentChild.isElement() && !currentChild.firstChild().isNull() && !currentChild.firstChild().hasChildNodes()) {
                d->mDeviceDescription[currentChild.nodeName()] = currentChild.toElement().text();
            }
            currentChild = currentChild.nextSibling();
        }

        qDebug() << "device description" << d->mDeviceDescription;

        if (!serviceDescriptionDocument.documentElement().firstChildElement(QStringLiteral("URLBase")).isNull()) {
            qDebug() << "base url" << serviceDescriptionDocument.documentElement().firstChildElement(QStringLiteral("URLBase")).nodeName();
            qDebug() << "base url" << serviceDescriptionDocument.documentElement().firstChildElement(QStringLiteral("URLBase")).nodeType();
            qDebug() << "base url" << serviceDescriptionDocument.documentElement().firstChildElement(QStringLiteral("URLBase")).toElement().text();

            const QString &deviceBaseUrl(serviceDescriptionDocument.documentElement().firstChildElement(QStringLiteral("URLBase")).toElement().text());
            auto serviceList = serviceDescriptionDocument.elementsByTagName(QStringLiteral("service"));
            for (int serviceIndex = 0; serviceIndex < serviceList.length(); ++serviceIndex) {
                const QDomNode &serviceNode(serviceList.at(serviceIndex));
                if (!serviceNode.isNull()) {
                    const QDomNode &serviceDescriptionUrl = serviceNode.firstChildElement(QStringLiteral("SCPDURL"));
                    if (!serviceDescriptionUrl.isNull()) {
                        qDebug() << "service url" << serviceDescriptionUrl.nodeName();
                        qDebug() << "service url" << serviceDescriptionUrl.nodeType();
                        qDebug() << "service url" << serviceDescriptionUrl.toElement().text();
                    }
                }
            }
        }
    }
}

#include "moc_upnpdevicediscovery.cpp"
