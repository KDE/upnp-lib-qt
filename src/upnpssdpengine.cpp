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

#include "upnpssdpengine.h"

#include "upnpabstractdevice.h"
#include "upnpabstractservice.h"

#include <QtNetwork/QHostAddress>

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QSet>
#include <QtCore/QUrl>

#include <QtNetwork/QUdpSocket>
#include <QtCore/QSysInfo>

#include <sys/types.h>          /* Consultez NOTES */
#include <sys/socket.h>

enum class SsdpMessageType
{
    query,
    queryAnswer,
    announce,
};

class UpnpSsdpEnginePrivate
{
public:

    QHash<QString, QMap<QString, UpnpDiscoveryResult> > mDiscoveryResults;

    QUdpSocket mSsdpQuerySocket;

    QUdpSocket mSsdpStandardSocket;

    QString mServerInformation;
};

UpnpSsdpEngine::UpnpSsdpEngine(QObject *parent)
    : QObject(parent), d(new UpnpSsdpEnginePrivate)
{
    connect(&d->mSsdpQuerySocket, &QUdpSocket::readyRead, this, &UpnpSsdpEngine::queryReceivedData);
    connect(&d->mSsdpStandardSocket, &QUdpSocket::readyRead, this, &UpnpSsdpEngine::standardReceivedData);

    d->mSsdpStandardSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), 1900, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);

    int reuse = 1;
    if (setsockopt(d->mSsdpStandardSocket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        qDebug() << "setsockopt() failed" << errno;
    }

    if (setsockopt(d->mSsdpStandardSocket.socketDescriptor(), SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
        qDebug() << "setsockopt() 2 failed" << errno;
    }

    int bindResult = d->mSsdpStandardSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), 1900, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    if (!bindResult) {
        qDebug() << "bind failed" << QHostAddress(QStringLiteral("239.255.255.250")) << d->mSsdpStandardSocket.error() << d->mSsdpStandardSocket.errorString();
    }

    d->mSsdpQuerySocket.bind(QHostAddress::Any);

    d->mServerInformation = QSysInfo::kernelType() + QStringLiteral(" ") + QSysInfo::kernelVersion()  + QStringLiteral(" UPnP/1.0 ");
}

UpnpSsdpEngine::~UpnpSsdpEngine()
{
    delete d;
}

bool UpnpSsdpEngine::searchAllUpnpDevice()
{
    QByteArray allDiscoveryMessage;

    allDiscoveryMessage += "M-SEARCH * HTTP/1.1\r\n";
    allDiscoveryMessage += "HOST: 239.255.255.250:1900\r\n";
    allDiscoveryMessage += "MAN: \"ssdp:discover\"\r\n";
    allDiscoveryMessage += "MX: 1\r\n";
    allDiscoveryMessage += "ST: ssdp:all\r\n\r\n";

    auto result = d->mSsdpQuerySocket.writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), 1900);

    return result != -1;
}

void UpnpSsdpEngine::publishDevice(UpnpAbstractDevice *device)
{
    QByteArray allDiscoveryMessageCommonContent;

    allDiscoveryMessageCommonContent += "NOTIFY * HTTP/1.1\r\n";
    allDiscoveryMessageCommonContent += "HOST: 239.255.255.250:1900\r\n";
    allDiscoveryMessageCommonContent += "CACHE-CONTROL: max-age=" + QByteArray::number(device->cacheControl()) + "\r\n";
    allDiscoveryMessageCommonContent += "NTS: ssdp:alive\r\n";
    allDiscoveryMessageCommonContent += "SERVER: " + d->mServerInformation.toLatin1() + " " + device->modelName().toLatin1() + " " + device->modelNumber().toLatin1() + "\r\n";

    QByteArray rootDeviceMessage(allDiscoveryMessageCommonContent);
    rootDeviceMessage += "NT: upnp:rootdevice\r\n";
    rootDeviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "::upnp:rootdevice\r\n";
    rootDeviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
    rootDeviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(rootDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), 1900);

    QByteArray uuidDeviceMessage(allDiscoveryMessageCommonContent);
    uuidDeviceMessage += "NT: uuid:" + device->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
    uuidDeviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(uuidDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), 1900);

    QByteArray deviceMessage(allDiscoveryMessageCommonContent);
    deviceMessage += "NT: " + device->deviceType().toLatin1() + "\r\n";
    deviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "::" + device->deviceType().toLatin1() + "\r\n";
    deviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
    deviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), 1900);

    const QList<QPointer<UpnpAbstractService> > &servicesList = device->services();
    for (auto itService = servicesList.begin(); itService != servicesList.end(); ++itService) {
        QByteArray deviceMessage(allDiscoveryMessageCommonContent);
        deviceMessage += "NT: " + (*itService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "::" + (*itService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
        deviceMessage += "\r\n";

        d->mSsdpQuerySocket.writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), 1900);
    }
}

void UpnpSsdpEngine::standardReceivedData()
{
    while (d->mSsdpStandardSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(d->mSsdpStandardSocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        d->mSsdpStandardSocket.readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        parseSsdpDatagram(datagram);
    }
}

void UpnpSsdpEngine::queryReceivedData()
{
    while (d->mSsdpQuerySocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(d->mSsdpQuerySocket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        d->mSsdpQuerySocket.readDatagram(datagram.data(), datagram.size(),
                                &sender, &senderPort);

        parseSsdpDatagram(datagram);
    }
}

void UpnpSsdpEngine::parseSsdpDatagram(const QByteArray &datagram)
{
    const QList<QByteArray> &headers(datagram.split('\n'));

    if (!headers.last().isEmpty()) {
        return;
    }

    SsdpMessageType messageType;

    if (headers[0].startsWith("M-SEARCH * HTTP/1.1")) {
        messageType = SsdpMessageType::query;
    }
    if (headers[0].startsWith("HTTP/1.1 200 OK\r")) {
        messageType = SsdpMessageType::queryAnswer;
    }
    if (headers[0].startsWith("NOTIFY * HTTP/1.1")) {
        messageType = SsdpMessageType::announce;
    }

    UpnpDiscoveryResult newDiscovery;
    newDiscovery.mNTS = NotificationSubType::Invalid;

    for (QList<QByteArray>::const_iterator itLine = headers.begin(); itLine != headers.end(); ++itLine) {
        if (itLine->startsWith("LOCATION")) {
            if ((*itLine)[9] == ' ') {
                newDiscovery.mLocation = QString::fromLatin1(itLine->mid(10, itLine->length() - 11));
            } else {
                newDiscovery.mLocation = QString::fromLatin1(itLine->mid(9, itLine->length() - 10));
            }
        }
        if (itLine->startsWith("Location")) {
            if ((*itLine)[9] == ' ') {
                newDiscovery.mLocation = QString::fromLatin1(itLine->mid(10, itLine->length() - 11));
            } else {
                newDiscovery.mLocation = QString::fromLatin1(itLine->mid(9, itLine->length() - 10));
            }
        }
        if (itLine->startsWith("USN")) {
            if ((*itLine)[4] == ' ') {
                newDiscovery.mUSN = QString::fromLatin1(itLine->mid(5, itLine->length() - 6));
            } else {
                newDiscovery.mUSN = QString::fromLatin1(itLine->mid(4, itLine->length() - 5));
            }
        }
        if (messageType == SsdpMessageType::queryAnswer && itLine->startsWith("ST")) {
            if ((*itLine)[3] == ' ') {
                newDiscovery.mNT = QString::fromLatin1(itLine->mid(4, itLine->length() - 5));
            } else {
                newDiscovery.mNT = QString::fromLatin1(itLine->mid(3, itLine->length() - 4));
            }
        }
        if (messageType == SsdpMessageType::announce && itLine->startsWith("NT:")) {
            if ((*itLine)[3] == ' ') {
                newDiscovery.mNT = QString::fromLatin1(itLine->mid(4, itLine->length() - 5));
            } else {
                newDiscovery.mNT = QString::fromLatin1(itLine->mid(3, itLine->length() - 4));
            }
        }
        if (messageType == SsdpMessageType::announce && itLine->startsWith("NTS")) {
            if (itLine->endsWith("ssdp:alive\r")) {
                newDiscovery.mNTS = NotificationSubType::Alive;
            }
            if (itLine->endsWith("ssdp:byebye\r")) {
                newDiscovery.mNTS = NotificationSubType::ByeBye;
            }
            if (itLine->endsWith("ssdp:discover\r")) {
                newDiscovery.mNTS = NotificationSubType::Discover;
            }
        }
        if (itLine->startsWith("DATE")) {
            if ((*itLine)[5] == ' ') {
                newDiscovery.mAnnounceDate = QString::fromLatin1(itLine->mid(6, itLine->length() - 7));
            } else {
                newDiscovery.mAnnounceDate = QString::fromLatin1(itLine->mid(5, itLine->length() - 6));
            }
        }
        if (itLine->startsWith("CACHE-CONTROL")) {
            if ((*itLine)[14] == ' ') {
                const QList<QByteArray> &splittedLine = itLine->mid(15, itLine->length() - 16).split('=');
                if (splittedLine.size() == 2) {
                    newDiscovery.mCacheDuration = splittedLine.last().toInt();
                }
            } else {
                const QList<QByteArray> &splittedLine = itLine->mid(14, itLine->length() - 15).split('=');
                if (splittedLine.size() == 2) {
                    newDiscovery.mCacheDuration = splittedLine.last().toInt();
                }
            }
        }
    }

    if (newDiscovery.mLocation.isEmpty() || newDiscovery.mUSN.isEmpty() ||
            newDiscovery.mNT.isEmpty() ||
            (messageType == SsdpMessageType::announce && newDiscovery.mNTS == NotificationSubType::Invalid)) {
        qDebug() << "not decoded" << datagram;
        return;
    }

    if (newDiscovery.mNTS == NotificationSubType::Alive || messageType == SsdpMessageType::queryAnswer) {
        bool shouldSendSignal = false;

        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery.mUSN);

        if (newDiscovery.mNT == QStringLiteral("upnp:rootdevice")) {
            if (itDiscovery == d->mDiscoveryResults.end()) {
                shouldSendSignal = true;
            }
        }

        if (itDiscovery == d->mDiscoveryResults.end()) {
            d->mDiscoveryResults[newDiscovery.mUSN];
            itDiscovery = d->mDiscoveryResults.find(newDiscovery.mUSN);
        }

        auto itAnnounceUuid = itDiscovery->find(newDiscovery.mNT);
        if (itAnnounceUuid == itDiscovery->end()) {
#if 0
            qDebug() << datagram;
            qDebug() << "AnnounceDate" << newDiscovery.mAnnounceDate;
            qDebug() << "CacheDuration" << newDiscovery.mCacheDuration;

            qDebug() << "new service";
            qDebug() << "DeviceId:" << newDiscovery.mUSN;
            qDebug() << "DeviceType:" << newDiscovery.mNT;
            qDebug() << "Location:" << newDiscovery.mLocation;
            qDebug() << "new service";
            qDebug() << "DeviceId:" << searchResult->DeviceId;
            qDebug() << "DeviceType:" << searchResult->DeviceType;
            qDebug() << "ServiceType:" << searchResult->ServiceType;
            qDebug() << "ServiceVer:" << searchResult->ServiceVer;
            qDebug() << "Os:" << searchResult->Os;
            qDebug() << "Date:" << searchResult->Date;
            qDebug() << "Ext:" << searchResult->Ext;
            qDebug() << "ErrCode:" << searchResult->ErrCode;
            qDebug() << "Expires:" << searchResult->Expires;
            qDebug() << "DestAddr:" << QHostAddress(reinterpret_cast<const sockaddr *>(&searchResult->DestAddr));
    #endif
        }

        (*itDiscovery)[newDiscovery.mNT] = newDiscovery;

        if (shouldSendSignal) {
            Q_EMIT newService(newDiscovery);
        }
    }

    if (newDiscovery.mNTS == NotificationSubType::ByeBye) {
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery.mUSN);
        if (itDiscovery != d->mDiscoveryResults.end()) {
            Q_EMIT removedService(newDiscovery);

            qDebug() << "bye bye" << newDiscovery.mUSN;

            d->mDiscoveryResults.erase(itDiscovery);
        }
    }
}

#include "moc_upnpssdpengine.cpp"
