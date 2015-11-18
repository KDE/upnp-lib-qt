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
        QTest::addColumn<QList<QPointer<UpnpDiscoveryResult> > >("results");

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
                                     << QList<QPointer<UpnpDiscoveryResult> >({new UpnpDiscoveryResult(QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    new UpnpDiscoveryResult(QStringLiteral("upnp:rootdevice"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    new UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:device:MediaServer:1"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    new UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:service:ContentDirectory:1"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::urn:schemas-upnp-org:service:ContentDirectory:1"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    1800),
                                                                    new UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"),
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
                                     << QList<QPointer<UpnpDiscoveryResult> >({new UpnpDiscoveryResult(QStringLiteral("upnp:rootdevice"),
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
                                        << QList<QPointer<UpnpDiscoveryResult> >({new UpnpDiscoveryResult(QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e"),
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
                                        << QList<QPointer<UpnpDiscoveryResult> >({new UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:device:MediaServer:1"),
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
                                        << QList<QPointer<UpnpDiscoveryResult> >({new UpnpDiscoveryResult(QStringLiteral("urn:schemas-upnp-org:service:ConnectionManager:1"),
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
        QFETCH(QList<QPointer<UpnpDiscoveryResult> >, results);

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(ssdpRequest, ssdpAnswers));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), &UpnpSsdpEngine::newService);

        newEngine->searchUpnp(searchType, searchCriteria, 2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == results.size());

        for (int i = 0; i < results.size(); ++i) {
            auto firstService = newServiceSignal[i][0].value<QSharedPointer<UpnpDiscoveryResult> >();
            QVERIFY(firstService->mAnnounceDate == results[i]->mAnnounceDate);
            QVERIFY(firstService->mCacheDuration == results[i]->mCacheDuration);
            QVERIFY(firstService->mLocation == results[i]->mLocation);
            QVERIFY(firstService->nt() == results[i]->nt());
            QVERIFY(firstService->mNTS == results[i]->mNTS);
            QVERIFY(firstService->mUSN == results[i]->mUSN);
        }
    }


    void searchDeviceWaitTimeout_data()
    {
        QTest::addColumn<UpnpSsdpEngine::SEARCH_TYPE>("searchType");
        QTest::addColumn<QString>("searchCriteria");
        QTest::addColumn<QByteArray>("ssdpRequest");
        QTest::addColumn<QStringList>("ssdpAnswers");
        QTest::addColumn<QList<QPointer<UpnpDiscoveryResult> > >("results");

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
                                     << QList<QPointer<UpnpDiscoveryResult> >({new UpnpDiscoveryResult(QStringLiteral("upnp:rootdevice"),
                                                                    QStringLiteral("uuid:4d696e69-444c-164e-9d41-ecf4bb9c317e::upnp:rootdevice"),
                                                                    QStringLiteral("http://127.0.0.1:8200/rootDesc.xml"),
                                                                    NotificationSubType::Invalid,
                                                                    QStringLiteral("mar., 27 oct. 2015 21:03:35 G\x7F"),
                                                                    3)});
    }

    void searchDeviceWaitTimeout()
    {
        QFETCH(UpnpSsdpEngine::SEARCH_TYPE, searchType);
        QFETCH(QString, searchCriteria);
        QFETCH(QByteArray, ssdpRequest);
        QFETCH(QStringList, ssdpAnswers);
        QFETCH(QList<QPointer<UpnpDiscoveryResult> >, results);

        QScopedPointer<MockSsdpClient> newClient(new MockSsdpClient(ssdpRequest, ssdpAnswers));
        newClient->listen(11900);

        QScopedPointer<UpnpSsdpEngine> newEngine(new UpnpSsdpEngine);
        newEngine->setPort(11900);
        newEngine->initialize();

        QSignalSpy newServiceSignal(newEngine.data(), &UpnpSsdpEngine::newService);
        QSignalSpy removedServiceSignal(newEngine.data(), &UpnpSsdpEngine::removedService);

        newEngine->searchUpnp(searchType, searchCriteria, 2);

        QTest::qSleep(2000);
        newServiceSignal.wait();

        QVERIFY(newServiceSignal.size() == results.size());

        for (int i = 0; i < results.size(); ++i) {
            auto firstService = newServiceSignal[i][0].value<QSharedPointer<UpnpDiscoveryResult> >();
            QVERIFY(firstService->mAnnounceDate == results[i]->mAnnounceDate);
            QVERIFY(firstService->mCacheDuration == results[i]->mCacheDuration);
            QVERIFY(firstService->mLocation == results[i]->mLocation);
            QVERIFY(firstService->nt() == results[i]->nt());
            QVERIFY(firstService->mNTS == results[i]->mNTS);
            QVERIFY(firstService->mUSN == results[i]->mUSN);
        }

        removedServiceSignal.wait();

        QVERIFY(removedServiceSignal.count() > 0);
    }
};

QTEST_MAIN(SsdpTests)

#include "ssdptests.moc"
