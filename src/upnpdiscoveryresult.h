/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPDISCOVERYRESULT_H
#define UPNPDISCOVERYRESULT_H

#include "upnplibqt_export.h"

#include "upnpssdpengine.h"

#include <QDateTime>
#include <QString>
#include <QTimer>

#include <memory>

class UpnpDiscoveryResultPrivate;
class QDebug;

class UPNPLIBQT_EXPORT UpnpDiscoveryResult
{

public:
    UpnpDiscoveryResult();

    UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
        UpnpSsdpEngine::NotificationSubType aNTS, const QString &aAnnounceDate,
        int aCacheDuration);

    UpnpDiscoveryResult(const UpnpDiscoveryResult &other);

    UpnpDiscoveryResult(UpnpDiscoveryResult &&other);

    ~UpnpDiscoveryResult();

    UpnpDiscoveryResult &operator=(const UpnpDiscoveryResult &other);

    UpnpDiscoveryResult &operator=(UpnpDiscoveryResult &&other);

    void setNT(const QString &value);

    const QString &nt() const;

    void setUSN(const QString &value);

    const QString &usn() const;

    void setLocation(const QString &value);

    const QString &location() const;

    void setNTS(UpnpSsdpEngine::NotificationSubType value);

    UpnpSsdpEngine::NotificationSubType nts() const;

    void setAnnounceDate(const QString &value);

    const QString &announceDate() const;

    void setCacheDuration(int value);

    int cacheDuration() const;

    void setValidityTimestamp(const QDateTime &value);

    QDateTime validityTimestamp() const;

private:
    std::unique_ptr<UpnpDiscoveryResultPrivate> d;
};

UPNPLIBQT_EXPORT QDebug operator<<(QDebug stream, const UpnpDiscoveryResult &data);

Q_DECLARE_TYPEINFO(UpnpDiscoveryResult, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(UpnpDiscoveryResult)

#endif // UPNPDISCOVERYRESULT_H
