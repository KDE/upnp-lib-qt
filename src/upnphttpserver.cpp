/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnphttpserver.h"
#include "upnpcontrolabstractservice.h"
#include "upnpservereventobject.h"

class UpnpHttpServerPrivate
{
public:
    UpnpControlAbstractService *mService;
};

UpnpHttpServer::UpnpHttpServer(QObject *parent)
    : KDSoapServer(parent)
    , d(std::make_unique<UpnpHttpServerPrivate>())
{
    d->mService = nullptr;
}

UpnpHttpServer::~UpnpHttpServer() = default;

QObject *UpnpHttpServer::createServerObject()
{
    auto newObject = std::make_unique<UpnpServerEventObject>();
    newObject->setService(d->mService);
    return newObject.release();
}

void UpnpHttpServer::setService(UpnpControlAbstractService *service)
{
    d->mService = service;
}

#include "moc_upnphttpserver.cpp"
