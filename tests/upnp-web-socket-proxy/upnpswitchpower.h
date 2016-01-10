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

#ifndef UPNPSWITCHPOWER_H
#define UPNPSWITCHPOWER_H

#include "upnpQt_export.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QByteArray>

class UpnpSwitchPowerPrivate;
class UpnpServiceDescription;

class UpnpSwitchPower : public QObject
{
    Q_OBJECT

    Q_PROPERTY(UpnpServiceDescription* description
               READ description
               NOTIFY descriptionChanged)

    Q_PROPERTY(bool target
               READ target
               WRITE setTarget)

    Q_PROPERTY(bool status
               READ status
               WRITE setStatus
               NOTIFY statusChanged)

public:
    explicit UpnpSwitchPower(QObject *parent = 0);

    virtual ~UpnpSwitchPower();

    QSharedPointer<UpnpServiceDescription> sharedDescription();

    UpnpServiceDescription* description();

    const UpnpServiceDescription* description() const;

    void setTarget(bool value);

    bool target() const;

    void setStatus(bool value);

    bool status() const;

Q_SIGNALS:

    void descriptionChanged();

    void statusChanged(const QString &serviceId, const QByteArray &propertyName);

public Q_SLOTS:

    void switchTarget();

    QList<QPair<QString, QVariant> > setTargetAction(bool newValue);

    QList<QPair<QString, QVariant> > getTargetAction();

    QList<QPair<QString, QVariant> > getStatusAction();

private:

    UpnpSwitchPowerPrivate *d;

};

#endif // UPNPSWITCHPOWER_H
