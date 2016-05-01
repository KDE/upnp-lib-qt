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
    if (nameValue.isNull() || !nameValue.isString()) {
        result.mIsValid = false;
        return result;
    }
    result.mName = nameValue.toString();

    auto directionValue = getField(argument, QStringLiteral("direction"));
    if (directionValue.isNull() || !directionValue.isString()) {
        result.mIsValid = false;
        return result;
    }

    if (directionValue == QStringLiteral("in")) {
        result.mDirection = UpnpArgumentDirection::In;
    } else if (directionValue == QStringLiteral("out")) {
        result.mDirection = UpnpArgumentDirection::Out;
    } else {
        result.mDirection = UpnpArgumentDirection::Invalid;
        result.mIsValid = false;
        return result;
    }

    auto stateVariableValue = getField(argument, QStringLiteral("stateVariable"));
    if (stateVariableValue.isNull() || !stateVariableValue.isString()) {
        result.mIsValid = false;
        return result;
    }
    result.mRelatedStateVariable = stateVariableValue.toString();

    auto isReturnValueValue = getField(argument, QStringLiteral("isReturnValue"));
    if (isReturnValueValue.isNull() || !isReturnValueValue.isBool()) {
        result.mIsValid = false;
        return result;
    }
    result.mIsReturnValue = isReturnValueValue.toBool();

    result.mIsValid = true;

    return result;
}

UpnpStateVariableDescription variableDescriptionFromJson(const QJsonObject &variableDescription)
{
    UpnpStateVariableDescription result;

    auto nameValue = getField(variableDescription, QStringLiteral("name"));
    if (nameValue.isNull() || !nameValue.isString()) {
        result.mIsValid = false;
        return result;
    }
    result.mUpnpName = nameValue.toString();

    auto typeValue = getField(variableDescription, QStringLiteral("type"));
    if (typeValue.isNull() || !typeValue.isString()) {
        result.mIsValid = false;
        return result;
    }
    result.mDataType = typeValue.toString();

    result.mIsValid = true;

    return result;
}

UpnpActionDescription actionDescriptionFromJson(const QJsonObject &actionDescription)
{
    UpnpActionDescription result;

    auto nameValue = getField(actionDescription, QStringLiteral("name"));
    if (nameValue.isNull() || !nameValue.isString()) {
        result.mIsValid = false;
        return result;
    }
    result.mName = nameValue.toString();

    auto argumentsValue = getField(actionDescription, QStringLiteral("arguments"));
    const auto allArguments = argumentsValue.toArray();
    for (const auto &oneArgument: allArguments) {
        auto decodedArgument = actionArgumentDescriptionFromJson(oneArgument.toObject());
        if (!decodedArgument.mIsValid) {
            result.mIsValid = false;
            return result;
        }
        result.mArguments.push_back(decodedArgument);
    }

    result.mIsValid = true;

    return result;
}

UpnpServiceDescription *serviceDescriptionFromJson(const QJsonObject &serviceDescription)
{
    QScopedPointer<UpnpServiceDescription> result(new UpnpServiceDescription);

    auto serviceTypeValue = getField(serviceDescription, QStringLiteral("serviceType"));
    if (serviceTypeValue.isNull() || !serviceTypeValue.isString()) {
        return nullptr;
    }
    result->setServiceType(serviceTypeValue.toString());

    auto serviceIdValue = getField(serviceDescription, QStringLiteral("serviceId"));
    if (serviceIdValue.isNull() || !serviceIdValue.isString()) {
        return nullptr;
    }
    result->setServiceId(serviceIdValue.toString());

    auto variablesValue = getField(serviceDescription, QStringLiteral("variables"));
    const auto allVariables = variablesValue.toArray();
    for (const auto &oneVariable: allVariables) {
        const auto &newVariable = variableDescriptionFromJson(oneVariable.toObject());
        if (!newVariable.mIsValid) {
            return nullptr;
        }
        result->addStateVariable(newVariable);
    }

    auto actionsValue = getField(serviceDescription, QStringLiteral("actions"));
    const auto allActions = actionsValue.toArray();
    for (const auto &oneAction: allActions) {
        const auto &newAction = actionDescriptionFromJson(oneAction.toObject());
        if (!newAction.mIsValid) {
            return nullptr;
        }
        result->addAction(newAction);
    }

    return result.take();
}

QSharedPointer<UpnpDeviceDescription> deviceDescriptionFromJson(const QJsonObject &deviceDescription)
{
    QScopedPointer<UpnpDeviceDescription> result(new UpnpDeviceDescription);

    auto udnValue = getField(deviceDescription, QStringLiteral("UDN"));
    if (udnValue.isNull() || !udnValue.isString()) {
        return {};
    }
    result->setUDN(udnValue.toString());

    auto upcValue = getField(deviceDescription, QStringLiteral("UPC"));
    if (upcValue.isNull() || !upcValue.isString()) {
        return {};
    }
    result->setUPC(upcValue.toString());

    auto deviceTypeValue = getField(deviceDescription, QStringLiteral("deviceType"));
    if (deviceTypeValue.isNull() || !deviceTypeValue.isString()) {
        return {};
    }
    result->setDeviceType(deviceTypeValue.toString());

    auto friendlyNameValue = getField(deviceDescription, QStringLiteral("friendlyName"));
    if (friendlyNameValue.isNull() || !friendlyNameValue.isString()) {
        return {};
    }
    result->setFriendlyName(friendlyNameValue.toString());

    auto manufacturerValue = getField(deviceDescription, QStringLiteral("manufacturer"));
    if (manufacturerValue.isNull() || !manufacturerValue.isString()) {
        return {};
    }
    result->setManufacturer(manufacturerValue.toString());

    auto manufacturerURLValue = getField(deviceDescription, QStringLiteral("manufacturerURL"));
    if (manufacturerURLValue.isNull() || !manufacturerURLValue.isString()) {
        return {};
    }
    result->setManufacturerURL(QUrl::fromUserInput(manufacturerURLValue.toString()));

    auto modelDescriptionValue = getField(deviceDescription, QStringLiteral("modelDescription"));
    if (modelDescriptionValue.isNull() || !modelDescriptionValue.isString()) {
        return {};
    }
    result->setModelDescription(modelDescriptionValue.toString());

    auto modelNameValue = getField(deviceDescription, QStringLiteral("modelName"));
    if (modelNameValue.isNull() || !modelNameValue.isString()) {
        return {};
    }
    result->setModelName(modelNameValue.toString());

    auto modelNumberValue = getField(deviceDescription, QStringLiteral("modelNumber"));
    if (modelNumberValue.isNull() || !modelNumberValue.isString()) {
        return {};
    }
    result->setModelNumber(modelNumberValue.toString());

    auto modelURLValue = getField(deviceDescription, QStringLiteral("modelURL"));
    if (modelURLValue.isNull() || !modelURLValue.isString()) {
        return {};
    }
    result->setModelURL(QUrl::fromUserInput(modelURLValue.toString()));

    auto serialNumberValue = getField(deviceDescription, QStringLiteral("serialNumber"));
    if (serialNumberValue.isNull() || !serialNumberValue.isString()) {
        return {};
    }
    result->setSerialNumber(serialNumberValue.toString());

    auto servicesValue = getField(deviceDescription, QStringLiteral("services"));
    const auto allServices = servicesValue.toArray();
    for (const auto &oneService: allServices) {
        QScopedPointer<UpnpServiceDescription> newService(serviceDescriptionFromJson(oneService.toObject()));
        if (!newService) {
            return {};
        }
        result->addService(QSharedPointer<UpnpServiceDescription>(newService.take()));
    }

    return QSharedPointer<UpnpDeviceDescription>(result.take());
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
    default:
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

    for (const auto &argument : actionDescription.mArguments) {
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

    for (const auto &variable : serviceDescription->stateVariables()) {
        allVariables.append(variableDescriptionToJson(variable));
    }

    result.insert(QStringLiteral("variables"), allVariables);

    QJsonArray allActions;

    for (const auto &action : serviceDescription->actions()) {
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

    for (const auto &service : deviceDescription->services()) {
        allServices.append(serviceDescriptionToJson(service.data()));
    }

    result.insert(QStringLiteral("services"), allServices);

    return result;
}

}
