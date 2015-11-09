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

#include "upnpcontroldimming.h"

#include <KDSoapClient/KDSoapPendingCall.h>
#include <KDSoapClient/KDSoapPendingCallWatcher.h>

#include <QtCore/QDebug>

class UpnpControlDimmingPrivate
{
public:

    bool mLoadLevelStatus;

    bool mLoadLevelTarget;
};

UpnpControlDimming::UpnpControlDimming(QObject *parent) : UpnpControlAbstractService(parent), d(new UpnpControlDimmingPrivate)
{
    d->mLoadLevelStatus = false;
    d->mLoadLevelTarget = false;
}

UpnpControlDimming::~UpnpControlDimming()
{
}

bool UpnpControlDimming::loadLevelStatus() const
{
    return d->mLoadLevelStatus;
}

bool UpnpControlDimming::loadLevelTarget() const
{
    return d->mLoadLevelTarget;
}

void UpnpControlDimming::setLoadLevelTarget(bool newTargetValue)
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("SetLoadLevelTarget"), {newTargetValue}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlDimming::finishedSetLoadLevelTargetCall);
}

void UpnpControlDimming::getLoadLevelTarget()
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetLoadLevelTarget"), {}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlDimming::finishedGetLoadLevelTargetCall);
}

void UpnpControlDimming::getLoadLevelStatus()
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetLoadLevelStatus"), {}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlDimming::finishedGetLoadLevelStatusCall);
}

void UpnpControlDimming::finishedSetLoadLevelTargetCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    Q_EMIT setLoadLevelTargetFinished(!self->returnMessage().isFault());
}

void UpnpControlDimming::finishedGetLoadLevelTargetCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    d->mLoadLevelTarget = self->returnValue().toBool();
    Q_EMIT getLoadLevelTargetFinished(!self->returnMessage().isFault(), self->returnValue().toBool());
}

void UpnpControlDimming::finishedGetLoadLevelStatusCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    Q_EMIT getLoadLevelStatusFinished(!self->returnMessage().isFault(), self->returnValue().toBool());

    d->mLoadLevelStatus = self->returnValue().toBool();
    Q_EMIT loadLevelStatusChanged();
}

void UpnpControlDimming::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    if (eventName == QStringLiteral("LoadLevelStatus")) {
        d->mLoadLevelStatus = (eventValue == QStringLiteral("1"));
        Q_EMIT loadLevelStatusChanged();
    }
}

#include "moc_upnpcontroldimming.cpp"
