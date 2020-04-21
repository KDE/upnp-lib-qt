/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpdiscoveryresult.h"

class UpnpDiscoveryResultPrivate
{

public:
    UpnpDiscoveryResultPrivate() = default;

    UpnpDiscoveryResultPrivate(const QString &aNT, const QString &aUSN, const QString &aLocation,
        UpnpSsdpEngine::NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration)
        : mNT(aNT)
        , mUSN(aUSN)
        , mLocation(aLocation)
        , mNTS(aNTS)
        , mAnnounceDate(aAnnounceDate)
        , mCacheDuration(aCacheDuration)
    {
    }

    /**
     * @brief mNT contains the header ST (i.e. search target) or NT (i.e. notification type) sent in an ssdp message. This is useful to know the type of the discovered service.
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
    UpnpSsdpEngine::NotificationSubType mNTS = UpnpSsdpEngine::NotificationSubType::Invalid;

    /**
     * @brief mAnnounceDate contains the date sent in the SSDP message by the other side
     */
    QString mAnnounceDate;

    /**
     * @brief mCacheDuration duration of validity of the announce in seconds
     */
    int mCacheDuration = 1800;

    /**
     * @brief mTimestamp contains the date and time at which the result will expire
     */
    QDateTime mValidityTimestamp;
};

UpnpDiscoveryResult::UpnpDiscoveryResult()
    : d(new UpnpDiscoveryResultPrivate)
{
}

UpnpDiscoveryResult::UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
    UpnpSsdpEngine::NotificationSubType aNTS, const QString &aAnnounceDate,
    int aCacheDuration)
    : d(new UpnpDiscoveryResultPrivate(aNT, aUSN, aLocation, aNTS, aAnnounceDate, aCacheDuration))
{
    d->mValidityTimestamp = QDateTime::fromString(d->mAnnounceDate, QStringLiteral("ddd., d MMM. yy hh:mm:ss G\u007F"));
    if (!d->mValidityTimestamp.isValid()) {
        d->mValidityTimestamp = QDateTime::currentDateTime();
    }

    d->mValidityTimestamp = d->mValidityTimestamp.addSecs(d->mCacheDuration);
}

UpnpDiscoveryResult::UpnpDiscoveryResult(const UpnpDiscoveryResult &other)
    : d(new UpnpDiscoveryResultPrivate(*other.d))
{
}

UpnpDiscoveryResult::UpnpDiscoveryResult(UpnpDiscoveryResult &&other)
    : d(other.d.release())
{
}

UpnpDiscoveryResult &UpnpDiscoveryResult::operator=(const UpnpDiscoveryResult &other)
{
    if (&other != this) {
        d.reset(new UpnpDiscoveryResultPrivate(*other.d));
    }

    return *this;
}

UpnpDiscoveryResult &UpnpDiscoveryResult::operator=(UpnpDiscoveryResult &&other)
{
    if (&other != this) {
        d.reset(other.d.release());
    }

    return *this;
}

UpnpDiscoveryResult::~UpnpDiscoveryResult() = default;

void UpnpDiscoveryResult::setNT(const QString &value)
{
    d->mNT = value;
}

const QString &UpnpDiscoveryResult::nt() const
{
    return d->mNT;
}

void UpnpDiscoveryResult::setUSN(const QString &value)
{
    d->mUSN = value;
}

const QString &UpnpDiscoveryResult::usn() const
{
    return d->mUSN;
}

void UpnpDiscoveryResult::setLocation(const QString &value)
{
    d->mLocation = value;
}

const QString &UpnpDiscoveryResult::location() const
{
    return d->mLocation;
}

void UpnpDiscoveryResult::setNTS(UpnpSsdpEngine::NotificationSubType value)
{
    d->mNTS = value;
}

UpnpSsdpEngine::NotificationSubType UpnpDiscoveryResult::nts() const
{
    return d->mNTS;
}

void UpnpDiscoveryResult::setAnnounceDate(const QString &value)
{
    d->mAnnounceDate = value;

    d->mValidityTimestamp = QDateTime::fromString(d->mAnnounceDate, QStringLiteral("ddd., d MMM. yy hh:mm:ss G\u007F"));
    if (!d->mValidityTimestamp.isValid()) {
        d->mValidityTimestamp = QDateTime::currentDateTime();
    }

    d->mValidityTimestamp = d->mValidityTimestamp.addSecs(d->mCacheDuration);
}

const QString &UpnpDiscoveryResult::announceDate() const
{
    return d->mAnnounceDate;
}

void UpnpDiscoveryResult::setCacheDuration(int value)
{
    d->mCacheDuration = value;

    d->mValidityTimestamp = QDateTime::fromString(d->mAnnounceDate, QStringLiteral("ddd., d MMM. yy hh:mm:ss G\u007F"));
    if (!d->mValidityTimestamp.isValid()) {
        d->mValidityTimestamp = QDateTime::currentDateTime();
    }

    d->mValidityTimestamp = d->mValidityTimestamp.addSecs(d->mCacheDuration);
}

int UpnpDiscoveryResult::cacheDuration() const
{
    return d->mCacheDuration;
}

void UpnpDiscoveryResult::setValidityTimestamp(const QDateTime &value)
{
    d->mValidityTimestamp = value;
}

QDateTime UpnpDiscoveryResult::validityTimestamp() const
{
    return d->mValidityTimestamp;
}

UPNPLIBQT_EXPORT QDebug operator<<(QDebug stream, const UpnpDiscoveryResult &data)
{
    stream << data.location() << "usn" << data.usn() << "nt" << data.nt() << "nts" << data.nts() << "announce date" << data.announceDate() << "cache" << data.cacheDuration() << "valid until" << data.validityTimestamp();
    return stream;
}
