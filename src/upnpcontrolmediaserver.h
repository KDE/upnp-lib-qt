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

#ifndef UPNPCONTROLMEDIASERVER_H
#define UPNPCONTROLMEDIASERVER_H

#include "upnpcontrolabstractdevice.h"

class UpnpControlConnectionManager;

class UpnpControlMediaServerPrivate;

class UPNPQT_EXPORT UpnpControlMediaServer : public UpnpControlAbstractDevice
{

    Q_OBJECT

    Q_PROPERTY(bool hasAVTransport
               READ hasAVTransport
               NOTIFY hasAVTransportChanged)

public:
    explicit UpnpControlMediaServer(QObject *parent = 0);

    ~UpnpControlMediaServer();

    QString viewName() const override;

    bool hasAVTransport() const;

Q_SIGNALS:

    void hasAVTransportChanged();

protected:

    void parseDeviceDescription(QIODevice *deviceDescriptionContent, const QString &fallBackURLBase) override;

private:

    UpnpControlMediaServerPrivate *d;

};

#endif // UPNPCONTROLMEDIASERVER_H
