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

#include "upnplogging.h"

#include "upnpabstractservice.h"
#include "upnpssdpengine.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QBuffer>
#include <QIODevice>
#include <QPointer>
#include <QSharedPointer>
#include <QXmlStreamWriter>

#include <QLoggingCategory>

class UpnpAbstractDevicePrivate
{
public:
    UpnpAbstractDevicePrivate()
        : mDevice(new UpnpDeviceDescription)
        , mXmlDescription()
    {
    }

    QSharedPointer<UpnpDeviceDescription> mDevice;

    QPointer<QIODevice> mXmlDescription;
};

UpnpAbstractDevice::UpnpAbstractDevice(QObject *parent)
    : QObject(parent)
    , d(new UpnpAbstractDevicePrivate)
{
}

UpnpAbstractDevice::~UpnpAbstractDevice()
{
}

UpnpServiceDescription UpnpAbstractDevice::serviceDescriptionById(const QString &serviceId) const
{
    auto result = UpnpServiceDescription {};
    const auto &allServices = d->mDevice->services();

    for (const auto &service : allServices) {
        if (service.serviceId() == serviceId) {
            result = service;
            return result;
        }
    }
    return result;
}

UpnpServiceDescription UpnpAbstractDevice::serviceDescriptionByIndex(int serviceIndex) const
{
    if (serviceIndex < 0 || serviceIndex > d->mDevice->services().size() - 1) {
        return UpnpServiceDescription {};
    }

    return d->mDevice->services()[serviceIndex];
}

const QList<UpnpServiceDescription> &UpnpAbstractDevice::services() const
{
    return d->mDevice->services();
}

QVector<QString> UpnpAbstractDevice::servicesName() const
{
    QVector<QString> result;
    const auto &allServices = d->mDevice->services();

    for (const auto &itService : allServices) {
        result.push_back(itService.serviceType());
    }

    return result;
}

void UpnpAbstractDevice::setDescription(UpnpDeviceDescription *value)
{
    d->mDevice.reset(value);
    Q_EMIT descriptionChanged();
}

UpnpDeviceDescription *UpnpAbstractDevice::description()
{
    return d->mDevice.data();
}

const UpnpDeviceDescription *UpnpAbstractDevice::description() const
{
    return d->mDevice.data();
}

QIODevice *UpnpAbstractDevice::buildAndGetXmlDescription()
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
        insertStream.writeTextElement(QStringLiteral("URLBase"), description()->URLBase());
        insertStream.writeStartElement(QStringLiteral("device"));
        insertStream.writeTextElement(QStringLiteral("deviceType"), description()->deviceType());
        insertStream.writeTextElement(QStringLiteral("friendlyName"), description()->friendlyName());
        insertStream.writeTextElement(QStringLiteral("manufacturer"), description()->manufacturer());
        insertStream.writeTextElement(QStringLiteral("manufacterURL"), description()->manufacturerURL().toString());
        insertStream.writeTextElement(QStringLiteral("modelDescription"), description()->modelDescription());
        insertStream.writeTextElement(QStringLiteral("modelName"), description()->modelName());
        insertStream.writeTextElement(QStringLiteral("modelNumber"), description()->modelNumber());
        insertStream.writeTextElement(QStringLiteral("modelURL"), description()->modelURL().toString());
        insertStream.writeTextElement(QStringLiteral("serialNumber"), description()->serialNumber());
        insertStream.writeTextElement(QStringLiteral("UDN"), QStringLiteral("uuid:") + description()->UDN());
        insertStream.writeTextElement(QStringLiteral("UPC"), description()->UPC());

        if (!d->mDevice->services().empty()) {
            insertStream.writeStartElement(QStringLiteral("serviceList"));
            const auto &allServices = d->mDevice->services();

            for (const auto &itService : allServices) {
                insertStream.writeStartElement(QStringLiteral("service"));
                insertStream.writeTextElement(QStringLiteral("serviceType"), itService.serviceType());
                insertStream.writeTextElement(QStringLiteral("serviceId"), itService.serviceId());
                insertStream.writeTextElement(QStringLiteral("SCPDURL"), itService.SCPDURL().toString());
                insertStream.writeTextElement(QStringLiteral("controlURL"), itService.controlURL().toString());
                insertStream.writeTextElement(QStringLiteral("eventSubURL"), itService.eventURL().toString());
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
    qCDebug(orgKdeUpnpLibQtUpnp()) << "UpnpAbstractDevice::newSearchQuery"
                                   << "search for" << searchQuery.mSearchTarget;
    switch (searchQuery.mSearchTargetType) {
    case SearchTargetType::All:
        qCDebug(orgKdeUpnpLibQtUpnp()) << "UpnpAbstractDevice::newSearchQuery"
                                       << "publish";
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

int UpnpAbstractDevice::addService(const UpnpServiceDescription &newService)
{
    d->mDevice->services().push_back(newService);
    return d->mDevice->services().count() - 1;
}

#include "moc_upnpabstractdevice.cpp"
