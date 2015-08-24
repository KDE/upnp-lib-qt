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
#include "upnpabstractservice.h"
#include "upnpssdpengine.h"

#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QPointer>
#include <QtCore/QXmlStreamWriter>

#include <QtCore/QDebug>

class UpnpAbstractDevicePrivate
{
public:

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

    int mCacheControl;

    QUrl mLocationUrl;

    QPointer<QIODevice> mXmlDescription;
};

UpnpAbstractDevice::UpnpAbstractDevice(QObject *parent) :
    QObject(parent), d(new UpnpAbstractDevicePrivate)
{
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

UpnpAbstractService *UpnpAbstractDevice::serviceByIndex(int serviceIndex) const
{
    if (serviceIndex < 0 || serviceIndex > d->mServices.size() - 1) {
        return nullptr;
    } else {
        return d->mServices[serviceIndex].data();
    }
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

const QString &UpnpAbstractDevice::UDN() const
{
    return d->mUDN;
}

void UpnpAbstractDevice::setUPC(const QString &value)
{
    d->mUPC = value;
    Q_EMIT UPCChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::UPC() const
{
    return d->mUPC;
}

void UpnpAbstractDevice::setDeviceType(const QString &value)
{
    d->mDeviceType = value;
    Q_EMIT deviceTypeChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::deviceType() const
{
    return d->mDeviceType;
}

void UpnpAbstractDevice::setFriendlyName(const QString &value)
{
    d->mFriendlyName = value;
    Q_EMIT friendlyNameChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::friendlyName() const
{
    return d->mFriendlyName;
}

void UpnpAbstractDevice::setManufacturer(const QString &value)
{
    d->mManufacturer = value;
    Q_EMIT manufacturerChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::manufacturer() const
{
    return d->mManufacturer;
}

void UpnpAbstractDevice::setManufacturerURL(const QUrl &value)
{
    d->mManufacturerURL = value;
    Q_EMIT manufacturerURLChanged(d->mUDN);
}

const QUrl &UpnpAbstractDevice::manufacturerURL() const
{
    return d->mManufacturerURL;
}

void UpnpAbstractDevice::setModelDescription(const QString &value)
{
    d->mModelDescription = value;
    Q_EMIT modelDescriptionChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::modelDescription() const
{
    return d->mModelDescription;
}

void UpnpAbstractDevice::setModelName(const QString &value)
{
    d->mModelName = value;
    Q_EMIT modelNameChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::modelName() const
{
    return d->mModelName;
}

void UpnpAbstractDevice::setModelNumber(const QString &value)
{
    d->mModelNumber = value;
    Q_EMIT modelNumberChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::modelNumber() const
{
    return d->mModelNumber;
}

void UpnpAbstractDevice::setModelURL(const QUrl &value)
{
    d->mModelURL = value;
    Q_EMIT modelURLChanged(d->mUDN);
}

const QUrl &UpnpAbstractDevice::modelURL() const
{
    return d->mModelURL;
}

void UpnpAbstractDevice::setSerialNumber(const QString &value)
{
    d->mSerialNumber = value;
    Q_EMIT serialNumberChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::serialNumber() const
{
    return d->mSerialNumber;
}

void UpnpAbstractDevice::setURLBase(const QString &value)
{
    d->mURLBase = value;
    Q_EMIT URLBaseChanged(d->mUDN);
}

const QString &UpnpAbstractDevice::URLBase() const
{
    return d->mURLBase;
}

void UpnpAbstractDevice::setCacheControl(int value)
{
    d->mCacheControl = value;
    Q_EMIT cacheControlChanged(d->mUDN);
}

int UpnpAbstractDevice::cacheControl() const
{
    return d->mCacheControl;
}

void UpnpAbstractDevice::setLocationUrl(const QUrl &value)
{
    d->mLocationUrl = value;
    Q_EMIT locationUrlChanged(d->mUDN);
}

const QUrl &UpnpAbstractDevice::locationUrl() const
{
    return d->mLocationUrl;
}

QIODevice* UpnpAbstractDevice::buildAndGetXmlDescription()
{
    if (!d->mXmlDescription) {
        QPointer<QBuffer> newDescription(new QBuffer);

        newDescription->open(QIODevice::ReadWrite);

        QXmlStreamWriter insertStream(newDescription.data());
        insertStream.setAutoFormatting(true);

        insertStream.writeStartDocument();
        insertStream.writeStartElement(QStringLiteral("root"));
        insertStream.writeAttribute(QStringLiteral("xmlns"), QStringLiteral("urn:schemas-upnp-org:device-1-0"));
        insertStream.writeStartElement(QStringLiteral("specVersion"));
        insertStream.writeTextElement(QStringLiteral("major"), QStringLiteral("1"));
        insertStream.writeTextElement(QStringLiteral("minor"), QStringLiteral("0"));
        insertStream.writeEndElement();
        insertStream.writeTextElement(QStringLiteral("URLBase"), URLBase());
        insertStream.writeStartElement(QStringLiteral("device"));
        insertStream.writeTextElement(QStringLiteral("deviceType"), deviceType());
        insertStream.writeTextElement(QStringLiteral("friendlyName"), friendlyName());
        insertStream.writeTextElement(QStringLiteral("manufacturer"), manufacturer());
        insertStream.writeTextElement(QStringLiteral("manufacterURL"), manufacturerURL().toString());
        insertStream.writeTextElement(QStringLiteral("modelDescription"), modelDescription());
        insertStream.writeTextElement(QStringLiteral("modelName"), modelName());
        insertStream.writeTextElement(QStringLiteral("modelNumber"), modelNumber());
        insertStream.writeTextElement(QStringLiteral("modelURL"), modelURL().toString());
        insertStream.writeTextElement(QStringLiteral("serialNumber"), serialNumber());
        insertStream.writeTextElement(QStringLiteral("UDN"), QStringLiteral("uuid:") + UDN());
        insertStream.writeTextElement(QStringLiteral("UPC"), UPC());

        if (!d->mServices.empty()) {
            insertStream.writeStartElement(QStringLiteral("serviceList"));
            for (auto itService = d->mServices.begin(); itService != d->mServices.end(); ++itService) {
                insertStream.writeStartElement(QStringLiteral("service"));
                insertStream.writeTextElement(QStringLiteral("serviceType"), (*itService)->serviceType());
                insertStream.writeTextElement(QStringLiteral("serviceId"), (*itService)->serviceId());
                insertStream.writeTextElement(QStringLiteral("SCPDURL"), (*itService)->SCPDURL().toString());
                insertStream.writeTextElement(QStringLiteral("controlURL"), (*itService)->controlURL().toString());
                insertStream.writeTextElement(QStringLiteral("eventSubURL"), (*itService)->eventURL().toString());
                insertStream.writeEndElement();
            }
            insertStream.writeEndElement();
        }

        insertStream.writeEndElement();
        insertStream.writeEndElement();
        insertStream.writeEndDocument();

        d->mXmlDescription = newDescription;
    }

    d->mXmlDescription->seek(0);

    return d->mXmlDescription;
}

void UpnpAbstractDevice::newSearchQuery(UpnpSsdpEngine *engine, const UpnpSearchQuery &searchQuery)
{
    qDebug() << "UpnpAbstractDevice::newSearchQuery" << "search for" << searchQuery.mSearchTarget;
    switch(searchQuery.mSearchTargetType)
    {
    case SearchTargetType::All:
        qDebug() << "UpnpAbstractDevice::newSearchQuery" << "publish";
        engine->publishDevice(this);
        break;
    case SearchTargetType::RootDevice:
        break;
    case SearchTargetType::DeviceUUID:
        break;
    case SearchTargetType::DeviceType:
        break;
    case SearchTargetType::ServiceType:
        break;
    }
}

int UpnpAbstractDevice::addService(QPointer<UpnpAbstractService> newService)
{
    d->mServices.push_back(newService);
    return d->mServices.count() - 1;
}

#include "moc_upnpabstractdevice.cpp"
