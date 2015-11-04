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

#include "upnpdevicesoapserver.h"
#include "upnpabstractdevice.h"
#include "upnpdevicesoapserverobject.h"

#include <QtCore/QList>
#include <QtCore/QUrl>

#include <QtNetwork/QNetworkInterface>

class UpnpDeviceSoapServerPrivate
{
public:

    QList<UpnpAbstractDevice*> mDevices;
};

UpnpDeviceSoapServer::UpnpDeviceSoapServer(QObject *parent) : KDSoapServer(parent), d(new UpnpDeviceSoapServerPrivate)
{
    listen();
}

UpnpDeviceSoapServer::~UpnpDeviceSoapServer()
{

}

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

    const QList<QHostAddress> &list = QNetworkInterface::allAddresses();
    for (auto address = list.begin(); address != list.end(); ++address) {
        if (!address->isLoopback()) {
            if (address->protocol() == QAbstractSocket::IPv4Protocol) {
                publicAddress = *address;
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
