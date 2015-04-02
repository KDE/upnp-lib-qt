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

#ifndef UPNPCONTROLDIMMING_H
#define UPNPCONTROLDIMMING_H

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

class UpnpControlDimmingPrivate;

class UpnpControlDimming : public UpnpControlAbstractService
{
    Q_OBJECT

    Q_PROPERTY(bool loadLevelTarget
               READ loadLevelTarget)

    Q_PROPERTY(QVariant loadLevelStatus
               READ loadLevelStatus
               NOTIFY loadLevelStatusChanged)

public:

    explicit UpnpControlDimming(QObject *parent = 0);

    virtual ~UpnpControlDimming();

    bool loadLevelStatus() const;

    bool loadLevelTarget() const;

public Q_SLOTS:

    void setLoadLevelTarget(bool newTargetValue);

    void getLoadLevelTarget();

    void getLoadLevelStatus();

Q_SIGNALS:

    void setLoadLevelTargetFinished(bool success);

    void getLoadLevelTargetFinished(bool success, bool loadLevelTarget);

    void getLoadLevelStatusFinished(bool success, bool status);

    void loadLevelStatusChanged();

private Q_SLOTS:

    void finishedSetLoadLevelTargetCall(KDSoapPendingCallWatcher *self);

    void finishedGetLoadLevelTargetCall(KDSoapPendingCallWatcher *self);

    void finishedGetLoadLevelStatusCall(KDSoapPendingCallWatcher *self);

protected:

    void parseEventNotification(const QString &eventName, const QString &eventValue) Q_DECL_OVERRIDE;

private:

    UpnpControlDimmingPrivate *d;

};

#endif // UPNPCONTROLDIMMING_H
