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

#include "upnplistenner.h"
#include "upnpdevicedescription.h"
#include "upnpdevicemodel.h"
#include "upnpbasictypes.h"
#include "upnpcontrolswitchpower.h"

#include <QtWidgets/QApplication>

#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlEngine>
#include <QtQml>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    UpnpListenner::UpnpInit();

    qmlRegisterType<UpnpListenner>("org.mgallien.QmlExtension", 1, 0, "UpnpListenner");
    qmlRegisterType<UpnpDeviceDescription>("org.mgallien.QmlExtension", 1, 0, "UpnpDeviceDescription");
    qmlRegisterType<UpnpDeviceModel>("org.mgallien.QmlExtension", 1, 0, "UpnpDeviceModel");
    qmlRegisterType<UpnpControlSwitchPower>("org.mgallien.QmlExtension", 1, 0, "UpnpControlSwitchPower");

    qRegisterMetaType<A_ARG_TYPE_InstanceID>();
    qRegisterMetaType<Upnp_Discovery>();

    QQmlApplicationEngine engine(QUrl(QStringLiteral("./main.qml")));

    app.exec();

    UpnpListenner::UpnpDiscoveryFinish();

    return 0;
}
