import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2

Item {
    id: viewAlbumDelegate

    property string title
    property string artist
    property string itemDecoration
    property string duration
    property int trackRating
    property bool isPlaying

    RowLayout {
        width: parent.width
        height: parent.height
        spacing: 0

        Image {
            id: mainIcon
            source: itemDecoration
            Layout.preferredWidth: parent.height * 0.9
            Layout.preferredHeight: parent.height * 0.9
            width: parent.height * 0.9
            height: parent.height * 0.9
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
        }

        ColumnLayout {
            Layout.preferredWidth: 10
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
            spacing: 0

            Item {
                Layout.preferredHeight: 10
                Layout.minimumHeight: 10
                Layout.maximumHeight: 10
            }

            Label {
                id: mainLabel
                text: title
                Layout.preferredWidth: 50
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.fillHeight: true
            }

            Label {
                id: authorLabel
                text: artist
                Layout.preferredWidth: 50
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignLeft
                elide: "ElideRight"
            }

            Item {
                Layout.preferredHeight: 10
                Layout.minimumHeight: 10
                Layout.maximumHeight: 10
            }
        }

        Image {
            id: playIcon
            source: 'image://icon/media-playback-start'
            Layout.preferredWidth: parent.height * 0.5
            Layout.preferredHeight: parent.height * 0.5
            Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            Layout.maximumWidth: parent.height * 0.7
            Layout.maximumHeight: parent.height * 0.7
            width: parent.height * 0.7
            height: parent.height * 0.7
            sourceSize.width: width
            sourceSize.height: width
            fillMode: Image.PreserveAspectFit
            visible: isPlaying
        }

        Item {
            Layout.preferredWidth: width
            Layout.minimumWidth: width
            Layout.maximumWidth: width
            width: 50
        }

        ColumnLayout {
            Layout.preferredHeight: viewAlbumDelegate.height
            Layout.minimumHeight: viewAlbumDelegate.height
            Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
            spacing: 0

            Item {
                Layout.preferredHeight: 10
                Layout.minimumHeight: 10
                Layout.maximumHeight: 10
            }

            Label {
                id: durationLabel
                text: duration
                elide: "ElideRight"
                Layout.alignment: Qt.AlignRight
            }

            Item {
                Layout.fillHeight: true
            }

            RatingStar {
                id: mainRating
                starSize: 20
                starRating: trackRating
                Layout.alignment: Qt.AlignRight
            }

            Item {
                Layout.preferredHeight: 10
                Layout.minimumHeight: 10
                Layout.maximumHeight: 10
            }
        }

        Item {
            Layout.preferredWidth: 20
            Layout.minimumWidth: 20
            Layout.maximumWidth: 20
        }
    }
}

