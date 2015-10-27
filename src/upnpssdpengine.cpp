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

#include <sys/types.h>
#include <sys/socket.h>

class UpnpSsdpEnginePrivate
{
public:

    UpnpSsdpEnginePrivate()
        : mPortNumber(1900), mCanExportServices(true), mDiscoveryResults(),
          mSsdpQuerySocket(), mSsdpStandardSocket(), mServerInformation()
    {
    }

    quint16 mPortNumber;

    bool mCanExportServices;

    QHash<QString, QMap<QString, UpnpDiscoveryResult> > mDiscoveryResults;

    QUdpSocket mSsdpQuerySocket;

    QUdpSocket mSsdpStandardSocket;

    QString mServerInformation;
};

UpnpSsdpEngine::UpnpSsdpEngine(QObject *parent)
    : QObject(parent), d(new UpnpSsdpEnginePrivate)
{
}

void UpnpSsdpEngine::initialize()
{
    connect(&d->mSsdpQuerySocket, &QUdpSocket::readyRead, this, &UpnpSsdpEngine::queryReceivedData);
    connect(&d->mSsdpStandardSocket, &QUdpSocket::readyRead, this, &UpnpSsdpEngine::standardReceivedData);

    if (!d->mSsdpStandardSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint)) {
        qDebug() << "Qt method failed";
        int reuse = 1;
        if (setsockopt(d->mSsdpStandardSocket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            qDebug() << "setsockopt() failed";
        }

        int bindResult = d->mSsdpStandardSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
        if (!bindResult) {
            qDebug() << "bind failed" << QHostAddress(QStringLiteral("239.255.255.250")) << d->mSsdpStandardSocket.error() << d->mSsdpStandardSocket.errorString();
        }
    }

    d->mSsdpQuerySocket.bind(QHostAddress::Any);

    d->mServerInformation = QSysInfo::kernelType() + QStringLiteral(" ") + QSysInfo::kernelVersion()  + QStringLiteral(" UPnP/1.0 ");
}

UpnpSsdpEngine::~UpnpSsdpEngine()
{
    delete d;
}

quint16 UpnpSsdpEngine::port() const
{
    return d->mPortNumber;
}

void UpnpSsdpEngine::setPort(quint16 value)
{
    d->mPortNumber = value;
    Q_EMIT portChanged();
}

bool UpnpSsdpEngine::canExportServices() const
{
    return d->mCanExportServices;
}

void UpnpSsdpEngine::setCanExportServices(bool value)
{
    d->mCanExportServices = value;
    Q_EMIT canExportServicesChanged();
}

bool UpnpSsdpEngine::searchAllUpnpDevice(int maxDelay)
{
    QByteArray allDiscoveryMessage;

    allDiscoveryMessage += "M-SEARCH * HTTP/1.1\r\n";
    allDiscoveryMessage += "HOST: 239.255.255.250:" + QByteArray::number(d->mPortNumber) + "\r\n";
    allDiscoveryMessage += "MAN: \"ssdp:discover\"\r\n";
    allDiscoveryMessage += "MX: " + QByteArray::number(maxDelay) + "\r\n";
    allDiscoveryMessage += "ST: ssdp:all\r\n\r\n";

    auto result = d->mSsdpQuerySocket.writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    return result != -1;
}

void UpnpSsdpEngine::subscribeDevice(UpnpAbstractDevice *device)
{
    connect(this, &UpnpSsdpEngine::newSearchQuery, device, &UpnpAbstractDevice::newSearchQuery);
    publishDevice(device);
}

void UpnpSsdpEngine::publishDevice(UpnpAbstractDevice *device)
{
    QByteArray allDiscoveryMessageCommonContent;

    allDiscoveryMessageCommonContent += "NOTIFY * HTTP/1.1\r\n";
    allDiscoveryMessageCommonContent += "HOST: 239.255.255.250:" + QByteArray::number(d->mPortNumber) + "\r\n";
    allDiscoveryMessageCommonContent += "CACHE-CONTROL: max-age=" + QByteArray::number(device->cacheControl()) + "\r\n";
    allDiscoveryMessageCommonContent += "NTS: ssdp:alive\r\n";
    allDiscoveryMessageCommonContent += "SERVER: " + d->mServerInformation.toLatin1() + " " + device->modelName().toLatin1() + " " + device->modelNumber().toLatin1() + "\r\n";

    QByteArray rootDeviceMessage(allDiscoveryMessageCommonContent);
    rootDeviceMessage += "NT: upnp:rootdevice\r\n";
    rootDeviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "::upnp:rootdevice\r\n";
    rootDeviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
    rootDeviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(rootDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    QByteArray uuidDeviceMessage(allDiscoveryMessageCommonContent);
    uuidDeviceMessage += "NT: uuid:" + device->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
    uuidDeviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(uuidDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    QByteArray deviceMessage(allDiscoveryMessageCommonContent);
    deviceMessage += "NT: " + device->deviceType().toLatin1() + "\r\n";
    deviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "::" + device->deviceType().toLatin1() + "\r\n";
    deviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
    deviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    const QList<QPointer<UpnpAbstractService> > &servicesList = device->services();
    for (auto itService = servicesList.begin(); itService != servicesList.end(); ++itService) {
        QByteArray deviceMessage(allDiscoveryMessageCommonContent);
        deviceMessage += "NT: " + (*itService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "USN: uuid:" + device->UDN().toLatin1() + "::" + (*itService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "LOCATION: "+ device->locationUrl().toString().toLatin1() + "\r\n";
        deviceMessage += "\r\n";

        d->mSsdpQuerySocket.writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
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

void UpnpSsdpEngine::parseSsdpQueryDatagram(const QByteArray &datagram, const QList<QByteArray> &headers)
{
    UpnpSearchQuery newSearch;
    bool hasAddress = false;
    bool hasAnswerDelay = false;
    bool hasSearchTarget = false;

    for (auto itLine = headers.begin(); itLine != headers.end(); ++itLine) {
        if (itLine->startsWith("HOST")) {
            QString hostName;
            if ((*itLine)[4] == ' ') {
                hostName = QString::fromLatin1(itLine->mid(7, itLine->length() - 8));
            } else {
                hostName = QString::fromLatin1(itLine->mid(6, itLine->length() - 7));
            }
            auto addressParts = hostName.split(QStringLiteral(":"));
            newSearch.mSearchHostAddress.setAddress(addressParts.first());
            newSearch.mSearchHostPort = addressParts.last().toInt();
            hasAddress = true;
        }
        if (itLine->startsWith("MAN")) {
            if (!itLine->contains("\"ssdp:discover\"")) {
                qDebug() << "not valid" << datagram;
                return;
            }
        }
        if (itLine->startsWith("MX")) {
            if ((*itLine)[2] == ' ') {
                newSearch.mAnswerDelay = QString::fromLatin1(itLine->mid(4, itLine->length() - 5)).toInt();
            } else {
                newSearch.mAnswerDelay = QString::fromLatin1(itLine->mid(3, itLine->length() - 4)).toInt();
            }
            hasAnswerDelay = true;
        }
        if (itLine->startsWith("ST")) {
            if ((*itLine)[2] == ' ') {
                newSearch.mSearchTarget = QString::fromLatin1(itLine->mid(5, itLine->length() - 6));
            } else {
                newSearch.mSearchTarget = QString::fromLatin1(itLine->mid(4, itLine->length() - 5));
            }
            if (newSearch.mSearchTarget.startsWith(QStringLiteral("ssdp:all"))) {
                newSearch.mSearchTargetType = SearchTargetType::All;
            } else if (newSearch.mSearchTarget.startsWith(QStringLiteral("upnp:rootdevice"))) {
                newSearch.mSearchTargetType = SearchTargetType::RootDevice;
            } else if (newSearch.mSearchTarget.startsWith(QStringLiteral("uuid:"))) {
                newSearch.mSearchTargetType = SearchTargetType::DeviceUUID;
            } else if (newSearch.mSearchTarget.startsWith(QStringLiteral("urn:"))) {
                if (newSearch.mSearchTarget.contains(QStringLiteral("device:"))) {
                    newSearch.mSearchTargetType = SearchTargetType::DeviceType;
                } else if (newSearch.mSearchTarget.contains(QStringLiteral("service:"))) {
                    newSearch.mSearchTargetType = SearchTargetType::ServiceType;
                }
            }
            hasSearchTarget = true;
        }
    }

    if (hasAddress && hasAnswerDelay && hasSearchTarget) {
        Q_EMIT newSearchQuery(this, newSearch);
    }
}

void UpnpSsdpEngine::parseSsdpAnnounceDatagram(const QByteArray &datagram, const QList<QByteArray> &headers, SsdpMessageType messageType)
{
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
        if (itLine->startsWith("HOST") || itLine->startsWith("Host")) {
            QString hostName;
            if ((*itLine)[4] == ' ') {
                newDiscovery.mLocation = QString::fromLatin1(itLine->mid(7, itLine->length() - 8));
            } else {
                newDiscovery.mLocation = QString::fromLatin1(itLine->mid(6, itLine->length() - 7));
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
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery.mUSN);

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

        Q_EMIT newService(newDiscovery);
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

    if (messageType == SsdpMessageType::query) {
        parseSsdpQueryDatagram(datagram, headers);
    } else if (messageType == SsdpMessageType::announce || messageType == SsdpMessageType::queryAnswer) {
        parseSsdpAnnounceDatagram(datagram, headers, messageType);
    } else {
        qDebug() << "not decoded" << datagram;
        return;
    }
}

#include "moc_upnpssdpengine.cpp"
