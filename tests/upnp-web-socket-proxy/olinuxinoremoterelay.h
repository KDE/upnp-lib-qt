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

#ifndef OLINUXINOREMOTERELAY_H
#define OLINUXINOREMOTERELAY_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QScopedPointer>

#include <QtNetwork/QSslError>

class QNetworkAccessManager;
class QNetworkReply;

class OlinuxinoRemoteRelay : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString relayAddress
               READ relayAddress
               WRITE setAddress
               NOTIFY addressChanged)

    Q_PROPERTY(int relayIndex
               READ relayIndex
               WRITE setRelayIndex
               NOTIFY relayIndexChanged)

    Q_PROPERTY(RelayState relayState
               READ relayState
               NOTIFY relayStateChanged)

    Q_PROPERTY(bool status
               READ status
               NOTIFY statusChanged)

public:

    enum RelayState {
        SwitchOn,
        SwitchOff,
    };

    Q_ENUM(RelayState)

    explicit OlinuxinoRemoteRelay(QObject *parent = 0);

    virtual ~OlinuxinoRemoteRelay();

    QString relayAddress() const;

    int relayIndex() const;

    RelayState relayState() const;

    bool status() const;

Q_SIGNALS:

    void addressChanged(QString relayAddress);

    void relayIndexChanged(int relayIndex);

    void relayStateChanged(RelayState relayState);

    void statusChanged(bool status);

public Q_SLOTS:

    void setAddress(QString relayAddress);

    void setRelayIndex(int relayIndice);

    void activate(RelayState newState);

    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    void finished();

private:

    QString mRelayAddress;

    int mRelayIndex;

    RelayState mRelayState;

    QScopedPointer<QNetworkAccessManager> mNetworkAccess;

    bool mStatus;
};

#endif // OLINUXINOREMOTERELAY_H
