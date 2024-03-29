/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpssdpengine.h"

#include "ssdplogging.h"

#include "upnpdiscoveryresult.h"

#include "upnpabstractdevice.h"
#include "upnpabstractservice.h"

#include "upnpdevicedescription.h"
#include "upnpservicedescription.h"

#include "ssdplogging.h"

#include <QHostAddress>
#include <QNetworkInformation>
#include <QNetworkInterface>
#include <QUdpSocket>

#include <QHash>
#include <QLoggingCategory>
#include <QSet>
#include <QSharedPointer>
#include <QSysInfo>
#include <QUrl>

#include <sys/socket.h>
#include <sys/types.h>

class UpnpSsdpEnginePrivate
{
public:
    QHash<QString, UpnpDiscoveryResult> mDiscoveryResults;

    QList<QPointer<QUdpSocket>> mSsdpQuerySocket;

    QList<QPointer<QUdpSocket>> mSsdpStandardSocket;

    QString mServerInformation;

    QString mActiveConfiguration;

    QTimer mTimeoutTimer;

    quint16 mPortNumber = 1900;

    bool mCanExportServices = true;
};

UpnpSsdpEngine::UpnpSsdpEngine(QObject *parent)
    : QObject(parent)
    , d(std::make_unique<UpnpSsdpEnginePrivate>())
{
    if (QNetworkInformation::loadDefaultBackend()) {
        qCInfo(orgKdeUpnpLibQtSsdp) << "network connectivity information is available";
        connect(QNetworkInformation::instance(), &QNetworkInformation::reachabilityChanged, this, &UpnpSsdpEngine::networkReachabilityChanged);
    } else {
        qCWarning(orgKdeUpnpLibQtSsdp) << "cannot get network connectivity information";
    }

    connect(&d->mTimeoutTimer, &QTimer::timeout, this, &UpnpSsdpEngine::discoveryResultTimeout);
    d->mTimeoutTimer.setSingleShot(false);
    d->mTimeoutTimer.start(1000);
}

void UpnpSsdpEngine::initialize()
{
    reconfigureNetwork();
}

UpnpSsdpEngine::~UpnpSsdpEngine() = default;

bool UpnpSsdpEngine::port() const
{
    return d->mPortNumber;
}

void UpnpSsdpEngine::setPort(quint16 value)
{
    if (d->mPortNumber == value) {
        return;
    }

    d->mPortNumber = value;
    Q_EMIT portChanged();
}

bool UpnpSsdpEngine::canExportServices() const
{
    return d->mCanExportServices;
}

void UpnpSsdpEngine::setCanExportServices(bool value)
{
    if (d->mCanExportServices == value) {
        return;
    }

    d->mCanExportServices = value;
    Q_EMIT canExportServicesChanged();
}

QList<UpnpDiscoveryResult> UpnpSsdpEngine::existingServices() const
{
    auto result = QList<UpnpDiscoveryResult>();

    for (const auto &oneService : qAsConst(d->mDiscoveryResults)) {
        result.push_back(oneService);
    }

    return result;
}

bool UpnpSsdpEngine::searchUpnp(SEARCH_TYPE searchType, const QString &searchCriteria, int maxDelay)
{
    switch (searchType) {
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
        qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::searchAllUpnpDevice" << result << oneSocket->errorString();
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
    allDiscoveryMessageCommonContent += "CACHE-CONTROL: max-age=" + QByteArray::number(device->description().cacheControl()) + "\r\n";
    allDiscoveryMessageCommonContent += "NTS: ssdp:alive\r\n";
    allDiscoveryMessageCommonContent += "SERVER: " + d->mServerInformation.toLatin1() + " " + device->description().modelName().toLatin1() + " " + device->description().modelNumber().toLatin1() + "\r\n";

    QByteArray rootDeviceMessage(allDiscoveryMessageCommonContent);
    rootDeviceMessage += "NT: upnp:rootdevice\r\n";
    rootDeviceMessage += "USN: uuid:" + device->description().UDN().toLatin1() + "::upnp:rootdevice\r\n";
    rootDeviceMessage += "LOCATION: " + device->description().locationUrl().toString().toLatin1() + "\r\n";
    rootDeviceMessage += "\r\n";

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        oneSocket->writeDatagram(rootDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

    QByteArray uuidDeviceMessage(allDiscoveryMessageCommonContent);
    uuidDeviceMessage += "NT: uuid:" + device->description().UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "USN: uuid:" + device->description().UDN().toLatin1() + "\r\n";
    uuidDeviceMessage += "LOCATION: " + device->description().locationUrl().toString().toLatin1() + "\r\n";
    uuidDeviceMessage += "\r\n";

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        oneSocket->writeDatagram(uuidDeviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

    QByteArray deviceMessage(allDiscoveryMessageCommonContent);
    deviceMessage += "NT: " + device->description().deviceType().toLatin1() + "\r\n";
    deviceMessage += "USN: uuid:" + device->description().UDN().toLatin1() + "::" + device->description().deviceType().toLatin1() + "\r\n";
    deviceMessage += "LOCATION: " + device->description().locationUrl().toString().toLatin1() + "\r\n";
    deviceMessage += "\r\n";

    for (auto &oneSocket : d->mSsdpQuerySocket) {
        oneSocket->writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
    }

    const auto &servicesList = device->description().services();
    for (const auto &oneService : servicesList) {
        QByteArray deviceMessage(allDiscoveryMessageCommonContent);
        deviceMessage += "NT: " + oneService.serviceType().toLatin1() + "\r\n";
        deviceMessage += "USN: uuid:" + device->description().UDN().toLatin1() + "::" + oneService.serviceType().toLatin1() + "\r\n";
        deviceMessage += "LOCATION: " + device->description().locationUrl().toString().toLatin1() + "\r\n";
        deviceMessage += "\r\n";

        for (auto &oneSocket : d->mSsdpQuerySocket) {
            oneSocket->writeDatagram(deviceMessage, QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber);
        }
    }
}

void UpnpSsdpEngine::standardReceivedData()
{
    qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::standardReceivedData";
    auto *receiverSocket = qobject_cast<QUdpSocket *>(sender());

    while (receiverSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(receiverSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        receiverSocket->readDatagram(datagram.data(), datagram.size(),
            &sender, &senderPort);

        parseSsdpDatagram(datagram);
    }
}

void UpnpSsdpEngine::queryReceivedData()
{
    qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::queryReceivedData";
    auto *receiverSocket = qobject_cast<QUdpSocket *>(sender());

    while (receiverSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(static_cast<int>(receiverSocket->pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;

        receiverSocket->readDatagram(datagram.data(), datagram.size(),
            &sender, &senderPort);

        parseSsdpDatagram(datagram);
    }
}

void UpnpSsdpEngine::discoveryResultTimeout()
{
    auto now = QDateTime::currentDateTime();

    auto timedOutDiscoveryResults = QList<QString>();

    for (const auto &itDiscovery : qAsConst(d->mDiscoveryResults)) {
        if (now > itDiscovery.validityTimestamp()) {
            qCDebug(orgKdeUpnpLibQtSsdp()) << "remove service due to timeout" << itDiscovery;
            Q_EMIT removedService(itDiscovery);

            timedOutDiscoveryResults.push_back(itDiscovery.usn());
        }
    }

    for (const auto &removedUsn : timedOutDiscoveryResults) {
        auto itDiscovery = d->mDiscoveryResults.find(removedUsn);
        if (itDiscovery != d->mDiscoveryResults.end()) {
            d->mDiscoveryResults.erase(itDiscovery);
        }
    }
}

void UpnpSsdpEngine::networkReachabilityChanged(QNetworkInformation::Reachability newReachability)
{
    qCInfo(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::networkReachabilityChanged" << newReachability;

    reconfigureNetwork();

    Q_EMIT networkChanged();
}

void UpnpSsdpEngine::networkUpdateCompleted()
{
    qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::networkUpdateCompleted";
}

void UpnpSsdpEngine::reconfigureNetwork()
{
    for (const auto &oneDevice : qAsConst(d->mDiscoveryResults)) {
        Q_EMIT removedService(oneDevice);
    }
    d->mDiscoveryResults.clear();

    const auto &allInterfaces = QNetworkInterface::allInterfaces();
    for (const auto &oneInterface : allInterfaces) {
        const auto &allAddresses = oneInterface.addressEntries();
        for (const auto &oneAddress : allAddresses) {

            if (oneAddress.ip().protocol() != QAbstractSocket::IPv4Protocol) {
                continue;
            }

            qCDebug(orgKdeUpnpLibQtSsdp()) << "I have one address:" << oneInterface;

            if (oneInterface.addressEntries().isEmpty()) {
                continue;
            }

            qCDebug(orgKdeUpnpLibQtSsdp()) << "try to open sockets";
            d->mSsdpQuerySocket.push_back({ new QUdpSocket });
            d->mSsdpStandardSocket.push_back({ new QUdpSocket });

            auto &newQuerySocket = d->mSsdpQuerySocket.last();

            connect(newQuerySocket.data(), &QUdpSocket::readyRead, this, &UpnpSsdpEngine::queryReceivedData);

            newQuerySocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
            newQuerySocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

            if (oneInterface.addressEntries().isEmpty()) {
                continue;
            }

            auto result = newQuerySocket->bind(oneAddress.ip());
            qCDebug(orgKdeUpnpLibQtSsdp()) << "bind" << (result ? "true" : "false");
            result = newQuerySocket->joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")), oneInterface);
            qCDebug(orgKdeUpnpLibQtSsdp()) << "joinMulticastGroup" << (result ? "true" : "false") << newQuerySocket->errorString();
        }
    }
    {
        auto &newStandardSocket = d->mSsdpStandardSocket.last();

        connect(newStandardSocket.data(), &QUdpSocket::readyRead, this, &UpnpSsdpEngine::standardReceivedData);

        newStandardSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
        newStandardSocket->setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

        auto result = newStandardSocket->bind(QHostAddress(QStringLiteral("239.255.255.250")), d->mPortNumber, QAbstractSocket::ShareAddress);
        qCDebug(orgKdeUpnpLibQtSsdp()) << "bind" << (result ? "true" : "false");
        result = newStandardSocket->joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")));
        qCDebug(orgKdeUpnpLibQtSsdp()) << "joinMulticastGroup" << (result ? "true" : "false") << newStandardSocket->errorString();
    }

    d->mServerInformation = QSysInfo::kernelType() + QStringLiteral(" ") + QSysInfo::kernelVersion() + QStringLiteral(" UPnP/1.0 ");
}

void UpnpSsdpEngine::parseSsdpQueryDatagram(const QByteArray &datagram, const QList<QByteArray> &headers)
{
    qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::parseSsdpQueryDatagram" << headers;
    UpnpSearchQuery newSearch;
    bool hasAddress = false;
    bool hasAnswerDelay = false;
    bool hasSearchTarget = false;

    for (const auto &header : headers) {
        if (header.startsWith("HOST")) {
            QString hostName;
            if ((header)[4] == ' ') {
                hostName = QString::fromLatin1(header.mid(7, header.length() - 8));
            } else {
                hostName = QString::fromLatin1(header.mid(6, header.length() - 7));
            }
            auto addressParts = hostName.split(QStringLiteral(":"));
            newSearch.mSearchHostAddress.setAddress(addressParts.first());
            newSearch.mSearchHostPort = static_cast<uint16_t>(addressParts.last().toInt());
            hasAddress = true;
        }
        if (header.startsWith("MAN")) {
            if (!header.contains("\"ssdp:discover\"")) {
                qCDebug(orgKdeUpnpLibQtSsdp()) << "not valid" << datagram;
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
    qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::parseSsdpAnnounceDatagram" << headers;
    UpnpDiscoveryResult newDiscovery;
    newDiscovery.setNTS(NotificationSubType::Invalid);

    for (const auto &header : headers) {
        if (header.startsWith("LOCATION") || header.startsWith("Location")) {
            newDiscovery.setLocation(QString::fromLatin1(header.mid(9, header.length() - 10).trimmed()));
        }
        if (header.startsWith("HOST:") || header.startsWith("Host:")) {
            newDiscovery.setLocation(QString::fromLatin1(header.mid(6, header.length() - 7).trimmed()));
        }
        if (header.startsWith("USN:")) {
            newDiscovery.setUSN(QString::fromLatin1(header.mid(4, header.length() - 5).trimmed()));
        }
        if (messageType == SsdpMessageType::queryAnswer && header.startsWith("ST")) {
            newDiscovery.setNT(QString::fromLatin1(header.mid(3, header.length() - 4).trimmed()));
        }
        if (messageType == SsdpMessageType::announce && header.startsWith("NT:")) {
            newDiscovery.setNT(QString::fromLatin1(header.mid(3, header.length() - 4).trimmed()));
        }
        if (messageType == SsdpMessageType::announce && header.startsWith("NTS:")) {
            if (header.endsWith("ssdp:alive\r")) {
                newDiscovery.setNTS(NotificationSubType::Alive);
            }
            if (header.endsWith("ssdp:byebye\r")) {
                newDiscovery.setNTS(NotificationSubType::ByeBye);
            }
            if (header.endsWith("ssdp:discover\r")) {
                newDiscovery.setNTS(NotificationSubType::Discover);
            }
        }
        if (header.startsWith("DATE:")) {
            newDiscovery.setAnnounceDate(QString::fromLatin1(header.mid(5, header.length() - 6).trimmed()));
        }
        if (header.startsWith("Cache-Control:") || header.startsWith("CACHE-CONTROL:")) {
            const QList<QByteArray> &splittedLine = header.mid(14, header.length() - 15).split('=');
            if (splittedLine.size() == 2) {
                newDiscovery.setCacheDuration(splittedLine.last().trimmed().toInt());
            }
        }
    }

    if (newDiscovery.location().isEmpty() || newDiscovery.usn().isEmpty() || newDiscovery.nt().isEmpty() || (messageType == SsdpMessageType::announce && newDiscovery.nts() == NotificationSubType::Invalid)) {
        qCDebug(orgKdeUpnpLibQtSsdp()) << "not decoded" << datagram;
        return;
    }

    if (newDiscovery.nts() == NotificationSubType::Alive || messageType == SsdpMessageType::queryAnswer) {
        qCDebug(orgKdeUpnpLibQtSsdp()) << "valid service announce";
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery.usn());

        if (itDiscovery != d->mDiscoveryResults.end()) {
            qCDebug(orgKdeUpnpLibQtSsdp()) << "refresh existing service";
            *itDiscovery = newDiscovery;
        } else {
            d->mDiscoveryResults[newDiscovery.usn()] = newDiscovery;
            itDiscovery = d->mDiscoveryResults.find(newDiscovery.usn());

            qCDebug(orgKdeUpnpLibQtSsdp()) << "new service" << newDiscovery;

            Q_EMIT newService(newDiscovery);
        }

        qCDebug(orgKdeUpnpLibQtSsdp()) << datagram;
        qCDebug(orgKdeUpnpLibQtSsdp()) << "AnnounceDate" << newDiscovery.announceDate();
        qCDebug(orgKdeUpnpLibQtSsdp()) << "CacheDuration" << newDiscovery.cacheDuration();

        qCDebug(orgKdeUpnpLibQtSsdp()) << "new service";
        qCDebug(orgKdeUpnpLibQtSsdp()) << "DeviceId:" << newDiscovery.usn();
        qCDebug(orgKdeUpnpLibQtSsdp()) << "DeviceType:" << newDiscovery.nt();
        qCDebug(orgKdeUpnpLibQtSsdp()) << "Location:" << newDiscovery.location();
        qCDebug(orgKdeUpnpLibQtSsdp()) << "new service";
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "DeviceId:" << searchResult->DeviceId;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "DeviceType:" << searchResult->DeviceType;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "ServiceType:" << searchResult->ServiceType;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "ServiceVer:" << searchResult->ServiceVer;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "Os:" << searchResult->Os;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "Date:" << searchResult->Date;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "Ext:" << searchResult->Ext;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "ErrCode:" << searchResult->ErrCode;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "Expires:" << searchResult->Expires;
        //        qCInfo(orgKdeUpnpLibQtSsdp()) << "DestAddr:" << QHostAddress(reinterpret_cast<const sockaddr *>(&searchResult->DestAddr));
    } else if (newDiscovery.nts() == NotificationSubType::ByeBye) {
        qCDebug(orgKdeUpnpLibQtSsdp()) << "ByeBye" << newDiscovery << d->mDiscoveryResults.keys();
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery.usn());
        if (itDiscovery != d->mDiscoveryResults.end()) {
            qCDebug(orgKdeUpnpLibQtSsdp()) << "removed device found";
            Q_EMIT removedService(newDiscovery);

            d->mDiscoveryResults.erase(itDiscovery);
        }
    }
}

void UpnpSsdpEngine::parseSsdpDatagram(const QByteArray &datagram)
{
    qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::parseSsdpDatagram" << datagram;
    const QList<QByteArray> &headers(datagram.split('\n'));

    if (!headers.last().isEmpty()) {
        qCDebug(orgKdeUpnpLibQtSsdp()) << "UpnpSsdpEngine::parseSsdpDatagram"
                                       << "out";
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
        qCDebug(orgKdeUpnpLibQtSsdp()) << "not decoded" << datagram;
        return;
    }
}

#include "moc_upnpssdpengine.cpp"
