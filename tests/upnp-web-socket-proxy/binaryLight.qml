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

import QtQml 2.2

import org.mgallien.QmlExtension 1.0

UpnpBinaryLight {
    id: myLight

    property var actuator

    udn: '4424b320-9657-419c-8935-a9fe76170f09'

    webSocketPublisher: UpnpWebSocketPublisher {
        id: server

        certificateConfiguration.certificateAuthorityFileName: './rootKey.crt'
        certificateConfiguration.certificateFileName: './moi_test.pem'

        description: myLight.description

        Component.onCompleted: {
            connectServer('wss://home:11443/')
        }
    }

    actuator: OlinuxinoRemoteRelay {
        id: myExteriorLight

        relayAddress: '192.168.0.5'
        relayIndex: 0
    }

    onSetTarget: {
        console.log(target + ' SetTarget')
        if (target) {
            myExteriorLight.activate(OlinuxinoRemoteRelay.SwitchOn)
        } else {
            myExteriorLight.activate(OlinuxinoRemoteRelay.SwitchOff)
        }
    }
}
