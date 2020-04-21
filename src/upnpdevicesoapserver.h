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

class UPNPLIBQT_EXPORT UpnpDeviceSoapServer : public KDSoapServer
{
    Q_OBJECT
public:
    explicit UpnpDeviceSoapServer(QObject *parent = nullptr);

    ~UpnpDeviceSoapServer() override;

    int addDevice(UpnpAbstractDevice *device);

    void removeDevice(int index);

    QObject *createServerObject() Q_DECL_OVERRIDE;

    QUrl urlPrefix() const;

private:
    std::unique_ptr<UpnpDeviceSoapServerPrivate> d;
};

#endif
