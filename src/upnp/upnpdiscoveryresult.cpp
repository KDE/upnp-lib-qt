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

void UpnpDiscoveryResult::validityTimeout()
{
    Q_EMIT timeout(mUSN);
}


#include "moc_upnpdiscoveryresult.cpp"
