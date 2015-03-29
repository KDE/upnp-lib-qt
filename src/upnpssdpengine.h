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

#include <QObject>

enum class NotificationSubType
{
    Invalid,
    Alive,
    ByeBye,
    Discover,
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
};

Q_DECLARE_METATYPE(UpnpDiscoveryResult)

class UpnpSsdpEnginePrivate;

class UpnpSsdpEngine : public QObject
{
    Q_OBJECT
public:
    explicit UpnpSsdpEngine(QObject *parent = 0);

    virtual ~UpnpSsdpEngine();

Q_SIGNALS:

    void newService(const UpnpDiscoveryResult &serviceDiscovery);

    void removedService(const UpnpDiscoveryResult &serviceDiscovery);

public Q_SLOTS:

    /**
     * @brief searchAllUpnpDevice will trigger a search for all upnp device
     */
    bool searchAllUpnpDevice();

    void publishDevice(const QString &urlDevice);

private Q_SLOTS:

    void standardReceivedData();

    void queryReceivedData();

private:

    void parseSsdpDatagram(const QByteArray &datagram);

    UpnpSsdpEnginePrivate *d;
};

#endif
