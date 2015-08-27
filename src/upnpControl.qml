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

    UpnpSsdpEngine {
        id: mySsdpEngine
    }

    UpnpDeviceModel {
        id: deviceModel
        listenner: mySsdpEngine

        Component.onCompleted: {
            mySsdpEngine.searchAllUpnpDevice();
        }
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
            id: serviceItem
            anchors.rightMargin: 4
            anchors.leftMargin: 4
            anchors.left: parent.left
            anchors.right: parent.right

            property UpnpControlSwitchPower upnpService

            Label {
                id: nameLabel
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 2
                anchors.leftMargin: 2
                color: styleData.textColor
                text: {
                    if (model != undefined)
                        model.name
                    else
                        ""
                }
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                height: 30
                font.italic: true
                font.pointSize: 12
            }
            Label {
                id: deviceTypeLabel
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: nameLabel.right
                anchors.rightMargin: 8
                anchors.leftMargin: 2
                color: styleData.textColor
                text: {if (model != undefined) model.upnpType ; if (model == undefined) ""}
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                height: 15
                visible: styleData.selected
                onVisibleChanged: {
                    if (visible)
                        deviceControlLoader.setSource(model.viewName, { 'aDevice' : deviceModel.getDeviceDescription(model.uuid)})
                }
            }
            Label {
                id: statusLabel
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: deviceTypeLabel.left
                anchors.rightMargin: 8
                anchors.leftMargin: 2
                color: styleData.textColor
                text: ''
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                height: 15
            }

            Button {
                id: setTargetButton
                text: "Set Target"
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: statusLabel.left

                onClicked: {
                    if (upnpService == undefined) upnpService = deviceModel.getDeviceDescription(model.uuid).serviceById('urn:upnp-org:serviceId:SwitchPower')
                    if (upnpService != undefined) upnpService.setTarget(statusLabel.text != 'on')
                }
            }
            Button {
                id: getTargetButton
                text: "Get Target"
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: setTargetButton.left

                onClicked: {
                    if (upnpService == undefined) upnpService = deviceModel.getDeviceDescription(model.uuid).serviceById('urn:upnp-org:serviceId:SwitchPower')
                    if (upnpService != undefined) upnpService.getTarget()
                }
            }
            Button {
                id: getStatusButton
                text: "Get Status"
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: getTargetButton.left

                onClicked: {
                    if (upnpService == undefined) upnpService = deviceModel.getDeviceDescription(model.uuid).serviceById('urn:upnp-org:serviceId:SwitchPower')
                    if (upnpService != undefined) upnpService.getStatus()
                }
            }
            Button {
                id: subscribeButton
                text: "Subscribe"
                anchors.top: nameLabel.bottom
                anchors.bottom: parent.bottom
                anchors.right: getStatusButton.left

                onClicked: {
                    if (upnpService == undefined) upnpService = deviceModel.getDeviceDescription(model.uuid).serviceById('urn:upnp-org:serviceId:SwitchPower')
                    if (upnpService != undefined) upnpService.subscribeEvents(600)
                }
            }

            Connections {
                target: upnpService

                onGetStatusFinished: {
                    console.log("success is ", success)
                    console.log("status is ", status)
                }
            }

            Connections {
                target: upnpService

                onStatusChanged: {
                    if (upnpService.status)
                        statusLabel.text = 'on'
                    else
                        statusLabel.text = 'off'
                }
            }
        }
    }

    TableView {
        id: peersView
        model: deviceModel
        itemDelegate: deviceDelegate
        rowDelegate: rowDelegate
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 500
        headerVisible: false
        TableViewColumn
        {
            width: peersView.width - 4
            resizable: false
            movable: false
        }

        onSelectionChanged:
        {

        }
    }
    Item {
        id: deviceControl
        anchors.left: peersView.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        Loader {
            anchors.fill: parent
            id: deviceControlLoader
        }
    }
}
