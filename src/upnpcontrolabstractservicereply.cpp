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

#include "upnpcontrolabstractservicereply.h"

#include <KDSoapClient/KDSoapPendingCall>
#include <KDSoapClient/KDSoapPendingCallWatcher>

class UpnpControlAbstractServiceReplyPrivate
{
public:
    explicit UpnpControlAbstractServiceReplyPrivate(const KDSoapPendingCall &soapAnswer)
        : mAnswer(soapAnswer)
        , mWatcher(mAnswer)
    {
    }

    KDSoapPendingCall mAnswer;

    KDSoapPendingCallWatcher mWatcher;

    QVariantMap mResult;
};

UpnpControlAbstractServiceReply::UpnpControlAbstractServiceReply(const KDSoapPendingCall &soapAnswer, QObject *parent)
    : QObject(parent)
    , d(new UpnpControlAbstractServiceReplyPrivate(soapAnswer))
{
    connect(&d->mWatcher, &KDSoapPendingCallWatcher::finished, this, &UpnpControlAbstractServiceReply::callFinished);
}

UpnpControlAbstractServiceReply::~UpnpControlAbstractServiceReply()
{
}

bool UpnpControlAbstractServiceReply::success() const
{
    return d->mAnswer.isFinished() && !d->mWatcher.returnMessage().isFault();
}

QVariantMap UpnpControlAbstractServiceReply::result() const
{
    return d->mResult;
}

void UpnpControlAbstractServiceReply::callFinished()
{
    parseAnswer();

    Q_EMIT finished(this);
}

void UpnpControlAbstractServiceReply::parseAnswer()
{
    if (!d->mAnswer.isFinished()) {
        return;
    }

    const auto &returnedValues = d->mAnswer.returnMessage().childValues();

    for (const KDSoapValue &oneValue : returnedValues) {
        d->mResult[oneValue.name()] = oneValue.value();
    }
}

#include "moc_upnpcontrolabstractservicereply.cpp"
