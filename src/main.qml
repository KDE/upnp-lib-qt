/*
 * Copyright 2014 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 800
    minimumHeight: 400
    title: 'upnp'
    id: mainWindow

    UpnpListenner {
        id: myListenner
    }

    UpnpDeviceDiscovery {
        id: deviceParser
    }

    UpnpServiceCaller {
        id: serviceCaller
        endPoint: 'http://127.0.0.1:49494/upnp/control/rendertransport1'
        messageNamespace: 'urn:schemas-upnp-org:service:AVTransport:1'
    }

    Button {
        id: backButton
        text: "Test"
        onClicked: deviceParser.downloadAndParseDeviceDescription('http://127.0.0.1:49494/description.xml')
    }
}
