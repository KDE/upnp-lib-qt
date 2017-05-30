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
#include <QtCore/QTimer>

#include <memory>

enum class NotificationSubType
{
    Invalid,
    Alive,
    ByeBye,
    Discover,
};

class UpnpDiscoveryResultPrivate;

class UPNPQT_EXPORT UpnpDiscoveryResult : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString nt
               READ nt
               WRITE setNT
               NOTIFY ntChanged)

    Q_PROPERTY(QString usn
               READ usn
               WRITE setUSN
               NOTIFY usnChanged)

    Q_PROPERTY(QString location
               READ location
               WRITE setLocation
               NOTIFY locationChanged)

    Q_PROPERTY(NotificationSubType nts
               READ nts
               WRITE setNTS
               NOTIFY ntsChanged)

    Q_PROPERTY(QString announceDate
               READ announceDate
               WRITE setAnnounceDate
               NOTIFY announceDateChanged)

    Q_PROPERTY(int cacheDuration
               READ cacheDuration
               WRITE setCacheDuration
               NOTIFY cacheDurationChanged)

public:

    explicit UpnpDiscoveryResult(QObject *parent = 0);

    UpnpDiscoveryResult(const QString &aNT, const QString &aUSN, const QString &aLocation,
                        NotificationSubType aNTS, const QString &aAnnounceDate, int aCacheDuration, QObject *parent = 0);

    virtual ~UpnpDiscoveryResult();

    void setNT(const QString &value);

    const QString &nt() const;

    void setUSN(const QString &value);

    const QString &usn() const;

    void setLocation(const QString &value);

    const QString &location() const;

    void setNTS(NotificationSubType value);

    NotificationSubType nts() const;

    void setAnnounceDate(const QString &value);

    const QString &announceDate() const;

    void setCacheDuration(int value);

    int cacheDuration() const;

    void discoveryIsAlive();

Q_SIGNALS:

    void timeout(const QString &usn);

    void ntChanged();

    void usnChanged();

    void locationChanged();

    void ntsChanged();

    void announceDateChanged();

    void cacheDurationChanged();

public Q_SLOTS:

    void validityTimeout();

private:

    std::unique_ptr<UpnpDiscoveryResultPrivate> d;

};

#endif // UPNPDISCOVERYRESULT_H

