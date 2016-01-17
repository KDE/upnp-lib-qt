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

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1

import org.mgallien.QmlExtension 1.0

ApplicationWindow {
    visible: true
    minimumWidth: 800
    minimumHeight: 400
    title: 'web socket'
    id: mainWindow

    UpnpWebSocketClient {
        id: server

        certificateConfiguration.certificateAuthorityFileName: './rootKey.crt'
        certificateConfiguration.certificateFileName: './moi_test.pem'

        Component.onCompleted: {
            connectServer('wss://moulinette:11443/')
        }
    }

    UpnpWebSocketDeviceModel {
        id: deviceModel
        webSocketClient: server
    }

    Component {
        id: rowDelegate

        Item {
            id: rowDelegateContent
            height: Screen.pixelDensity * 15.
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        TableView {
            backgroundVisible: false
            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate
            model: deviceModel

            TableViewColumn {
                role: "name"
                title: "name"
            }

            TableViewColumn {
                role: "udn"
                title: "UUID"
            }

            onClicked: {
                model.modelIndex(row)
            }

            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        BinaryLightControl {
            id: lightControl

            Layout.fillHeight: true
            Layout.preferredWidth: Screen.pixelDensity * 30.
            Layout.minimumWidth: Screen.pixelDensity * 20.
        }
    }
}
