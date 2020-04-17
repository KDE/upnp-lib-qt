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

#include <QBuffer>
#include <QIODevice>
#include <QPointer>
#include <QXmlStreamWriter>

#include <QDebug>

class UpnpDeviceDescriptionPrivate
{
public:
    QList<UpnpServiceDescription> mServices;

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

    int mCacheControl;

    QUrl mLocationUrl;
};

UpnpDeviceDescription::UpnpDeviceDescription()
    : d(std::make_unique<UpnpDeviceDescriptionPrivate>())
{
}

UpnpDeviceDescription::UpnpDeviceDescription(const UpnpDeviceDescription &other)
    : d(std::make_unique<UpnpDeviceDescriptionPrivate>(*other.d))
{
}

UpnpDeviceDescription::UpnpDeviceDescription(UpnpDeviceDescription &&other)
    : d()
{
    other.d.swap(d);
}

UpnpDeviceDescription::~UpnpDeviceDescription() = default;

UpnpDeviceDescription &UpnpDeviceDescription::operator=(const UpnpDeviceDescription &other)
{
    if (this != &other) {
        *d = *other.d;
    }
    return *this;
}

UpnpDeviceDescription &UpnpDeviceDescription::operator=(UpnpDeviceDescription &&other)
{
    if (this != &other) {
        d.reset();
        d.swap(other.d);
    }
    return *this;
}

UpnpServiceDescription UpnpDeviceDescription::serviceById(const QString &serviceId) const
{
    for (const auto &oneService : const_cast<const decltype(d->mServices) &>(d->mServices)) {
        if (oneService.serviceId() == serviceId) {
            return oneService;
        }
    }

    return UpnpServiceDescription {};
}

const UpnpServiceDescription &UpnpDeviceDescription::serviceByIndex(int serviceIndex) const
{
    return d->mServices[serviceIndex];
}

UpnpServiceDescription &UpnpDeviceDescription::serviceByIndex(int serviceIndex)
{
    return d->mServices[serviceIndex];
}

const QList<UpnpServiceDescription> &UpnpDeviceDescription::services() const
{
    return d->mServices;
}

QList<UpnpServiceDescription> &UpnpDeviceDescription::services()
{
    return d->mServices;
}

QList<QString> UpnpDeviceDescription::servicesName() const
{
    QList<QString> result;

    for (const auto &itService : const_cast<const decltype(d->mServices) &>(d->mServices)) {
        result.push_back(itService.serviceType());
    }

    return result;
}

void UpnpDeviceDescription::setUDN(const QString &value)
{
    d->mDeviceUUID = value.mid(5);
    d->mUDN = value;
}

const QString &UpnpDeviceDescription::UDN() const
{
    return d->mUDN;
}

void UpnpDeviceDescription::setUPC(const QString &value)
{
    d->mUPC = value;
}

const QString &UpnpDeviceDescription::UPC() const
{
    return d->mUPC;
}

void UpnpDeviceDescription::setDeviceType(const QString &value)
{
    d->mDeviceType = value;
}

const QString &UpnpDeviceDescription::deviceType() const
{
    return d->mDeviceType;
}

void UpnpDeviceDescription::setFriendlyName(const QString &value)
{
    d->mFriendlyName = value;
}

const QString &UpnpDeviceDescription::friendlyName() const
{
    return d->mFriendlyName;
}

void UpnpDeviceDescription::setManufacturer(const QString &value)
{
    d->mManufacturer = value;
}

const QString &UpnpDeviceDescription::manufacturer() const
{
    return d->mManufacturer;
}

void UpnpDeviceDescription::setManufacturerURL(const QUrl &value)
{
    d->mManufacturerURL = value;
}

const QUrl &UpnpDeviceDescription::manufacturerURL() const
{
    return d->mManufacturerURL;
}

void UpnpDeviceDescription::setModelDescription(const QString &value)
{
    d->mModelDescription = value;
}

const QString &UpnpDeviceDescription::modelDescription() const
{
    return d->mModelDescription;
}

void UpnpDeviceDescription::setModelName(const QString &value)
{
    d->mModelName = value;
}

const QString &UpnpDeviceDescription::modelName() const
{
    return d->mModelName;
}

void UpnpDeviceDescription::setModelNumber(const QString &value)
{
    d->mModelNumber = value;
}

const QString &UpnpDeviceDescription::modelNumber() const
{
    return d->mModelNumber;
}

void UpnpDeviceDescription::setModelURL(const QUrl &value)
{
    d->mModelURL = value;
}

const QUrl &UpnpDeviceDescription::modelURL() const
{
    return d->mModelURL;
}

void UpnpDeviceDescription::setSerialNumber(const QString &value)
{
    d->mSerialNumber = value;
}

const QString &UpnpDeviceDescription::serialNumber() const
{
    return d->mSerialNumber;
}

void UpnpDeviceDescription::setURLBase(const QString &value)
{
    d->mURLBase = value;
}

const QString &UpnpDeviceDescription::URLBase() const
{
    return d->mURLBase;
}

void UpnpDeviceDescription::setCacheControl(int value)
{
    d->mCacheControl = value;
}

int UpnpDeviceDescription::cacheControl() const
{
    return d->mCacheControl;
}

void UpnpDeviceDescription::setLocationUrl(const QUrl &value)
{
    d->mLocationUrl = value;
}

const QUrl &UpnpDeviceDescription::locationUrl() const
{
    return d->mLocationUrl;
}

int UpnpDeviceDescription::addService(const UpnpServiceDescription &newService)
{
    d->mServices.push_back(newService);
    return d->mServices.count() - 1;
}
