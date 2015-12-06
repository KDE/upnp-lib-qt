import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Window 2.2

Item {
    id: headerBar

    property string title
    property string artist
    property string album
    property string image
    property string tracksCount
    property int trackRating
    property bool ratingVisible

    ColumnLayout {
        width: parent.width
        height: parent.height
        spacing: 0

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        RowLayout {
            spacing: 0

            Layout.alignment: Qt.AlignVCenter
            Layout.preferredHeight: Screen.pixelDensity * 20.
            Layout.minimumHeight: Screen.pixelDensity * 20.
            Layout.maximumHeight: Screen.pixelDensity * 20.
            Layout.fillWidth: true

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 50.
                Layout.minimumWidth: Screen.pixelDensity * 50.
                Layout.maximumWidth: Screen.pixelDensity * 50.
                Layout.fillHeight: true
            }

            Image {
                id: mainIcon
                source: image
                fillMode: Image.PreserveAspectFit

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.preferredHeight: Screen.pixelDensity * 20.
                Layout.minimumHeight: Screen.pixelDensity * 20.
                Layout.maximumHeight: Screen.pixelDensity * 20.
                Layout.preferredWidth: Screen.pixelDensity * 20.
                Layout.minimumWidth: Screen.pixelDensity * 20.
                Layout.maximumWidth: Screen.pixelDensity * 20.
            }

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 2.
                Layout.minimumWidth: Screen.pixelDensity * 2.
                Layout.maximumWidth: Screen.pixelDensity * 2.
                Layout.fillHeight: true
            }

            ColumnLayout {
                spacing: 0

                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                Layout.preferredWidth: Screen.pixelDensity * 80.
                Layout.minimumWidth: Screen.pixelDensity * 80.
                Layout.maximumWidth: Screen.pixelDensity * 80.
                Layout.fillHeight: true

                Item {
                    Layout.fillHeight: true
                }

                Label {
                    id: mainLabel
                    text: title
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
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    elide: "ElideRight"
                }

                Item {
                    Layout.fillHeight: true
                }

                Label {
                    id: albumLabel
                    text: album
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                    elide: "ElideRight"
                }

                Item {
                    Layout.fillHeight: true
                }

                RatingStar {
                    id: mainRating
                    visible: ratingVisible
                    starSize: 20
                    starRating: trackRating
                    Layout.alignment: Qt.AlignLeft
                }

                Item {
                    Layout.fillHeight: true
                }
            }

            Item {
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Label {
                id: remainingTracksLabel
                text: tracksCount + ' tracks remaining'
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                elide: "ElideRight"
                visible: tracksCount > 0
            }

            Item {
                Layout.preferredWidth: Screen.pixelDensity * 40.
                Layout.minimumWidth: Screen.pixelDensity * 40.
                Layout.maximumWidth: Screen.pixelDensity * 40.
                Layout.fillHeight: true
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
