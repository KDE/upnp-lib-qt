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

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QXmlStreamWriter>

#include <QtCore/QDebug>

class UpnpAbstractDevicePrivate
{
public:

    UpnpAbstractDevicePrivate() : mDevice(new UpnpDeviceDescription), mXmlDescription()
    {
    }

    QSharedPointer<UpnpDeviceDescription> mDevice;

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

UpnpServiceDescription* UpnpAbstractDevice::serviceById(const QString &serviceId) const
{
    UpnpServiceDescription *result = nullptr;
    for (auto service : d->mDevice->services()) {
        if (service->serviceId() == serviceId) {
            return service.data();
        }
    }
    return result;
}

UpnpServiceDescription *UpnpAbstractDevice::serviceByIndex(int serviceIndex) const
{
    if (serviceIndex < 0 || serviceIndex > d->mDevice->services().size() - 1) {
        return nullptr;
    } else {
        return d->mDevice->services()[serviceIndex].data();
    }
}

const QList<QSharedPointer<UpnpServiceDescription> >& UpnpAbstractDevice::services() const
{
    return d->mDevice->services();
}

QList<QString> UpnpAbstractDevice::servicesName() const
{
    QList<QString> result;

    for (auto itService: d->mDevice->services()) {
        result.push_back(itService->serviceType());
    }

    return result;
}

void UpnpAbstractDevice::setDevice(UpnpDeviceDescription *value)
{
    d->mDevice.reset(value);
    Q_EMIT deviceChanged();
}

UpnpDeviceDescription *UpnpAbstractDevice::device()
{
    return d->mDevice.data();
}

const UpnpDeviceDescription *UpnpAbstractDevice::device() const
{
    return d->mDevice.data();
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
        insertStream.writeTextElement(QStringLiteral("URLBase"), device()->URLBase());
        insertStream.writeStartElement(QStringLiteral("device"));
        insertStream.writeTextElement(QStringLiteral("deviceType"), device()->deviceType());
        insertStream.writeTextElement(QStringLiteral("friendlyName"), device()->friendlyName());
        insertStream.writeTextElement(QStringLiteral("manufacturer"), device()->manufacturer());
        insertStream.writeTextElement(QStringLiteral("manufacterURL"), device()->manufacturerURL().toString());
        insertStream.writeTextElement(QStringLiteral("modelDescription"), device()->modelDescription());
        insertStream.writeTextElement(QStringLiteral("modelName"), device()->modelName());
        insertStream.writeTextElement(QStringLiteral("modelNumber"), device()->modelNumber());
        insertStream.writeTextElement(QStringLiteral("modelURL"), device()->modelURL().toString());
        insertStream.writeTextElement(QStringLiteral("serialNumber"), device()->serialNumber());
        insertStream.writeTextElement(QStringLiteral("UDN"), QStringLiteral("uuid:") + device()->UDN());
        insertStream.writeTextElement(QStringLiteral("UPC"), device()->UPC());

        if (!d->mDevice->services().empty()) {
            insertStream.writeStartElement(QStringLiteral("serviceList"));
            for (const auto &itService : d->mDevice->services()) {
                insertStream.writeStartElement(QStringLiteral("service"));
                insertStream.writeTextElement(QStringLiteral("serviceType"), itService->serviceType());
                insertStream.writeTextElement(QStringLiteral("serviceId"), itService->serviceId());
                insertStream.writeTextElement(QStringLiteral("SCPDURL"), itService->SCPDURL().toString());
                insertStream.writeTextElement(QStringLiteral("controlURL"), itService->controlURL().toString());
                insertStream.writeTextElement(QStringLiteral("eventSubURL"), itService->eventURL().toString());
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

int UpnpAbstractDevice::addService(QSharedPointer<UpnpServiceDescription> newService)
{
    d->mDevice->services().push_back(newService);
    return d->mDevice->services().count() - 1;
}

#include "moc_upnpabstractdevice.cpp"
