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

#include "olinuxinoremoterelay.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QUrl>

OlinuxinoRemoteRelay::OlinuxinoRemoteRelay(QObject *parent)
    : QObject(parent), mRelayAddress(), mRelayIndex(0), mRelayState(OlinuxinoRemoteRelay::SwitchOff),
      mNetworkAccess(new QNetworkAccessManager), mStatus(true)
{   
    connect(mNetworkAccess.data(), &QNetworkAccessManager::sslErrors, this, &OlinuxinoRemoteRelay::sslErrors);
}

OlinuxinoRemoteRelay::~OlinuxinoRemoteRelay()
{
}

QString OlinuxinoRemoteRelay::relayAddress() const
{
    return mRelayAddress;
}

int OlinuxinoRemoteRelay::relayIndex() const
{
    return mRelayIndex;
}

OlinuxinoRemoteRelay::RelayState OlinuxinoRemoteRelay::relayState() const
{
    return mRelayState;
}

bool OlinuxinoRemoteRelay::status() const
{
    return mStatus;
}

void OlinuxinoRemoteRelay::setAddress(QString relayAddress)
{
    if (mRelayAddress == relayAddress)
        return;

    mRelayAddress = relayAddress;
    Q_EMIT addressChanged(relayAddress);
}

void OlinuxinoRemoteRelay::setRelayIndex(int relayIndice)
{
    if (mRelayIndex == relayIndice)
        return;

    mRelayIndex = relayIndice;
    emit relayIndexChanged(relayIndice);
}

void OlinuxinoRemoteRelay::activate(OlinuxinoRemoteRelay::RelayState newState)
{
    QUrl myRequestUrl;

    myRequestUrl.setScheme(QStringLiteral("https"));

    myRequestUrl.setHost(mRelayAddress);

    switch (mRelayIndex)
    {
    case 0:
        myRequestUrl.setPath(QStringLiteral("/cgi-bin/luci/relay/switch"));
        break;
    case 1:
        myRequestUrl.setPath(QStringLiteral("/cgi-bin/luci/relay/switch1"));
        break;
    default:
        return;
    }

    switch(newState)
    {
    case OlinuxinoRemoteRelay::SwitchOff:
        myRequestUrl.setQuery(QStringLiteral("rstate=0"));
        break;
    case OlinuxinoRemoteRelay::SwitchOn:
        myRequestUrl.setQuery(QStringLiteral("rstate=1"));
        break;
    }

    auto reply = mNetworkAccess->get(QNetworkRequest(myRequestUrl));

    connect(reply, &QNetworkReply::finished, this, &OlinuxinoRemoteRelay::finished);
}

void OlinuxinoRemoteRelay::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    reply->ignoreSslErrors();
}

void OlinuxinoRemoteRelay::finished()
{
    auto reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        mStatus = false;
        Q_EMIT statusChanged(mStatus);
    } else {
        if (!mStatus) {
            mStatus = true;
            Q_EMIT statusChanged(mStatus);
        }
    }
}


#include "moc_olinuxinoremoterelay.cpp"
