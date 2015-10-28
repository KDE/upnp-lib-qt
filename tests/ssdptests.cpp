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

    explicit MockSsdpClient(const QStringList &aAnswerData, QObject *parent = 0)
        : QObject(parent), mAnswerData(aAnswerData)
    {
    }

    void listen(quint16 portNumber)
    {
        connect(&mClientSocket, &QUdpSocket::readyRead, this, &MockSsdpClient::dataReceived);

        if (!mClientSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), portNumber, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint)) {
            int reuse = 1;
            if (setsockopt(mClientSocket.socketDescriptor(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
                qDebug() << "setsockopt() failed";
            }

            int bindResult = mClientSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), portNumber, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
            if (!bindResult) {
                qDebug() << "bind failed" << QHostAddress(QStringLiteral("239.255.255.250")) << mClientSocket.error() << mClientSocket.errorString();
            }
        }
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

            QTest::qSleep(1700);

            for (auto &answer : mAnswerData) {
                mClientSocket.writeDatagram(answer.toLatin1(), sender, senderPort);
            }
        }
    }

private:

    QUdpSocket mClientSocket;

    QStringList mAnswerData;
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
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: upnp:rootdevice\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                     QStringLiteral("HTTP/1.1 200 OK\r\n"
                                     "CACHE-CONTROL: max-age=1800\r\n"
                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                     "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                     "EXT:\r\n"
                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(hardCodedAnswer));
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
        QVERIFY(firstService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
        QVERIFY(firstService.mNT == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"));
        QVERIFY(firstService.mNTS == NotificationSubType::Invalid);
        QVERIFY(firstService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"));

        auto secondService = newServiceSignal[1][0].value<UpnpDiscoveryResult>();
        QVERIFY(secondService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(secondService.mCacheDuration == 1800);
        QVERIFY(secondService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
        QVERIFY(secondService.mNT == QStringLiteral("upnp:rootdevice"));
        QVERIFY(secondService.mNTS == NotificationSubType::Invalid);
        QVERIFY(secondService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"));

        auto thirdService = newServiceSignal[2][0].value<UpnpDiscoveryResult>();
        QVERIFY(thirdService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(thirdService.mCacheDuration == 1800);
        QVERIFY(thirdService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
        QVERIFY(thirdService.mNT == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"));
        QVERIFY(thirdService.mNTS == NotificationSubType::Invalid);
        QVERIFY(thirdService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1"));

        auto fourthService = newServiceSignal[3][0].value<UpnpDiscoveryResult>();
        QVERIFY(fourthService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fourthService.mCacheDuration == 1800);
        QVERIFY(fourthService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
        QVERIFY(fourthService.mNT == QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"));
        QVERIFY(fourthService.mNTS == NotificationSubType::Invalid);
        QVERIFY(fourthService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1"));

        auto fithService = newServiceSignal[4][0].value<UpnpDiscoveryResult>();
        QVERIFY(fithService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fithService.mCacheDuration == 1800);
        QVERIFY(fithService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
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
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(hardCodedAnswer));
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
        QVERIFY(secondService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
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
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(hardCodedAnswer));
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
        QVERIFY(firstService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
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
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n"),
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(hardCodedAnswer));
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
        QVERIFY(thirdService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
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
                                     "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                     "Content-Length: 0\r\n\r\n")
                                    });

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(hardCodedAnswer));
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
        QVERIFY(fithService.mLocation == QStringLiteral("http://192.168.6.51:8200/rootDesc.xml"));
        QVERIFY(fithService.mNT == QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"));
        QVERIFY(fithService.mNTS == NotificationSubType::Invalid);
        QVERIFY(fithService.mUSN == QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1"));
    }

};

QTEST_MAIN(SsdpTests)

#include "ssdptests.moc"
