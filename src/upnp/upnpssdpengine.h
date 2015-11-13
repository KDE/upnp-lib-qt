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

#include "upnpQt_export.h"

#include <QtCore/QObject>
#include <QtNetwork/QHostAddress>

enum class NotificationSubType
{
    Invalid,
    Alive,
    ByeBye,
    Discover,
};

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

struct UpnpDiscoveryResult
{
    UpnpDiscoveryResult()
        : mNT(), mUSN(), mLocation(), mNTS(), mAnnounceDate(), mCacheDuration()
    {
    }

    UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
                        NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration)
        : mNT(aNT), mUSN(aUSN), mLocation(aLocation), mNTS(aNTS), mAnnounceDate(aAnnounceDate), mCacheDuration(aCacheDuration)
    {
    }

    /**
     * @brief mNT contains the header ST (i.e. search target) or NT (i.e. notification type) sent in an ssdp message
     */
    QString mNT;

    QString mUSN;

    QString mLocation;

    /**
     * @brief mNTS contains the header NTS (i.e. notification sub type) sent in an ssdp message
     */
    NotificationSubType mNTS;

    /**
     * @brief mAnnounceDate contains the date sent in the SSDP message by the other side
     */
    QString mAnnounceDate;

    /**
     * @brief mCacheDuration duration of validity of the announce
     */
    int mCacheDuration;
};

Q_DECLARE_METATYPE(UpnpDiscoveryResult)

class UpnpAbstractDevice;
class UpnpSsdpEnginePrivate;

class UPNPQT_EXPORT UpnpSsdpEngine : public QObject
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

    enum SEARCH_TYPE
    {
        AllDevices,
        RootDevices,
        DeviceByUUID,
        DeviceByType,
        ServiceByType,
    };

    Q_ENUM(SEARCH_TYPE)

    explicit UpnpSsdpEngine(QObject *parent = 0);

    virtual ~UpnpSsdpEngine();

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

public Q_SLOTS:

    /**
     * @brief searchUpnpDevice will trigger a search for upnp device depending on the parameters
     */
    bool searchUpnp(SEARCH_TYPE searchType, const QString &searchCriteria, int maxDelay = 1);

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

private:

    void parseSsdpDatagram(const QByteArray &datagram);

    void parseSsdpQueryDatagram(const QByteArray &datagram, const QList<QByteArray> &headers);

    void parseSsdpAnnounceDatagram(const QByteArray &datagram, const QList<QByteArray> &headers, SsdpMessageType messageType);

    UpnpSsdpEnginePrivate *d;
};

#endif
