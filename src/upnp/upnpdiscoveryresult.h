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

#include "upnpQt_export.h"

#include <QtCore/QObject>
#include <QtCore/QString>

enum class NotificationSubType
{
    Invalid,
    Alive,
    ByeBye,
    Discover,
};

class UPNPQT_EXPORT UpnpDiscoveryResult : public QObject
{
    Q_OBJECT

public:

    explicit UpnpDiscoveryResult(QObject *parent = 0);

    UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
                        NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration, QObject *parent = 0);

    virtual ~UpnpDiscoveryResult();

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

    /**
     * @brief mAnnounceDate contains the date sent in the SSDP message by the other side
     */
    QString mAnnounceDate;

    /**
     * @brief mCacheDuration duration of validity of the announce
     */
    int mCacheDuration;
};

#endif // UPNPDISCOVERYRESULT_H

