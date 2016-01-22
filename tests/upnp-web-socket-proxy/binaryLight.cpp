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

#include "upnpbinarylight.h"
#include "olinuxinoremoterelay.h"

#include "upnpwebsocketclient.h"
#include "upnpwebsocketcertificateconfiguration.h"
#include "upnpwebsocketpublisher.h"

#include "upnpdevicedescription.h"

#include <QtWidgets/QApplication>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlFileSelector>
#include <QtQml>

int __attribute__((visibility("default"))) main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QQmlFileSelector selector(&engine);

    qmlRegisterType<UpnpWebSocketClient>("org.mgallien.QmlExtension", 1, 0, "UpnpWebSocketClient");
    qmlRegisterType<UpnpWebSocketCertificateConfiguration>("org.mgallien.QmlExtension", 1, 0, "UpnpSsdpCertificateConfiguration");
    qmlRegisterType<UpnpWebSocketPublisher>("org.mgallien.QmlExtension", 1, 0, "UpnpWebSocketPublisher");
    qmlRegisterType<UpnpBinaryLight>("org.mgallien.QmlExtension", 1, 0, "UpnpBinaryLight");
    qmlRegisterType<OlinuxinoRemoteRelay>("org.mgallien.QmlExtension", 1, 0, "OlinuxinoRemoteRelay");

    qRegisterMetaType<QSharedPointer<UpnpDeviceDescription>>();

    engine.load(QUrl(QStringLiteral("qrc:/binaryLight.qml")));

    return app.exec();
}
