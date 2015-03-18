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

#ifndef UPNPSERVICECALLER_H
#define UPNPSERVICECALLER_H

#include <KDSoapClient/KDSoapPendingCall.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantList>
#include <QtCore/QUrl>

class UpnpServiceDescriptionPrivate;
class QNetworkReply;

class UpnpServiceDescription : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant serviceType
               READ serviceType
               WRITE setServiceType
               NOTIFY serviceTypeChanged)

    Q_PROPERTY(QVariant serviceId
               READ serviceId
               WRITE setServiceId
               NOTIFY serviceIdChanged)

    Q_PROPERTY(QVariant baseURL
               READ baseURL
               WRITE setBaseURL
               NOTIFY baseURLChanged)

    Q_PROPERTY(QVariant SCPDURL
               READ SCPDURL
               WRITE setSCPDURL
               NOTIFY SCPDURLChanged)

    Q_PROPERTY(QVariant controlURL
               READ controlURL
               WRITE setControlURL
               NOTIFY controlURLChanged)

    Q_PROPERTY(QVariant eventSubURL
               READ eventSubURL
               WRITE setEventSubURL
               NOTIFY eventSubURLChanged)

public:

    explicit UpnpServiceDescription(QObject *parent = 0);

    ~UpnpServiceDescription();

    void setBaseURL(const QVariant &newBaseURL);

    const QVariant& baseURL() const;

    void setServiceType(const QVariant &newServiceType);

    const QVariant& serviceType() const;

    void setServiceId(const QVariant &newServiceId);

    const QVariant& serviceId() const;

    void setSCPDURL(const QVariant &newSCPDURL);

    const QVariant& SCPDURL() const;

    void setControlURL(const QVariant &newControlURL);

    const QVariant& controlURL() const;

    void setEventSubURL(const QVariant &newEventSubURL);

    const QVariant& eventSubURL() const;

    Q_INVOKABLE KDSoapPendingCall callAction(const QString &action, const QList<QVariant> &arguments);

    Q_INVOKABLE void subscribeEvents();

Q_SIGNALS:

    void serviceTypeChanged();

    void serviceIdChanged();

    void baseURLChanged();

    void SCPDURLChanged();

    void controlURLChanged();

    void eventSubURLChanged();

public Q_SLOTS:

    void downloadAndParseServiceDescription(const QUrl &serviceUrl);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

private:

    UpnpServiceDescriptionPrivate *d;
};

#endif // UPNPSERVICECALLER_H
