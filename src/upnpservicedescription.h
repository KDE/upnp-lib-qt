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

    [[nodiscard]] const QString &baseURL() const;

    void setServiceType(const QString &newServiceType);

    [[nodiscard]] const QString &serviceType() const;

    void setServiceId(const QString &newServiceId);

    [[nodiscard]] const QString &serviceId() const;

    void setSCPDURL(const QUrl &newSCPDURL);

    [[nodiscard]] const QUrl &SCPDURL() const;

    void setControlURL(const QUrl &newControlURL);

    [[nodiscard]] const QUrl &controlURL() const;

    void setEventURL(const QUrl &newEventURL);

    [[nodiscard]] const QUrl &eventURL() const;

    void setMaximumSubscriptionDuration(int newValue);

    [[nodiscard]] int maximumSubscriptionDuration() const;

    void addAction(const UpnpActionDescription &newAction);

    [[nodiscard]] const UpnpActionDescription &action(const QString &name) const;

    [[nodiscard]] QMap<QString, UpnpActionDescription> &actions();

    [[nodiscard]] const QMap<QString, UpnpActionDescription> &actions() const;

    void addStateVariable(const UpnpStateVariableDescription &newVariable);

    [[nodiscard]] const UpnpStateVariableDescription &stateVariable(const QString &name) const;

    [[nodiscard]] QMap<QString, UpnpStateVariableDescription> &stateVariables();

    [[nodiscard]] const QMap<QString, UpnpStateVariableDescription> &stateVariables() const;

    [[nodiscard]] const UpnpDeviceDescription &deviceDescription() const;

    void setDeviceDescription(const UpnpDeviceDescription &deviceDescription);

private:
    std::unique_ptr<UpnpServiceDescriptionPrivate> d;
};

#endif // UpnpServiceDescription_H
