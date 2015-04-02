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

#ifndef UPNPABSTRACTSERVICE_H
#define UPNPABSTRACTSERVICE_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QUrl>

class UpnpAbstractServicePrivate;

class UpnpAbstractService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString serviceType
               READ serviceType
               WRITE setServiceType
               NOTIFY serviceTypeChanged)

    Q_PROPERTY(QString serviceId
               READ serviceId
               WRITE setServiceId
               NOTIFY serviceIdChanged)

    Q_PROPERTY(QString baseURL
               READ baseURL
               WRITE setBaseURL
               NOTIFY baseURLChanged)

    Q_PROPERTY(QUrl SCPDURL
               READ SCPDURL
               WRITE setSCPDURL
               NOTIFY SCPDURLChanged)

    Q_PROPERTY(QUrl controlURL
               READ controlURL
               WRITE setControlURL
               NOTIFY controlURLChanged)

    Q_PROPERTY(QUrl eventSubURL
               READ eventSubURL
               WRITE setEventSubURL
               NOTIFY eventSubURLChanged)

public:
    explicit UpnpAbstractService(QObject *parent = 0);

    virtual ~UpnpAbstractService();

    void setBaseURL(const QString &newBaseURL);

    const QString& baseURL() const;

    void setServiceType(const QString &newServiceType);

    const QString& serviceType() const;

    void setServiceId(const QString &newServiceId);

    const QString& serviceId() const;

    void setSCPDURL(const QUrl &newSCPDURL);

    const QUrl& SCPDURL() const;

    void setControlURL(const QUrl &newControlURL);

    const QUrl& controlURL() const;

    void setEventSubURL(const QUrl &newEventSubURL);

    const QUrl& eventSubURL() const;

Q_SIGNALS:

    void serviceTypeChanged();

    void serviceIdChanged();

    void baseURLChanged();

    void SCPDURLChanged();

    void controlURLChanged();

    void eventSubURLChanged();

public Q_SLOTS:

private:

    UpnpAbstractServicePrivate *d;

};

#endif // UPNPABSTRACTSERVICE_H
