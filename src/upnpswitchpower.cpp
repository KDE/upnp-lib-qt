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

class UpnpSwitchPowerPrivate
{
public:

    bool mTarget;

    bool mStatus;
};

UpnpSwitchPower::UpnpSwitchPower(QObject *parent) :
    UpnpAbstractService(parent), d(new UpnpSwitchPowerPrivate)
{
    //setBaseURL();
    setServiceId(QStringLiteral("urn:upnp-org:serviceId:SwitchPower"));
    setServiceType(QStringLiteral("urn:schemas-upnp-org:service:SwitchPower:1"));

    UpnpActionDescription setTargetAction;
    setTargetAction.mName = QStringLiteral("SetTarget");

    UpnpActionArgumentDescription setTargetNewValue;
    setTargetNewValue.mName = QStringLiteral("newTargetValue");
    setTargetNewValue.mIsReturnValue = false;
    setTargetNewValue.mDirection = UpnpArgumentDirection::In;
    setTargetNewValue.mRelatedStateVariable = QStringLiteral("Target");
    setTargetAction.mArguments.push_back(setTargetNewValue);

    addAction(setTargetAction);

    UpnpActionDescription getTargetAction;
    getTargetAction.mName = QStringLiteral("GetTarget");

    UpnpActionArgumentDescription getTargetRetTargetValue;
    getTargetRetTargetValue.mName = QStringLiteral("RetTargetValue");
    getTargetRetTargetValue.mIsReturnValue = false;
    getTargetRetTargetValue.mDirection = UpnpArgumentDirection::Out;
    getTargetRetTargetValue.mRelatedStateVariable = QStringLiteral("Target");
    getTargetAction.mArguments.push_back(getTargetRetTargetValue);

    addAction(getTargetAction);

    UpnpActionDescription getStatusAction;
    getStatusAction.mName = QStringLiteral("GetStatus");

    UpnpActionArgumentDescription getTargetResultStatus;
    getTargetResultStatus.mName = QStringLiteral("ResultStatus");
    getTargetResultStatus.mIsReturnValue = false;
    getTargetResultStatus.mDirection = UpnpArgumentDirection::Out;
    getTargetResultStatus.mRelatedStateVariable = QStringLiteral("Status");
    getStatusAction.mArguments.push_back(getTargetResultStatus);

    addAction(getStatusAction);

    UpnpStateVariableDescription targetStateVariable;
    targetStateVariable.mName = QStringLiteral("Target");
    targetStateVariable.mEvented = false;
    targetStateVariable.mDataType = QStringLiteral("boolean");
    targetStateVariable.mDefaultValue = QStringLiteral("0");

    addStateVariable(targetStateVariable);

    UpnpStateVariableDescription statusStateVariable;
    statusStateVariable.mName = QStringLiteral("Status");
    statusStateVariable.mEvented = true;
    statusStateVariable.mDataType = QStringLiteral("boolean");
    statusStateVariable.mDefaultValue = QStringLiteral("0");

    addStateVariable(statusStateVariable);
}

UpnpSwitchPower::~UpnpSwitchPower()
{
    delete d;
}

void UpnpSwitchPower::setTarget(bool value)
{
    d->mTarget = value;
}

bool UpnpSwitchPower::target() const
{
    return d->mTarget;
}

void UpnpSwitchPower::setStatus(bool value)
{
    d->mStatus = value;
    Q_EMIT statusChanged(serviceId());
}

bool UpnpSwitchPower::status() const
{
    return d->mStatus;
}

#include "moc_upnpswitchpower.cpp"
