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

#include "upnpQt_export.h"

#include "upnpabstractdevice.h"

#include <QtCore/QObject>

class UpnpControlAbstractDevicePrivate;
class QNetworkReply;
class UpnpControlAbstractService;

class UPNPQT_EXPORT UpnpControlAbstractDevice : public UpnpAbstractDevice
{

    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription* description
               READ description
               WRITE setDescription
               NOTIFY descriptionChanged)

public:
    explicit UpnpControlAbstractDevice(QObject *parent = 0);

    ~UpnpControlAbstractDevice();

    UpnpDeviceDescription* description() const;

    void setDescription(UpnpDeviceDescription *newDescription);

Q_SIGNALS:

    void inError();

    void descriptionChanged();

public Q_SLOTS:

private Q_SLOTS:

protected:

private:

    UpnpControlAbstractDevicePrivate *d;

};

#endif // UPNPDEVICEDISCOVERY_H