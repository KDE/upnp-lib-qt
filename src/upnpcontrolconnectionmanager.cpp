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

#include "upnpcontrolconnectionmanager.h"

#include <KDSoapClient/KDSoapPendingCall.h>
#include <KDSoapClient/KDSoapPendingCallWatcher.h>

#include <QtCore/QDebug>

class UpnpControlConnectionManagerPrivate
{
public:

    QString mSourceProtocolInfo;

    QString mSinkProtocolInfo;

    QString mCurrentConnectionIDs;

    QString mConnectionStatus;

    QString mConnectionManager;

    QString mDirection;

    QString mProtocolInfo;

    int mConnectionID;

    int mAVTransportID;

    int mRcsID;

};

UpnpControlConnectionManager::UpnpControlConnectionManager(QObject *parent) : UpnpControlAbstractService(parent), d(new UpnpControlConnectionManagerPrivate)
{
}

UpnpControlConnectionManager::~UpnpControlConnectionManager()
{
}

const QString &UpnpControlConnectionManager::sourceProtocolInfo() const
{
    return d->mSourceProtocolInfo;
}

const QString &UpnpControlConnectionManager::sinkProtocolInfo() const
{
    return d->mSinkProtocolInfo;
}

const QString &UpnpControlConnectionManager::currentConnectionIDs() const
{
    return d->mCurrentConnectionIDs;
}

const QString &UpnpControlConnectionManager::connectionStatus() const
{
    return d->mConnectionStatus;
}

const QString &UpnpControlConnectionManager::connectionManager() const
{
    return d->mConnectionManager;
}

const QString &UpnpControlConnectionManager::direction() const
{
    return d->mDirection;
}

const QString &UpnpControlConnectionManager::protocolInfo() const
{
    return d->mProtocolInfo;
}

int UpnpControlConnectionManager::connectionID() const
{
    return d->mConnectionID;
}

int UpnpControlConnectionManager::AVTransportID() const
{
    return d->mAVTransportID;
}

int UpnpControlConnectionManager::rcsID() const
{
    return d->mRcsID;
}

void UpnpControlConnectionManager::getProtocolInfo()
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetProtocolInfo"), {}));

    KDSoapPendingCallWatcher *replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedGetProtocolInfoCall);
}

void UpnpControlConnectionManager::prepareForConnection(const QString &remoteProtocolInfo,
                                                        const QString &remoteUDN,
                                                        const QString &remoteServiceID,
                                                        int remotePeerConnectionID,
                                                        const QString &connectionDirection)
{
    const QString &remoteConnectionManager(remoteUDN + QStringLiteral("/") + remoteServiceID);

    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("PrepareForConnection"),
                                                      {remoteProtocolInfo, remoteConnectionManager, remotePeerConnectionID, connectionDirection}));

    KDSoapPendingCallWatcher *replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedPrepareForConnectionCall);
}

void UpnpControlConnectionManager::connectionComplete(int currentConnectionID)
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("ConnectionComplete"), {currentConnectionID}));

    KDSoapPendingCallWatcher *replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedConnectionCompleteCall);
}

void UpnpControlConnectionManager::getCurrentConnectionIDs()
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetCurrentConnectionIDs"), {}));

    KDSoapPendingCallWatcher *replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedGetCurrentConnectionIDsCall);
}

void UpnpControlConnectionManager::getCurrentConnectionInfo(int currentConnectionID)
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetCurrentConnectionInfo"), {currentConnectionID}));

    KDSoapPendingCallWatcher *replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedGetCurrentConnectionInfoCall);
}

void UpnpControlConnectionManager::finishedGetProtocolInfoCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qDebug() << self->returnValue();

    d->mSourceProtocolInfo = self->returnValue().toString();
    d->mSinkProtocolInfo;

    Q_EMIT getProtocolInfoFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedPrepareForConnectionCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qDebug() << self->returnValue();

    Q_EMIT prepareForConnectionFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedConnectionCompleteCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qDebug() << self->returnValue();

    Q_EMIT connectionCompleteFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedGetCurrentConnectionIDsCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qDebug() << self->returnValue();

    Q_EMIT getCurrentConnectionIDsFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedGetCurrentConnectionInfoCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    qDebug() << self->returnValue();

    Q_EMIT getCurrentConnectionInfoFinished(!self->returnMessage().isFault());
}

//void UpnpControlConnectionManager::finishedGetStatusCall(KDSoapPendingCallWatcher *self)
//{
//    self->deleteLater();

//    Q_EMIT getStatusFinished(!self->returnMessage().isFault(), self->returnValue().toBool());

//    d->mStatus = self->returnValue().toBool();
//    Q_EMIT statusChanged();
//}

void UpnpControlConnectionManager::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    if (eventName == QStringLiteral("SourceProtocolInfo")) {
        d->mSourceProtocolInfo = eventValue;
        Q_EMIT sourceProtocolInfoChanged(d->mSourceProtocolInfo);
    }
    if (eventName == QStringLiteral("SinkProtocolInfo")) {
        d->mSinkProtocolInfo = eventValue;
        Q_EMIT sinkProtocolInfoChanged(d->mSinkProtocolInfo);
    }
    if (eventName == QStringLiteral("CurrentConnectionIDs")) {
        d->mCurrentConnectionIDs = eventValue;
        Q_EMIT currentConnectionIDsChanged(d->mCurrentConnectionIDs);
    }
}

#include "moc_upnpcontrolconnectionmanager.cpp"
