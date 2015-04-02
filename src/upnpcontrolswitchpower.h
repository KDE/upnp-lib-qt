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

#ifndef UPNPCONTROLSWITCHPOWER_H
#define UPNPCONTROLSWITCHPOWER_H

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

class UpnpControlSwitchPowerPrivate;

class UpnpControlSwitchPower : public UpnpControlAbstractService
{
    Q_OBJECT

    Q_PROPERTY(bool target
               READ target)

    Q_PROPERTY(QVariant status
               READ status
               NOTIFY statusChanged)

public:

    explicit UpnpControlSwitchPower(QObject *parent = 0);

    virtual ~UpnpControlSwitchPower();

    bool status() const;

    bool target() const;

public Q_SLOTS:

    void setTarget(bool newTargetValue);

    void getTarget();

    void getStatus();

Q_SIGNALS:

    void setTargetFinished(bool success);

    void getTargetFinished(bool success, bool target);

    void getStatusFinished(bool success, bool status);

    void statusChanged();

private Q_SLOTS:

    void finishedSetTargetCall(KDSoapPendingCallWatcher *self);

    void finishedGetTargetCall(KDSoapPendingCallWatcher *self);

    void finishedGetStatusCall(KDSoapPendingCallWatcher *self);

protected:

    void parseEventNotification(const QString &eventName, const QString &eventValue) Q_DECL_OVERRIDE;

private:

    UpnpControlSwitchPowerPrivate *d;

};

#endif // UPNPCONTROLSWITCHPOWER_H
