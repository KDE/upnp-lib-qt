/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpdevicesoapserver.h"
#include "upnpabstractdevice.h"
#include "upnpdevicesoapserverobject.h"

#include <QList>
#include <QUrl>

#include <QNetworkInterface>

class UpnpDeviceSoapServerPrivate
{
public:
    QList<UpnpAbstractDevice *> mDevices;
};

UpnpDeviceSoapServer::UpnpDeviceSoapServer(QObject *parent)
    : KDSoapServer(parent)
    , d(std::make_unique<UpnpDeviceSoapServerPrivate>())
{
    listen();
}

UpnpDeviceSoapServer::~UpnpDeviceSoapServer() = default;

int UpnpDeviceSoapServer::addDevice(UpnpAbstractDevice *device)
{
    d->mDevices.push_back(device);
    return d->mDevices.count() - 1;
}

void UpnpDeviceSoapServer::removeDevice(int index)
{
    d->mDevices.removeAt(index);
}

QObject *UpnpDeviceSoapServer::createServerObject()
{
    return new UpnpDeviceSoapServerObject(d->mDevices);
}

QUrl UpnpDeviceSoapServer::urlPrefix() const
{
    QHostAddress publicAddress;

    const auto &list = QNetworkInterface::allAddresses();
    for (const auto &address : list) {
        if (!address.isLoopback()) {
            if (address.protocol() == QAbstractSocket::IPv4Protocol) {
                publicAddress = address;
                break;
            }
        }
    }

    QUrl webServerUrl;

    if (!publicAddress.isNull()) {
        webServerUrl.setHost(publicAddress.toString());
    } else {
        webServerUrl.setHost(QStringLiteral("127.0.0.1"));
    }

    webServerUrl.setPort(serverPort());
    webServerUrl.setScheme(QStringLiteral("http"));

    return webServerUrl;
}

#include "moc_upnpdevicesoapserver.cpp"
