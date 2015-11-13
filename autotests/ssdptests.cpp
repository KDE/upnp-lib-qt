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

    explicit MockSsdpClient(const QByteArray &aExpectedQuery, const QStringList &aAnswerData, QObject *parent = nullptr)
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

    void searchAll_data()
    {
        QTest::addColumn<UpnpSsdpEngine::SEARCH_TYPE>("searchType");
        QTest::addColumn<QString>("searchCriteria");
        QTest::addColumn<QByteArray>("ssdpRequest");
        QTest::addColumn<QStringList>("ssdpAnswers");
        QTest::addColumn<QList<UpnpDiscoveryResult> >("results");

        QTest::newRow("all devices") << UpnpSsdpEngine::AllDevices
                                     << QString()
                                     << QByteArray("M-SEARCH * HTTP/1.1\r\n"
                                                   "HOST: 239.255.255.250:11900\r\n"
                                                   "MAN: \"ssdp:discover\"\r\n"
                                                   "MX: 2\r\n"
                                                   "ST: ssdp:all\r\n\r\n")
                                     << QStringList({QStringLiteral("HTTP/1.1 200 OK\r\n"
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
                                                    })
                                     << QList<UpnpDiscoveryResult>({UpnpDiscoveryResult(QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    UpnpDiscoveryResult(QStringLiteral("upnp:rootdevice"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800)});

        QTest::newRow("root device") << UpnpSsdpEngine::RootDevices
                                     << QString()
                                     << QByteArray("M-SEARCH * HTTP/1.1\r\n"
                                                   "HOST: 239.255.255.250:11900\r\n"
                                                   "MAN: \"ssdp:discover\"\r\n"
                                                   "MX: 2\r\n"
                                                   "ST: upnp:rootdevice\r\n\r\n")
                                     << QStringList({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                                     "CACHE-CONTROL: max-age=1800\r\n"
                                                     "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                                     "ST: upnp:rootdevice\r\n"
                                                     "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice\r\n"
                                                     "EXT:\r\n"
                                                     "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                                     "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                                     "Content-Length: 0\r\n\r\n")
                                                    })
                                     << QList<UpnpDiscoveryResult>({UpnpDiscoveryResult(QStringLiteral("upnp:rootdevice"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800)});

        QTest::newRow("device by uuid") << UpnpSsdpEngine::DeviceByUUID
                                        << QStringLiteral("4d696e69-444c-164e-9d41-ecf4bb9c317e")
                                        << QByteArray("M-SEARCH * HTTP/1.1\r\n"
                                                      "HOST: 239.255.255.250:11900\r\n"
                                                      "MAN: \"ssdp:discover\"\r\n"
                                                      "MX: 2\r\n"
                                                      "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n\r\n")
                                        << QStringList({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                                        "CACHE-CONTROL: max-age=1800\r\n"
                                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                                        "ST: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e\r\n"
                                                        "EXT:\r\n"
                                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                                        "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                                        "Content-Length: 0\r\n\r\n"),
                                                       })
                                        << QList<UpnpDiscoveryResult>({UpnpDiscoveryResult(QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
                                                                       QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
                                                                       QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                       NotificationSubType::Invalid,
                                                                       QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                       1800),
                                                                      });

        QTest::newRow("device by type") << UpnpSsdpEngine::DeviceByType
                                        << QStringLiteral("schemas-upnp-org:device:MediaServer:1")
                                        << QByteArray("M-SEARCH * HTTP/1.1\r\n"
                                                      "HOST: 239.255.255.250:11900\r\n"
                                                      "MAN: \"ssdp:discover\"\r\n"
                                                      "MX: 2\r\n"
                                                      "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n\r\n")
                                        << QStringList({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                                        "CACHE-CONTROL: max-age=1800\r\n"
                                                        "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                                        "ST: urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                                        "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1\r\n"
                                                        "EXT:\r\n"
                                                        "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                                        "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                                        "Content-Length: 0\r\n\r\n"),
                                                       })
                                        << QList<UpnpDiscoveryResult>({UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"),
                                                                       QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1"),
                                                                       QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                       NotificationSubType::Invalid,
                                                                       QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                       1800),
                                                                      });

        QTest::newRow("service by type") << UpnpSsdpEngine::ServiceByType
                                        << QStringLiteral("schemas-upnp-org:service:ConnectionManager:1")
                                        << QByteArray("M-SEARCH * HTTP/1.1\r\n"
                                                      "HOST: 239.255.255.250:11900\r\n"
                                                      "MAN: \"ssdp:discover\"\r\n"
                                                      "MX: 2\r\n"
                                                      "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n\r\n")
                                        << QStringList({QStringLiteral("HTTP/1.1 200 OK\r\n"
                                               "CACHE-CONTROL: max-age=1800\r\n"
                                               "DATE: mar., 27 oct. 2015 21:03:35 G\x7F\r\n"
                                               "ST: urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                               "USN: uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1\r\n"
                                               "EXT:\r\n"
                                               "SERVER: Debian DLNADOC/1.50 UPnP/1.0 MiniDLNA/1.1.4\r\n"
                                               "LOCATION: http://127.0.0.1:8200/rootDesc.xml\r\n"
                                               "Content-Length: 0\r\n\r\n")
                                              })
                                        << QList<UpnpDiscoveryResult>({UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"),
                                                                       QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ConnectionManager:1"),
                                                                       QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                       NotificationSubType::Invalid,
                                                                       QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                       1800),
                                                                      });
    }

    void searchAll()
    {
        QFETCH(UpnpSsdpEngine::SEARCH_TYPE, searchType);
        QFETCH(QString, searchCriteria);
        QFETCH(QByteArray, ssdpRequest);
        QFETCH(QStringList, ssdpAnswers);
        QFETCH(QList<UpnpDiscoveryResult>, results);

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(ssdpRequest, ssdpAnswers));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), SIGNAL(newService(const UpnpDiscoveryResult&)));

        newEngine->searchUpnp(searchType, searchCriteria, 2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == results.size());

        for (int i = 0; i < results.size(); ++i) {
            auto firstService = newServiceSignal[i][0].value<UpnpDiscoveryResult>();
            QVERIFY(firstService.mAnnounceDate == results[i].mAnnounceDate);
            QVERIFY(firstService.mCacheDuration == results[i].mCacheDuration);
            QVERIFY(firstService.mLocation == results[i].mLocation);
            QVERIFY(firstService.mNT == results[i].mNT);
            QVERIFY(firstService.mNTS == results[i].mNTS);
            QVERIFY(firstService.mUSN == results[i].mUSN);
        }
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

        QVERIFY(deviceName == QStringLiteral(""));
        QVERIFY(deviceType == QStringLiteral(""));
        QVERIFY(deviceUUID == QStringLiteral("4d696e69-444c-164e-9d41-ecf4bb9c317e"));

        newDeviceDataChangedSignal.wait();

        qDebug() << newDeviceDataChangedSignal.size();

        QVERIFY(newDeviceDataChangedSignal.size() == 12);

        deviceName = newModel->data(newRowIndex, UpnpDeviceModel::NameRole).toString();
        deviceType = newModel->data(newRowIndex, UpnpDeviceModel::TypeRole).toString();
        deviceUUID = newModel->data(newRowIndex, UpnpDeviceModel::uuidRole).toString();

        QVERIFY(deviceName == QStringLiteral("moulinette: minidlna"));
        QVERIFY(deviceType == QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"));
        QVERIFY(deviceUUID == QStringLiteral("4d696e69-444c-164e-9d41-ecf4bb9c317e"));
    }
};

QTEST_MAIN(SsdpTests)

#include "ssdptests.moc"
