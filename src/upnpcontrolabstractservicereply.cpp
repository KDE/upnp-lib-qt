/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    , d(std::make_unique<UpnpControlAbstractServiceReplyPrivate>(soapAnswer))
{
    connect(&d->mWatcher, &KDSoapPendingCallWatcher::finished, this, &UpnpControlAbstractServiceReply::callFinished);
}

UpnpControlAbstractServiceReply::~UpnpControlAbstractServiceReply() = default;

bool UpnpControlAbstractServiceReply::success() const
{
    return d->mAnswer.isFinished() && !d->mWatcher.returnMessage().isFault();
}

QVariantMap UpnpControlAbstractServiceReply::result() const
{
    return d->mResult;
}

QString UpnpControlAbstractServiceReply::error() const
{
    return d->mWatcher.returnMessage().faultAsString();
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
