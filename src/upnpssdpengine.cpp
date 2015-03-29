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

#include <QtNetwork/QHostAddress>

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QSet>

#include <QtNetwork/QUdpSocket>

#include <sys/types.h>          /* Consultez NOTES */
#include <sys/socket.h>

class UpnpSsdpEnginePrivate
{
public:

    QHash<QString, UpnpDiscoveryResult> mDiscoveryResults;

    QUdpSocket mSsdpQuerySocket;

    QUdpSocket mSsdpStandardSocket;
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

void UpnpSsdpEngine::publishDevice(const QString &urlDevice)
{
}

#if 0
int UpnpSsdpEngine::upnpInternalCallBack(Upnp_EventType EventType, void *Event)
{
    switch(EventType)
    {
    case UPNP_CONTROL_ACTION_REQUEST:
        //qDebug() << "UPNP_CONTROL_ACTION_REQUEST";
        break;
    case UPNP_CONTROL_ACTION_COMPLETE:
        //qDebug() << "UPNP_CONTROL_ACTION_COMPLETE";
        break;
    case UPNP_CONTROL_GET_VAR_REQUEST:
        //qDebug() << "UPNP_CONTROL_GET_VAR_REQUEST";
        break;
    case UPNP_CONTROL_GET_VAR_COMPLETE:
        //qDebug() << "UPNP_CONTROL_GET_VAR_COMPLETE";
        break;
    case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
    case UPNP_DISCOVERY_SEARCH_RESULT:
    {
        Upnp_Discovery *searchResult = static_cast<Upnp_Discovery*>(Event);

        if (searchResult->DeviceType[0] == 0) {
            break;
        }

        qDebug() << "UPNP_DISCOVERY_SEARCH_RESULT" << "UPNP_DISCOVERY_ADVERTISEMENT_ALIVE";

        const QString &deviceIdString(QString::fromLatin1(searchResult->DeviceId));

        auto itDiscovery = d->mDiscoveryResults.find(deviceIdString);
        if (itDiscovery == d->mDiscoveryResults.end()) {
            d->mDiscoveryResults[deviceIdString] = *searchResult;

            qDebug() << "new service";
            qDebug() << "DeviceId:" << searchResult->DeviceId;
            qDebug() << "DeviceType:" << searchResult->DeviceType;
            qDebug() << "Expires:" << searchResult->Expires;
#if 0
                qDebug() << "new service";
                qDebug() << "DeviceId:" << searchResult->DeviceId;
                qDebug() << "DeviceType:" << searchResult->DeviceType;
                qDebug() << "ServiceType:" << searchResult->ServiceType;
                qDebug() << "ServiceVer:" << searchResult->ServiceVer;
                qDebug() << "Location:" << searchResult->Location;
                qDebug() << "Os:" << searchResult->Os;
                qDebug() << "Date:" << searchResult->Date;
                qDebug() << "Ext:" << searchResult->Ext;
                qDebug() << "ErrCode:" << searchResult->ErrCode;
                qDebug() << "Expires:" << searchResult->Expires;
                qDebug() << "DestAddr:" << QHostAddress(reinterpret_cast<const sockaddr *>(&searchResult->DestAddr));
#endif

                Q_EMIT newService(d->mDiscoveryResults[deviceIdString]);
        }

        break;
    }
    case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
    {
        qDebug() << "UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE";
        Upnp_Discovery *searchResult = static_cast<Upnp_Discovery*>(Event);

        qDebug() << "removed service";
        qDebug() << "DeviceId:" << searchResult->DeviceId;
        qDebug() << "DeviceType:" << searchResult->DeviceType;
        qDebug() << "Expires:" << searchResult->Expires;
#if 0
        qDebug() << "ServiceType:" << searchResult->ServiceType;
        qDebug() << "ServiceVer:" << searchResult->ServiceVer;
        qDebug() << "Location:" << searchResult->Location;
        qDebug() << "Os:" << searchResult->Os;
        qDebug() << "Date:" << searchResult->Date;
        qDebug() << "Ext:" << searchResult->Ext;
        qDebug() << "ErrCode:" << searchResult->ErrCode;
        qDebug() << "Expires:" << searchResult->Expires;
        qDebug() << "DestAddr:" << QHostAddress(reinterpret_cast<const sockaddr *>(&searchResult->DestAddr));
#endif


        const QString &deviceIdString(QString::fromLatin1(searchResult->DeviceId));

        auto itDiscovery = d->mDiscoveryResults.find(deviceIdString);
        if (itDiscovery != d->mDiscoveryResults.end()) {
            Q_EMIT removedService(*itDiscovery);

            qDebug() << "UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE" << searchResult->DeviceId;

            d->mDiscoveryResults.erase(itDiscovery);
        }

        break;
    }
    case UPNP_DISCOVERY_SEARCH_TIMEOUT:
        qDebug() << "UPNP_DISCOVERY_SEARCH_TIMEOUT";
        Q_EMIT searchTimeOut();
        break;
    case UPNP_EVENT_SUBSCRIPTION_REQUEST:
        //qDebug() << "UPNP_EVENT_SUBSCRIPTION_REQUEST";
        break;
    case UPNP_EVENT_RECEIVED:
        //qDebug() << "UPNP_EVENT_RECEIVED";
        break;
    case UPNP_EVENT_RENEWAL_COMPLETE:
        //qDebug() << "UPNP_EVENT_RENEWAL_COMPLETE";
        break;
    case UPNP_EVENT_SUBSCRIBE_COMPLETE:
        qDebug() << "UPNP_EVENT_SUBSCRIBE_COMPLETE";
        break;
    case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
        //qDebug() << "UPNP_EVENT_UNSUBSCRIBE_COMPLETE";
        break;
    case UPNP_EVENT_AUTORENEWAL_FAILED:
        //qDebug() << "UPNP_EVENT_AUTORENEWAL_FAILED";
        break;
    case UPNP_EVENT_SUBSCRIPTION_EXPIRED:
        //qDebug() << "UPNP_EVENT_SUBSCRIPTION_EXPIRED";
        break;
    }

    // return value is ignored according to libupnp documentation
    return 0;
}
#endif

void UpnpSsdpEngine::standardReceivedData()
{
    qDebug() << "UpnpSsdpEngine::standardReceivedData";

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
    qDebug() << "UpnpSsdpEngine::queryReceivedData";

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

    if (headers[0].startsWith("M-SEARCH * HTTP/1.1")) {
        qDebug() << "query";
    }
    if (headers[0].startsWith("HTTP/1.1 200 OK\r")) {
        qDebug() << "query answer";
    }
    if (headers[0].startsWith("NOTIFY * HTTP/1.1")) {
        qDebug() << "auto announce";
    }

    UpnpDiscoveryResult newDiscovery;

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
        if (itLine->startsWith("ST")) {
            if ((*itLine)[3] == ' ') {
                newDiscovery.mST = QString::fromLatin1(itLine->mid(4, itLine->length() - 5));
            } else {
                newDiscovery.mST = QString::fromLatin1(itLine->mid(3, itLine->length() - 4));
            }
        }
        if (itLine->startsWith("NT")) {
            if ((*itLine)[3] == ' ') {
                newDiscovery.mST = QString::fromLatin1(itLine->mid(4, itLine->length() - 5));
            } else {
                newDiscovery.mST = QString::fromLatin1(itLine->mid(3, itLine->length() - 4));
            }
        }
    }

    if (newDiscovery.mLocation.isEmpty() || newDiscovery.mUSN.isEmpty() || newDiscovery.mST.isEmpty()) {
        qDebug() << datagram;
        return;
    }

    if (newDiscovery.mST.startsWith(QStringLiteral("urn:schemas-upnp-org:device:"))) {
        auto itDiscovery = d->mDiscoveryResults.find(newDiscovery.mUSN);
        if (itDiscovery == d->mDiscoveryResults.end()) {
            d->mDiscoveryResults[newDiscovery.mUSN] = newDiscovery;

            qDebug() << "new service";
            qDebug() << "DeviceId:" << newDiscovery.mUSN;
            qDebug() << "DeviceType:" << newDiscovery.mST;
            qDebug() << "Location:" << newDiscovery.mLocation;
#if 0
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

            Q_EMIT newService(newDiscovery);
        }
    }

}

#include "moc_upnpssdpengine.cpp"
