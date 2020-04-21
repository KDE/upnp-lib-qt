/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPSERVICEDESCRIPTION_H
#define UPNPSERVICEDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QList>
#include <QPair>
#include <QString>
#include <QUrl>
#include <QVariant>

#include <memory>

class UpnpServiceDescriptionPrivate;
class UpnpActionDescription;
class UpnpStateVariableDescription;
class UpnpEventSubscriber;
class UpnpDeviceDescription;

class UPNPLIBQT_EXPORT UpnpServiceDescription
{

public:
    explicit UpnpServiceDescription();

    UpnpServiceDescription(const UpnpServiceDescription &other);

    UpnpServiceDescription(UpnpServiceDescription &&other);

    ~UpnpServiceDescription();

    UpnpServiceDescription &operator=(const UpnpServiceDescription &other);

    UpnpServiceDescription &operator=(UpnpServiceDescription &&other);

    void setBaseURL(const QString &newBaseURL);

    const QString &baseURL() const;

    void setServiceType(const QString &newServiceType);

    const QString &serviceType() const;

    void setServiceId(const QString &newServiceId);

    const QString &serviceId() const;

    void setSCPDURL(const QUrl &newSCPDURL);

    const QUrl &SCPDURL() const;

    void setControlURL(const QUrl &newControlURL);

    const QUrl &controlURL() const;

    void setEventURL(const QUrl &newEventURL);

    const QUrl &eventURL() const;

    void setMaximumSubscriptionDuration(int newValue);

    int maximumSubscriptionDuration() const;

    void addAction(const UpnpActionDescription &newAction);

    const UpnpActionDescription &action(const QString &name) const;

    QMap<QString, UpnpActionDescription> &actions();

    const QMap<QString, UpnpActionDescription> &actions() const;

    void addStateVariable(const UpnpStateVariableDescription &newVariable);

    const UpnpStateVariableDescription &stateVariable(const QString &name) const;

    QMap<QString, UpnpStateVariableDescription> &stateVariables();

    const QMap<QString, UpnpStateVariableDescription> &stateVariables() const;

    const UpnpDeviceDescription &deviceDescription() const;

    void setDeviceDescription(const UpnpDeviceDescription &deviceDescription);

private:
    std::unique_ptr<UpnpServiceDescriptionPrivate> d;
};

#endif // UpnpServiceDescription_H
