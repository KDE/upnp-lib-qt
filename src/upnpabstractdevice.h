/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPABSTRACTDEVICE_H
#define UPNPABSTRACTDEVICE_H

#include "upnplibqt_export.h"

#include <QList>
#include <QObject>
#include <QPointer>

#include <memory>

class UpnpAbstractService;
class UpnpAbstractDevicePrivate;
class QIODevice;
struct UpnpSearchQuery;
class UpnpSsdpEngine;
class UpnpDeviceDescription;
class UpnpServiceDescription;

/**
 * @brief The UpnpAbstractDevice is the base class to implement a wrapper around an UPnP device
 *
 * It can be derived into a subclass to implement support for a specific device type or can be used directly to interact with a device.
 */
class UPNPLIBQT_EXPORT UpnpAbstractDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription description
            READ description
                WRITE setDescription
                    NOTIFY descriptionChanged)

public:
    explicit UpnpAbstractDevice(QObject *parent = nullptr);

    ~UpnpAbstractDevice() override;

    [[nodiscard]] UpnpServiceDescription serviceDescriptionById(const QString &serviceId) const;

    [[nodiscard]] UpnpServiceDescription serviceDescriptionByIndex(int serviceIndex) const;

    [[nodiscard]] const QList<UpnpServiceDescription> &services() const;

    [[nodiscard]] QVector<QString> servicesName() const;

    void setDescription(UpnpDeviceDescription value);

    [[nodiscard]] UpnpDeviceDescription& description();

    [[nodiscard]] const UpnpDeviceDescription &description() const;

    [[nodiscard]] int cacheControl() const;

    [[nodiscard]] std::unique_ptr<QIODevice> buildAndGetXmlDescription();

Q_SIGNALS:

    void descriptionChanged();

public Q_SLOTS:

    void newSearchQuery(UpnpSsdpEngine *engine, const UpnpSearchQuery &searchQuery);

protected:
    int addService(const UpnpServiceDescription &newService);

private:
    std::unique_ptr<UpnpAbstractDevicePrivate> d;
};

#endif // UPNPABSTRACTDEVICE_H
