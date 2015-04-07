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

#include "upnpbinarylight.h"

#include "upnpswitchpower.h"
#include "upnpdevicesoapserver.h"

#include <QtCore/QUuid>

class BinaryLightPrivate
{
public:

    UpnpDeviceSoapServer mServer;

};

BinaryLight::BinaryLight(int cacheDuration, QObject *parent)
    : UpnpAbstractDevice(parent), d(new BinaryLightPrivate)
{
    setDeviceType(QStringLiteral("urn:schemas-upnp-org:device:BinaryLight:1"));
    setFriendlyName(QStringLiteral("Binary Light for Test"));
    setManufacturer(QStringLiteral("Matthieu Gallien"));
    //setManufacturerURL();
    //setModelDescription();
    setModelName(QStringLiteral("Automatiq Binary Light"));
    //setModelNumber();
    //setModelURL();
    //setSerialNumber();

    const QString &uuidString(QUuid::createUuid().toString());
    setUDN(uuidString.mid(1, uuidString.length() - 2));
    //setUPC();
    //setURLBase();
    setCacheControl(cacheDuration);

    QPointer<UpnpAbstractService> switchPowerService(new UpnpSwitchPower);
    addService(switchPowerService);

    d->mServer.addDevice(this);

    QUrl eventUrl = d->mServer.urlPrefix();
    eventUrl.setPath(QStringLiteral("/event"));
    eventUrl.setQuery(UDN());

    QUrl controlUrl = d->mServer.urlPrefix();
    controlUrl.setPath(QStringLiteral("/control"));
    controlUrl.setQuery(UDN());

    QUrl serviceDescriptionUrl = d->mServer.urlPrefix();
    serviceDescriptionUrl.setPath(QStringLiteral("/service.xml"));
    serviceDescriptionUrl.setQuery(UDN());

    switchPowerService->setControlURL(controlUrl);
    switchPowerService->setEventSubURL(eventUrl);
    switchPowerService->setSCPDURL(serviceDescriptionUrl);

    QUrl deviceDescriptionUrl = d->mServer.urlPrefix();
    deviceDescriptionUrl.setPath(QStringLiteral("/device.xml"));
    deviceDescriptionUrl.setQuery(UDN());
    setLocationUrl(deviceDescriptionUrl);
}

BinaryLight::~BinaryLight()
{
    delete d;
}

#include "moc_upnpbinarylight.cpp"
