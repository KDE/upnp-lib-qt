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
class UpnpServiceDescription;

class UPNPQT_EXPORT UpnpAbstractService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpServiceDescription* service
               READ service
               WRITE setService
               NOTIFY serviceChanged)

public:
    explicit UpnpAbstractService(QObject *parent = 0);

    virtual ~UpnpAbstractService();

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

    void setService(UpnpServiceDescription *value);

    UpnpServiceDescription* service();

    const UpnpServiceDescription* service() const;

Q_SIGNALS:

    void serviceChanged();

public Q_SLOTS:

private:

    void sendEventNotification(const QPointer<UpnpEventSubscriber> &currentSubscriber);

    UpnpAbstractServicePrivate *d;

};

#endif // UPNPABSTRACTSERVICE_H
