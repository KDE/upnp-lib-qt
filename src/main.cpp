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

#include <QtCore/QCoreApplication>

#include "upnplistenner.h"

#include <KDSoapClient/KDSoapClientInterface.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    KDSoapClientInterface clientInterface(QStringLiteral("http://127.0.0.1:49494/upnp/control/rendertransport1"), QStringLiteral("urn:schemas-upnp-org:service:AVTransport:1"));

    clientInterface.setSoapVersion(KDSoapClientInterface::SOAP1_1);
    clientInterface.setStyle(KDSoapClientInterface::RPCStyle);

    KDSoapMessage message;
    message.addArgument(QStringLiteral("InstanceID"), QStringLiteral("0"));
    message.addArgument(QStringLiteral("Speed"), QStringLiteral("1"));
    clientInterface.call(QStringLiteral("Play"), message, QStringLiteral("urn:schemas-upnp-org:service:AVTransport:1#Play"));

/*
    UpnpListenner::UpnpInit();

    UpnpListenner *myListenner = new UpnpListenner;
*/

    app.exec();

    UpnpListenner::UpnpFinish();

    return 0;
}
