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

#include "upnpabstractdevice.h"
#include "upnpssdpengine.h"
#include "upnpdevicesoapserver.h"
#include "upnpabstractservice.h"

class UpnpAbstractDevicePrivate
{
public:

    UpnpSsdpEngine *mSsdpEngine;

    UpnpDeviceSoapServer *mSoapServer;

    QList<QPointer<UpnpAbstractService> > mServices;

    QString mDeviceUUID;

    QString mUDN;

    /**
     * @brief mUPC: Universal Product Code
     * mUPC is optional. 12-digit, all-numeric code that identifies the consumer package.
     * Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
     */
    QString mUPC;

    QString mDeviceType;

    QString mFriendlyName;

    QString mManufacturer;

    QUrl mManufacturerURL;

    QString mModelDescription;

    QString mModelName;

    QString mModelNumber;

    QUrl mModelURL;

    QString mSerialNumber;

    QString mURLBase;

};

UpnpAbstractDevice::UpnpAbstractDevice(QObject *parent) :
    QObject(parent), d(new UpnpAbstractDevicePrivate)
{
    d->mSsdpEngine = nullptr;
    d->mSoapServer = new UpnpDeviceSoapServer(this);
}

UpnpAbstractDevice::~UpnpAbstractDevice()
{
    delete d;
}

UpnpAbstractService* UpnpAbstractDevice::serviceById(const QString &serviceId) const
{
    UpnpAbstractService *result = nullptr;
    for (auto it = d->mServices.begin(); it != d->mServices.end(); ++it) {
        if ((*it)->serviceId() == serviceId) {
            return it->data();
        }
    }
    return result;
}

QList<QPointer<UpnpAbstractService> > &UpnpAbstractDevice::services() const
{
    return d->mServices;
}

void UpnpAbstractDevice::setUDN(const QString &value)
{
    d->mUDN = value;
    Q_EMIT UDNChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::UDN()
{
    return d->mUDN;
}

void UpnpAbstractDevice::setUPC(const QString &value)
{
    d->mUPC = value;
    Q_EMIT UPCChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::UPC()
{
    return d->mUPC;
}

void UpnpAbstractDevice::setDeviceType(const QString &value)
{
    d->mDeviceType = value;
    Q_EMIT deviceTypeChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::deviceType()
{
    return d->mDeviceType;
}

void UpnpAbstractDevice::setFriendlyName(const QString &value)
{
    d->mFriendlyName = value;
    Q_EMIT friendlyNameChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::friendlyName()
{
    return d->mFriendlyName;
}

void UpnpAbstractDevice::setManufacturer(const QString &value)
{
    d->mManufacturer = value;
    Q_EMIT manufacturerChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::manufacturer()
{
    return d->mManufacturer;
}

void UpnpAbstractDevice::setManufacturerURL(const QUrl &value)
{
    d->mManufacturerURL = value;
    Q_EMIT manufacturerURLChanged(d->mUDN);
}

const QUrl &UpnpAbstractDevice::manufacturerURL()
{
    return d->mManufacturerURL;
}

void UpnpAbstractDevice::setModelDescription(const QString &value)
{
    d->mModelDescription = value;
    Q_EMIT modelDescriptionChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::modelDescription()
{
    return d->mModelDescription;
}

void UpnpAbstractDevice::setModelName(const QString &value)
{
    d->mModelName = value;
    Q_EMIT modelNameChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::modelName()
{
    return d->mModelName;
}

void UpnpAbstractDevice::setModelNumber(const QString &value)
{
    d->mModelNumber = value;
    Q_EMIT modelNumberChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::modelNumber()
{
    return d->mModelNumber;
}

void UpnpAbstractDevice::setModelURL(const QUrl &value)
{
    d->mModelURL = value;
    Q_EMIT modelURLChanged(d->mUDN);
}

const QUrl &UpnpAbstractDevice::modelURL()
{
    return d->mModelURL;
}

void UpnpAbstractDevice::setSerialNumber(const QString &value)
{
    d->mSerialNumber = value;
    Q_EMIT serialNumberChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::serialNumber()
{
    return d->mSerialNumber;
}

void UpnpAbstractDevice::setURLBase(const QString &value)
{
    d->mURLBase = value;
    Q_EMIT URLBaseChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::URLBase()
{
    return d->mURLBase;
}

#include "moc_upnpabstractdevice.cpp"
