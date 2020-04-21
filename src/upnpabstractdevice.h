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

class UPNPLIBQT_EXPORT UpnpAbstractDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription *description
            READ description
                WRITE setDescription
                    NOTIFY descriptionChanged)

public:
    explicit UpnpAbstractDevice(QObject *parent = nullptr);

    ~UpnpAbstractDevice() override;

    UpnpServiceDescription serviceDescriptionById(const QString &serviceId) const;

    UpnpServiceDescription serviceDescriptionByIndex(int serviceIndex) const;

    const QList<UpnpServiceDescription> &services() const;

    QVector<QString> servicesName() const;

    void setDescription(UpnpDeviceDescription *value);

    UpnpDeviceDescription *description();

    const UpnpDeviceDescription *description() const;

    int cacheControl() const;

    QIODevice *buildAndGetXmlDescription();

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
