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

#ifndef UPNPSERVICEDESCRIPTION_H
#define UPNPSERVICEDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QString>
#include <QVariant>
#include <QUrl>
#include <QList>
#include <QPair>

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

    UpnpServiceDescription& operator=(const UpnpServiceDescription &other);

    UpnpServiceDescription& operator=(UpnpServiceDescription &&other);

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

    void addAction(const UpnpActionDescription &newAction);

    const UpnpActionDescription& action(const QString &name) const;

    QMap<QString, UpnpActionDescription>& actions();

    const QMap<QString, UpnpActionDescription>& actions() const;

    void addStateVariable(const UpnpStateVariableDescription &newVariable);

    const UpnpStateVariableDescription& stateVariable(const QString &name) const;

    QMap<QString, UpnpStateVariableDescription>& stateVariables();

    const QMap<QString, UpnpStateVariableDescription>& stateVariables() const;

    const UpnpDeviceDescription &deviceDescription() const;

    void setDeviceDescription(const UpnpDeviceDescription &deviceDescription);

private:

    std::unique_ptr<UpnpServiceDescriptionPrivate> d;

};

#endif // UpnpServiceDescription_H
