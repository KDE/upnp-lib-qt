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

#ifndef ABSTRACTRELAYACTUATOR_H
#define ABSTRACTRELAYACTUATOR_H

#include <QtCore/QObject>

class AbstractRelayActuator : public QObject
{

    Q_OBJECT

    Q_PROPERTY(RelayState relayState
               READ relayState
               WRITE setRelayState
               NOTIFY relayStateChanged)

    Q_PROPERTY(bool status
               READ status
               WRITE setStatus
               NOTIFY statusChanged)

public:

    enum RelayState {
        SwitchOn,
        SwitchOff,
    };

    Q_ENUM(RelayState)

    explicit AbstractRelayActuator(QObject *parent = 0);

    RelayState relayState() const;

    bool status() const;

Q_SIGNALS:

    void relayStateChanged(RelayState relayState);

    void statusChanged(bool status);

    void finishedActivating(RelayState newState);

public Q_SLOTS:

    bool activate(RelayState newState);

    void setRelayState(RelayState relayState);

    void setStatus(bool status);

protected:

    virtual bool doActivate(RelayState newState) = 0;

private:

    RelayState mRelayState;

    bool mStatus;

};



#endif // ABSTRACTRELAYACTUATOR_H
