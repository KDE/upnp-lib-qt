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

#ifndef UPNPLISTENNER_H
#define UPNPLISTENNER_H

#include <QObject>

#include "upnp.h"

class UpnpListennerPrivate;

class UpnpListenner : public QObject
{
    Q_OBJECT
public:
    explicit UpnpListenner(QObject *parent = 0);
    ~UpnpListenner();

    static bool UpnpInit();

    static void UpnpFinish();

    int upnpError() const;

Q_SIGNALS:

    void searchTimeOut();

    void newService(const Upnp_Discovery &serviceDiscovery);

    void removedService(const Upnp_Discovery &serviceDiscovery);

public Q_SLOTS:

    /**
     * @brief searchUpnpContentDirectory will trigger a search for upnp service of MediaServer (UPNP/DLNA)
     */
    bool searchUpnpContentDirectory();

    /**
     * @brief searchUpnpPlayerControl will trigger a search for upnp service of MediaRenderer (UPNP/DLNA)
     */
    bool searchUpnpPlayerControl();

    int upnpInternalCallBack(Upnp_EventType EventType, void *Event);

private:

    UpnpListennerPrivate *d;
};

#endif // UPNPLISTENNER_H
