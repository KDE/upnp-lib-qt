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

#ifndef UPNPDEVICEDISCOVERY_H
#define UPNPDEVICEDISCOVERY_H

#include <QtCore/QObject>

class UpnpDeviceDiscoveryPrivate;
class QNetworkReply;

class UpnpDeviceDescription : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant UDN
               READ UDN
               NOTIFY UDNChanged)

    Q_PROPERTY(QVariant UPC
               READ UPC
               NOTIFY UDNChanged)

    Q_PROPERTY(QVariant deviceType
               READ deviceType
               NOTIFY deviceTypeChanged)

    Q_PROPERTY(QVariant friendlyName
               READ friendlyName
               NOTIFY friendlyNameChanged)

    Q_PROPERTY(QVariant manufacturer
               READ manufacturer
               NOTIFY manufacturerChanged)

    Q_PROPERTY(QVariant manufacturerURL
               READ manufacturerURL
               NOTIFY manufacturerURLChanged)

    Q_PROPERTY(QVariant modelDescription
               READ modelDescription
               NOTIFY modelDescriptionChanged)

    Q_PROPERTY(QVariant modelName
               READ modelName
               NOTIFY modelNameChanged)

    Q_PROPERTY(QVariant modelNumber
               READ modelNumber
               NOTIFY modelNumberChanged)

    Q_PROPERTY(QVariant modelURL
               READ modelURL
               NOTIFY modelURLChanged)

    Q_PROPERTY(QVariant serialNumber
               READ serialNumber
               NOTIFY serialNumberChanged)

    Q_PROPERTY(QVariant URLBase
               READ URLBase
               NOTIFY URLBaseChanged)

public:
    explicit UpnpDeviceDescription(QObject *parent = 0);

    ~UpnpDeviceDescription();

    const QVariant& UDN();

    const QVariant& UPC();

    const QVariant& deviceType();

    const QVariant& friendlyName();

    const QVariant& manufacturer();

    const QVariant& manufacturerURL();

    const QVariant& modelDescription();

    const QVariant& modelName();

    const QVariant& modelNumber();

    const QVariant& modelURL();

    const QVariant& serialNumber();

    const QVariant& URLBase();

Q_SIGNALS:

    void UDNChanged();

    void UPCChanged();

    void deviceTypeChanged();

    void friendlyNameChanged();

    void manufacturerChanged();

    void manufacturerURLChanged();

    void modelDescriptionChanged();

    void modelNameChanged();

    void modelNumberChanged();

    void modelURLChanged();

    void serialNumberChanged();

    void URLBaseChanged();

public Q_SLOTS:

    void downloadAndParseDeviceDescription(const QUrl &serviceUrl);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

private:

    UpnpDeviceDiscoveryPrivate *d;

};

#endif // UPNPDEVICEDISCOVERY_H
