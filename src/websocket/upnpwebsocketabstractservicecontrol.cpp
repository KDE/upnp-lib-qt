/*
 * Copyright 2015-2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "upnpwebsocketabstractservicecontrol.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"
#include "upnpactiondescription.h"
#include "upnpwebsocketclient.h"

#include <QtCore/QJsonArray>

class UpnpWebSocketAbstractDeviceControlPrivate
{
public:

    QSharedPointer<UpnpDeviceDescription> mDeviceDescription;

    QSharedPointer<UpnpServiceDescription> mServiceDescription;

    QString mServiceId;

    UpnpWebSocketClient *mWebSocketClient = nullptr;

    qint64 mSequenceNumber = 0;

};

UpnpWebSocketAbstractServiceControl::UpnpWebSocketAbstractServiceControl(QObject *parent)
    : QObject(parent), d(new UpnpWebSocketAbstractDeviceControlPrivate)
{
}

UpnpWebSocketAbstractServiceControl::~UpnpWebSocketAbstractServiceControl()
{
    delete d;
}

qint64 UpnpWebSocketAbstractServiceControl::callAction(const QString &action, const QList<QVariant> &arguments)
{
    if (!d->mWebSocketClient) {
        return INVALID_SEQUENCE_NUMBER;
    }
    if (!d->mServiceDescription) {
        return INVALID_SEQUENCE_NUMBER;
    }

    ++d->mSequenceNumber;

    auto newMessage = d->mWebSocketClient->createMessage(UpnpWebSocketMessageType::CallAction);
    newMessage.insert(QStringLiteral("action"), action);
    newMessage.insert(QStringLiteral("udn"), d->mDeviceDescription->UDN());
    newMessage.insert(QStringLiteral("serviceId"), d->mServiceDescription->serviceId());
    newMessage.insert(QStringLiteral("sequenceNumber"), d->mSequenceNumber);

    QJsonObject allArgs;
    int cptArg = 0;
    for (const auto &oneArg : d->mServiceDescription->action(action).mArguments) {
        if (oneArg.mDirection == UpnpArgumentDirection::In) {
            allArgs.insert(oneArg.mName, arguments.at(cptArg).toString());
            ++cptArg;
        }
    }
    newMessage.insert(QStringLiteral("arguments"), allArgs);

    d->mWebSocketClient->sendMessage(newMessage);

    return d->mSequenceNumber;
}

void UpnpWebSocketAbstractServiceControl::subscribeEvents()
{
    if (d->mServiceId.isEmpty()) {
        return;
    }
    if (!d->mWebSocketClient) {
        return;
    }

    if (!d->mServiceDescription) {
        return;
    }

    d->mWebSocketClient->subscribeService(d->mDeviceDescription->UDN(), d->mServiceId);
}

void UpnpWebSocketAbstractServiceControl::handleEventNotification()
{

}

void UpnpWebSocketAbstractServiceControl::setDeviceDescription(QSharedPointer<UpnpDeviceDescription> value)
{
    d->mDeviceDescription = value;
    Q_EMIT deviceDescriptionChanged();
    if (d->mDeviceDescription && !d->mServiceId.isEmpty()) {
        d->mServiceDescription = d->mDeviceDescription->serviceById(d->mServiceId);

        subscribeEvents();
    }
}

QSharedPointer<UpnpDeviceDescription> UpnpWebSocketAbstractServiceControl::deviceDescription() const
{
    return d->mDeviceDescription;
}

void UpnpWebSocketAbstractServiceControl::setServiceId(const QString &value)
{
    d->mServiceId = value;
    Q_EMIT serviceIdChanged();
    if (d->mDeviceDescription && !d->mServiceId.isEmpty()) {
        d->mServiceDescription = d->mDeviceDescription->serviceById(d->mServiceId);

        subscribeEvents();
    }
}

const QString &UpnpWebSocketAbstractServiceControl::serviceId() const
{
    return d->mServiceId;
}

void UpnpWebSocketAbstractServiceControl::setWebSocketClient(UpnpWebSocketClient *value)
{
    if (d->mWebSocketClient) {
        disconnect(d->mWebSocketClient, &UpnpWebSocketClient::actionAck, this, &UpnpWebSocketAbstractServiceControl::actionCompleted);
    }

    d->mWebSocketClient = value;
    Q_EMIT webSocketClientChanged();

    connect(d->mWebSocketClient, &UpnpWebSocketClient::actionAck, this, &UpnpWebSocketAbstractServiceControl::actionCompleted);
}

UpnpWebSocketClient *UpnpWebSocketAbstractServiceControl::webSocketClient() const
{
    return d->mWebSocketClient;
}

void UpnpWebSocketAbstractServiceControl::actionCompleted(const QString &action, qint64 sequenceNumber)
{
    qDebug() << "UpnpWebSocketAbstractDeviceControl::actionCompleted" << action << sequenceNumber;
}

void UpnpWebSocketAbstractServiceControl::parseEventNotification(const QString &eventName, const QString &eventValue)
{
    Q_UNUSED(eventName);
    Q_UNUSED(eventValue);
}


#include "moc_upnpwebsocketabstractservicecontrol.cpp"
