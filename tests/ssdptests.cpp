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
#include <QtCore/QDebug>

#include <QtNetwork/QUdpSocket>

#include <QtTest/QtTest>

#include <sys/types.h>
#include <sys/socket.h>

class MockSsdpClient : public QObject
{
    Q_OBJECT

public:

    MockSsdpClient(QObject *parent = 0) : QObject(parent)
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

            mClientSocket.writeDatagram("HTTP/1.1 200 OK\r\n"
                                        "CACHE-CONTROL: max-age=1800\r\n"
                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                        "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                        "EXT:\r\n"
                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                        "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                        "Content-Length: 0\r\n\r\n", sender, senderPort);

            mClientSocket.writeDatagram("HTTP/1.1 200 OK\r\n"
                                        "CACHE-CONTROL: max-age=1800\r\n"
                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                        "ST: upnp:rootdevice\r\n"
                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice\r\n"
                                        "EXT:\r\n"
                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                        "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                        "Content-Length: 0\r\n\r\n", sender, senderPort);

            mClientSocket.writeDatagram("HTTP/1.1 200 OK\r\n"
                                        "CACHE-CONTROL: max-age=1800\r\n"
                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                        "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                        "EXT:\r\n"
                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                        "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                        "Content-Length: 0\r\n\r\n", sender, senderPort);

            mClientSocket.writeDatagram("HTTP/1.1 200 OK\r\n"
                                        "CACHE-CONTROL: max-age=1800\r\n"
                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                        "ST: urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1\r\n"
                                        "EXT:\r\n"
                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                        "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                        "Content-Length: 0\r\n\r\n", sender, senderPort);

            mClientSocket.writeDatagram("HTTP/1.1 200 OK\r\n"
                                        "CACHE-CONTROL: max-age=1800\r\n"
                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                        "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                        "EXT:\r\n"
                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                        "LOCATION: http://192.168.6.51:8200/rootDesc.xml\r\n"
                                        "Content-Length: 0\r\n\r\n", sender, senderPort);
        }
    }

private:

    QUdpSocket mClientSocket;
};

class SsdpTests: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<UpnpSearchQuery>("UpnpSearchQuery");
    }

    void simpleTest()
    {
        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient);
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchAllUpnpDevice(2);

        newServiceSignal.wait(2000);

        QVERIFY(newServiceSignal.size() == 5);

        auto firstService = newServiceSignal[0][0].value<UpnpDiscoveryResult>();
        QVERIFY(firstService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(firstService.mCacheDuration == 1800);

        auto secondService = newServiceSignal[1][0].value<UpnpDiscoveryResult>();
        QVERIFY(secondService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(secondService.mCacheDuration == 1800);

        auto thirdService = newServiceSignal[2][0].value<UpnpDiscoveryResult>();
        QVERIFY(thirdService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(thirdService.mCacheDuration == 1800);

        auto fourthService = newServiceSignal[3][0].value<UpnpDiscoveryResult>();
        QVERIFY(fourthService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fourthService.mCacheDuration == 1800);

        auto fithService = newServiceSignal[4][0].value<UpnpDiscoveryResult>();
        QVERIFY(fithService.mAnnounceDate == QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"));
        QVERIFY(fithService.mCacheDuration == 1800);
    }

};

QTEST_MAIN(SsdpTests)

#include "ssdptests.moc"
