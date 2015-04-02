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

#ifndef UPNPDEVICESOAPSERVER_H
#define UPNPDEVICESOAPSERVER_H

#include <KDSoapServer/KDSoapServer.h>

#include <QtCore/QObject>

class UpnpControlAbstractService;
class UpnpDeviceSoapServerPrivate;

class UpnpDeviceSoapServer : public KDSoapServer
{
    Q_OBJECT
public:
    UpnpDeviceSoapServer(QObject * parent = 0);

    virtual ~UpnpDeviceSoapServer();

    QObject* createServerObject() Q_DECL_OVERRIDE;

private:

    UpnpDeviceSoapServerPrivate *d;
};

#endif