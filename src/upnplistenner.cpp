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

#include "upnplistenner.h"

#include <QtNetwork/QHostAddress>

#include <QtCore/QDebug>
#include <QtCore/QSet>

uint qHash(const Upnp_Discovery &discovery, uint seed = 0)
{
    return qHashBits(&discovery, sizeof(discovery), seed);
}

bool operator==(const Upnp_Discovery &first, const Upnp_Discovery &second)
{
    return strncmp(first.DeviceId, second.DeviceId, sizeof(first.DeviceId)) == 0;
}

class UpnpListennerPrivate
{
public:
    int mUpnpLibraryStatus;

    UpnpClient_Handle mClientHandle;

    QSet<Upnp_Discovery> mDiscoveryResults;
};

int upnpCallBack(Upnp_EventType EventType,void *Event,void *Cookie)
{
    if (Cookie) {
        return static_cast<UpnpListenner*>(Cookie)->upnpInternalCallBack(EventType, Event);
    }

    return -1;
}

UpnpListenner::UpnpListenner(QObject *parent)
    : QObject(parent), d(new UpnpListennerPrivate)
{
    d->mUpnpLibraryStatus = UpnpRegisterClient(&upnpCallBack, this, &d->mClientHandle);
    if (d->mUpnpLibraryStatus == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    searchUpnpContentDirectory();

    searchUpnpPlayerControl();
}

UpnpListenner::~UpnpListenner()
{
    delete d;
}

bool UpnpListenner::UpnpInit()
{
    int res = UpnpInit2(nullptr, 0);
    if (res == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    return (res == UPNP_E_SUCCESS);
}

void UpnpListenner::UpnpFinish()
{
    UpnpFinish();
}

int UpnpListenner::upnpError() const
{
    return d->mUpnpLibraryStatus;
}

bool UpnpListenner::searchUpnpContentDirectory()
{
    d->mUpnpLibraryStatus = UpnpSearchAsync(d->mClientHandle, 60, "urn:schemas-upnp-org:device:MediaServer:1", this);

    if (d->mUpnpLibraryStatus == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    if (d->mUpnpLibraryStatus != UPNP_E_SUCCESS) {
        return (d->mUpnpLibraryStatus == UPNP_E_SUCCESS);
    }

    d->mUpnpLibraryStatus = UpnpSearchAsync(d->mClientHandle, 60, "urn:schemas-upnp-org:service:ContentDirectory:1", this);

    if (d->mUpnpLibraryStatus == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    return (d->mUpnpLibraryStatus == UPNP_E_SUCCESS);
}

bool UpnpListenner::searchUpnpPlayerControl()
{
    d->mUpnpLibraryStatus = UpnpSearchAsync(d->mClientHandle, 60, "urn:schemas-upnp-org:device:MediaRenderer:1", this);

    if (d->mUpnpLibraryStatus == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    if (d->mUpnpLibraryStatus != UPNP_E_SUCCESS) {
        return (d->mUpnpLibraryStatus == UPNP_E_SUCCESS);
    }

    d->mUpnpLibraryStatus = UpnpSearchAsync(d->mClientHandle, 60, "urn:schemas-upnp-org:service:AVTransport:1", this);

    if (d->mUpnpLibraryStatus == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    if (d->mUpnpLibraryStatus != UPNP_E_SUCCESS) {
        return (d->mUpnpLibraryStatus == UPNP_E_SUCCESS);
    }

    d->mUpnpLibraryStatus = UpnpSearchAsync(d->mClientHandle, 60, "urn:schemas-upnp-org:service:RenderingControl:1", this);
    if (d->mUpnpLibraryStatus == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    return (d->mUpnpLibraryStatus == UPNP_E_SUCCESS);
}

int UpnpListenner::upnpInternalCallBack(Upnp_EventType EventType, void *Event)
{
    switch(EventType)
    {
    case UPNP_CONTROL_ACTION_REQUEST:
        qDebug() << "UPNP_CONTROL_ACTION_REQUEST";
        break;
    case UPNP_CONTROL_ACTION_COMPLETE:
        qDebug() << "UPNP_CONTROL_ACTION_COMPLETE";
        break;
    case UPNP_CONTROL_GET_VAR_REQUEST:
        qDebug() << "UPNP_CONTROL_GET_VAR_REQUEST";
        break;
    case UPNP_CONTROL_GET_VAR_COMPLETE:
        qDebug() << "UPNP_CONTROL_GET_VAR_COMPLETE";
        break;
    case UPNP_DISCOVERY_ADVERTISEMENT_ALIVE:
    {
        qDebug() << "UPNP_DISCOVERY_ADVERTISEMENT_ALIVE";
        Upnp_Discovery *searchResult = static_cast<Upnp_Discovery*>(Event);

        const int oldSize = d->mDiscoveryResults.size();

        d->mDiscoveryResults.insert(*searchResult);

        if (oldSize < d->mDiscoveryResults.size()) {
            auto it = d->mDiscoveryResults.find(*searchResult);
            Q_EMIT newService(*it);
        }

        break;
    }
    case UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE:
    {
        qDebug() << "UPNP_DISCOVERY_ADVERTISEMENT_BYEBYE";
        Upnp_Discovery *searchResult = static_cast<Upnp_Discovery*>(Event);

        auto it = d->mDiscoveryResults.find(*searchResult);
        if (it != d->mDiscoveryResults.end()) {
            Q_EMIT removedService(*it);

            d->mDiscoveryResults.erase(it);
        }

        break;
    }
    case UPNP_DISCOVERY_SEARCH_RESULT:
    {
        qDebug() << "UPNP_DISCOVERY_SEARCH_RESULT";
        Upnp_Discovery *searchResult = static_cast<Upnp_Discovery*>(Event);

        const int oldSize = d->mDiscoveryResults.size();

        d->mDiscoveryResults.insert(*searchResult);

        if (oldSize < d->mDiscoveryResults.size()) {
            auto it = d->mDiscoveryResults.find(*searchResult);

            qDebug() << "new service";
            qDebug() << "DeviceId:" << searchResult->DeviceId;
            qDebug() << "DeviceType:" << searchResult->DeviceType;
            qDebug() << "ServiceType:" << searchResult->ServiceType;
            qDebug() << "ServiceVer:" << searchResult->ServiceVer;
            qDebug() << "Location:" << searchResult->Location;
            qDebug() << "Os:" << searchResult->Os;
            qDebug() << "Date:" << searchResult->Date;
            qDebug() << "Ext:" << searchResult->Ext;
            qDebug() << "ErrCode:" << searchResult->ErrCode;
            qDebug() << "Expires:" << searchResult->Expires;
            qDebug() << "DestAddr:" << QHostAddress(reinterpret_cast<const sockaddr *>(&searchResult->DestAddr));

            Q_EMIT newService(*it);
        }

        break;
    }
    case UPNP_DISCOVERY_SEARCH_TIMEOUT:
        qDebug() << "UPNP_DISCOVERY_SEARCH_TIMEOUT";
        Q_EMIT searchTimeOut();
        break;
    case UPNP_EVENT_SUBSCRIPTION_REQUEST:
        qDebug() << "UPNP_EVENT_SUBSCRIPTION_REQUEST";
        break;
    case UPNP_EVENT_RECEIVED:
        qDebug() << "UPNP_EVENT_RECEIVED";
        break;
    case UPNP_EVENT_RENEWAL_COMPLETE:
        qDebug() << "UPNP_EVENT_RENEWAL_COMPLETE";
        break;
    case UPNP_EVENT_SUBSCRIBE_COMPLETE:
        qDebug() << "UPNP_EVENT_SUBSCRIBE_COMPLETE";
        break;
    case UPNP_EVENT_UNSUBSCRIBE_COMPLETE:
        qDebug() << "UPNP_EVENT_UNSUBSCRIBE_COMPLETE";
        break;
    case UPNP_EVENT_AUTORENEWAL_FAILED:
        qDebug() << "UPNP_EVENT_AUTORENEWAL_FAILED";
        break;
    case UPNP_EVENT_SUBSCRIPTION_EXPIRED:
        qDebug() << "UPNP_EVENT_SUBSCRIPTION_EXPIRED";
        break;
    }

    // return value is ignored according to libupnp documentation
    return 0;
}

#include "moc_upnplistenner.cpp"
