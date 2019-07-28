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

#ifndef UPNPEVENTSUBSCRIBER_H
#define UPNPEVENTSUBSCRIBER_H

#include "upnplibqt_export.h"

#include <QtNetwork/QNetworkReply>

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QString>

#include <memory>

class UpnpEventSubscriberPrivate;
class UpnpAbstractService;

class UpnpEventSubscriber : public QObject
{
    Q_OBJECT

public:

    UpnpEventSubscriber(QObject *parent = nullptr);

    ~UpnpEventSubscriber() override;

    void setSecondTimeout(int newValue);

    int secondTimeout() const;

    void setCallback(const QUrl &callbackAddress);

    const QUrl& callback() const;

    const QString& uuid() const;

    void setUpnpService(UpnpAbstractService *service);

public Q_SLOTS:

    void sendEventNotification();

    void notifyPropertyChange(const QString &serviceId, const QByteArray &propertyName);

private Q_SLOTS:

    void eventingFinished();

    void eventingInErrorFinished(QNetworkReply::NetworkError code);

private:

    std::unique_ptr<UpnpEventSubscriberPrivate> d;

};

#endif // UPNPEVENTSUBSCRIBER_H
