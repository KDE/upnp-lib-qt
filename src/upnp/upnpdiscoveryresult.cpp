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
}

int UpnpDiscoveryResult::cacheDuration() const
{
    return mCacheDuration;
}

void UpnpDiscoveryResult::validityTimeout()
{
    Q_EMIT timeout(mUSN);
}


#include "moc_upnpdiscoveryresult.cpp"
