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
class UpnpAbstractServiceDescription;

class UpnpDeviceDescription : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString UUID
               READ UUID
               WRITE setUUID
               NOTIFY UUIDChanged)

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

    const QString& UUID() const;

    void setUUID(const QString &uuid);

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

    Q_INVOKABLE UpnpAbstractServiceDescription* serviceById(const QString &serviceId) const;

Q_SIGNALS:

    void UUIDChanged();

    void UDNChanged(const QString &uuid);

    void UPCChanged(const QString &uuid);

    void deviceTypeChanged(const QString &uuid);

    void friendlyNameChanged(const QString &uuid);

    void manufacturerChanged(const QString &uuid);

    void manufacturerURLChanged(const QString &uuid);

    void modelDescriptionChanged(const QString &uuid);

    void modelNameChanged(const QString &uuid);

    void modelNumberChanged(const QString &uuid);

    void modelURLChanged(const QString &uuid);

    void serialNumberChanged(const QString &uuid);

    void URLBaseChanged(const QString &uuid);

public Q_SLOTS:

    void downloadAndParseDeviceDescription(const QUrl &serviceUrl);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

private:

    UpnpDeviceDiscoveryPrivate *d;

};

#endif // UPNPDEVICEDISCOVERY_H
