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

#ifndef UPNPPROXYWEBSOCKET_H
#define UPNPPROXYWEBSOCKET_H

#include <QtCore/QObject>

#include "upnpQtWebSocket_export.h"

class UpnpProxyWebSocketPrivate;

class UPNPQTWEBSOCKET_EXPORT UpnpProxyWebSocket : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString serverName
               READ serverName
               WRITE setServerName
               NOTIFY serverNameChanged)

public:

    explicit UpnpProxyWebSocket(QObject *parent = 0);

    virtual ~UpnpProxyWebSocket();

    const QString& serverName() const;

    void setServerName(const QString &value);

    Q_INVOKABLE void init();

Q_SIGNALS:

    void serverNameChanged();

private Q_SLOTS:

    void newClientConnection();

private:

    UpnpProxyWebSocketPrivate *d;

};

#endif
