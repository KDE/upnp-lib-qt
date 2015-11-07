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

#ifndef UPNPABSTRACTDEVICE_H
#define UPNPABSTRACTDEVICE_H

#include "upnpQt_export.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QList>

class UpnpAbstractService;
class UpnpAbstractDevicePrivate;
class QIODevice;
class UpnpSearchQuery;
class UpnpSsdpEngine;
class UpnpDeviceDescription;
class UpnpServiceDescription;

class UPNPQT_EXPORT UpnpAbstractDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription* device
               READ device
               WRITE setDevice
               NOTIFY deviceChanged)

public:

    explicit UpnpAbstractDevice(QObject *parent = 0);

    virtual ~UpnpAbstractDevice();

    Q_INVOKABLE UpnpServiceDescription* serviceById(const QString &serviceId) const;

    Q_INVOKABLE UpnpServiceDescription* serviceByIndex(int serviceIndex) const;

    const QList<QSharedPointer<UpnpServiceDescription> > &services() const;

    QList<QString> servicesName() const;

    void setDevice(UpnpDeviceDescription *value);

    UpnpDeviceDescription* device();

    const UpnpDeviceDescription* device() const;

    int cacheControl() const;

    QIODevice* buildAndGetXmlDescription();

Q_SIGNALS:

    void deviceChanged();

public Q_SLOTS:

    void newSearchQuery(UpnpSsdpEngine *engine, const UpnpSearchQuery &searchQuery);

protected:

    int addService(QSharedPointer<UpnpServiceDescription> newService);

private:

    UpnpAbstractDevicePrivate *d;

};

#endif // UPNPABSTRACTDEVICE_H
