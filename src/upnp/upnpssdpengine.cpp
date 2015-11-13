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

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

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

    d->mSsdpStandardSocket.bind(QHostAddress::AnyIPv4, d->mPortNumber, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    d->mSsdpStandardSocket.joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")));
    d->mSsdpStandardSocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
    d->mSsdpStandardSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

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

bool UpnpSsdpEngine::searchUpnp(SEARCH_TYPE searchType, const QString &searchCriteria, int maxDelay)
{
    switch (searchType)
    {
    case AllDevices:
        return searchAllUpnpDevice(maxDelay);
    case RootDevices:
        return searchAllRootDevice(maxDelay);
    case DeviceByUUID:
        return searchByDeviceUUID(searchCriteria, maxDelay);
    case DeviceByType:
        return searchByDeviceType(searchCriteria, maxDelay);
    case ServiceByType:
        return searchByServiceType(searchCriteria, maxDelay);
    }

    return false;
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

bool UpnpSsdpEngine::searchAllRootDevice(int maxDelay)
{
    QByteArray allDiscoveryMessage;

    allDiscoveryMessage += "M-SEARCH * HTTP/1.1\r\n";
    allDiscoveryMessage += "HOST: 239.255.255.250:" + QByteArray::number(d->mPortNumber) + "\r\n";
    allDiscoveryMessage += "MAN: \"ssdp:discover\"\r\n";
    allDiscoveryMessage += "MX: " + QByteArray::number(maxDelay) + "\r\n";
    allDiscoveryMessage += "ST: upnp:rootdevice\r\n\r\n";

    auto result = d->mSsdpQuerySocket.writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    return result != -1;
}

bool UpnpSsdpEngine::searchByDeviceUUID(const QString &uuid, int maxDelay)
{
    QByteArray allDiscoveryMessage;

    allDiscoveryMessage += "M-SEARCH * HTTP/1.1\r\n";
    allDiscoveryMessage += "HOST: 239.255.255.250:" + QByteArray::number(d->mPortNumber) + "\r\n";
    allDiscoveryMessage += "MAN: \"ssdp:discover\"\r\n";
    allDiscoveryMessage += "MX: " + QByteArray::number(maxDelay) + "\r\n";
    allDiscoveryMessage += "ST: uuid:" + uuid.toLatin1() + "\r\n\r\n";

    auto result = d->mSsdpQuerySocket.writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    return result != -1;
}

bool UpnpSsdpEngine::searchByDeviceType(const QString &upnpDeviceType, int maxDelay)
{
    QByteArray allDiscoveryMessage;

    allDiscoveryMessage += "M-SEARCH * HTTP/1.1\r\n";
    allDiscoveryMessage += "HOST: 239.255.255.250:" + QByteArray::number(d->mPortNumber) + "\r\n";
    allDiscoveryMessage += "MAN: \"ssdp:discover\"\r\n";
    allDiscoveryMessage += "MX: " + QByteArray::number(maxDelay) + "\r\n";
    allDiscoveryMessage += "ST: urn:" + upnpDeviceType.toLatin1() + "\r\n\r\n";

    auto result = d->mSsdpQuerySocket.writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    return result != -1;
}

bool UpnpSsdpEngine::searchByServiceType(const QString &upnpServiceType, int maxDelay)
{
    QByteArray allDiscoveryMessage;

    allDiscoveryMessage += "M-SEARCH * HTTP/1.1\r\n";
    allDiscoveryMessage += "HOST: 239.255.255.250:" + QByteArray::number(d->mPortNumber) + "\r\n";
    allDiscoveryMessage += "MAN: \"ssdp:discover\"\r\n";
    allDiscoveryMessage += "MX: " + QByteArray::number(maxDelay) + "\r\n";
    allDiscoveryMessage += "ST: urn:" + upnpServiceType.toLatin1() + "\r\n\r\n";

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
    allDiscoveryMessageCommonContent += "CACHE-CONTROL: max-age=" + QByteArray::number(device->description()->cacheControl()) + "\r\n";
    allDiscoveryMessageCommonContent += "NTS: ssdp:alive\r\n";
    allDiscoveryMessageCommonContent += "SERVER: " + d->mServerInformation.toLatin1() + " " + device->description()->modelName().toLatin1() + " " + device->description()->modelNumber().toLatin1() + "\r\n";

    QByteArray rootDeviceMessage(allDiscoveryMessageCommonContent);
    rootDeviceMessage += "NT: upnp:rootdevice\r\n";
    rootDeviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "::upnp:rootdevice\r\n";
    rootDeviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
    rootDeviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(rootDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    QByteArray uuidDeviceMessage(allDiscoveryMessageCommonContent);
    uuidDeviceMessage += "NT: uuid:" + device->description()->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
    uuidDeviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(uuidDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    QByteArray deviceMessage(allDiscoveryMessageCommonContent);
    deviceMessage += "NT: " + device->description()->deviceType().toLatin1() + "\r\n";
    deviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "::" + device->description()->deviceType().toLatin1() + "\r\n";
    deviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
    deviceMessage += "\r\n";

    d->mSsdpQuerySocket.writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);

    const QList<QSharedPointer<UpnpServiceDescription> > &servicesList = device->description()->services();
    for (const auto &oneService : servicesList) {
        QByteArray deviceMessage(allDiscoveryMessageCommonContent);
        deviceMessage += "NT: " + (oneService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "::" + (oneService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
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

    for (const auto & header : headers) {
        if (header.startsWith("HOST")) {
            QString hostName;
            if ((header)[4] == ' ') {
                hostName = QString::fromLatin1(header.mid(7, header.length() - 8));
            } else {
                hostName = QString::fromLatin1(header.mid(6, header.length() - 7));
            }
            auto addressParts = hostName.split(QStringLiteral(":"));
            newSearch.mSearchHostAddress.setAddress(addressParts.first());
            newSearch.mSearchHostPort = addressParts.last().toInt();
            hasAddress = true;
        }
        if (header.startsWith("MAN")) {
            if (!header.contains("\"ssdp:discover\"")) {
                qDebug() << "not valid" << datagram;
                return;
            }
        }
        if (header.startsWith("MX")) {
            if ((header)[2] == ' ') {
                newSearch.mAnswerDelay = QString::fromLatin1(header.mid(4, header.length() - 5)).toInt();
            } else {
                newSearch.mAnswerDelay = QString::fromLatin1(header.mid(3, header.length() - 4)).toInt();
            }
            hasAnswerDelay = true;
        }
        if (header.startsWith("ST")) {
            if ((header)[2] == ' ') {
                newSearch.mSearchTarget = QString::fromLatin1(header.mid(5, header.length() - 6));
            } else {
                newSearch.mSearchTarget = QString::fromLatin1(header.mid(4, header.length() - 5));
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

    for (const auto & header : headers) {
        if (header.startsWith("LOCATION")) {
            if ((header)[9] == ' ') {
                newDiscovery.mLocation = QString::fromLatin1(header.mid(10, header.length() - 11));
            } else {
                newDiscovery.mLocation = QString::fromLatin1(header.mid(9, header.length() - 10));
            }
        }
        if (header.startsWith("Location")) {
            if ((header)[9] == ' ') {
                newDiscovery.mLocation = QString::fromLatin1(header.mid(10, header.length() - 11));
            } else {
                newDiscovery.mLocation = QString::fromLatin1(header.mid(9, header.length() - 10));
            }
        }
        if (header.startsWith("HOST") || header.startsWith("Host")) {
            QString hostName;
            if ((header)[4] == ' ') {
                newDiscovery.mLocation = QString::fromLatin1(header.mid(7, header.length() - 8));
            } else {
                newDiscovery.mLocation = QString::fromLatin1(header.mid(6, header.length() - 7));
            }
        }
        if (header.startsWith("USN")) {
            if ((header)[4] == ' ') {
                newDiscovery.mUSN = QString::fromLatin1(header.mid(5, header.length() - 6));
            } else {
                newDiscovery.mUSN = QString::fromLatin1(header.mid(4, header.length() - 5));
            }
        }
        if (messageType == SsdpMessageType::queryAnswer && header.startsWith("ST")) {
            if ((header)[3] == ' ') {
                newDiscovery.mNT = QString::fromLatin1(header.mid(4, header.length() - 5));
            } else {
                newDiscovery.mNT = QString::fromLatin1(header.mid(3, header.length() - 4));
            }
        }
        if (messageType == SsdpMessageType::announce && header.startsWith("NT:")) {
            if ((header)[3] == ' ') {
                newDiscovery.mNT = QString::fromLatin1(header.mid(4, header.length() - 5));
            } else {
                newDiscovery.mNT = QString::fromLatin1(header.mid(3, header.length() - 4));
            }
        }
        if (messageType == SsdpMessageType::announce && header.startsWith("NTS")) {
            if (header.endsWith("ssdp:alive\r")) {
                newDiscovery.mNTS = NotificationSubType::Alive;
            }
            if (header.endsWith("ssdp:byebye\r")) {
                newDiscovery.mNTS = NotificationSubType::ByeBye;
            }
            if (header.endsWith("ssdp:discover\r")) {
                newDiscovery.mNTS = NotificationSubType::Discover;
            }
        }
        if (header.startsWith("DATE")) {
            if ((header)[5] == ' ') {
                newDiscovery.mAnnounceDate = QString::fromLatin1(header.mid(6, header.length() - 7));
            } else {
                newDiscovery.mAnnounceDate = QString::fromLatin1(header.mid(5, header.length() - 6));
            }
        }
        if (header.startsWith("CACHE-CONTROL")) {
            if ((header)[14] == ' ') {
                const QList<QByteArray> &splittedLine = header.mid(15, header.length() - 16).split('=');
                if (splittedLine.size() == 2) {
                    newDiscovery.mCacheDuration = splittedLine.last().toInt();
                }
            } else {
                const QList<QByteArray> &splittedLine = header.mid(14, header.length() - 15).split('=');
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
