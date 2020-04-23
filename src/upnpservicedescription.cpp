/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpservicedescription.h"
#include "upnpactiondescription.h"
#include "upnpdevicedescription.h"
#include "upnpstatevariabledescription.h"

#include <QBuffer>
#include <QIODevice>
#include <QMetaObject>
#include <QMetaProperty>
#include <QPointer>
#include <QTimer>
#include <QXmlStreamWriter>

#include <QDebug>

class UpnpServiceDescriptionPrivate
{
public:
    QString mBaseURL;

    QString mServiceType;

    QString mServiceId;

    QUrl mSCPDURL;

    QUrl mControlURL;

    QUrl mEventURL;

    QPointer<QIODevice> mXmlDescription;

    QMap<QString, UpnpActionDescription> mActions;

    QMap<QString, UpnpStateVariableDescription> mStateVariables;

    QVector<QPointer<UpnpEventSubscriber>> mSubscribers;

    int mMaximumSubscriptionDuration = 3600;
};

UpnpServiceDescription::UpnpServiceDescription()
    : d(std::make_unique<UpnpServiceDescriptionPrivate>())
{
}

UpnpServiceDescription::UpnpServiceDescription(const UpnpServiceDescription &other)
    : d(std::make_unique<UpnpServiceDescriptionPrivate>(*other.d))
{
}

UpnpServiceDescription::UpnpServiceDescription(UpnpServiceDescription &&other) noexcept
    : d()
{
    d.swap(other.d);
}

UpnpServiceDescription::~UpnpServiceDescription() = default;

UpnpServiceDescription &UpnpServiceDescription::operator=(const UpnpServiceDescription &other)
{
    if (this != &other) {
        *d = *other.d;
    }

    return *this;
}

UpnpServiceDescription &UpnpServiceDescription::operator=(UpnpServiceDescription &&other) noexcept
{
    if (this != &other) {
        d.reset();
        d.swap(other.d);
    }

    return *this;
}

void UpnpServiceDescription::setBaseURL(const QString &newBaseURL)
{
    d->mBaseURL = newBaseURL;
}

const QString &UpnpServiceDescription::baseURL() const
{
    return d->mBaseURL;
}

void UpnpServiceDescription::setServiceType(const QString &newServiceType)
{
    d->mServiceType = newServiceType;
}

const QString &UpnpServiceDescription::serviceType() const
{
    return d->mServiceType;
}

void UpnpServiceDescription::setServiceId(const QString &newServiceId)
{
    d->mServiceId = newServiceId;
}

const QString &UpnpServiceDescription::serviceId() const
{
    return d->mServiceId;
}

void UpnpServiceDescription::setSCPDURL(const QUrl &newSCPDURL)
{
    d->mSCPDURL = newSCPDURL;
}

const QUrl &UpnpServiceDescription::SCPDURL() const
{
    return d->mSCPDURL;
}

void UpnpServiceDescription::setControlURL(const QUrl &newControlURL)
{
    d->mControlURL = newControlURL;
}

const QUrl &UpnpServiceDescription::controlURL() const
{
    return d->mControlURL;
}

void UpnpServiceDescription::setEventURL(const QUrl &newEventURL)
{
    d->mEventURL = newEventURL;
}

const QUrl &UpnpServiceDescription::eventURL() const
{
    return d->mEventURL;
}

void UpnpServiceDescription::setMaximumSubscriptionDuration(int newValue)
{
    d->mMaximumSubscriptionDuration = newValue;
}

int UpnpServiceDescription::maximumSubscriptionDuration() const
{
    return d->mMaximumSubscriptionDuration;
}

void UpnpServiceDescription::addAction(const UpnpActionDescription &newAction)
{
    d->mActions[newAction.mName] = newAction;
}

const UpnpActionDescription &UpnpServiceDescription::action(const QString &name) const
{
    return d->mActions[name];
}

QMap<QString, UpnpActionDescription> &UpnpServiceDescription::actions()
{
    return d->mActions;
}

const QMap<QString, UpnpActionDescription> &UpnpServiceDescription::actions() const
{
    return d->mActions;
}

void UpnpServiceDescription::addStateVariable(const UpnpStateVariableDescription &newVariable)
{
    d->mStateVariables[newVariable.mUpnpName] = newVariable;
}

const UpnpStateVariableDescription &UpnpServiceDescription::stateVariable(const QString &name) const
{
    return d->mStateVariables[name];
}

QMap<QString, UpnpStateVariableDescription> &UpnpServiceDescription::stateVariables()
{
    return d->mStateVariables;
}

const QMap<QString, UpnpStateVariableDescription> &UpnpServiceDescription::stateVariables() const
{
    return d->mStateVariables;
}
