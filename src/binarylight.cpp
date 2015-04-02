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

#include "binarylight.h"

#include "upnpswitchpower.h"

#include <QtCore/QUuid>

BinaryLight::BinaryLight(const QUrl &serviceControlUrlValue, const QUrl &serviceEventUrlValue,
                         const QUrl &serviceSCPDUrlValue, const QUrl &locationUrlValue, QObject *parent) :
    UpnpAbstractDevice(parent)
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
    setUDN(QUuid::createUuid().toString());
    //setUPC();
    //setURLBase();
    setLocationUrl(locationUrlValue);

    QPointer<UpnpAbstractService> switchPowerService(new UpnpSwitchPower(serviceControlUrlValue, serviceEventUrlValue, serviceSCPDUrlValue));
    addService(switchPowerService);
}

BinaryLight::~BinaryLight()
{
}

#include "moc_binarylight.cpp"
