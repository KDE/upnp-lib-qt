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
    , d(new UpnpHttpServerPrivate)
{
    d->mService = nullptr;
}

UpnpHttpServer::~UpnpHttpServer()
{
}

QObject *UpnpHttpServer::createServerObject()
{
    auto newObject = new UpnpServerEventObject;
    newObject->setService(d->mService);
    return newObject;
}

void UpnpHttpServer::setService(UpnpControlAbstractService *service)
{
    d->mService = service;
}

#include "moc_upnphttpserver.cpp"
