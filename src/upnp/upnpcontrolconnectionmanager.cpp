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

    bool mHasPrepareForConnection;

    bool mHasConnectionComplete;

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

bool UpnpControlConnectionManager::hasPrepareForConnection() const
{
    return d->mHasPrepareForConnection;
}

bool UpnpControlConnectionManager::hasConnectionComplete() const
{
    return d->mHasConnectionComplete;
}

void UpnpControlConnectionManager::getProtocolInfo()
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetProtocolInfo"), {}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

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

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedPrepareForConnectionCall);
}

void UpnpControlConnectionManager::connectionComplete(int currentConnectionID)
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("ConnectionComplete"), {currentConnectionID}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedConnectionCompleteCall);
}

void UpnpControlConnectionManager::getCurrentConnectionIDs()
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetCurrentConnectionIDs"), {}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedGetCurrentConnectionIDsCall);
}

void UpnpControlConnectionManager::getCurrentConnectionInfo(int currentConnectionID)
{
    const KDSoapPendingCall &pendingAnswer(callAction(QStringLiteral("GetCurrentConnectionInfo"), {currentConnectionID}));

    auto replyHandler = new KDSoapPendingCallWatcher(pendingAnswer, this);

    connect(replyHandler, &KDSoapPendingCallWatcher::finished, this, &UpnpControlConnectionManager::finishedGetCurrentConnectionInfoCall);
}

void UpnpControlConnectionManager::finishedGetProtocolInfoCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QStringLiteral("Source")) {
            d->mSourceProtocolInfo = oneValue.value().toString();
        }
        if (oneValue.name() == QStringLiteral("Sink")) {
            d->mSinkProtocolInfo = oneValue.value().toString();
        }
    }

    qDebug() << "SourceProtocolInfo:" << d->mSourceProtocolInfo;
    qDebug() << "SinkProtocolInfo:" << d->mSinkProtocolInfo;

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
    d->mCurrentConnectionIDs = self->returnValue().toString();
    Q_EMIT currentConnectionIDsChanged(d->mCurrentConnectionIDs);

    Q_EMIT getCurrentConnectionIDsFinished(!self->returnMessage().isFault());
}

void UpnpControlConnectionManager::finishedGetCurrentConnectionInfoCall(KDSoapPendingCallWatcher *self)
{
    self->deleteLater();

    auto answer = self->returnMessage();
    auto allValues = answer.childValues();
    int rcsID = -1;
    int avTransportID = -1;
    QString protocolInfo;
    QString connectionManager;
    int peerConnectionID = -1;
    QString direction;
    QString connectionStatus;

    for (KDSoapValue oneValue : allValues) {
        if (oneValue.name() == QStringLiteral("RcsID")) {
            rcsID = oneValue.value().toInt();
        }
        if (oneValue.name() == QStringLiteral("AVTransportID")) {
            avTransportID = oneValue.value().toInt();
        }
        if (oneValue.name() == QStringLiteral("protocolInfo")) {
            protocolInfo = oneValue.value().toInt();
        }
        if (oneValue.name() == QStringLiteral("PeerConnectionManager")) {
            connectionManager = oneValue.value().toInt();
        }
        if (oneValue.name() == QStringLiteral("PeerConnectionID")) {
            peerConnectionID = oneValue.value().toInt();
        }
        if (oneValue.name() == QStringLiteral("Direction")) {
            direction = oneValue.value().toInt();
        }
        if (oneValue.name() == QStringLiteral("Status")) {
            connectionStatus = oneValue.value().toInt();
        }
    }

    qDebug() << "RcsID:" << rcsID;
    qDebug() << "AVTransportID:" << avTransportID;
    qDebug() << "protocolInfo:" << protocolInfo;
    qDebug() << "PeerConnectionManager:" << connectionManager;
    qDebug() << "PeerConnectionID:" << peerConnectionID;
    qDebug() << "Direction:" << direction;
    qDebug() << "Status:" << connectionStatus;

    Q_EMIT getCurrentConnectionInfoFinished(rcsID, avTransportID, protocolInfo, connectionManager, peerConnectionID,
                                            direction, connectionStatus, !self->returnMessage().isFault());
}

void UpnpControlConnectionManager::parseServiceDescription(QIODevice *serviceDescriptionContent)
{
    UpnpControlAbstractService::parseServiceDescription(serviceDescriptionContent);

    const QList<QString> &allActions(actions());

    d->mHasPrepareForConnection = allActions.contains(QStringLiteral("PrepareForConnection"));
    Q_EMIT hasPrepareForConnectionChanged();

    d->mHasConnectionComplete = allActions.contains(QStringLiteral("ConnectionComplete"));
    Q_EMIT hasConnectionCompleteChanged();
}

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
