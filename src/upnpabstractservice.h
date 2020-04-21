/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPABSTRACTSERVICE_H
#define UPNPABSTRACTSERVICE_H

#include "upnplibqt_export.h"

#include <QList>
#include <QObject>
#include <QPair>
#include <QString>
#include <QUrl>
#include <QVariant>

#include <memory>

class UpnpAbstractServicePrivate;
class UpnpActionDescription;
class UpnpStateVariableDescription;
class UpnpEventSubscriber;
class QIODevice;
class UpnpServiceDescription;

class UPNPLIBQT_EXPORT UpnpAbstractService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpServiceDescription description
            READ description
                WRITE setDescription
                    NOTIFY descriptionChanged)

public:
    explicit UpnpAbstractService(QObject *parent = nullptr);

    ~UpnpAbstractService() override;

    [[nodiscard]] QIODevice *buildAndGetXmlDescription();

    [[nodiscard]] QPointer<UpnpEventSubscriber> subscribeToEvents(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers);

    void unsubscribeToEvents(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers);

    void addAction(const UpnpActionDescription &newAction);

    [[nodiscard]] const UpnpActionDescription &action(const QString &name) const;

    [[nodiscard]] QList<QString> actions() const;

    void addStateVariable(const UpnpStateVariableDescription &newVariable);

    [[nodiscard]] const UpnpStateVariableDescription &stateVariable(const QString &name) const;

    [[nodiscard]] QList<QString> stateVariables() const;

    [[nodiscard]] virtual QVector<QPair<QString, QVariant>> invokeAction(const QString &actionName, const QVector<QVariant> &arguments, bool &isInError);

    void setDescription(const UpnpServiceDescription &value);

    [[nodiscard]] UpnpServiceDescription &description();

    [[nodiscard]] const UpnpServiceDescription &description() const;

Q_SIGNALS:

    void descriptionChanged();

public Q_SLOTS:

private:
    void sendEventNotification(const QPointer<UpnpEventSubscriber> &currentSubscriber);

    std::unique_ptr<UpnpAbstractServicePrivate> d;
};

#endif // UPNPABSTRACTSERVICE_H
