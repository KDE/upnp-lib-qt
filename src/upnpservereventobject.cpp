/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpservereventobject.h"

#include "upnpcontrolabstractservice.h"

#include <QDebug>

class UpnpServerEventObjectPrivate
{
public:
    UpnpControlAbstractService *mService;
};

UpnpServerEventObject::UpnpServerEventObject(QObject *parent)
    : QObject(parent)
    , KDSoapServerCustomVerbRequestInterface()
    , d(new UpnpServerEventObjectPrivate)
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
