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
#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkConfiguration>

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QSet>
#include <QtCore/QUrl>
#include <QtCore/QSharedPointer>
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

    QMap<QString, bool> mInterfaceStatus;

    QHash<QString, QSharedPointer<UpnpDiscoveryResult> > mDiscoveryResults;

    QList<QPointer<QUdpSocket>> mSsdpQuerySocket;

    QList<QPointer<QUdpSocket>> mSsdpStandardSocket;

    QString mServerInformation;

    QNetworkConfigurationManager mNetworkManager;
};

UpnpSsdpEngine::UpnpSsdpEngine(QObject *parent)
    : QObject(parent), d(new UpnpSsdpEnginePrivate)
{
    connect(&d->mNetworkManager, &QNetworkConfigurationManager::configurationAdded, this, &UpnpSsdpEngine::networkConfigurationAdded);
    connect(&d->mNetworkManager, &QNetworkConfigurationManager::configurationRemoved, this, &UpnpSsdpEngine::networkConfigurationRemoved);
    connect(&d->mNetworkManager, &QNetworkConfigurationManager::configurationChanged, this, &UpnpSsdpEngine::networkConfigurationChanged);
    connect(&d->mNetworkManager, &QNetworkConfigurationManager::onlineStateChanged, this, &UpnpSsdpEngine::networkOnlineStateChanged);
    connect(&d->mNetworkManager, &QNetworkConfigurationManager::updateCompleted, this, &UpnpSsdpEngine::networkUpdateCompleted);
}

void UpnpSsdpEngine::initialize()
{
    const auto &allConfigurations = d->mNetworkManager.allConfigurations();
    for (const auto &oneConfiguration : allConfigurations) {
        const auto &oneInterface = QNetworkInterface::interfaceFromName(oneConfiguration.name());

        if (oneInterface.isValid()) {
            d->mInterfaceStatus[oneInterface.name()] = oneConfiguration.isValid() && oneConfiguration.state().testFlag(QNetworkConfiguration::Active);
        }
    }

    const auto &allInterfaces = QNetworkInterface::allInterfaces();
    for (const auto &oneInterface : allInterfaces) {
        const auto &allAddresses = oneInterface.addressEntries();
        for (const auto &oneAddress : allAddresses) {

            if (oneAddress.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                continue;
            }

            qDebug() << "I have one address:" << oneInterface;

            if (oneInterface.addressEntries().isEmpty()) {
                continue;
            }

            qDebug() << "try to open sockets";
            d->mSsdpQuerySocket.push_back({new QUdpSocket});
            d->mSsdpStandardSocket.push_back({new QUdpSocket});

            auto &newQuerySocket = d->mSsdpQuerySocket.last();

            connect(newQuerySocket.data(), &QUdpSocket::readyRead, this, &UpnpSsdpEngine::queryReceivedData);

            newQuerySocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
            newQuerySocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

            if (oneInterface.addressEntries().isEmpty()) {
                continue;
            }

            auto result = newQuerySocket->bind(oneAddress.ip());
            qDebug() << "bind" << (result ? "true" : "false");
            result = newQuerySocket->joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")), oneInterface);
            qDebug() << "joinMulticastGroup" << (result ? "true" : "false") << newQuerySocket->errorString();
        }
    }
    {
        auto &newStandardSocket = d->mSsdpStandardSocket.last();

        connect(newStandardSocket.data(), &QUdpSocket::readyRead, this, &UpnpSsdpEngine::standardReceivedData);

        newStandardSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
        newStandardSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

        auto result = newStandardSocket->bind(QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber, QAbstractSocket::ShareAddress);
        qDebug() << "bind" << (result ? "true" : "false");
        result = newStandardSocket->joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")));
        qDebug() << "joinMulticastGroup" << (result ? "true" : "false") << newStandardSocket->errorString();
    }

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

    qint64 result = -1;

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        result = oneSocket->writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
        qDebug() << "UpnpSsdpEngine::searchAllUpnpDevice" << result << oneSocket->errorString();
    }

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

    qint64 result = -1;

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        result = oneSocket->writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

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

    qint64 result = -1;

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        result = oneSocket->writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

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

    qint64 result = -1;

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        result = oneSocket->writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

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

    qint64 result = -1;

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        result = oneSocket->writeDatagram(allDiscoveryMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

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

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        oneSocket->writeDatagram(rootDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

    QByteArray uuidDeviceMessage(allDiscoveryMessageCommonContent);
    uuidDeviceMessage += "NT: uuid:" + device->description()->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
    uuidDeviceMessage += "\r\n";

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        oneSocket->writeDatagram(uuidDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

    QByteArray deviceMessage(allDiscoveryMessageCommonContent);
    deviceMessage += "NT: " + device->description()->deviceType().toLatin1() + "\r\n";
    deviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "::" + device->description()->deviceType().toLatin1() + "\r\n";
    deviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
    deviceMessage += "\r\n";

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        oneSocket->writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

    const auto &servicesList = device->description()->services();
    for (const auto &oneService : servicesList) {
        QByteArray deviceMessage(allDiscoveryMessageCommonContent);
        deviceMessage += "NT: " + (oneService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "USN: uuid:" + device->description()->UDN().toLatin1() + "::" + (oneService)->serviceType().toLatin1() + "\r\n";
        deviceMessage += "LOCATION: "+ device->description()->locationUrl().toString().toLatin1() + "\r\n";
        deviceMessage += "\r\n";

        for (auto &oneSocket : d->mSsdpQuerySocket) {
            oneSocket->writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
        }
    }
}

void UpnpSsdpEngine::standardReceivedData()
{
    QUdpSocket *receiverSocket = qobject_cast<QUdpSocket*>(sender());

    while (receiverSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(receiverSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        receiverSocket->readDatagram(datagram.data(), datagram.size(),
                                     &sender, &senderPort);

        parseSsdpDatagram(datagram);
    }
}

void UpnpSsdpEngine::queryReceivedData()
{
    QUdpSocket *receiverSocket = qobject_cast<QUdpSocket*>(sender());

    while (receiverSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(receiverSocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        receiverSocket->readDatagram(datagram.data(), datagram.size(),
                                     &sender, &senderPort);

        parseSsdpDatagram(datagram);
    }
}

void UpnpSsdpEngine::discoveryResultTimeout(const QString &usn)
{
    auto itDiscovery = d->mDiscoveryResults.find(usn);
    if (itDiscovery != d->mDiscoveryResults.end()) {
        Q_EMIT removedService(*itDiscovery);

        d->mDiscoveryResults.erase(itDiscovery);
    }
}

void UpnpSsdpEngine::networkConfigurationAdded(const QNetworkConfiguration &config)
{
    qDebug() << "UpnpSsdpEngine::networkConfigurationAdded" << config.name();
}

void UpnpSsdpEngine::networkConfigurationRemoved(const QNetworkConfiguration &config)
{
    qDebug() << "UpnpSsdpEngine::networkConfigurationRemoved" << config.name();
}

void UpnpSsdpEngine::networkConfigurationChanged(const QNetworkConfiguration &config)
{
    qDebug() << "UpnpSsdpEngine::networkConfigurationChanged" << config.name();
}

void UpnpSsdpEngine::networkOnlineStateChanged(bool isOnline)
{
    qDebug() << "UpnpSsdpEngine::networkOnlineStateChanged" << (isOnline ? "true" : "false");
}

void UpnpSsdpEngine::networkUpdateCompleted()
{
    qDebug() << "UpnpSsdpEngine::networkUpdateCompleted";
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
    QSharedPointer<UpnpDiscoveryResult> newDiscovery(new UpnpDiscoveryResult);
    newDiscovery->setNTS(NotificationSubType::Invalid);

    for (const auto & header : headers) {
        if (header.startsWith("LOCATION")) {
            if ((header)[9] == ' ') {
                newDiscovery->setLocation(QString::fromLatin1(header.mid(10, header.length() - 11)));
            } else {
                newDiscovery->setLocation(QString::fromLatin1(header.mid(9, header.length() - 10)));
            }
        }
        if (header.startsWith("Location")) {
            if ((header)[9] == ' ') {
                newDiscovery->setLocation(QString::fromLatin1(header.mid(10, header.length() - 11)));
            } else {
                newDiscovery->setLocation(QString::fromLatin1(header.mid(9, header.length() - 10)));
            }
        }
        if (header.startsWith("HOST") || header.startsWith("Host")) {
            if ((header)[4] == ' ') {
                newDiscovery->setLocation(QString::fromLatin1(header.mid(7, header.length() - 8)));
            } else {
                newDiscovery->setLocation(QString::fromLatin1(header.mid(6, header.length() - 7)));
            }
        }
        if (header.startsWith("USN")) {
            if ((header)[4] == ' ') {
                newDiscovery->setUSN(QString::fromLatin1(header.mid(5, header.length() - 6)));
            } else {
                newDiscovery->setUSN(QString::fromLatin1(header.mid(4, header.length() - 5)));
            }
        }
        if (messageType == SsdpMessageType::queryAnswer && header.startsWith("ST")) {
            if ((header)[3] == ' ') {
                newDiscovery->setNT(QString::fromLatin1(header.mid(4, header.length() - 5)));
            } else {
                newDiscovery->setNT(QString::fromLatin1(header.mid(3, header.length() - 4)));
            }
        }
        if (messageType == SsdpMessageType::announce && header.startsWith("NT:")) {
            if ((header)[3] == ' ') {
                newDiscovery->setNT(QString::fromLatin1(header.mid(4, header.length() - 5)));
            } else {
                newDiscovery->setNT(QString::fromLatin1(header.mid(3, header.length() - 4)));
            }
        }
        if (messageType == SsdpMessageType::announce && header.startsWith("NTS")) {
            if (header.endsWith("ssdp:alive\r")) {
                newDiscovery->setNTS(NotificationSubType::Alive);
            }
            if (header.endsWith("ssdp:byebye\r")) {
                newDiscovery->setNTS(NotificationSubType::ByeBye);
            }
            if (header.endsWith("ssdp:discover\r")) {
                newDiscovery->setNTS(NotificationSubType::Discover);
            }
        }
        if (header.startsWith("DATE")) {
            if ((header)[5] == ' ') {
                newDiscovery->setAnnounceDate(QString::fromLatin1(header.mid(6, header.length() - 7)));
            } else {
                newDiscovery->setAnnounceDate(QString::fromLatin1(header.mid(5, header.length() - 6)));
            }
        }
        if (header.startsWith("CACHE-CONTROL")) {
            if ((header)[14] == ' ') {
                const QList<QByteArray> &splittedLine = header.mid(15, header.length() - 16).split('=');
                if (splittedLine.size() == 2) {
                    newDiscovery->setCacheDuration(splittedLine.last().toInt());
                }
            } else {
                const QList<QByteArray> &splittedLine = header.mid(14, header.length() - 15).split('=');
                if (splittedLine.size() == 2) {
                    newDiscovery->setCacheDuration(splittedLine.last().toInt());
                }
            }
        }
    }

    if (newDiscovery->location().isEmpty() || newDiscovery->usn().isEmpty() ||
            newDiscovery->nt().isEmpty() ||
            (messageType == SsdpMessageType::announce && newDiscovery->nts() == NotificationSubType::Invalid)) {
        qDebug() << "not decoded" << datagram;
        return;
    }

    if (newDiscovery->nts() == NotificationSubType::Alive || messageType == SsdpMessageType::queryAnswer) {
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery->usn());

        if (itDiscovery == d->mDiscoveryResults.end()) {
            d->mDiscoveryResults[newDiscovery->usn()] = newDiscovery;
            itDiscovery = d->mDiscoveryResults.find(newDiscovery->usn());

            connect(newDiscovery.data(), &UpnpDiscoveryResult::timeout, this, &UpnpSsdpEngine::discoveryResultTimeout);

            Q_EMIT newService(newDiscovery);
        } else {
            (*itDiscovery)->discoveryIsAlive();
        }

#if 0
        qDebug() << datagram;
        qDebug() << "AnnounceDate" << newDiscovery->mAnnounceDate;
        qDebug() << "CacheDuration" << newDiscovery->mCacheDuration;

        qDebug() << "new service";
        qDebug() << "DeviceId:" << newDiscovery->mUSN;
        qDebug() << "DeviceType:" << newDiscovery->mNT;
        qDebug() << "Location:" << newDiscovery->mLocation;
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

    if (newDiscovery->nts() == NotificationSubType::ByeBye) {
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery->usn());
        if (itDiscovery != d->mDiscoveryResults.end()) {
            Q_EMIT removedService(newDiscovery);

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

    SsdpMessageType messageType = SsdpMessageType::invalid;

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
