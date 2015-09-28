import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlMediaServer aDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager
    property string globalBrowseFlag: 'BrowseDirectChildren'
    property string globalFilter: '*'
    property string globalSortCriteria: ''

    width: parent.width
    height: parent.height

    Audio {
        id: player
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        MediaPlayerControl {
            id: playControl

            volume: player.volume
            position: player.position
            duration: player.duration
            muted: player.muted
            isPlaying: (player.playbackState == Audio.PlayingState)
            seekable: player.seekable

            Layout.preferredHeight: 60
            Layout.minimumHeight: Layout.preferredHeight
            Layout.maximumHeight: Layout.preferredHeight
            Layout.fillWidth: true

            onPlay: player.play()
            onPause: player.pause()
            onSeek: player.seek(position)
            onVolumeChanged: player.volume = volume
            onMutedChanged: player.muted = muted
        }

        Button {
            id: backButton

            height: 25
            Layout.preferredHeight: height
            Layout.minimumHeight: height
            Layout.maximumHeight: height
            Layout.fillWidth: true

            onClicked: if (listingView.depth > 1) {
                           listingView.pop()
                       } else {
                           parentStackView.pop()
                       }
            text: 'Back'
        }

        StackView {
            id: listingView

            Layout.fillHeight: true
            Layout.fillWidth: true

            // Implements back key navigation
            focus: true
            Keys.onReleased: if (event.key === Qt.Key_Back && stackView.depth > 1) {
                                 stackView.pop();
                                 event.accepted = true;
                             }
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
                               'contentModel': contentDirectoryModel,
                               'player': player,
                               'playControl': playControl
                           }
                       })
    }
}

