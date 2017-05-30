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

#include "upnpdiscoveryresult.h"

class UpnpDiscoveryResultPrivate
{

public:

    UpnpDiscoveryResultPrivate() = default;

    UpnpDiscoveryResultPrivate(const QString &aNT, const QString &aUSN, const QString &aLocation,
                               NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration)
        : mNT(aNT), mUSN(aUSN), mLocation(aLocation), mNTS(aNTS), mAnnounceDate(aAnnounceDate), mCacheDuration(aCacheDuration)
    {
    }

    /**
     * @brief mNT contains the header ST (i.e. search target) or NT (i.e. notification type) sent in an ssdp message. This is usefull to know the type of the discovered service.
     */
    QString mNT;

    /**
     * @brief mUSN contains the header USN (i.e. unique service name) sent in an ssdp message. This uniquely identify the discovered service.
     */
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

    QTimer mValidityTimer;

};

UpnpDiscoveryResult::UpnpDiscoveryResult(QObject *parent)
    : QObject(parent), d(new UpnpDiscoveryResultPrivate)
{
    connect(&d->mValidityTimer, &QTimer::timeout, this, &UpnpDiscoveryResult::validityTimeout);
}

UpnpDiscoveryResult::UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
                    NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration, QObject *parent)
    : QObject(parent), d(new UpnpDiscoveryResultPrivate(aNT, aUSN, aLocation, aNTS, aAnnounceDate, aCacheDuration))
{
    connect(&d->mValidityTimer, &QTimer::timeout, this, &UpnpDiscoveryResult::validityTimeout);
}

UpnpDiscoveryResult::~UpnpDiscoveryResult()
{
}

void UpnpDiscoveryResult::setNT(const QString &value)
{
    d->mNT = value;
    Q_EMIT ntChanged();
}

const QString &UpnpDiscoveryResult::nt() const
{
    return d->mNT;
}

void UpnpDiscoveryResult::setUSN(const QString &value)
{
    d->mUSN = value;
    Q_EMIT usnChanged();
}

const QString &UpnpDiscoveryResult::usn() const
{
    return d->mUSN;
}

void UpnpDiscoveryResult::setLocation(const QString &value)
{
    d->mLocation = value;
    Q_EMIT locationChanged();
}

const QString &UpnpDiscoveryResult::location() const
{
    return d->mLocation;
}

void UpnpDiscoveryResult::setNTS(NotificationSubType value)
{
    d->mNTS = value;
    Q_EMIT ntsChanged();
}

NotificationSubType UpnpDiscoveryResult::nts() const
{
    return d->mNTS;
}

void UpnpDiscoveryResult::setAnnounceDate(const QString &value)
{
    d->mAnnounceDate = value;
    Q_EMIT announceDateChanged();
}

const QString &UpnpDiscoveryResult::announceDate() const
{
    return d->mAnnounceDate;
}

void UpnpDiscoveryResult::setCacheDuration(int value)
{
    d->mCacheDuration = value;
    Q_EMIT cacheDurationChanged();

    d->mValidityTimer.setSingleShot(false);
    d->mValidityTimer.start(d->mCacheDuration * 1000);
}

int UpnpDiscoveryResult::cacheDuration() const
{
    return d->mCacheDuration;
}

void UpnpDiscoveryResult::discoveryIsAlive()
{

    d->mValidityTimer.start(d->mCacheDuration * 1000);
}

void UpnpDiscoveryResult::validityTimeout()
{
    Q_EMIT timeout(d->mUSN);
}


#include "moc_upnpdiscoveryresult.cpp"
