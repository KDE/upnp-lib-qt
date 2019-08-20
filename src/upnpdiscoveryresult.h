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

#ifndef UPNPDISCOVERYRESULT_H
#define UPNPDISCOVERYRESULT_H

#include "upnplibqt_export.h"

#include "upnpssdpengine.h"

#include <QString>
#include <QTimer>
#include <QDateTime>

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

    UpnpDiscoveryResult& operator=(const UpnpDiscoveryResult &other);

    UpnpDiscoveryResult& operator=(UpnpDiscoveryResult &&other);

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

