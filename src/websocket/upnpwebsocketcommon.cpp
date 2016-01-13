/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "upnpwebsocketcommon.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

namespace UpnpWebSocketProtocol
{

QJsonValue getField(const QJsonObject &data, const QString &fieldName)
{
    auto fieldKey = data.find(fieldName);
    if (fieldKey == data.end()) {
        return {};
    }

    if (fieldKey.value().isUndefined() || fieldKey.value().isNull()) {
        return {};
    }

    return fieldKey.value();
}

UpnpActionArgumentDescription actionArgumentDescriptionFromJson(const QJsonObject &argument)
{
    UpnpActionArgumentDescription result;

    auto nameValue = getField(argument, QStringLiteral("name"));
    result.mName = nameValue.toString();

    auto directionValue = getField(argument, QStringLiteral("direction"));
    if (directionValue == QStringLiteral("in")) {
        result.mDirection = UpnpArgumentDirection::In;
    }
    if (directionValue == QStringLiteral("out")) {
        result.mDirection = UpnpArgumentDirection::Out;
    }

    auto stateVariableValue = getField(argument, QStringLiteral("stateVariable"));
    result.mRelatedStateVariable = stateVariableValue.toString();

    auto isReturnValueValue = getField(argument, QStringLiteral("isReturnValue"));
    result.mIsReturnValue = isReturnValueValue.toBool();

    return result;
}

UpnpStateVariableDescription variableDescriptionFromJson(const QJsonObject &variableDescription)
{
    UpnpStateVariableDescription result;

    auto nameValue = getField(variableDescription, QStringLiteral("name"));
    result.mUpnpName = nameValue.toString();

    auto typeValue = getField(variableDescription, QStringLiteral("type"));
    result.mDataType = typeValue.toBool();

    return result;
}

UpnpActionDescription actionDescriptionFromJson(const QJsonObject &actionDescription)
{
    UpnpActionDescription result;

    auto nameValue = getField(actionDescription, QStringLiteral("name"));
    result.mName = nameValue.toString();

    auto argumentsValue = getField(actionDescription, QStringLiteral("arguments"));
    const auto allArguments = argumentsValue.toArray();
    for (auto oneArgument: allArguments) {
        result.mArguments.push_back(actionArgumentDescriptionFromJson(oneArgument.toObject()));
    }

    return result;
}

UpnpServiceDescription *serviceDescriptionFromJson(const QJsonObject &serviceDescription)
{
    QScopedPointer<UpnpServiceDescription> result(new UpnpServiceDescription);

    auto serviceTypeValue = getField(serviceDescription, QStringLiteral("serviceType"));
    result->setServiceType(serviceTypeValue.toString());

    auto serviceIdValue = getField(serviceDescription, QStringLiteral("serviceId"));
    result->setServiceId(serviceIdValue.toString());

    auto variablesValue = getField(serviceDescription, QStringLiteral("variables"));
    const auto allVariables = variablesValue.toArray();
    for (auto oneVariable: allVariables) {
        result->addStateVariable(variableDescriptionFromJson(oneVariable.toObject()));
    }

    auto actionsValue = getField(serviceDescription, QStringLiteral("actions"));
    const auto allActions = actionsValue.toArray();
    for (auto oneAction: allActions) {
        result->addAction(actionDescriptionFromJson(oneAction.toObject()));
    }

    return result.take();
}

UpnpDeviceDescription *deviceDescriptionFromJson(const QJsonObject &deviceDescription)
{
    QScopedPointer<UpnpDeviceDescription> result(new UpnpDeviceDescription);

    auto udnValue = getField(deviceDescription, QStringLiteral("UDN"));
    result->setUDN(udnValue.toString());

    auto upcValue = getField(deviceDescription, QStringLiteral("UPC"));
    result->setUPC(upcValue.toString());

    auto deviceTypeValue = getField(deviceDescription, QStringLiteral("deviceType"));
    result->setDeviceType(deviceTypeValue.toString());

    auto friendlyNameValue = getField(deviceDescription, QStringLiteral("friendlyName"));
    result->setFriendlyName(friendlyNameValue.toString());

    auto manufacturerValue = getField(deviceDescription, QStringLiteral("manufacturer"));
    result->setManufacturer(manufacturerValue.toString());

    auto manufacturerURLValue = getField(deviceDescription, QStringLiteral("manufacturerURL"));
    result->setManufacturerURL(QUrl::fromUserInput(manufacturerURLValue.toString()));

    auto modelDescriptionValue = getField(deviceDescription, QStringLiteral("modelDescription"));
    result->setModelDescription(modelDescriptionValue.toString());

    auto modelNameValue = getField(deviceDescription, QStringLiteral("modelName"));
    result->setModelName(modelNameValue.toString());

    auto modelNumberValue = getField(deviceDescription, QStringLiteral("modelNumber"));
    result->setModelNumber(modelNumberValue.toString());

    auto modelURLValue = getField(deviceDescription, QStringLiteral("modelURL"));
    result->setModelURL(QUrl::fromUserInput(modelURLValue.toString()));

    auto serialNumberValue = getField(deviceDescription, QStringLiteral("serialNumber"));
    result->setSerialNumber(serialNumberValue.toString());

    auto servicesValue = getField(deviceDescription, QStringLiteral("services"));
    const auto allServices = servicesValue.toArray();
    for (auto oneService: allServices) {
        result->addService(QSharedPointer<UpnpServiceDescription>(serviceDescriptionFromJson(oneService.toObject())));
    }

    return result.take();
}

QJsonObject actionArgumentDescriptionToJson(const UpnpActionArgumentDescription &argument)
{
    QJsonObject result;

    result.insert(QStringLiteral("name"), argument.mName);
    switch(argument.mDirection)
    {
    case UpnpArgumentDirection::In:
        result.insert(QStringLiteral("direction"), QStringLiteral("in"));
        break;
    case UpnpArgumentDirection::Out:
        result.insert(QStringLiteral("direction"), QStringLiteral("out"));
        break;
    }

    result.insert(QStringLiteral("stateVariable"), argument.mRelatedStateVariable);
    result.insert(QStringLiteral("isReturnValue"), argument.mIsReturnValue);

    return result;
}

QJsonObject variableDescriptionToJson(const UpnpStateVariableDescription &variableDescription)
{
    QJsonObject result;

    result.insert(QStringLiteral("name"), variableDescription.mUpnpName);
    result.insert(QStringLiteral("type"), variableDescription.mDataType);

    return result;
}

QJsonObject actionDescriptionToJson(const UpnpActionDescription &actionDescription)
{
    QJsonObject result;

    result.insert(QStringLiteral("name"), actionDescription.mName);

    QJsonArray allArguments;

    for (auto argument : actionDescription.mArguments) {
        allArguments.append(actionArgumentDescriptionToJson(argument));
    }

    result.insert(QStringLiteral("arguments"), allArguments);

    return result;
}

QJsonObject serviceDescriptionToJson(const UpnpServiceDescription *serviceDescription)
{
    QJsonObject result;

    result.insert(QStringLiteral("serviceType"), serviceDescription->serviceType());
    result.insert(QStringLiteral("serviceId"), serviceDescription->serviceId());

    QJsonArray allVariables;

    for (auto variable : serviceDescription->stateVariables()) {
        allVariables.append(variableDescriptionToJson(variable));
    }

    result.insert(QStringLiteral("variables"), allVariables);

    QJsonArray allActions;

    for (auto action : serviceDescription->actions()) {
        allActions.append(actionDescriptionToJson(action));
    }

    result.insert(QStringLiteral("actions"), allActions);

    return result;
}

QJsonObject deviceDescriptionToJson(const UpnpDeviceDescription *deviceDescription)
{
    QJsonObject result;

    result.insert(QStringLiteral("UDN"), deviceDescription->UDN());
    result.insert(QStringLiteral("UPC"), deviceDescription->UPC());
    result.insert(QStringLiteral("deviceType"), deviceDescription->deviceType());
    result.insert(QStringLiteral("friendlyName"), deviceDescription->friendlyName());
    result.insert(QStringLiteral("manufacturer"), deviceDescription->manufacturer());
    result.insert(QStringLiteral("manufacturerURL"), deviceDescription->manufacturerURL().toString());
    result.insert(QStringLiteral("modelDescription"), deviceDescription->modelDescription());
    result.insert(QStringLiteral("modelName"), deviceDescription->modelName());
    result.insert(QStringLiteral("modelNumber"), deviceDescription->modelNumber());
    result.insert(QStringLiteral("modelURL"), deviceDescription->modelURL().toString());
    result.insert(QStringLiteral("serialNumber"), deviceDescription->serialNumber());

    QJsonArray allServices;

    for (auto service : deviceDescription->services()) {
        allServices.append(serviceDescriptionToJson(service.data()));
    }

    result.insert(QStringLiteral("services"), allServices);

    return result;
}

}
