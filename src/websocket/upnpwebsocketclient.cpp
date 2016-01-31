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

#include "upnpwebsocketclient.h"

#include "upnpwebsocketcertificateconfiguration.h"

#include "upnpdevicedescription.h"

#include <QtWebSockets/QWebSocket>

#include <QtNetwork/QAuthenticator>

#include <QtCore/QPointer>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

class UpnpWebSocketClientPrivate
{
public:
    QMap<QString, QSharedPointer<UpnpDeviceDescription>> mAllDevices;
};

UpnpWebSocketClient::UpnpWebSocketClient(QObject *parent)
    : UpnpWebSocketBaseClient(parent), d(new UpnpWebSocketClientPrivate)
{
}

UpnpWebSocketClient::~UpnpWebSocketClient()
{
    delete d;
}

void UpnpWebSocketClient::askDeviceList()
{
    auto newObject = createMessage(UpnpWebSocketMessageType::AskDeviceList);

    sendMessage(newObject);
}

void UpnpWebSocketClient::subscribeService(const QString &udn, const QString serviceId)
{
    auto newObject = createMessage(UpnpWebSocketMessageType::SubscribeService);

    newObject.insert(QStringLiteral("udn"), udn);
    newObject.insert(QStringLiteral("serviceId"), serviceId);

    sendMessage(newObject);
}

QSharedPointer<UpnpDeviceDescription> UpnpWebSocketClient::device(const QString &udn) const
{
    auto deviceIterator = d->mAllDevices.find(udn);

    if (deviceIterator == d->mAllDevices.end()) {
        return {};
    }

    return *deviceIterator;
}

bool UpnpWebSocketClient::handleMessage(const QJsonObject &newMessage)
{
    bool messageHandled = UpnpWebSocketBaseClient::handleMessage(newMessage);

    if (messageHandled) {
        return true;
    }

    switch(getType(newMessage))
    {
    case UpnpWebSocketMessageType::HelloAck:
        handleHelloAck(newMessage);
        messageHandled = true;
        break;
    case UpnpWebSocketMessageType::DeviceList:
        handleServiceList(newMessage);
        messageHandled = true;
        break;
    case UpnpWebSocketMessageType::NewDevice:
        handleNewService(newMessage);
        messageHandled = true;
        break;
    case UpnpWebSocketMessageType::RemovedDevice:
        handleRemovedService(newMessage);
        messageHandled = true;
        break;
    case UpnpWebSocketMessageType::CallActionAck:
        handleCallActionAck(newMessage);
        messageHandled = true;
        break;
    default:
        qDebug() << "unknown message" << static_cast<int>(getType(newMessage));
        break;
    }

    return messageHandled;
}

void UpnpWebSocketClient::hasBeenDisconnected()
{
    for (auto oneDevice : d->mAllDevices) {
        qDebug() << "removed device" << oneDevice->UDN();
        Q_EMIT removedDevice(oneDevice->UDN());
    }
    d->mAllDevices.clear();
}

void UpnpWebSocketClient::handleHelloAck(QJsonObject aObject)
{
    Q_UNUSED(aObject);

    askDeviceList();
}

void UpnpWebSocketClient::handleServiceList(QJsonObject aObject)
{
    const auto &listDeviceValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("devices"));
    if (listDeviceValue.isNull() || !listDeviceValue.isArray()) {
        return;
    }

    const auto &listDeviceArray = listDeviceValue.toArray();
    for (auto oneDeviceValue : listDeviceArray) {
        if (oneDeviceValue.isNull() || !oneDeviceValue.isObject()) {
            return;
        }

        auto oneDevice = UpnpWebSocketProtocol::deviceDescriptionFromJson(oneDeviceValue.toObject());
        d->mAllDevices[oneDevice->UDN()] = oneDevice;
        qDebug() << "new device" << oneDevice->UDN();
        Q_EMIT newDevice(oneDevice->UDN());
    }
}

void UpnpWebSocketClient::handleNewService(QJsonObject aObject)
{
    const auto &newDeviceValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("device"));
    if (newDeviceValue.isNull() || !newDeviceValue.isObject()) {
        return;
    }

    auto oneNewDevice = UpnpWebSocketProtocol::deviceDescriptionFromJson(newDeviceValue.toObject());
    d->mAllDevices[oneNewDevice->UDN()] = oneNewDevice;
    qDebug() << "new device" << oneNewDevice->UDN();
    Q_EMIT newDevice(oneNewDevice->UDN());
}

void UpnpWebSocketClient::handleRemovedService(QJsonObject aObject)
{
    const auto &removedDeviceValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("device"));
    if (removedDeviceValue.isNull() || !removedDeviceValue.isObject()) {
        return;
    }

    auto oneRemovedDevice = UpnpWebSocketProtocol::deviceDescriptionFromJson(removedDeviceValue.toObject());

    auto removedDeviceIterator = d->mAllDevices.find(oneRemovedDevice->UDN());

    if (removedDeviceIterator == d->mAllDevices.end()) {
        return;
    }

    d->mAllDevices.erase(removedDeviceIterator);

    qDebug() << "removed device" << oneRemovedDevice->UDN();

    Q_EMIT removedDevice(oneRemovedDevice->UDN());
}

void UpnpWebSocketClient::handleCallActionAck(QJsonObject aObject)
{
    const auto &actionValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("action"));
    if (actionValue.isNull() || !actionValue.isString()) {
        return;
    }

    const auto &sequenceNumberValue = UpnpWebSocketProtocol::getField(aObject, QStringLiteral("sequenceNumber"));
    if (sequenceNumberValue.isNull() || !sequenceNumberValue.isDouble()) {
        return;
    }

    Q_EMIT actionAck(actionValue.toString(), qint64(sequenceNumberValue.toDouble()));
}


#include "moc_upnpwebsocketclient.cpp"
