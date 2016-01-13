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

#ifndef UPNPWEBSOCKETCOMMON_H
#define UPNPWEBSOCKETCOMMON_H

#include "upnpQtWebSocket_export.h"

#include "upnpactiondescription.h"
#include "upnpstatevariabledescription.h"

enum class UpnpWebSocketMessageType {
    Undefined = 0,
    Error,
    Hello,
    HelloAck,
    AskDeviceList,
    DeviceList,
    NewDevice,
    RemovedDevice,
    PublishDevice,
    DeviceIsPublished,
    DeviceIsNotPublished,
    UnpublishDevice,
    SubscribeService,
    ServiceIsSubscribed,
};

class UpnpServiceDescription;
class UpnpDeviceDescription;

namespace UpnpWebSocketProtocol
{

QJsonValue getField(const QJsonObject &data, const QString &fieldName);

UpnpActionArgumentDescription actionArgumentDescriptionFromJson(const QJsonObject &argument);

UpnpStateVariableDescription variableDescriptionFromJson(const QJsonObject &variableDescription);

UpnpActionDescription actionDescriptionFromJson(const QJsonObject &actionDescription);

UpnpServiceDescription *serviceDescriptionFromJson(const QJsonObject &serviceDescription);

QSharedPointer<UpnpDeviceDescription> deviceDescriptionFromJson(const QJsonObject &deviceDescription);

QJsonObject actionArgumentDescriptionToJson(const UpnpActionArgumentDescription &argument);

QJsonObject variableDescriptionToJson(const UpnpStateVariableDescription &variableDescription);

QJsonObject actionDescriptionToJson(const UpnpActionDescription &actionDescription);

QJsonObject serviceDescriptionToJson(const UpnpServiceDescription *serviceDescription);

QJsonObject deviceDescriptionToJson(const UpnpDeviceDescription *deviceDescription);

}

#endif // UPNPWEBSOCKETCOMMON_H

