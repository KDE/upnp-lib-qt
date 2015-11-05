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
#include "upnpdevicemodel.h"

#include <QtCore/QScopedPointer>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

#include <QtNetwork/QUdpSocket>

#include <QtTest/QtTest>

#include <sys/types.h>
#include <sys/socket.h>

class MockSsdpClient : public QObject
{
    Q_OBJECT

public:

    explicit MockSsdpClient(const QByteArray &aExpectedQuery, const QStringList &aAnswerData, QObject *parent = 0)
        : QObject(parent), mClientSocket(), mAnswerData(aAnswerData), mExpectedQuery(aExpectedQuery), mHttpClientSocket()
    {
    }

    void listen(quint16 portNumber)
    {
        connect(&mClientSocket, &QUdpSocket::readyRead, this, &MockSsdpClient::dataReceived);

        mClientSocket.bind(QHostAddress::AnyIPv4, portNumber, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
        mClientSocket.joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")));
        mClientSocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
        mClientSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

        connect(&mHttpClientSocket, &QUdpSocket::readyRead, this, &MockSsdpClient::httpDataReceived);

        mHttpClientSocket.bind(QHostAddress::AnyIPv4, 8200);
    }

public Q_SLOTS:

    void dataReceived()
    {
        while (mClientSocket.hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(mClientSocket.pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;

            mClientSocket.readDatagram(datagram.data(), datagram.size(),
                                       &sender, &senderPort);

            QVERIFY(datagram == mExpectedQuery);

            QTest::qSleep(1700);

            for (auto &answer : mAnswerData) {
                mClientSocket.writeDatagram(answer.toLatin1(), sender, senderPort);
            }
        }
    }

    void httpDataReceived()
    {
        while (mClientSocket.hasPendingDatagrams()) {
            QByteArray datagram;
            datagram.resize(mClientSocket.pendingDatagramSize());
            QHostAddress sender;
            quint16 senderPort;

            mClientSocket.readDatagram(datagram.data(), datagram.size(),
                                       &sender, &senderPort);

            qDebug() << "httpDataReceived" << datagram;
        }
    }

private:

    QUdpSocket mClientSocket;

    QStringList mAnswerData;

    QByteArray mExpectedQuery;

    QUdpSocket mHttpClientSocket;

};

class SsdpTests: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<UpnpSearchQuery>("UpnpSearchQuery");
    }

    void searchAll()
    {
        QStringList hardCodedAnswer({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: upnp:rootdevice\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient("M-SEARCH * HTTP/1.1\r\n"
                                                                    "HOST: 239.255.255.250:11900\r\n"
                                                                    "MAN: \"ssdp:discover\"\r\n"
                                                                    "MX: 2\r\n"
                                                                    "ST: ssdp:all\r\n\r\n",
                                                                    hardCodedAnswer));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchAllUpnpDevice(2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == 5);

        auto firstService = newServiceSignal[0][0].value<UpnpDiscoveryResult>();
        QVERIFY(firstService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(firstService.mCacheDuration == 1800);
        QVERIFY(firstService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(firstService.mNT == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"));
        QVERIFY(firstService.mNTS == NotificationSubType::Invalid);
        QVERIFY(firstService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"));

        auto secondService = newServiceSignal[1][0].value<UpnpDiscoveryResult>();
        QVERIFY(secondService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(secondService.mCacheDuration == 1800);
        QVERIFY(secondService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(secondService.mNT == QStringLiteral("upnp:rootdevice"));
        QVERIFY(secondService.mNTS == NotificationSubType::Invalid);
        QVERIFY(secondService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"));

        auto thirdService = newServiceSignal[2][0].value<UpnpDiscoveryResult>();
        QVERIFY(thirdService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(thirdService.mCacheDuration == 1800);
        QVERIFY(thirdService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(thirdService.mNT == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"));
        QVERIFY(thirdService.mNTS == NotificationSubType::Invalid);
        QVERIFY(thirdService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1"));

        auto fourthService = newServiceSignal[3][0].value<UpnpDiscoveryResult>();
        QVERIFY(fourthService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fourthService.mCacheDuration == 1800);
        QVERIFY(fourthService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(fourthService.mNT == QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"));
        QVERIFY(fourthService.mNTS == NotificationSubType::Invalid);
        QVERIFY(fourthService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1"));

        auto fithService = newServiceSignal[4][0].value<UpnpDiscoveryResult>();
        QVERIFY(fithService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fithService.mCacheDuration == 1800);
        QVERIFY(fithService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(fithService.mNT == QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"));
        QVERIFY(fithService.mNTS == NotificationSubType::Invalid);
        QVERIFY(fithService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1"));
    }

    void searchRootDevice()
    {
        QStringList hardCodedAnswer({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: upnp:rootdevice\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient("M-SEARCH * HTTP/1.1\r\n"
                                                                    "HOST: 239.255.255.250:11900\r\n"
                                                                    "MAN: \"ssdp:discover\"\r\n"
                                                                    "MX: 2\r\n"
                                                                    "ST: upnp:rootdevice\r\n\r\n",
                                                                    hardCodedAnswer));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchAllRootDevice(2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == 1);

        auto secondService = newServiceSignal[0][0].value<UpnpDiscoveryResult>();
        QVERIFY(secondService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(secondService.mCacheDuration == 1800);
        QVERIFY(secondService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(secondService.mNT == QStringLiteral("upnp:rootdevice"));
        QVERIFY(secondService.mNTS == NotificationSubType::Invalid);
        QVERIFY(secondService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"));
    }

    void searchByDeviceUUID()
    {
        QStringList hardCodedAnswer({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient("M-SEARCH * HTTP/1.1\r\n"
                                                                    "HOST: 239.255.255.250:11900\r\n"
                                                                    "MAN: \"ssdp:discover\"\r\n"
                                                                    "MX: 2\r\n"
                                                                    "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n\r\n",
                                                                    hardCodedAnswer));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchByDeviceUUID(QStringLiteral("4d696e69-444c-164e-9d41-ecf4bb9c317e"), 2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == 1);

        auto firstService = newServiceSignal[0][0].value<UpnpDiscoveryResult>();
        QVERIFY(firstService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(firstService.mCacheDuration == 1800);
        QVERIFY(firstService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(firstService.mNT == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"));
        QVERIFY(firstService.mNTS == NotificationSubType::Invalid);
        QVERIFY(firstService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"));
    }

    void searchByDeviceType()
    {
        QStringList hardCodedAnswer({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient("M-SEARCH * HTTP/1.1\r\n"
                                                                    "HOST: 239.255.255.250:11900\r\n"
                                                                    "MAN: \"ssdp:discover\"\r\n"
                                                                    "MX: 2\r\n"
                                                                    "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n\r\n",
                                                                    hardCodedAnswer));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchByDeviceType(QStringLiteral("schemas-upnp-org:device:MediaServer:1"), 2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == 1);

        auto thirdService = newServiceSignal[0][0].value<UpnpDiscoveryResult>();
        QVERIFY(thirdService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(thirdService.mCacheDuration == 1800);
        QVERIFY(thirdService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(thirdService.mNT == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"));
        QVERIFY(thirdService.mNTS == NotificationSubType::Invalid);
        QVERIFY(thirdService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1"));
    }

    void searchByServiceType()
    {
        QStringList hardCodedAnswer({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient("M-SEARCH * HTTP/1.1\r\n"
                                                                    "HOST: 239.255.255.250:11900\r\n"
                                                                    "MAN: \"ssdp:discover\"\r\n"
                                                                    "MX: 2\r\n"
                                                                    "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n\r\n",
                                                                    hardCodedAnswer));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchByServiceType(QStringLiteral("schemas-upnp-org:service:ConnectionManager:1"), 2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == 1);

        auto fithService = newServiceSignal[0][0].value<UpnpDiscoveryResult>();
        QVERIFY(fithService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fithService.mCacheDuration == 1800);
        QVERIFY(fithService.mLocation == QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"));
        QVERIFY(fithService.mNT == QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"));
        QVERIFY(fithService.mNTS == NotificationSubType::Invalid);
        QVERIFY(fithService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1"));
    }

    void searchAllWithModel()
    {
        QStringList hardCodedAnswer({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: upnp:rootdevice\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient("M-SEARCH * HTTP/1.1\r\n"
                                                                    "HOST: 239.255.255.250:11900\r\n"
                                                                    "MAN: \"ssdp:discover\"\r\n"
                                                                    "MX: 2\r\n"
                                                                    "ST: ssdp:all\r\n\r\n",
                                                                    hardCodedAnswer));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QScopedPointer<UpnpDeviceModel> newModel(new UpnpDeviceModel);
        connect(newEngine.data(), &UpnpSsdpEngine::newService, newModel.data(), &UpnpDeviceModel::newDevice);
        connect(newEngine.data(), &UpnpSsdpEngine::removedService, newModel.data(), &UpnpDeviceModel::removedDevice);

        QSignalSpy newDeviceSignal(newModel.data(), SIGNAL(rowsInserted(QModelIndex,int,int)));
        QSignalSpy newDeviceDataChangedSignal(newModel.data(), SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)));

        newEngine->searchAllUpnpDevice(2);

        QTest::qSleep(2000);
        newDeviceSignal.wait();

        QVERIFY(newDeviceSignal.size() == 1);

        auto rootIndex = newDeviceSignal[0][0].value<QModelIndex>();
        auto lowBound = newDeviceSignal[0][1].value<int>();
        auto upBound = newDeviceSignal[0][2].value<int>();

        QVERIFY(!rootIndex.parent().isValid());
        QVERIFY(lowBound == 0);
        QVERIFY(upBound == 0);

        auto newRowIndex = newModel->index(lowBound, 0, rootIndex);

        auto deviceName = newModel->data(newRowIndex, UpnpDeviceModel::NameRole).toString();
        auto deviceType = newModel->data(newRowIndex, UpnpDeviceModel::TypeRole).toString();
        auto deviceUUID = newModel->data(newRowIndex, UpnpDeviceModel::uuidRole).toString();
        auto deviceViewName = newModel->data(newRowIndex, UpnpDeviceModel::ViewNameRole).toString();
        auto deviceMobileViewName = newModel->data(newRowIndex, UpnpDeviceModel::MobileViewNameRole).toString();

        QVERIFY(deviceName == QStringLiteral(""));
        QVERIFY(deviceType == QStringLiteral(""));
        QVERIFY(deviceUUID == QStringLiteral("4d696e69-444c-164e-9d41-ecf4bb9c317e"));
        QVERIFY(deviceViewName == QStringLiteral("mediaServer.qml"));
        QVERIFY(deviceMobileViewName == QStringLiteral("mediaServerMobile.qml"));

        newDeviceDataChangedSignal.wait();

        qDebug() << newDeviceDataChangedSignal.size();

        QVERIFY(newDeviceDataChangedSignal.size() == 12);

        deviceName = newModel->data(newRowIndex, UpnpDeviceModel::NameRole).toString();
        deviceType = newModel->data(newRowIndex, UpnpDeviceModel::TypeRole).toString();
        deviceUUID = newModel->data(newRowIndex, UpnpDeviceModel::uuidRole).toString();
        deviceViewName = newModel->data(newRowIndex, UpnpDeviceModel::ViewNameRole).toString();
        deviceMobileViewName = newModel->data(newRowIndex, UpnpDeviceModel::MobileViewNameRole).toString();

        QVERIFY(deviceName == QStringLiteral("moulinette: minidlna"));
        QVERIFY(deviceType == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"));
        QVERIFY(deviceUUID == QStringLiteral("4d696e69-444c-164e-9d41-ecf4bb9c317e"));
        QVERIFY(deviceViewName == QStringLiteral("mediaServer.qml"));
        QVERIFY(deviceMobileViewName == QStringLiteral("mediaServerMobile.qml"));
    }
};

QTEST_MAIN(SsdpTests)

#include "ssdptests.moc"
