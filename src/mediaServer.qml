import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

Item {
    property UpnpControlMediaServer aDevice
    property UpnpControlConnectionManager connectionManager

    Button {
        id: getProtocolInfoButton
        text: 'getProtocolInfo'
        onClicked: connectionManager.getProtocolInfo()
    }

    Button {
        id: prepareForConnectionButton
        anchors.top: getProtocolInfoButton.bottom
        text: 'prepareForConnection'
        onClicked: connectionManager.prepareForConnection()
    }

    Button {
        id: connectionCompleteButton
        anchors.top: prepareForConnectionButton.bottom
        text: 'connectionComplete'
        onClicked: connectionManager.connectionComplete()
    }

    Button {
        id: getCurrentConnectionIDsButton
        anchors.top: connectionCompleteButton.bottom
        text: 'getCurrentConnectionIDs'
        onClicked: connectionManager.getCurrentConnectionIDs()
    }

    Button {
        id: getCurrentConnectionInfoButton
        anchors.top: getCurrentConnectionIDsButton.bottom
        text: 'getCurrentConnectionInfo'
        onClicked: connectionManager.getCurrentConnectionInfo()
    }

    Component.onCompleted: {
        connectionManager = aDevice.serviceById('urn:upnp-org:serviceId:ConnectionManager')
    }
}

