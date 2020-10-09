/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPDEVICESOAPSERVER_H
#define UPNPDEVICESOAPSERVER_H

#include "upnplibqt_export.h"

#include <KDSoapServer/KDSoapServer.h>

#include <QObject>

#include <memory>

class UpnpAbstractDevice;
class UpnpDeviceSoapServerPrivate;

/**
 * @brief The UpnpDeviceSoapServer class should be used to implement an UPnP device that will be providing services
 */
class UPNPLIBQT_EXPORT UpnpDeviceSoapServer : public KDSoapServer
{
    Q_OBJECT
public:
    explicit UpnpDeviceSoapServer(QObject *parent = nullptr);

    ~UpnpDeviceSoapServer() override;

    int addDevice(UpnpAbstractDevice *device);

    void removeDevice(int index);

    QObject *createServerObject() override;

    [[nodiscard]] QUrl urlPrefix() const;

private:
    std::unique_ptr<UpnpDeviceSoapServerPrivate> d;
};

#endif
