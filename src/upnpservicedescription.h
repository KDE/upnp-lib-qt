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

#ifndef UPNPSERVICECALLER_H
#define UPNPSERVICECALLER_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtCore/QUrl>

class UpnpServiceDescriptionPrivate;
class QNetworkReply;

class UpnpServiceDescription : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant serviceType
               READ serviceType
               WRITE setServiceType
               NOTIFY serviceTypeChanged)

    Q_PROPERTY(QVariant serviceId
               READ serviceId
               WRITE setServiceId
               NOTIFY serviceIdChanged)

public:

    explicit UpnpServiceDescription(QObject *parent = 0);

    ~UpnpServiceDescription();

    void setServiceType(const QVariant &newServiceType);

    const QVariant& serviceType() const;

    void setServiceId(const QVariant &newServiceId);

    const QVariant& serviceId() const;

Q_SIGNALS:

    void serviceTypeChanged();

    void serviceIdChanged();

public Q_SLOTS:

    void downloadAndParseServiceDescription(const QUrl &serviceUrl);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

private:

    UpnpServiceDescriptionPrivate *d;
};

#endif // UPNPSERVICECALLER_H
