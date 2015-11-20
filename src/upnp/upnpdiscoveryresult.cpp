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

UpnpDiscoveryResult::UpnpDiscoveryResult(QObject *parent)
    : QObject(parent), mNT(), mUSN(), mLocation(), mNTS(), mAnnounceDate(), mCacheDuration(), mValidityTimer()
{
    connect(&mValidityTimer, &QTimer::timeout, this, &UpnpDiscoveryResult::validityTimeout);
}

UpnpDiscoveryResult::UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
                    NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration, QObject *parent)
    : QObject(parent), mNT(aNT), mUSN(aUSN), mLocation(aLocation), mNTS(aNTS),
      mAnnounceDate(aAnnounceDate), mCacheDuration(aCacheDuration), mValidityTimer()
{
    connect(&mValidityTimer, &QTimer::timeout, this, &UpnpDiscoveryResult::validityTimeout);
}

UpnpDiscoveryResult::~UpnpDiscoveryResult()
{
}

void UpnpDiscoveryResult::setNT(const QString &value)
{
    mNT = value;
    Q_EMIT ntChanged();
}

const QString &UpnpDiscoveryResult::nt() const
{
    return mNT;
}

void UpnpDiscoveryResult::setUSN(const QString &value)
{
    mUSN = value;
    Q_EMIT usnChanged();
}

const QString &UpnpDiscoveryResult::usn() const
{
    return mUSN;
}

void UpnpDiscoveryResult::setLocation(const QString &value)
{
    mLocation = value;
    Q_EMIT locationChanged();
}

const QString &UpnpDiscoveryResult::location() const
{
    return mLocation;
}

void UpnpDiscoveryResult::setNTS(NotificationSubType value)
{
    mNTS = value;
    Q_EMIT ntsChanged();
}

NotificationSubType UpnpDiscoveryResult::nts() const
{
    return mNTS;
}

void UpnpDiscoveryResult::setAnnounceDate(const QString &value)
{
    mAnnounceDate = value;
    Q_EMIT announceDateChanged();
}

const QString &UpnpDiscoveryResult::announceDate() const
{
    return mAnnounceDate;
}

void UpnpDiscoveryResult::setCacheDuration(int value)
{
    mCacheDuration = value;
    Q_EMIT cacheDurationChanged();

    mValidityTimer.setSingleShot(false);
    mValidityTimer.start(mCacheDuration * 1000);
}

int UpnpDiscoveryResult::cacheDuration() const
{
    return mCacheDuration;
}

void UpnpDiscoveryResult::discoveryIsAlive()
{

    mValidityTimer.start(mCacheDuration * 1000);
}

void UpnpDiscoveryResult::validityTimeout()
{
    Q_EMIT timeout(mUSN);
}


#include "moc_upnpdiscoveryresult.cpp"
