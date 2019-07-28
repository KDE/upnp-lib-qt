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

#include "upnplibqt_export.h"

#include "upnpabstractdevice.h"

#include <QtCore/QObject>

#include <memory>

class UpnpControlAbstractDevicePrivate;
class QNetworkReply;
class UpnpControlAbstractService;

class UPNPLIBQT_EXPORT UpnpControlAbstractDevice : public UpnpAbstractDevice
{

    Q_OBJECT

public:
    explicit UpnpControlAbstractDevice(QObject *parent = nullptr);

    ~UpnpControlAbstractDevice() override;

    Q_INVOKABLE UpnpControlAbstractService* serviceById(const QString &serviceId) const;

    Q_INVOKABLE UpnpControlAbstractService* serviceByIndex(int serviceIndex) const;

Q_SIGNALS:

    void inError();

public Q_SLOTS:

private Q_SLOTS:

protected:

    UpnpControlAbstractService* serviceFromDescription(UpnpServiceDescription *description) const;

private:

    std::unique_ptr<UpnpControlAbstractDevicePrivate> d;

};

#endif // UPNPDEVICEDISCOVERY_H
