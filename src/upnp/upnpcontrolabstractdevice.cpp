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
#include "upnpcontrolavtransport.h"
#include "upnpcontrolswitchpower.h"
#include "upnpcontrolconnectionmanager.h"
#include "upnpcontrolcontentdirectory.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QVariantMap>

class UpnpControlAbstractDevicePrivate
{
public:

    UpnpControlAbstractDevicePrivate()
        : mDescription(nullptr)
    {
    }

    UpnpDeviceDescription *mDescription;
};

UpnpControlAbstractDevice::UpnpControlAbstractDevice(QObject *parent) : UpnpAbstractDevice(parent), d(new UpnpControlAbstractDevicePrivate)
{
}

UpnpControlAbstractDevice::~UpnpControlAbstractDevice()
{
    delete d;
}

UpnpControlAbstractService* UpnpControlAbstractDevice::serviceById(const QString &serviceId) const
{
    auto serviceDescription = serviceDescriptionById(serviceId);
    return serviceFromDescription(serviceDescription);
}

UpnpControlAbstractService *UpnpControlAbstractDevice::serviceByIndex(int serviceIndex) const
{
    auto serviceDescription = serviceDescriptionByIndex(serviceIndex);
    return serviceFromDescription(serviceDescription);
}

UpnpControlAbstractService* UpnpControlAbstractDevice::serviceFromDescription(UpnpServiceDescription *description) const
{
    UpnpControlAbstractService *newService = nullptr;

    if (description->serviceType() == QStringLiteral("urn:schemas-upnp-org:service:AVTransport:1")) {
        newService = new UpnpControlAVTransport;
    } else if (description->serviceType() == QStringLiteral("urn:schemas-upnp-org:service:RenderingControl:1")) {
        newService = new UpnpControlSwitchPower;
    } else if (description->serviceType() == QStringLiteral("urn:schemas-upnp-org:service:SwitchPower:1")) {
        newService = new UpnpControlSwitchPower;
    } else if (description->serviceType() == QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1")) {
        newService = new UpnpControlConnectionManager;
    } else if (description->serviceType() == QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1")) {
        newService = new UpnpControlContentDirectory;
    } else {
        newService = new UpnpControlAbstractService;
    }

    newService->setDescription(description);
    return newService;
}

#include "moc_upnpcontrolabstractdevice.cpp"
