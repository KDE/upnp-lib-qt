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

    UpnpDeviceModel {
        id: deviceModel
        listenner: myListenner

        Component.onCompleted: {
           myListenner.searchAllUpnpDevice();
        }
    }

    UpnpDeviceDescription {
        id: deviceParser
    }

    Component {
        id: rowDelegate
        Item {
            height: 45
            Rectangle {
                color: styleData.selected ? "#448" : (styleData.alternate ? "#eee" : "#fff")
                border.color:"#ccc"
                border.width: 1
                anchors.fill: parent
            }
        }
    }

    Component {
        id: deviceDelegate
        Item {
            anchors.rightMargin: 4
            anchors.leftMargin: 4
            anchors.left: parent.left
            anchors.right: parent.right
            Label {
                id: name
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 2
                anchors.leftMargin: 2
                color: styleData.textColor
                text: {if (model != undefined) model.name ; if (model == undefined) ""}
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                height: 30
                font.italic: true
                font.pointSize: 12
            }
            Label {
                anchors.top: name.bottom
                anchors.bottom: parent.bottom
                anchors.right: name.right
                anchors.rightMargin: 8
                anchors.leftMargin: 2
                color: styleData.textColor
                text: {if (model != undefined) model.upnpType ; if (model == undefined) ""}
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                height: 15
                visible: styleData.selected
            }
        }
    }

    Row {
        id: row1

        Button {
            id: backButton
            text: "Test"
            onClicked: deviceParser.downloadAndParseDeviceDescription('http://192.168.5.2:42478/a619bc5b-952e-434e-9bf9-d9fa5662be68.xml')
        }

        Button {
            id: callButton
            width: 80
            height: 25
            anchors.top: backButton.bottom
            text: "Call Action"
            anchors.topMargin: 6
            onClicked: deviceParser.serviceById("urn:upnp-org:serviceId:ConnectionManager").callAction("PrepareForConnection", ["0"])
        }

        Button {
            id: subscribeButton
            width: 80
            height: 25
            anchors.top: backButton.bottom
            text: "Call Subscribe"
            anchors.topMargin: 6
            onClicked:
            {
                deviceParser.serviceById("urn:upnp-org:serviceId:ConnectionManager").subscribeEvents()
                deviceParser.serviceById("urn:upnp-org:serviceId:AVTransport").subscribeEvents()
                deviceParser.serviceById("urn:upnp-org:serviceId:RenderingControl").subscribeEvents()
            }
        }

        Button {
            id: button1
            text: "Button"
            anchors.top: backButton.bottom
            anchors.topMargin: 6
            onClicked: deviceParser.serviceById("urn:upnp-org:serviceId:SwitchPower:1").SetTarget(true)
        }

        TableView {
            id: peersView
            model: deviceModel
            itemDelegate: deviceDelegate
            rowDelegate: rowDelegate
            width: 200
            height: 400
            headerVisible: false
            TableViewColumn
            {
                width: peersView.width - 2
                resizable: false
                movable: false
            }
        }
    }
}
