#include "upnpdiscoveryresult.h"

UpnpDiscoveryResult::UpnpDiscoveryResult(QObject *parent)
    : QObject(parent), mNT(), mUSN(), mLocation(), mNTS(), mAnnounceDate(), mCacheDuration()
{
}

UpnpDiscoveryResult::UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
                    NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration, QObject *parent)
    : QObject(parent), mNT(aNT), mUSN(aUSN), mLocation(aLocation), mNTS(aNTS), mAnnounceDate(aAnnounceDate), mCacheDuration(aCacheDuration)
{
}

UpnpDiscoveryResult::~UpnpDiscoveryResult()
{
}


#include "moc_upnpdiscoveryresult.cpp"
