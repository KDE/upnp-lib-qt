import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

Item {
    property UpnpControlMediaServer aDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager
    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    width: parent.width
    height: parent.height

    Button {
        id: backButton
        height: 25
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: if (listingView.depth > 1) {
                       listingView.pop()
                   } else {
                       parentStackView.pop()
                   }
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

    UpnpContentDirectoryModel {
        id: contentDirectoryModel
        browseFlag: globalBrowseFlag
        filter: globalFilter
        sortCriteria: globalSortCriteria
        contentDirectory: aDevice.serviceById('urn:upnp-org:serviceId:ContentDirectory')
    }

    Component.onCompleted: {
        connectionManager = aDevice.serviceById('urn:upnp-org:serviceId:ConnectionManager')
        listingView.push({
                           item: Qt.resolvedUrl("mediaServerListing.qml"),
                           properties: {
                               'contentDirectoryService': contentDirectoryModel.contentDirectory,
                               'rootId': '0',
                               'stackView': listingView,
                               'contentModel': contentDirectoryModel
                           }
                       })
    }
}

