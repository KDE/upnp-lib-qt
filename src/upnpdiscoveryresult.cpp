/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpdiscoveryresult.h"

class UpnpDiscoveryResultPrivate
{

public:
    UpnpDiscoveryResultPrivate() = default;

    UpnpDiscoveryResultPrivate(QString aNT, QString aUSN, QString aLocation,
        UpnpSsdpEngine::NotificationSubType aNTS, QString aAnnounceDate, int aCacheDuration)
        : mNT(std::move(aNT))
        , mUSN(std::move(aUSN))
        , mLocation(std::move(aLocation))
        , mAnnounceDate(std::move(aAnnounceDate))
        , mNTS(aNTS)
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
     * @brief mAnnounceDate contains the date sent in the SSDP message by the other side
     */
    QString mAnnounceDate;

    /**
     * @brief mTimestamp contains the date and time at which the result will expire
     */
    QDateTime mValidityTimestamp;

    /**
     * @brief mNTS contains the header NTS (i.e. notification sub type) sent in an ssdp message
     */
    UpnpSsdpEngine::NotificationSubType mNTS = UpnpSsdpEngine::NotificationSubType::Invalid;

    /**
     * @brief mCacheDuration duration of validity of the announce in seconds
     */
    int mCacheDuration = 1800;
};

UpnpDiscoveryResult::UpnpDiscoveryResult()
    : d(std::make_unique<UpnpDiscoveryResultPrivate>())
{
}

UpnpDiscoveryResult::UpnpDiscoveryResult(QString aNT, QString aUSN, QString aLocation,
    UpnpSsdpEngine::NotificationSubType aNTS, QString aAnnounceDate,
    int aCacheDuration)
    : d(std::make_unique<UpnpDiscoveryResultPrivate>(std::move(aNT), std::move(aUSN),
                                                     std::move(aLocation), aNTS,
                                                     std::move(aAnnounceDate), aCacheDuration))
{
    d->mValidityTimestamp = QDateTime::fromString(d->mAnnounceDate, QStringLiteral("ddd., d MMM. yy hh:mm:ss G\u007F"));
    if (!d->mValidityTimestamp.isValid()) {
        d->mValidityTimestamp = QDateTime::currentDateTime();
    }

    d->mValidityTimestamp = d->mValidityTimestamp.addSecs(d->mCacheDuration);
}

UpnpDiscoveryResult::UpnpDiscoveryResult(const UpnpDiscoveryResult &other)
    : d(std::make_unique<UpnpDiscoveryResultPrivate>(*other.d))
{
}

UpnpDiscoveryResult::UpnpDiscoveryResult(UpnpDiscoveryResult &&other)
    : d(other.d.release())
{
}

UpnpDiscoveryResult &UpnpDiscoveryResult::operator=(const UpnpDiscoveryResult &other)
{
    if (&other != this) {
        d = std::make_unique<UpnpDiscoveryResultPrivate>(*other.d);
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
