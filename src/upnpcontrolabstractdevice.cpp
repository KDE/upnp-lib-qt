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
