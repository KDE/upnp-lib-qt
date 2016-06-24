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

#include "upnpservereventobject.h"

#include "upnpcontrolabstractservice.h"

#include <QtCore/QDebug>

class UpnpServerEventObjectPrivate
{
public:

    UpnpControlAbstractService *mService;
};

UpnpServerEventObject::UpnpServerEventObject(QObject *parent) : QObject(parent), KDSoapServerCustomVerbRequestInterface(), d(new UpnpServerEventObjectPrivate)
{
    d->mService = nullptr;
}

UpnpServerEventObject::~UpnpServerEventObject()
{
}

bool UpnpServerEventObject::processCustomVerbRequest(const QByteArray &requestType, const QByteArray &requestData,
                                                     const QMap<QByteArray, QByteArray> &httpHeaders, QByteArray &customAnswer)
{
    Q_UNUSED(requestType);

    if (!d->mService) {
        return false;
    }

    d->mService->handleEventNotification(requestData, httpHeaders);

    customAnswer = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n";

    return true;
}

void UpnpServerEventObject::setService(UpnpControlAbstractService *service)
{
    d->mService = service;
}

#include "moc_upnpservereventobject.cpp"
