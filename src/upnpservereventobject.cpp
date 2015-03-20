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

#include "upnpabstractservicedescription.h"

#include <QtCore/QDebug>

class UpnpServerEventObjectPrivate
{
public:

    UpnpAbstractServiceDescription *mService;
};

UpnpServerEventObject::UpnpServerEventObject(QObject *parent) : QObject(parent), KDSoapServerObjectInterface(), d(new UpnpServerEventObjectPrivate)
{
    d->mService = nullptr;
}

UpnpServerEventObject::~UpnpServerEventObject()
{
    delete d;
}

void UpnpServerEventObject::processRequest(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction)
{
    Q_UNUSED(request);
    Q_UNUSED(response);
    Q_UNUSED(request);
    Q_UNUSED(soapAction);
}

QIODevice *UpnpServerEventObject::processFileRequest(const QString &path, QByteArray &contentType)
{
    Q_UNUSED(path);
    Q_UNUSED(contentType);
    return nullptr;
}

void UpnpServerEventObject::processRequestWithPath(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction, const QString &path)
{
    Q_UNUSED(request);
    Q_UNUSED(response);
    Q_UNUSED(soapAction);
    Q_UNUSED(path);
}

bool UpnpServerEventObject::processCustomVerbRequest(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    if (d->mService) {
        d->mService->handleEventNotification(requestData, headers);

        return true;
    } else {
        return false;
    }
}

void UpnpServerEventObject::setService(UpnpAbstractServiceDescription *service)
{
    d->mService = service;
}

#include "moc_upnpservereventobject.cpp"
