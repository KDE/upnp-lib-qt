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

#ifndef UPNPDEVICEDESCRIPTION_H
#define UPNPDEVICEDESCRIPTION_H

#include "upnpQt_export.h"

#include <QtCore/QPointer>
#include <QtCore/QVector>

#include <QtCore/QObject>

class UpnpServiceDescription;
class UpnpDeviceDescriptionPrivate;

class UPNPQT_EXPORT UpnpDeviceDescription : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString UDN
               READ UDN
               WRITE setUDN
               NOTIFY UDNChanged)

    Q_PROPERTY(QString UPC
               READ UPC
               WRITE setUPC
               NOTIFY UDNChanged)

    Q_PROPERTY(QString deviceType
               READ deviceType
               WRITE setDeviceType
               NOTIFY deviceTypeChanged)

    Q_PROPERTY(QString friendlyName
               READ friendlyName
               WRITE setFriendlyName
               NOTIFY friendlyNameChanged)

    Q_PROPERTY(QString manufacturer
               READ manufacturer
               WRITE setManufacturer
               NOTIFY manufacturerChanged)

    Q_PROPERTY(QUrl manufacturerURL
               READ manufacturerURL
               WRITE setManufacturerURL
               NOTIFY manufacturerURLChanged)

    Q_PROPERTY(QString modelDescription
               READ modelDescription
               WRITE setModelDescription
               NOTIFY modelDescriptionChanged)

    Q_PROPERTY(QString modelName
               READ modelName
               WRITE setModelName
               NOTIFY modelNameChanged)

    Q_PROPERTY(QString modelNumber
               READ modelNumber
               WRITE setModelNumber
               NOTIFY modelNumberChanged)

    Q_PROPERTY(QUrl modelURL
               READ modelURL
               WRITE setModelURL
               NOTIFY modelURLChanged)

    Q_PROPERTY(QString serialNumber
               READ serialNumber
               WRITE setSerialNumber
               NOTIFY serialNumberChanged)

    Q_PROPERTY(QString URLBase
               READ URLBase
               WRITE setURLBase
               NOTIFY URLBaseChanged)

    Q_PROPERTY(int cacheControl
               READ cacheControl
               WRITE setCacheControl
               NOTIFY cacheControlChanged)

    Q_PROPERTY(QUrl locationUrl
               READ locationUrl
               WRITE setLocationUrl
               NOTIFY locationUrlChanged)

public:

    explicit UpnpDeviceDescription(QObject *parent = 0);

    ~UpnpDeviceDescription() override;

    const QSharedPointer<UpnpServiceDescription> serviceById(const QString &serviceId) const;

    const QSharedPointer<UpnpServiceDescription> serviceByIndex(int serviceIndex) const;

    const QVector<QSharedPointer<UpnpServiceDescription> >& services() const;

    QVector<QSharedPointer<UpnpServiceDescription> >& services();

    QVector<QString> servicesName() const;

    /**
     * @brief setUDN will set the UDN (i.e. Unique Device Name) of this device
     *
     * setUDN will set the UDN (i.e. Unique Device Name) of this device. It is required.
     * Universally-unique identifier for the device, whether
     * root or embedded. Must be the same over time for a specific device instance (i.e.,
     * must survive reboots). Must match the value of the NT header in device discovery
     * messages. Must match the prefix of the USN header in all discovery messages. Must
     * begin with uuid: followed by a UUID suffix specified by a UPnP vendor. Single URI.
     *
     * @param value will be the new UDN of this device
     */
    void setUDN(const QString &value);

    /**
     * @brief UDN will return the UDN (i.e. Unique Device Name) of this device
     *
     * UDN will return the UDN (i.e. Unique Device Name) of this device It is required.
     * Universally-unique identifier for the device, whether
     * root or embedded. Must be the same over time for a specific device instance (i.e.,
     * must survive reboots). Must match the value of the NT header in device discovery
     * messages. Must match the prefix of the USN header in all discovery messages. Must
     * begin with uuid: followed by a UUID suffix specified by a UPnP vendor. Single URI.
     */
    const QString& UDN() const;

    /**
     * @brief setUPC: Set Universal Product Code
     * Setting UPC is optional. 12-digit, all-numeric code that identifies the consumer package.
     * Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
     *
     * @param value will be the new UPC of this device
     */
    void setUPC(const QString &value);

    /**
     * @brief UPC: Get Universal Product Code
     * UPC is optional. 12-digit, all-numeric code that identifies the consumer package.
     * Managed by the Uniform Code Council. Specified by UPnP vendor. Single UPC.
     */
    const QString& UPC() const;

    void setDeviceType(const QString &value);

    const QString& deviceType() const;

    void setFriendlyName(const QString &value);

    const QString& friendlyName() const;

    void setManufacturer(const QString &value);

    const QString& manufacturer() const;

    void setManufacturerURL(const QUrl &value);

    const QUrl& manufacturerURL() const;

    void setModelDescription(const QString &value);

    const QString& modelDescription() const;

    void setModelName(const QString &value);

    const QString& modelName() const;

    void setModelNumber(const QString &value);

    const QString& modelNumber() const;

    void setModelURL(const QUrl &value);

    const QUrl& modelURL() const;

    void setSerialNumber(const QString &value);

    const QString& serialNumber() const;

    void setURLBase(const QString &value);

    const QString& URLBase() const;

    void setCacheControl(int value);

    int cacheControl() const;

    void setLocationUrl(const QUrl &value);

    const QUrl& locationUrl() const;

    int addService(const QSharedPointer<UpnpServiceDescription> &newService);

Q_SIGNALS:

    void UDNChanged(const QString &UDN);

    void UPCChanged(const QString &UDN);

    void deviceTypeChanged(const QString &UDN);

    void friendlyNameChanged(const QString &UDN);

    void manufacturerChanged(const QString &UDN);

    void manufacturerURLChanged(const QString &UDN);

    void modelDescriptionChanged(const QString &UDN);

    void modelNameChanged(const QString &UDN);

    void modelNumberChanged(const QString &UDN);

    void modelURLChanged(const QString &UDN);

    void serialNumberChanged(const QString &UDN);

    void URLBaseChanged(const QString &UDN);

    void cacheControlChanged(const QString &UDN);

    void locationUrlChanged(const QString &UDN);

private:

    UpnpDeviceDescriptionPrivate *d;

};

#endif
