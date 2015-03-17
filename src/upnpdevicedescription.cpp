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

#include "upnpdevicedescription.h"

#include "upnpservicedescription.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QVariantMap>

class UpnpDeviceDiscoveryPrivate
{
public:

    UpnpDeviceDiscoveryPrivate()
        : mUUID(QStringLiteral("")), mNetworkAccess(), mDiscoveryResults(), mUDN(QStringLiteral("")), mUPC(QStringLiteral("")), mDeviceType(QStringLiteral("")), mFriendlyName(QStringLiteral("")),
          mManufacturer(QStringLiteral("")), mManufacturerURL(QStringLiteral("")), mModelDescription(QStringLiteral("")), mModelName(QStringLiteral("")), mModelNumber(QStringLiteral("")),
          mModelURL(QStringLiteral("")), mSerialNumber(QStringLiteral("")), mURLBase(QStringLiteral(""))
    {
    }

    QString mUUID;

    QNetworkAccessManager mNetworkAccess;

    QList<QVariantMap> mDiscoveryResults;

    QVariant mUDN;

    QVariant mUPC;

    QVariant mDeviceType;

    QVariant mFriendlyName;

    QVariant mManufacturer;

    QVariant mManufacturerURL;

    QVariant mModelDescription;

    QVariant mModelName;

    QVariant mModelNumber;

    QVariant mModelURL;

    QVariant mSerialNumber;

    QVariant mURLBase;

    QMap<QString, QPointer<UpnpServiceDescription> > mServices;
};

UpnpDeviceDescription::UpnpDeviceDescription(QObject *parent) : QObject(parent), d(new UpnpDeviceDiscoveryPrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpDeviceDescription::finishedDownload);
}

UpnpDeviceDescription::~UpnpDeviceDescription()
{
    delete d;
}

const QString &UpnpDeviceDescription::UUID() const
{
    return d->mUUID;
}

void UpnpDeviceDescription::setUUID(const QString &uuid)
{
    d->mUUID = uuid;
    Q_EMIT UUIDChanged();
}

const QVariant &UpnpDeviceDescription::UDN()
{
    return d->mUDN;
}

const QVariant &UpnpDeviceDescription::UPC()
{
    return d->mUPC;
}

const QVariant &UpnpDeviceDescription::deviceType()
{
    return d->mDeviceType;
}

const QVariant &UpnpDeviceDescription::friendlyName()
{
    return d->mFriendlyName;
}

const QVariant &UpnpDeviceDescription::manufacturer()
{
    return d->mManufacturer;
}

const QVariant &UpnpDeviceDescription::manufacturerURL()
{
    return d->mManufacturerURL;
}

const QVariant &UpnpDeviceDescription::modelDescription()
{
    return d->mModelDescription;
}

const QVariant &UpnpDeviceDescription::modelName()
{
    return d->mModelName;
}

const QVariant &UpnpDeviceDescription::modelNumber()
{
    return d->mModelNumber;
}

const QVariant &UpnpDeviceDescription::modelURL()
{
    return d->mModelURL;
}

const QVariant &UpnpDeviceDescription::serialNumber()
{
    return d->mSerialNumber;
}

const QVariant &UpnpDeviceDescription::URLBase()
{
    return d->mURLBase;
}

UpnpServiceDescription* UpnpDeviceDescription::serviceById(const QString &serviceId) const
{
    return d->mServices[serviceId].data();
}

void UpnpDeviceDescription::downloadAndParseDeviceDescription(const QUrl &serviceUrl)
{
    d->mNetworkAccess.get(QNetworkRequest(serviceUrl));
}

void UpnpDeviceDescription::finishedDownload(QNetworkReply *reply)
{
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        QDomDocument deviceDescriptionDocument;
        deviceDescriptionDocument.setContent(reply);

        const QDomElement &documentRoot = deviceDescriptionDocument.documentElement();

        QVariantMap deviceDescription;

        QDomNode currentChild = documentRoot.firstChild();
        while (!currentChild.isNull()) {
            if (currentChild.isElement() && !currentChild.firstChild().isNull() && !currentChild.firstChild().hasChildNodes()) {
                deviceDescription[currentChild.nodeName()] = currentChild.toElement().text();
            }
            currentChild = currentChild.nextSibling();
        }

        const QDomElement &deviceRoot = documentRoot.firstChildElement(QStringLiteral("device"));

        currentChild = deviceRoot.firstChild();
        while (!currentChild.isNull()) {
            if (currentChild.isElement() && !currentChild.firstChild().isNull() && !currentChild.firstChild().hasChildNodes()) {
                deviceDescription[currentChild.nodeName()] = currentChild.toElement().text();
            }
            currentChild = currentChild.nextSibling();
        }

        d->mUDN = deviceDescription[QStringLiteral("UDN")];
        Q_EMIT UDNChanged(d->mUUID);

        d->mUPC = deviceDescription[QStringLiteral("UPC")];
        Q_EMIT UPCChanged(d->mUUID);

        d->mDeviceType = deviceDescription[QStringLiteral("deviceType")];
        Q_EMIT deviceTypeChanged(d->mUUID);

        d->mFriendlyName = deviceDescription[QStringLiteral("friendlyName")];
        qDebug() << "friendlyName" << d->mFriendlyName;
        Q_EMIT friendlyNameChanged(d->mUUID);

        d->mManufacturer = deviceDescription[QStringLiteral("manufacturer")];
        Q_EMIT manufacturerChanged(d->mUUID);

        d->mManufacturerURL = deviceDescription[QStringLiteral("manufacturerURL")];
        Q_EMIT manufacturerURLChanged(d->mUUID);

        d->mModelDescription = deviceDescription[QStringLiteral("modelDescription")];
        Q_EMIT modelDescriptionChanged(d->mUUID);

        d->mModelName = deviceDescription[QStringLiteral("modelName")];
        Q_EMIT modelNameChanged(d->mUUID);

        d->mModelNumber = deviceDescription[QStringLiteral("modelNumber")];
        Q_EMIT modelNumberChanged(d->mUUID);

        d->mModelURL = deviceDescription[QStringLiteral("modelURL")];
        Q_EMIT modelURLChanged(d->mUUID);

        d->mSerialNumber = deviceDescription[QStringLiteral("serialNumber")];
        qDebug() << "SerialNumber" << d->mSerialNumber;
        Q_EMIT serialNumberChanged(d->mUUID);

        if (deviceDescription[QStringLiteral("URLBase")].isValid() && !deviceDescription[QStringLiteral("URLBase")].toString().isEmpty()) {
            d->mURLBase = deviceDescription[QStringLiteral("URLBase")];
        } else {
            d->mURLBase = reply->url().adjusted(QUrl::RemovePath);
        }
        Q_EMIT URLBaseChanged(d->mUUID);

        auto serviceList = deviceDescriptionDocument.elementsByTagName(QStringLiteral("service"));
        for (int serviceIndex = 0; serviceIndex < serviceList.length(); ++serviceIndex) {
            const QDomNode &serviceNode(serviceList.at(serviceIndex));
            if (!serviceNode.isNull()) {
                QPointer<UpnpServiceDescription> newService;

                const QDomNode &serviceTypeNode = serviceNode.firstChildElement(QStringLiteral("serviceType"));
                newService = new UpnpServiceDescription;

                newService->setBaseURL(d->mURLBase);
                if (!serviceTypeNode.isNull()) {
                    newService->setServiceType(serviceTypeNode.toElement().text());
                }

                const QDomNode &serviceIdNode = serviceNode.firstChildElement(QStringLiteral("serviceId"));
                if (!serviceIdNode.isNull()) {
                    newService->setServiceId(serviceIdNode.toElement().text());
                }

                const QDomNode &SCPDURLNode = serviceNode.firstChildElement(QStringLiteral("SCPDURL"));
                if (!SCPDURLNode.isNull()) {
                    newService->setSCPDURL(SCPDURLNode.toElement().text());
                }

                const QDomNode &controlURLNode = serviceNode.firstChildElement(QStringLiteral("controlURL"));
                if (!controlURLNode.isNull()) {
                    newService->setControlURL(controlURLNode.toElement().text());
                }

                const QDomNode &eventSubURLNode = serviceNode.firstChildElement(QStringLiteral("eventSubURL"));
                if (!eventSubURLNode.isNull()) {
                    newService->setEventSubURL(eventSubURLNode.toElement().text());
                }

                QUrl serviceUrl(d->mURLBase.toUrl());
                serviceUrl.setPath(newService->SCPDURL().toString());

                newService->downloadAndParseServiceDescription(serviceUrl);
                d->mServices[serviceIdNode.toElement().text()] = newService;
            }
        }
    }
}

#include "moc_upnpdevicedescription.cpp"