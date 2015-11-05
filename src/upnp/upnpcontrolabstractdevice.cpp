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

#include "upnpcontrolabstractdevice.h"

#include "upnpcontrolabstractservice.h"
#include "upnpcontrolavtransport.h"
#include "upnpcontrolswitchpower.h"
#include "upnpcontrolconnectionmanager.h"
#include "upnpcontrolcontentdirectory.h"

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

class UpnpControlAbstractDevicePrivate
{
public:

    UpnpControlAbstractDevicePrivate()
        : mNetworkAccess(), mDiscoveryResults()
    {
    }

    QNetworkAccessManager mNetworkAccess;

    QList<QVariantMap> mDiscoveryResults;
};

UpnpControlAbstractDevice::UpnpControlAbstractDevice(QObject *parent) : UpnpAbstractDevice(parent), d(new UpnpControlAbstractDevicePrivate)
{
    connect(&d->mNetworkAccess, &QNetworkAccessManager::finished, this, &UpnpControlAbstractDevice::finishedDownload);
}

UpnpControlAbstractDevice::~UpnpControlAbstractDevice()
{
    delete d;
}

QString UpnpControlAbstractDevice::mobileViewName() const
{
    return QStringLiteral("genericDeviceMobile.qml");
}

QString UpnpControlAbstractDevice::viewName() const
{
    return QStringLiteral("genericDevice.qml");
}

void UpnpControlAbstractDevice::downloadDeviceDescription(const QUrl &deviceUrl)
{
    d->mNetworkAccess.get(QNetworkRequest(deviceUrl));
}

void UpnpControlAbstractDevice::finishedDownload(QNetworkReply *reply)
{
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        parseDeviceDescription(reply, reply->url().adjusted(QUrl::RemovePath).toString());
    } else if (reply->isFinished()) {
        Q_EMIT inError();
    }
}

void UpnpControlAbstractDevice::parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase)
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

    device()->setUDN(deviceDescription[QStringLiteral("UDN")].toString());
    device()->setUPC(deviceDescription[QStringLiteral("UPC")].toString());
    device()->setDeviceType(deviceDescription[QStringLiteral("deviceType")].toString());
    device()->setFriendlyName(deviceDescription[QStringLiteral("friendlyName")].toString());
    device()->setManufacturer(deviceDescription[QStringLiteral("manufacturer")].toString());
    device()->setManufacturerURL(deviceDescription[QStringLiteral("manufacturerURL")].toUrl());
    device()->setModelDescription(deviceDescription[QStringLiteral("modelDescription")].toString());
    device()->setModelName(deviceDescription[QStringLiteral("modelName")].toString());
    device()->setModelNumber(deviceDescription[QStringLiteral("modelNumber")].toString());
    device()->setModelURL(deviceDescription[QStringLiteral("modelURL")].toUrl());
    device()->setSerialNumber(deviceDescription[QStringLiteral("serialNumber")].toString());

    if (deviceDescription[QStringLiteral("URLBase")].isValid() && !deviceDescription[QStringLiteral("URLBase")].toString().isEmpty()) {
        device()->setURLBase(deviceDescription[QStringLiteral("URLBase")].toString());
    } else {
        device()->setURLBase(fallBackURLBase);
    }

    auto serviceList = deviceDescriptionDocument.elementsByTagName(QStringLiteral("service"));
    for (int serviceIndex = 0; serviceIndex < serviceList.length(); ++serviceIndex) {
        const QDomNode &serviceNode(serviceList.at(serviceIndex));
        if (!serviceNode.isNull()) {
            QPointer<UpnpAbstractService> newService;

            const QDomNode &serviceTypeNode = serviceNode.firstChildElement(QStringLiteral("serviceType"));
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

            newService->service()->setBaseURL(device()->URLBase());
            if (!serviceTypeNode.isNull()) {
                newService->service()->setServiceType(serviceTypeNode.toElement().text());
            }

            const QDomNode &serviceIdNode = serviceNode.firstChildElement(QStringLiteral("serviceId"));
            if (!serviceIdNode.isNull()) {
                newService->service()->setServiceId(serviceIdNode.toElement().text());
            }

            const QDomNode &SCPDURLNode = serviceNode.firstChildElement(QStringLiteral("SCPDURL"));
            if (!SCPDURLNode.isNull()) {
                newService->service()->setSCPDURL(QUrl(SCPDURLNode.toElement().text()));
            }

            const QDomNode &controlURLNode = serviceNode.firstChildElement(QStringLiteral("controlURL"));
            if (!controlURLNode.isNull()) {
                QUrl controlUrl(controlURLNode.toElement().text());
                if (!controlUrl.isValid() || controlUrl.scheme().isEmpty()) {
                    controlUrl = QUrl(device()->URLBase());
                    controlUrl.setPath(controlURLNode.toElement().text());
                }
                newService->service()->setControlURL(controlUrl);
            }

            const QDomNode &eventSubURLNode = serviceNode.firstChildElement(QStringLiteral("eventSubURL"));
            if (!eventSubURLNode.isNull()) {
                QUrl eventUrl(eventSubURLNode.toElement().text());
                if (!eventUrl.isValid() || eventUrl.scheme().isEmpty()) {
                    eventUrl = QUrl(device()->URLBase());
                    eventUrl.setPath(eventSubURLNode.toElement().text());
                }
                newService->service()->setEventURL(eventUrl);
            }

            QUrl serviceUrl(newService->service()->SCPDURL().toString());
            if (!serviceUrl.isValid() || serviceUrl.scheme().isEmpty()) {
                serviceUrl.setUrl(device()->URLBase());
                serviceUrl.setPath(newService->service()->SCPDURL().toString());
            }

            qobject_cast<UpnpControlAbstractService*>(newService.data())->downloadServiceDescription(serviceUrl);
            addService(newService);
        }
    }
}

#include "moc_upnpcontrolabstractdevice.cpp"
