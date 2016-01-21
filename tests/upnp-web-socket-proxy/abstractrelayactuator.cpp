/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

#include "abstractrelayactuator.h"

AbstractRelayActuator::AbstractRelayActuator(QObject *parent)
    : QObject(parent), mRelayState(AbstractRelayActuator::SwitchOff), mStatus(true)
{
}

AbstractRelayActuator::RelayState AbstractRelayActuator::relayState() const
{
    return mRelayState;
}

bool AbstractRelayActuator::status() const
{
    return mStatus;
}

void AbstractRelayActuator::setRelayState(AbstractRelayActuator::RelayState relayState)
{
    if (mRelayState == relayState)
        return;

    mRelayState = relayState;
    emit relayStateChanged(relayState);
}

void AbstractRelayActuator::setStatus(bool status)
{
    if (mStatus == status)
        return;

    mStatus = status;
    emit statusChanged(status);
}

bool AbstractRelayActuator::activate(AbstractRelayActuator::RelayState newState)
{
    return doActivate(newState);
}


#include "moc_abstractrelayactuator.cpp"
