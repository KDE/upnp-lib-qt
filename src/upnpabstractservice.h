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

#include "upnpQt_export.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QPair>

class UpnpAbstractServicePrivate;
class UpnpActionDescription;
class UpnpStateVariableDescription;
class UpnpEventSubscriber;
class QIODevice;

class UPNPQT_EXPORT UpnpAbstractService : public QObject
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

    Q_PROPERTY(QUrl eventURL
               READ eventURL
               WRITE setEventURL
               NOTIFY eventURLChanged)

    Q_PROPERTY(int maximumSubscriptionDuration
               READ maximumSubscriptionDuration
               WRITE setMaximumSubscriptionDuration
               NOTIFY maximumSubscriptionDurationChanged)

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

    void setEventURL(const QUrl &newEventURL);

    const QUrl& eventURL() const;

    void setMaximumSubscriptionDuration(int newValue);

    int maximumSubscriptionDuration() const;

    QIODevice* buildAndGetXmlDescription();

    QPointer<UpnpEventSubscriber> subscribeToEvents(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers);

    void unsubscribeToEvents(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers);

    void addAction(const UpnpActionDescription &newAction);

    Q_INVOKABLE const UpnpActionDescription& action(const QString &name) const;

    QList<QString> actions() const;

    void addStateVariable(const UpnpStateVariableDescription &newVariable);

    Q_INVOKABLE const UpnpStateVariableDescription& stateVariable(const QString &name) const;

    QList<QString> stateVariables() const;

    virtual QList<QPair<QString, QVariant> > invokeAction(const QString &actionName, const QList<QVariant> &arguments, bool &isInError);

Q_SIGNALS:

    void serviceTypeChanged(const QString &serviceId);

    void serviceIdChanged(const QString &serviceId);

    void baseURLChanged(const QString &serviceId);

    void SCPDURLChanged(const QString &serviceId);

    void controlURLChanged(const QString &serviceId);

    void eventURLChanged(const QString &serviceId);

    void maximumSubscriptionDurationChanged(const QString &serviceId);

public Q_SLOTS:

private:

    void sendEventNotification(const QPointer<UpnpEventSubscriber> &currentSubscriber);

    UpnpAbstractServicePrivate *d;

};

#endif // UPNPABSTRACTSERVICE_H
