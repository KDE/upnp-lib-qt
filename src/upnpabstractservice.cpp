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

#include "upnpabstractservice.h"

class UpnpAbstractServicePrivate
{
public:

    QString mBaseURL;

    QString mServiceType;

    QString mServiceId;

    QUrl mSCPDURL;

    QUrl mControlURL;

    QUrl mEventSubURL;

};

UpnpAbstractService::UpnpAbstractService(QObject *parent) :
    QObject(parent), d(new UpnpAbstractServicePrivate)
{
}

UpnpAbstractService::~UpnpAbstractService()
{
    delete d;
}

void UpnpAbstractService::setBaseURL(const QString &newBaseURL)
{
    d->mBaseURL = newBaseURL;
}

const QString &UpnpAbstractService::baseURL() const
{
    return d->mBaseURL;
}

void UpnpAbstractService::setServiceType(const QString &newServiceType)
{
    d->mServiceType = newServiceType;
}

const QString &UpnpAbstractService::serviceType() const
{
    return d->mServiceType;
}

void UpnpAbstractService::setServiceId(const QString &newServiceId)
{
    d->mServiceId = newServiceId;
}

const QString &UpnpAbstractService::serviceId() const
{
    return d->mServiceId;
}

void UpnpAbstractService::setSCPDURL(const QUrl &newSCPDURL)
{
    d->mSCPDURL = newSCPDURL;
}

const QUrl &UpnpAbstractService::SCPDURL() const
{
    return d->mSCPDURL;
}

void UpnpAbstractService::setControlURL(const QUrl &newControlURL)
{
    d->mControlURL = newControlURL;
}

const QUrl &UpnpAbstractService::controlURL() const
{
    return d->mControlURL;
}

void UpnpAbstractService::setEventSubURL(const QUrl &newEventSubURL)
{
    d->mEventSubURL = newEventSubURL;
}

const QUrl &UpnpAbstractService::eventSubURL() const
{
    return d->mEventSubURL;
}

#include "moc_upnpabstractservice.cpp"
