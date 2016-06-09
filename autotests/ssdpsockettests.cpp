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

#include <QtNetwork/QUdpSocket>

#include <QtTest/QtTest>

class SsdpSocketTests: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        mFakeSocket.bind(QHostAddress(QStringLiteral("239.255.255.250")), 11900, QAbstractSocket::ShareAddress);
        mFakeSocket.joinMulticastGroup(QHostAddress(QStringLiteral("239.255.255.250")));
        mFakeSocket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
        mFakeSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 4);

        QTest::qSleep(250000);
    }

private:

    QUdpSocket mFakeSocket;

};

QTEST_MAIN(SsdpSocketTests)

#include "ssdpsockettests.moc"
