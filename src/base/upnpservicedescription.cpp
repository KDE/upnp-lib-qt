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

#include "upnpservicedescription.h"
#include "upnpactiondescription.h"
#include "upnpstatevariabledescription.h"

#include <QtCore/QPointer>
#include <QtCore/QBuffer>
#include <QtCore/QIODevice>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QTimer>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

#include <QtCore/QDebug>

class UpnpServiceDescriptionPrivate
{
public:

    UpnpServiceDescriptionPrivate()
        : mBaseURL(), mServiceType(), mServiceId(), mSCPDURL(), mControlURL(),
          mEventURL(), mXmlDescription(), mActions(), mStateVariables(),
          mSubscribers(), mMaximumSubscriptionDuration(3600)
    {
    }

    QString mBaseURL;

    QString mServiceType;

    QString mServiceId;

    QUrl mSCPDURL;

    QUrl mControlURL;

    QUrl mEventURL;

    QPointer<QIODevice> mXmlDescription;

    QMap<QString, UpnpActionDescription> mActions;

    QMap<QString, UpnpStateVariableDescription> mStateVariables;

    QList<QPointer<UpnpEventSubscriber> > mSubscribers;

    int mMaximumSubscriptionDuration;
};

UpnpServiceDescription::UpnpServiceDescription(QObject *parent) : d(new UpnpServiceDescriptionPrivate)
{
}

UpnpServiceDescription::~UpnpServiceDescription()
{
    delete d;
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

QMap<QString, UpnpActionDescription>& UpnpServiceDescription::actions()
{
    return d->mActions;
}

const QMap<QString, UpnpActionDescription>& UpnpServiceDescription::actions() const
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

#include "moc_upnpservicedescription.cpp"
