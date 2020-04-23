/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    UpnpDeviceDescription mDevice;

    std::unique_ptr<QIODevice> mXmlDescription;
};

UpnpAbstractDevice::UpnpAbstractDevice(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<UpnpAbstractDevicePrivate>())
{
}

UpnpAbstractDevice::~UpnpAbstractDevice() = default;

UpnpServiceDescription UpnpAbstractDevice::serviceDescriptionById(const QString &serviceId) const
{
    auto result = UpnpServiceDescription {};
    const auto &allServices = d->mDevice.services();

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
    if (serviceIndex < 0 || serviceIndex > d->mDevice.services().size() - 1) {
        return UpnpServiceDescription {};
    }

    return d->mDevice.services()[serviceIndex];
}

const QList<UpnpServiceDescription> &UpnpAbstractDevice::services() const
{
    return d->mDevice.services();
}

QVector<QString> UpnpAbstractDevice::servicesName() const
{
    QVector<QString> result;
    const auto &allServices = d->mDevice.services();

    for (const auto &itService : allServices) {
        result.push_back(itService.serviceType());
    }

    return result;
}

void UpnpAbstractDevice::setDescription(UpnpDeviceDescription value)
{
    d->mDevice = std::move(value);
    Q_EMIT descriptionChanged();
}

UpnpDeviceDescription &UpnpAbstractDevice::description()
{
    return d->mDevice;
}

const UpnpDeviceDescription &UpnpAbstractDevice::description() const
{
    return d->mDevice;
}

std::unique_ptr<QIODevice> UpnpAbstractDevice::buildAndGetXmlDescription()
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
        insertStream.writeTextElement(QStringLiteral("URLBase"), d->mDevice.URLBase());
        insertStream.writeStartElement(QStringLiteral("device"));
        insertStream.writeTextElement(QStringLiteral("deviceType"), d->mDevice.deviceType());
        insertStream.writeTextElement(QStringLiteral("friendlyName"), d->mDevice.friendlyName());
        insertStream.writeTextElement(QStringLiteral("manufacturer"), d->mDevice.manufacturer());
        insertStream.writeTextElement(QStringLiteral("manufacterURL"), d->mDevice.manufacturerURL().toString());
        insertStream.writeTextElement(QStringLiteral("modelDescription"), d->mDevice.modelDescription());
        insertStream.writeTextElement(QStringLiteral("modelName"), d->mDevice.modelName());
        insertStream.writeTextElement(QStringLiteral("modelNumber"), d->mDevice.modelNumber());
        insertStream.writeTextElement(QStringLiteral("modelURL"), d->mDevice.modelURL().toString());
        insertStream.writeTextElement(QStringLiteral("serialNumber"), d->mDevice.serialNumber());
        insertStream.writeTextElement(QStringLiteral("UDN"), QStringLiteral("uuid:") + d->mDevice.UDN());
        insertStream.writeTextElement(QStringLiteral("UPC"), d->mDevice.UPC());

        if (!d->mDevice.services().empty()) {
            insertStream.writeStartElement(QStringLiteral("serviceList"));
            const auto &allServices = d->mDevice.services();

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

        d->mXmlDescription .reset( newDescription);
    }

    d->mXmlDescription->seek(0);

    return std::move(d->mXmlDescription);
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
    case SearchTargetType::DeviceUUID:
    case SearchTargetType::DeviceType:
    case SearchTargetType::ServiceType:
        break;
    }
}

int UpnpAbstractDevice::addService(const UpnpServiceDescription &newService)
{
    d->mDevice.services().push_back(newService);
    return d->mDevice.services().count() - 1;
}

#include "moc_upnpabstractdevice.cpp"
