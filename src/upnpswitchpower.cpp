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

#include "upnpswitchpower.h"

class UpnpSwitchPowerPrivate
{
public:

    bool mTarget;

    bool mStatus;
};

UpnpSwitchPower::UpnpSwitchPower(const QUrl &controlUrlValue, const QUrl &eventUrlValue, const QUrl &SCPDUrlValue, QObject *parent) :
    UpnpAbstractService(parent), d(new UpnpSwitchPowerPrivate)
{
    //setBaseURL();
    setControlURL(controlUrlValue);
    setEventSubURL(eventUrlValue);
    setSCPDURL(SCPDUrlValue);
    setServiceId(QStringLiteral("urn:upnp-org:serviceId:SwitchPower"));
    setServiceType(QStringLiteral("urn:schemas-upnp-org:service:SwitchPower:1"));
}

UpnpSwitchPower::~UpnpSwitchPower()
{
    delete d;
}

void UpnpSwitchPower::setTarget(bool value)
{
    d->mTarget = value;
}

bool UpnpSwitchPower::target() const
{
    return d->mTarget;
}

void UpnpSwitchPower::setStatus(bool value)
{
    d->mStatus = value;
    Q_EMIT statusChanged(serviceId());
}

bool UpnpSwitchPower::status() const
{
    return d->mStatus;
}

#include "moc_upnpswitchpower.cpp"
