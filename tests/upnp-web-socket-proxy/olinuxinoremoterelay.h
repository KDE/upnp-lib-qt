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

#include "abstractrelayactuator.h"

#include <QtCore/QList>
#include <QtCore/QScopedPointer>

#include <QtNetwork/QSslError>

class QNetworkAccessManager;
class QNetworkReply;

class OlinuxinoRemoteRelay : public AbstractRelayActuator
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

public:

    explicit OlinuxinoRemoteRelay(QObject *parent = 0);

    virtual ~OlinuxinoRemoteRelay();

    QString relayAddress() const;

    int relayIndex() const;

Q_SIGNALS:

    void addressChanged(QString relayAddress);

    void relayIndexChanged(int relayIndex);

public Q_SLOTS:

    void setAddress(QString relayAddress);

    void setRelayIndex(int relayIndice);

    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

    void finished(QNetworkReply *reply);

protected:

    bool doActivate(RelayState newState) override;

private:

    QString mRelayAddress;

    int mRelayIndex;

    QScopedPointer<QNetworkAccessManager> mNetworkAccess;

};

#endif // OLINUXINOREMOTERELAY_H
