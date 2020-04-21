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
    UpnpControlAbstractDevicePrivate()
        : mDescription(nullptr)
    {
    }

    UpnpDeviceDescription *mDescription;
};

UpnpControlAbstractDevice::UpnpControlAbstractDevice(QObject *parent)
    : UpnpAbstractDevice(parent)
    , d(new UpnpControlAbstractDevicePrivate)
{
}

UpnpControlAbstractDevice::~UpnpControlAbstractDevice()
{
}

UpnpControlAbstractService *UpnpControlAbstractDevice::serviceById(const QString &serviceId) const
{
    auto serviceDescription = serviceDescriptionById(serviceId);
    return serviceFromDescription(serviceDescription);
}

UpnpControlAbstractService *UpnpControlAbstractDevice::serviceByIndex(int serviceIndex) const
{
    auto serviceDescription = serviceDescriptionByIndex(serviceIndex);
    return serviceFromDescription(serviceDescription);
}

UpnpControlAbstractService *UpnpControlAbstractDevice::serviceFromDescription(const UpnpServiceDescription &description) const
{
    UpnpControlAbstractService *newService = nullptr;

    newService = new UpnpControlAbstractService;

    newService->setDescription(description);
    return newService;
}

#include "moc_upnpcontrolabstractdevice.cpp"
