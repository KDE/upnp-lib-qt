/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpcontrolabstractdevice.h"

#include "upnpcontrolabstractservice.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QDebug>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QVariantMap>

class UpnpControlAbstractDevicePrivate
{
public:
};

UpnpControlAbstractDevice::UpnpControlAbstractDevice(QObject *parent)
    : UpnpAbstractDevice(parent)
    , d(std::make_unique<UpnpControlAbstractDevicePrivate>())
{
}

UpnpControlAbstractDevice::~UpnpControlAbstractDevice() = default;

std::unique_ptr<UpnpControlAbstractService> UpnpControlAbstractDevice::serviceById(const QString &serviceId) const
{
    auto serviceDescription = serviceDescriptionById(serviceId);
    return serviceFromDescription(serviceDescription);
}

std::unique_ptr<UpnpControlAbstractService> UpnpControlAbstractDevice::serviceByIndex(int serviceIndex) const
{
    auto serviceDescription = serviceDescriptionByIndex(serviceIndex);
    return serviceFromDescription(serviceDescription);
}

std::unique_ptr<UpnpControlAbstractService> UpnpControlAbstractDevice::serviceFromDescription(const UpnpServiceDescription &description) const
{
    auto newService = std::make_unique<UpnpControlAbstractService>();

    newService->setDescription(description);

    return newService;
}

#include "moc_upnpcontrolabstractdevice.cpp"
