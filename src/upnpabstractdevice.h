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

#include "upnplibqt_export.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QList>

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

    Q_PROPERTY(UpnpDeviceDescription* description
               READ description
               WRITE setDescription
               NOTIFY descriptionChanged)

public:

    explicit UpnpAbstractDevice(QObject *parent = nullptr);

    ~UpnpAbstractDevice() override;

    UpnpServiceDescription* serviceDescriptionById(const QString &serviceId) const;

    UpnpServiceDescription* serviceDescriptionByIndex(int serviceIndex) const;

    const QVector<QSharedPointer<UpnpServiceDescription> > &services() const;

    QVector<QString> servicesName() const;

    void setDescription(UpnpDeviceDescription *value);

    UpnpDeviceDescription* description();

    const UpnpDeviceDescription* description() const;

    int cacheControl() const;

    QIODevice* buildAndGetXmlDescription();

Q_SIGNALS:

    void descriptionChanged();

public Q_SLOTS:

    void newSearchQuery(UpnpSsdpEngine *engine, const UpnpSearchQuery &searchQuery);

protected:

    int addService(QSharedPointer<UpnpServiceDescription> newService);

private:

    std::unique_ptr<UpnpAbstractDevicePrivate> d;

};

#endif // UPNPABSTRACTDEVICE_H
