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

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QtCore/QUuid>

class BinaryLightPrivate
{
public:

    UpnpDeviceSoapServer mServer;

};

BinaryLight::BinaryLight(int cacheDuration, QObject *parent)
    : UpnpAbstractDevice(parent), d(new BinaryLightPrivate)
{
    description()->setDeviceType(QStringLiteral("urn:schemas-upnp-org:device:BinaryLight:1"));
    description()->setFriendlyName(QStringLiteral("Binary Light for Test"));
    description()->setManufacturer(QStringLiteral("Matthieu Gallien"));
    description()->setManufacturerURL(QUrl(QStringLiteral("https://gitlab.com/homeautomationqt/upnp-player-qt")));
    description()->setModelDescription(QStringLiteral("Test Device"));
    description()->setModelName(QStringLiteral("Automatiq Binary Light"));
    description()->setModelNumber(QStringLiteral("0.1"));
    description()->setModelURL(QUrl(QStringLiteral("https://gitlab.com/homeautomationqt/upnp-player-qt")));
    description()->setSerialNumber(QStringLiteral("test-0.1"));

    const QString &uuidString(QUuid::createUuid().toString());
    description()->setUDN(uuidString.mid(1, uuidString.length() - 2));
    description()->setUPC(QStringLiteral("test"));
    description()->setCacheControl(cacheDuration);

    QPointer<UpnpAbstractService> switchPowerService(new UpnpSwitchPower);
    const int serviceIndex = 0/*addService(switchPowerService)*/;

    const int deviceIndex = d->mServer.addDevice(this);

    QUrl eventUrl = d->mServer.urlPrefix();
    eventUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/") + QString::number(serviceIndex) + QStringLiteral("/event"));

    QUrl controlUrl = d->mServer.urlPrefix();
    controlUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/") + QString::number(serviceIndex) + QStringLiteral("/control"));

    QUrl serviceDescriptionUrl = d->mServer.urlPrefix();
    serviceDescriptionUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/") + QString::number(serviceIndex) + QStringLiteral("/service.xml"));

    switchPowerService->description()->setControlURL(controlUrl);
    switchPowerService->description()->setEventURL(eventUrl);
    switchPowerService->description()->setSCPDURL(serviceDescriptionUrl);

    QUrl deviceDescriptionUrl = d->mServer.urlPrefix();
    description()->setURLBase(d->mServer.urlPrefix().toString());
    deviceDescriptionUrl.setPath(QStringLiteral("/") + QString::number(deviceIndex) + QStringLiteral("/device.xml"));
    description()->setLocationUrl(deviceDescriptionUrl);
}

BinaryLight::~BinaryLight()
{
    delete d;
}

#include "moc_upnpbinarylight.cpp"
