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

#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QPointer>
#include <QtCore/QXmlStreamWriter>

#include <QtCore/QDebug>

class UpnpDeviceDescriptionPrivate
{
public:

    QList<QSharedPointer<UpnpServiceDescription> > mServices;

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

UpnpDeviceDescription::UpnpDeviceDescription(QObject *parent) : QObject(parent), d(new UpnpDeviceDescriptionPrivate)
{
}

UpnpDeviceDescription::~UpnpDeviceDescription()
{
    delete d;
}

const QSharedPointer<UpnpServiceDescription> UpnpDeviceDescription::serviceByIndex(int serviceIndex) const
{
    return d->mServices[serviceIndex];
}

const QList<QSharedPointer<UpnpServiceDescription> >& UpnpDeviceDescription::services() const
{
    return d->mServices;
}

QList<QString> UpnpDeviceDescription::servicesName() const
{
    QList<QString> result;

    for (const auto &itService: d->mServices) {
        result.push_back(itService->serviceType());
    }

    return result;
}

void UpnpDeviceDescription::setUDN(const QString &value)
{
    d->mDeviceUUID = value.mid(5);
    d->mUDN = value;

    Q_EMIT UDNChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::UDN() const
{
    return d->mUDN;
}

void UpnpDeviceDescription::setUPC(const QString &value)
{
    d->mUPC = value;

    Q_EMIT UPCChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::UPC() const
{
    return d->mUPC;
}

void UpnpDeviceDescription::setDeviceType(const QString &value)
{
    d->mDeviceType = value;

    Q_EMIT deviceTypeChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::deviceType() const
{
    return d->mDeviceType;
}

void UpnpDeviceDescription::setFriendlyName(const QString &value)
{
    d->mFriendlyName = value;

    Q_EMIT friendlyNameChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::friendlyName() const
{
    return d->mFriendlyName;
}

void UpnpDeviceDescription::setManufacturer(const QString &value)
{
    d->mManufacturer = value;

    Q_EMIT manufacturerChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::manufacturer() const
{
    return d->mManufacturer;
}

void UpnpDeviceDescription::setManufacturerURL(const QUrl &value)
{
    d->mManufacturerURL = value;

    Q_EMIT manufacturerURLChanged(d->mDeviceUUID);
}

const QUrl &UpnpDeviceDescription::manufacturerURL() const
{
    return d->mManufacturerURL;
}

void UpnpDeviceDescription::setModelDescription(const QString &value)
{
    d->mModelDescription = value;

    Q_EMIT modelDescriptionChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::modelDescription() const
{
    return d->mModelDescription;
}

void UpnpDeviceDescription::setModelName(const QString &value)
{
    d->mModelName = value;

    Q_EMIT modelNameChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::modelName() const
{
    return d->mModelName;
}

void UpnpDeviceDescription::setModelNumber(const QString &value)
{
    d->mModelNumber = value;

    Q_EMIT modelNumberChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::modelNumber() const
{
    return d->mModelNumber;
}

void UpnpDeviceDescription::setModelURL(const QUrl &value)
{
    d->mModelURL = value;

    Q_EMIT modelURLChanged(d->mDeviceUUID);
}

const QUrl &UpnpDeviceDescription::modelURL() const
{
    return d->mModelURL;
}

void UpnpDeviceDescription::setSerialNumber(const QString &value)
{
    d->mSerialNumber = value;

    Q_EMIT serialNumberChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::serialNumber() const
{
    return d->mSerialNumber;
}

void UpnpDeviceDescription::setURLBase(const QString &value)
{
    d->mURLBase = value;

    Q_EMIT URLBaseChanged(d->mDeviceUUID);
}

const QString &UpnpDeviceDescription::URLBase() const
{
    return d->mURLBase;
}

void UpnpDeviceDescription::setCacheControl(int value)
{
    d->mCacheControl = value;

    Q_EMIT cacheControlChanged(d->mDeviceUUID);
}

int UpnpDeviceDescription::cacheControl() const
{
    return d->mCacheControl;
}

void UpnpDeviceDescription::setLocationUrl(const QUrl &value)
{
    d->mLocationUrl = value;

    Q_EMIT locationUrlChanged(d->mDeviceUUID);
}

const QUrl &UpnpDeviceDescription::locationUrl() const
{
    return d->mLocationUrl;
}

int UpnpDeviceDescription::addService(const QSharedPointer<UpnpServiceDescription> &newService)
{
    d->mServices.push_back(newService);
    return d->mServices.count() - 1;
}

#include "moc_upnpdevicedescription.cpp"
