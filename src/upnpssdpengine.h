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
public:
    explicit UpnpSsdpEngine(QObject *parent = 0);

    virtual ~UpnpSsdpEngine();

Q_SIGNALS:

    void newSearchQuery(UpnpSsdpEngine *engine, const UpnpSearchQuery &searchQuery);

    void newService(const UpnpDiscoveryResult &serviceDiscovery);

    void removedService(const UpnpDiscoveryResult &serviceDiscovery);

public Q_SLOTS:

    /**
     * @brief searchAllUpnpDevice will trigger a search for all upnp device
     */
    bool searchAllUpnpDevice();

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
