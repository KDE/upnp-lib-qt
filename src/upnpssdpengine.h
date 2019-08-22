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

#ifndef UPNPSSDPENGINE_H
#define UPNPSSDPENGINE_H

#include "upnplibqt_export.h"

#include <QHostAddress>
#include <QNetworkConfiguration>

#include <QObject>
#include <QString>

#include <memory>

enum class SearchTargetType
{
    All,
    RootDevice,
    DeviceUUID,
    DeviceType,
    ServiceType,
};

enum class SsdpMessageType
{
    query,
    queryAnswer,
    announce,
    invalid,
};

struct UpnpSearchQuery
{
    /**
     * @brief mSearchHostAddress is the host address to which we should answer
     */
    QHostAddress mSearchHostAddress;

    /**
     * @brief mSearchHostPort is the host port to which we should answer
     */
    quint16 mSearchHostPort;

    /**
     * @brief mSearchTargetType is the search target type as defined by enum SearchTargetType
     */
    SearchTargetType mSearchTargetType;

    /**
     * @brief mSearchTarget is the search target string that we should use when determining if we should answer
     */
    QString mSearchTarget;

    /**
     * @brief mAnswerDelay is the delay defined by UPnP SSDP that will be interpreted as the maximum delay before sending the answer
     */
    int mAnswerDelay;
};

class UpnpAbstractDevice;
class UpnpDiscoveryResult;
class UpnpSsdpEnginePrivate;

class UPNPLIBQT_EXPORT UpnpSsdpEngine : public QObject
{

    Q_OBJECT

    Q_PROPERTY(quint16 port
               READ port
               WRITE setPort
               NOTIFY portChanged)

    Q_PROPERTY(bool canExportServices
               READ canExportServices
               WRITE setCanExportServices
               NOTIFY canExportServicesChanged)

public:

    enum class NotificationSubType
    {
        Invalid,
        Alive,
        ByeBye,
        Discover,
    };

    Q_ENUM(NotificationSubType)

    enum SEARCH_TYPE
    {
        AllDevices,
        RootDevices,
        DeviceByUUID,
        DeviceByType,
        ServiceByType,
    };

    Q_ENUM(SEARCH_TYPE)

    explicit UpnpSsdpEngine(QObject *parent = nullptr);

    ~UpnpSsdpEngine() override;

    quint16 port() const;

    void setPort(quint16 value);

    bool canExportServices() const;

    void setCanExportServices(bool value);

    Q_INVOKABLE void initialize();

Q_SIGNALS:

    void newSearchQuery(UpnpSsdpEngine *engine, const UpnpSearchQuery &searchQuery);

    void newService(const UpnpDiscoveryResult &serviceDiscovery);

    void removedService(const UpnpDiscoveryResult &serviceDiscovery);

    void portChanged();

    void canExportServicesChanged();

    void networkChanged();

public Q_SLOTS:

    /**
     * @brief searchUpnpDevice will trigger a search for upnp device depending on the parameters
     */
    bool searchUpnp(UpnpSsdpEngine::SEARCH_TYPE searchType, const QString &searchCriteria, int maxDelay = 1);

    /**
     * @brief searchAllUpnpDevice will trigger a search for all upnp device
     */
    bool searchAllUpnpDevice(int maxDelay = 1);

    /**
     * @brief searchAllRootDevice will trigger a search for all upnp root device
     */
    bool searchAllRootDevice(int maxDelay = 1);

    /**
     * @brief searchByDeviceUUID will trigger a search by device UUID
     */
    bool searchByDeviceUUID(const QString &uuid, int maxDelay = 1);

    /**
     * @brief searchByDeviceType will trigger a search by device type
     */
    bool searchByDeviceType(const QString &upnpDeviceType, int maxDelay = 1);

    /**
     * @brief searchByServiceType will trigger a search by service type
     */
    bool searchByServiceType(const QString &upnpServiceType, int maxDelay = 1);

    void subscribeDevice(UpnpAbstractDevice *device);

    void publishDevice(UpnpAbstractDevice *device);

private Q_SLOTS:

    void standardReceivedData();

    void queryReceivedData();

    void discoveryResultTimeout();

    void networkConfigurationAdded(const QNetworkConfiguration &config);

    void networkConfigurationRemoved(const QNetworkConfiguration &config);

    void networkConfigurationChanged(const QNetworkConfiguration &config);

    void networkOnlineStateChanged(bool isOnline);

    void networkUpdateCompleted();

private:

    void reconfigureNetwork();

    void parseSsdpDatagram(const QByteArray &datagram);

    void parseSsdpQueryDatagram(const QByteArray &datagram, const QList<QByteArray> &headers);

    void parseSsdpAnnounceDatagram(const QByteArray &datagram, const QList<QByteArray> &headers, SsdpMessageType messageType);

    std::unique_ptr<UpnpSsdpEnginePrivate> d;
};

#endif
