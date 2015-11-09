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

class UpnpSwitchPowerPrivate
{
public:

    UpnpSwitchPowerPrivate() : mTarget(true), mStatus(true)
    {
    }

    bool mTarget;

    bool mStatus;
};

UpnpSwitchPower::UpnpSwitchPower(QObject *parent) :
    UpnpAbstractService(parent), d(new UpnpSwitchPowerPrivate)
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

    addAction(setTargetAction);

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

    addAction(getTargetAction);

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

    addAction(getStatusAction);

    UpnpStateVariableDescription targetStateVariable;
    targetStateVariable.mUpnpName = QStringLiteral("Target");
    targetStateVariable.mPropertyName = "target";

    for (int propertyIndex = 0; propertyIndex < metaObject()->propertyCount(); ++propertyIndex) {
        if (metaObject()->property(propertyIndex).name() == targetStateVariable.mPropertyName) {
            targetStateVariable.mPropertyIndex = propertyIndex;
            break;
        }
    }

    targetStateVariable.mEvented = false;
    targetStateVariable.mDataType = QStringLiteral("boolean");
    targetStateVariable.mDefaultValue = QStringLiteral("0");

    addStateVariable(targetStateVariable);

    UpnpStateVariableDescription statusStateVariable;
    statusStateVariable.mUpnpName = QStringLiteral("Status");
    statusStateVariable.mPropertyName = "status";

    for (int propertyIndex = 0; propertyIndex < metaObject()->propertyCount(); ++propertyIndex) {
        if (metaObject()->property(propertyIndex).name() == statusStateVariable.mPropertyName) {
            statusStateVariable.mPropertyIndex = propertyIndex;
            break;
        }
    }

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
    Q_EMIT statusChanged(description()->serviceId(), "status");
}

bool UpnpSwitchPower::status() const
{
    return d->mStatus;
}

QList<QPair<QString, QVariant> > UpnpSwitchPower::invokeAction(const QString &actionName, const QList<QVariant> &arguments, bool &isInError)
{
    if (actionName == QStringLiteral("GetStatus")) {
        const QList<QPair<QString, QVariant> > &returnValues(getStatusAction());

        isInError = false;

        return returnValues;
    }

    if (actionName == QStringLiteral("SetTarget")) {
        if (!arguments.first().canConvert<bool>()) {
            qDebug() << "invalid type for argument";
        }

        const QList<QPair<QString, QVariant> > &returnValues(setTargetAction(arguments.first().toBool()));

        isInError = false;

        return returnValues;
    }

    if (actionName == QStringLiteral("GetTarget")) {
        const QList<QPair<QString, QVariant> > &returnValues(getTargetAction());

        isInError = false;

        return returnValues;
    }

    isInError = true;
    return {};
}

void UpnpSwitchPower::switchTarget()
{
    setTargetAction(!target());
}

QList<QPair<QString, QVariant> > UpnpSwitchPower::setTargetAction(bool newValue)
{
    qDebug() << "call setTargetAction";
    d->mStatus = newValue;
    d->mTarget = newValue;

    Q_EMIT statusChanged(description()->serviceId(), "status");

    qDebug() << "call setTargetAction" << d->mStatus;

    return {};
}

QList<QPair<QString, QVariant> > UpnpSwitchPower::getTargetAction()
{
    qDebug() << "call getTargetAction";

    return {{QStringLiteral("RetTargetValue"), d->mTarget}};
}

QList<QPair<QString, QVariant> > UpnpSwitchPower::getStatusAction()
{
    qDebug() << "call getStatusAction";

    return {{QStringLiteral("ResultStatus"), d->mStatus}};
}

#include "moc_upnpswitchpower.cpp"
