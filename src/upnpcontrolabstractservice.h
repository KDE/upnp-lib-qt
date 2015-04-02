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

#ifndef UPNPABSTRACTSERVICEDESCRIPTION_H
#define UPNPABSTRACTSERVICEDESCRIPTION_H

#include <KDSoapClient/KDSoapPendingCall.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtCore/QUrl>

#include "upnpabstractservice.h"

class UpnpAbstractServiceDescriptionPrivate;
class QNetworkReply;
class QHostInfo;

class UpnpControlAbstractService : public UpnpAbstractService
{
    Q_OBJECT

public:

    explicit UpnpControlAbstractService(QObject *parent = 0);

    ~UpnpControlAbstractService();

    Q_INVOKABLE KDSoapPendingCall callAction(const QString &action, const QList<QVariant> &arguments);

    Q_INVOKABLE void subscribeEvents(int duration);

    void handleEventNotification(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers);

Q_SIGNALS:

public Q_SLOTS:

    void downloadAndParseServiceDescription(const QUrl &serviceUrl);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

    void eventSubscriptionTimeout();

protected:

    virtual void parseEventNotification(const QString &eventName, const QString &eventValue);

private:

    UpnpAbstractServiceDescriptionPrivate *d;
};

#endif // UPNPSERVICECALLER_H
