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

#include "upnpswitchpower.h"

#include "upnpbasictypes.h"

#include "upnpactiondescription.h"
#include "upnpstatevariabledescription.h"
#include "upnpservicedescription.h"

#include <QtCore/QDebug>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QSharedPointer>

class UpnpSwitchPowerPrivate
{
public:

    UpnpSwitchPowerPrivate() : mService(new UpnpServiceDescription)
    {
    }

    QSharedPointer<UpnpServiceDescription> mService;

    bool mTarget = false;

    bool mStatus = false;

};

UpnpSwitchPower::UpnpSwitchPower(QObject *parent) :
    QObject(parent), d(new UpnpSwitchPowerPrivate)
{
    //setBaseURL();
    description()->setServiceId(QStringLiteral("urn:upnp-org:serviceId:SwitchPower"));
    description()->setServiceType(QStringLiteral("urn:schemas-upnp-org:service:SwitchPower:1"));

    UpnpActionDescription setTargetAction;
    setTargetAction.mName = QStringLiteral("SetTarget");

    UpnpActionArgumentDescription setTargetNewValue;
    setTargetNewValue.mName = QStringLiteral("newTargetValue");
    setTargetNewValue.mIsReturnValue = false;
    setTargetNewValue.mDirection = UpnpArgumentDirection::In;
    setTargetNewValue.mRelatedStateVariable = QStringLiteral("Target");
    setTargetAction.mArguments.push_back(setTargetNewValue);
    setTargetAction.mNumberInArgument = 1;
    setTargetAction.mNumberOutArgument = 0;

    description()->addAction(setTargetAction);

    UpnpActionDescription getTargetAction;
    getTargetAction.mName = QStringLiteral("GetTarget");

    UpnpActionArgumentDescription getTargetRetTargetValue;
    getTargetRetTargetValue.mName = QStringLiteral("RetTargetValue");
    getTargetRetTargetValue.mIsReturnValue = false;
    getTargetRetTargetValue.mDirection = UpnpArgumentDirection::Out;
    getTargetRetTargetValue.mRelatedStateVariable = QStringLiteral("Target");
    getTargetAction.mArguments.push_back(getTargetRetTargetValue);
    getTargetAction.mNumberInArgument = 0;
    getTargetAction.mNumberOutArgument = 1;

    description()->addAction(getTargetAction);

    UpnpActionDescription getStatusAction;
    getStatusAction.mName = QStringLiteral("GetStatus");

    UpnpActionArgumentDescription getTargetResultStatus;
    getTargetResultStatus.mName = QStringLiteral("ResultStatus");
    getTargetResultStatus.mIsReturnValue = false;
    getTargetResultStatus.mDirection = UpnpArgumentDirection::Out;
    getTargetResultStatus.mRelatedStateVariable = QStringLiteral("Status");
    getStatusAction.mArguments.push_back(getTargetResultStatus);
    getStatusAction.mNumberInArgument = 0;
    getStatusAction.mNumberOutArgument = 1;

    description()->addAction(getStatusAction);

    UpnpStateVariableDescription targetStateVariable;
    targetStateVariable.mUpnpName = QStringLiteral("Target");
    targetStateVariable.mPropertyName = "target";
    targetStateVariable.mObject = this;

    for (int propertyIndex = 0; propertyIndex < metaObject()->propertyCount(); ++propertyIndex) {
        if (metaObject()->property(propertyIndex).name() == targetStateVariable.mPropertyName) {
            targetStateVariable.mPropertyIndex = propertyIndex;
            break;
        }
    }

    targetStateVariable.mEvented = false;
    targetStateVariable.mDataType = QStringLiteral("boolean");
    targetStateVariable.mDefaultValue = QStringLiteral("0");

    description()->addStateVariable(targetStateVariable);

    UpnpStateVariableDescription statusStateVariable;
    statusStateVariable.mUpnpName = QStringLiteral("Status");
    statusStateVariable.mPropertyName = "status";
    statusStateVariable.mObject = this;

    for (int propertyIndex = 0; propertyIndex < metaObject()->propertyCount(); ++propertyIndex) {
        if (metaObject()->property(propertyIndex).name() == statusStateVariable.mPropertyName) {
            statusStateVariable.mPropertyIndex = propertyIndex;
            break;
        }
    }

    statusStateVariable.mEvented = true;
    statusStateVariable.mDataType = QStringLiteral("boolean");
    statusStateVariable.mDefaultValue = QStringLiteral("0");

    description()->addStateVariable(statusStateVariable);
}

UpnpSwitchPower::~UpnpSwitchPower()
{
    delete d;
}

QSharedPointer<UpnpServiceDescription> UpnpSwitchPower::sharedDescription()
{
    return d->mService;
}

UpnpServiceDescription *UpnpSwitchPower::description()
{
    return d->mService.data();
}

const UpnpServiceDescription *UpnpSwitchPower::description() const
{
    return d->mService.data();
}

bool UpnpSwitchPower::target() const
{
    return d->mTarget;
}

bool UpnpSwitchPower::status() const
{
    return d->mStatus;
}

#include "moc_upnpswitchpower.cpp"
