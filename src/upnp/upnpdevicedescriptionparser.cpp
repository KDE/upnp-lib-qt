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

#include "upnpdevicedescriptionparser.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include "upnpservicedescriptionparser.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtXml/QDomDocument>

class UpnpDeviceDescriptionParserPrivate
{
public:

    UpnpDeviceDescriptionParserPrivate(QNetworkAccessManager *aNetworkAccess, QSharedPointer<UpnpDeviceDescription> deviceDescription)
        : mNetworkAccess(aNetworkAccess), mDeviceDescription(std::move(deviceDescription)), mDeviceURL()
    {
    }

    QNetworkAccessManager *mNetworkAccess;

    QSharedPointer<UpnpDeviceDescription> mDeviceDescription;

    QMap<QString, QSharedPointer<UpnpServiceDescriptionParser> > mServiceDescriptionParsers;

    QUrl mDeviceURL;
};

UpnpDeviceDescriptionParser::UpnpDeviceDescriptionParser(QNetworkAccessManager *aNetworkAccess, QSharedPointer<UpnpDeviceDescription> deviceDescription, QObject *parent)
    : QObject(parent), d(new UpnpDeviceDescriptionParserPrivate(aNetworkAccess, deviceDescription))
{
}

UpnpDeviceDescriptionParser::~UpnpDeviceDescriptionParser()
{
    delete d;
}

void UpnpDeviceDescriptionParser::downloadDeviceDescription(const QUrl &deviceUrl)
{
    d->mDeviceURL = deviceUrl;
    d->mNetworkAccess->get(QNetworkRequest(deviceUrl));
}

void UpnpDeviceDescriptionParser::serviceDescriptionParsed(const QString &upnpServiceId)
{
    d->mServiceDescriptionParsers.remove(upnpServiceId);

    if (d->mServiceDescriptionParsers.isEmpty()) {
        Q_EMIT descriptionParsed(d->mDeviceDescription->UDN());
    }
}

void UpnpDeviceDescriptionParser::finishedDownload(QNetworkReply *reply)
{
    if (reply->url() == d->mDeviceURL) {
        if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
            parseDeviceDescription(reply, reply->url().adjusted(QUrl::RemovePath).toString());
        } else if (reply->isFinished()) {
            Q_EMIT deviceDescriptionInError(d->mDeviceDescription->UDN());
        }
    }
}

void UpnpDeviceDescriptionParser::parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase)
{
    QDomDocument deviceDescriptionDocument;
    deviceDescriptionDocument.setContent(deviceDescriptionContent);

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

    d->mDeviceDescription->setUDN(deviceDescription[QStringLiteral("UDN")].toString());
    d->mDeviceDescription->setUPC(deviceDescription[QStringLiteral("UPC")].toString());
    d->mDeviceDescription->setDeviceType(deviceDescription[QStringLiteral("deviceType")].toString());
    d->mDeviceDescription->setFriendlyName(deviceDescription[QStringLiteral("friendlyName")].toString());
    d->mDeviceDescription->setManufacturer(deviceDescription[QStringLiteral("manufacturer")].toString());
    d->mDeviceDescription->setManufacturerURL(deviceDescription[QStringLiteral("manufacturerURL")].toUrl());
    d->mDeviceDescription->setModelDescription(deviceDescription[QStringLiteral("modelDescription")].toString());
    d->mDeviceDescription->setModelName(deviceDescription[QStringLiteral("modelName")].toString());
    d->mDeviceDescription->setModelNumber(deviceDescription[QStringLiteral("modelNumber")].toString());
    d->mDeviceDescription->setModelURL(deviceDescription[QStringLiteral("modelURL")].toUrl());
    d->mDeviceDescription->setSerialNumber(deviceDescription[QStringLiteral("serialNumber")].toString());

    if (deviceDescription[QStringLiteral("URLBase")].isValid() && !deviceDescription[QStringLiteral("URLBase")].toString().isEmpty()) {
        d->mDeviceDescription->setURLBase(deviceDescription[QStringLiteral("URLBase")].toString());
    } else {
        d->mDeviceDescription->setURLBase(fallBackURLBase);
    }

    auto serviceList = deviceDescriptionDocument.elementsByTagName(QStringLiteral("service"));
    for (int serviceIndex = 0; serviceIndex < serviceList.length(); ++serviceIndex) {
        const QDomNode &serviceNode(serviceList.at(serviceIndex));
        if (!serviceNode.isNull()) {
            QSharedPointer<UpnpServiceDescription> newService(new UpnpServiceDescription);

            const QDomNode &serviceTypeNode = serviceNode.firstChildElement(QStringLiteral("serviceType"));
#if 0
            if (!serviceTypeNode.isNull()) {
                if (serviceTypeNode.toElement().text() == QStringLiteral("urn:schemas-upnp-org:service:AVTransport:1")) {
                    newService = new UpnpControlAVTransport;
                } else if (serviceTypeNode.toElement().text() == QStringLiteral("urn:schemas-upnp-org:service:RenderingControl:1")) {
                    newService = new UpnpControlSwitchPower;
                } else if (serviceTypeNode.toElement().text() == QStringLiteral("urn:schemas-upnp-org:service:SwitchPower:1")) {
                    newService = new UpnpControlSwitchPower;
                } else if (serviceTypeNode.toElement().text() == QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1")) {
                    newService = new UpnpControlConnectionManager;
                } else if (serviceTypeNode.toElement().text() == QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1")) {
                    newService = new UpnpControlContentDirectory;
                } else {
                    newService = new UpnpControlAbstractService;
                }
            } else {
                newService = new UpnpControlAbstractService;
            }
#endif

            newService->setBaseURL(d->mDeviceDescription->URLBase());
            if (!serviceTypeNode.isNull()) {
                newService->setServiceType(serviceTypeNode.toElement().text());
            }

            const QDomNode &serviceIdNode = serviceNode.firstChildElement(QStringLiteral("serviceId"));
            if (!serviceIdNode.isNull()) {
                newService->setServiceId(serviceIdNode.toElement().text());
            }

            const QDomNode &SCPDURLNode = serviceNode.firstChildElement(QStringLiteral("SCPDURL"));
            if (!SCPDURLNode.isNull()) {
                newService->setSCPDURL(QUrl(SCPDURLNode.toElement().text()));
            }

            const QDomNode &controlURLNode = serviceNode.firstChildElement(QStringLiteral("controlURL"));
            if (!controlURLNode.isNull()) {
                QUrl controlUrl(controlURLNode.toElement().text());
                if (!controlUrl.isValid() || controlUrl.scheme().isEmpty()) {
                    controlUrl = QUrl(d->mDeviceDescription->URLBase());
                    controlUrl.setPath(controlURLNode.toElement().text());
                }
                newService->setControlURL(controlUrl);
            }

            const QDomNode &eventSubURLNode = serviceNode.firstChildElement(QStringLiteral("eventSubURL"));
            if (!eventSubURLNode.isNull()) {
                QUrl eventUrl(eventSubURLNode.toElement().text());
                if (!eventUrl.isValid() || eventUrl.scheme().isEmpty()) {
                    eventUrl = QUrl(d->mDeviceDescription->URLBase());
                    eventUrl.setPath(eventSubURLNode.toElement().text());
                }
                newService->setEventURL(eventUrl);
            }

            QUrl serviceUrl(newService->SCPDURL().toString());
            if (!serviceUrl.isValid() || serviceUrl.scheme().isEmpty()) {
                serviceUrl.setUrl(d->mDeviceDescription->URLBase());
                serviceUrl.setPath(newService->SCPDURL().toString());
            }

            d->mDeviceDescription->addService(newService);

            d->mServiceDescriptionParsers[newService->serviceId()].reset(new UpnpServiceDescriptionParser(d->mNetworkAccess, d->mDeviceDescription->serviceByIndex(serviceIndex)));

            connect(d->mServiceDescriptionParsers[newService->serviceId()].data(), &UpnpServiceDescriptionParser::descriptionParsed,
                    this, &UpnpDeviceDescriptionParser::serviceDescriptionParsed);
            connect(d->mNetworkAccess, &QNetworkAccessManager::finished,
                    d->mServiceDescriptionParsers[newService->serviceId()].data(), &UpnpServiceDescriptionParser::finishedDownload);

            d->mServiceDescriptionParsers[newService->serviceId()]->downloadServiceDescription(serviceUrl);
        }
    }
}


#include "moc_upnpdevicedescriptionparser.cpp"
