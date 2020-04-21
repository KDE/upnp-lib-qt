/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPHTTPSERVER_H
#define UPNPHTTPSERVER_H

#include "upnplibqt_export.h"

#include <KDSoapServer/KDSoapServer.h>

#include <QObject>

#include <memory>

class UpnpControlAbstractService;
class UpnpHttpServerPrivate;

class UpnpHttpServer : public KDSoapServer
{
    Q_OBJECT
public:
    explicit UpnpHttpServer(QObject *parent = nullptr);

    ~UpnpHttpServer() override;

    QObject *createServerObject() Q_DECL_OVERRIDE;

    void setService(UpnpControlAbstractService *service);

private:
    std::unique_ptr<UpnpHttpServerPrivate> d;
};

#endif // UPNPHTTPSERVER_H
