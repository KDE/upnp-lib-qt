/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPCONTROLABSTRACTSERVICE_H
#define UPNPCONTROLABSTRACTSERVICE_H

#include "upnplibqt_export.h"

#include "upnpabstractservice.h"
#include "upnpcontrolabstractservicereply.h"

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariant>
#include <QVariantList>

#include <memory>

class UpnpAbstractServiceDescriptionPrivate;
class QNetworkReply;
class QHostInfo;

class UPNPLIBQT_EXPORT UpnpControlAbstractService : public UpnpAbstractService
{
    Q_OBJECT

public:
    explicit UpnpControlAbstractService(QObject *parent = nullptr);

    ~UpnpControlAbstractService() override;

    [[nodiscard]] UpnpControlAbstractServiceReply *callAction(const QString &action, const QVector<QVariant> &arguments);

    void subscribeEvents(int duration);

    void handleEventNotification(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers);

Q_SIGNALS:

public Q_SLOTS:

    void downloadServiceDescription(const QUrl &serviceUrl);

private Q_SLOTS:

    void finishedDownload(QNetworkReply *reply);

    void eventSubscriptionTimeout();

protected:
    virtual void parseServiceDescription(QIODevice *serviceDescriptionContent);

    virtual void parseEventNotification(const QString &eventName, const QString &eventValue);

private:
    std::unique_ptr<UpnpAbstractServiceDescriptionPrivate> d;
};

#endif // UPNPSERVICECALLER_H
