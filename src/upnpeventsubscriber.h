/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPEVENTSUBSCRIBER_H
#define UPNPEVENTSUBSCRIBER_H

#include "upnplibqt_export.h"

#include <QNetworkReply>

#include <QObject>
#include <QString>
#include <QUrl>

#include <memory>

class UpnpEventSubscriberPrivate;
class UpnpAbstractService;

/**
 * @brief The UpnpEventSubscriber class is used to implement event notifications for services
 *
 * A service provides a notification facility for subscribers to know when variable changes. The \class UpnpEventSubscriber provides the mechanism to implement that notification facility.
 */
class UpnpEventSubscriber : public QObject
{
    Q_OBJECT

public:
    explicit UpnpEventSubscriber(QObject *parent = nullptr);

    ~UpnpEventSubscriber() override;

    void setSecondTimeout(int newValue);

    [[nodiscard]] int secondTimeout() const;

    void setCallback(const QUrl &callbackAddress);

    [[nodiscard]] const QUrl &callback() const;

    [[nodiscard]] const QString &uuid() const;

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
