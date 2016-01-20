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

#ifndef UPNPBINARYLIGHT_H
#define UPNPBINARYLIGHT_H

#include <QtCore/QObject>
#include <QtCore/QUrl>

class BinaryLightPrivate;
class UpnpWebSocketPublisher;
class UpnpDeviceDescription;

class UpnpBinaryLight : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpDeviceDescription* description
               READ description
               NOTIFY descriptionChanged)

    Q_PROPERTY(UpnpWebSocketPublisher* webSocketPublisher
               READ webSocketPublisher
               WRITE setWebSocketPublisher
               NOTIFY webSocketPublisherChanged)

    Q_PROPERTY(QString udn
               READ udn
               WRITE setUdn
               NOTIFY udnChanged)

public:
    explicit UpnpBinaryLight(int cacheDuration = 1800, QObject *parent = 0);

    virtual ~UpnpBinaryLight();

    UpnpDeviceDescription* description();

    const UpnpDeviceDescription* description() const;

    void setWebSocketPublisher(UpnpWebSocketPublisher *value);

    UpnpWebSocketPublisher* webSocketPublisher() const;

    void setUdn(const QString &value);

    const QString &udn() const;

Q_SIGNALS:

    void descriptionChanged();

    void webSocketPublisherChanged();

    void udnChanged();

    void setTarget(qint64 sequenceNumber, bool target);

    void getTarget(qint64 sequenceNumber);

    void getStatus(qint64 sequenceNumber);

public Q_SLOTS:

    void actionCalled(const QString &action, const QVariantMap &arguments, qint64 sequenceNumber, const QString &serviceId);

private:

    BinaryLightPrivate *d;

};

#endif
