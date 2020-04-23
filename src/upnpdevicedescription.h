/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPDEVICEDESCRIPTION_H
#define UPNPDEVICEDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QPointer>
#include <QVector>

#include <memory>

class UpnpServiceDescription;
class UpnpDeviceDescriptionPrivate;

class UPNPLIBQT_EXPORT UpnpDeviceDescription
{

public:
    UpnpDeviceDescription();

    UpnpDeviceDescription(const UpnpDeviceDescription &other);

    UpnpDeviceDescription(UpnpDeviceDescription &&other) noexcept;

    ~UpnpDeviceDescription();

    UpnpDeviceDescription &operator=(const UpnpDeviceDescription &other);

    UpnpDeviceDescription &operator=(UpnpDeviceDescription &&other) noexcept;

    [[nodiscard]] UpnpServiceDescription serviceById(const QString &serviceId) const;

    [[nodiscard]] const UpnpServiceDescription &serviceByIndex(int serviceIndex) const;

    [[nodiscard]] UpnpServiceDescription &serviceByIndex(int serviceIndex);

    [[nodiscard]] const QList<UpnpServiceDescription> &services() const;

    [[nodiscard]] QList<UpnpServiceDescription> &services();

    [[nodiscard]] QList<QString> servicesName() const;

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
    [[nodiscard]] const QString &UDN() const;

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
    [[nodiscard]] const QString &UPC() const;

    void setDeviceType(const QString &value);

    [[nodiscard]] const QString &deviceType() const;

    void setFriendlyName(const QString &value);

    [[nodiscard]] const QString &friendlyName() const;

    void setManufacturer(const QString &value);

    [[nodiscard]] const QString &manufacturer() const;

    void setManufacturerURL(const QUrl &value);

    [[nodiscard]] const QUrl &manufacturerURL() const;

    void setModelDescription(const QString &value);

    [[nodiscard]] const QString &modelDescription() const;

    void setModelName(const QString &value);

    [[nodiscard]] const QString &modelName() const;

    void setModelNumber(const QString &value);

    [[nodiscard]] const QString &modelNumber() const;

    void setModelURL(const QUrl &value);

    [[nodiscard]] const QUrl &modelURL() const;

    void setSerialNumber(const QString &value);

    [[nodiscard]] const QString &serialNumber() const;

    void setURLBase(const QString &value);

    [[nodiscard]] const QString &URLBase() const;

    void setCacheControl(int value);

    [[nodiscard]] int cacheControl() const;

    void setLocationUrl(const QUrl &value);

    [[nodiscard]] const QUrl &locationUrl() const;

    int addService(const UpnpServiceDescription &newService);

private:
    std::unique_ptr<UpnpDeviceDescriptionPrivate> d;
};

Q_DECLARE_TYPEINFO(UpnpDeviceDescription, Q_MOVABLE_TYPE);

Q_DECLARE_METATYPE(UpnpDeviceDescription)

#endif
