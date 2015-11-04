import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

Item {
    property UpnpControlMediaServer aDevice
    property StackView stackView
    property UpnpControlConnectionManager connectionManager

    width: parent.width
    height: parent.height

    Button {
        id: backButton
        height: Screen.pixelDensity * 10
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: if (listingView.depth > 1)
                       listingView.pop()
                   else
                       stackView.pop()
        text: 'Back'
    }

    StackView {
        id: listingView
        anchors.top: backButton.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left

        // Implements back key navigation
        focus: true
        Keys.onReleased: if (event.key === Qt.Key_Back && stackView.depth > 1) {
                             stackView.pop();
                             event.accepted = true;
                         }
    }

    Component.onCompleted: {
        connectionManager = aDevice.serviceById('urn:upnp-org:serviceId:ConnectionManager')
        listingView.push({
                           item: Qt.resolvedUrl("mediaServerListingMobile.qml"),
                           properties: {
                               'contentDirectoryService': aDevice.serviceById('urn:upnp-org:serviceId:ContentDirectory'),
                               'rootId': '0',
                               'stackView': listingView
                           }
                       })
    }
}

