/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPCONTROLABSTRACTDEVICE_H
#define UPNPCONTROLABSTRACTDEVICE_H

#include "upnplibqt_export.h"

#include "upnpabstractdevice.h"

#include <QObject>

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

    [[nodiscard]] std::unique_ptr<UpnpControlAbstractService> serviceById(const QString &serviceId) const;

    [[nodiscard]] std::unique_ptr<UpnpControlAbstractService> serviceByIndex(int serviceIndex) const;

Q_SIGNALS:

    void inError();

public Q_SLOTS:

private Q_SLOTS:

protected:
    [[nodiscard]] std::unique_ptr<UpnpControlAbstractService> serviceFromDescription(const UpnpServiceDescription &description) const;

private:
    std::unique_ptr<UpnpControlAbstractDevicePrivate> d;
};

#endif // UPNPDEVICEDISCOVERY_H
