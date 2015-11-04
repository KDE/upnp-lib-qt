import QtQuick 2.0
import QtQuick.Window 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView

    Component {
        id: mediaServerEntry

        Item {
            width: contentDirectoryView.cellWidth
            height: contentDirectoryView.cellHeight

            MouseArea {
                id: clickHandle

                width: parent.width
                height: parent.height

                onClicked: {
                    if (itemClass == UpnpContentDirectoryModel.AudioTrack)
                    {
                        stackView.push({
                                           item: Qt.resolvedUrl("mediaPlayerMobile.qml"),
                                           properties: {
                                               'audioUrl': '',
                                               'stackView': stackView
                                           }
                                       })
                    }
                    else if (itemClass == UpnpContentDirectoryModel.Album)
                    {
                        stackView.push({
                                           item: Qt.resolvedUrl("mediaAlbumViewMobile.qml"),
                                           properties: {
                                               'contentDirectoryService': contentDirectoryService,
                                               'rootId': contentDirectoryModel.objectIdByRow(index),
                                               'stackView': stackView
                                           }
                                       })
                    }
                    else
                    {
                        stackView.push({
                                           item: Qt.resolvedUrl("mediaServerListingMobile.qml"),
                                           properties: {
                                               'contentDirectoryService': contentDirectoryService,
                                               'rootId': contentDirectoryModel.objectIdByRow(index),
                                               'stackView': stackView
                                           }
                                       })
                    }
                }
            }

            Column {
                width: parent.width
                height: parent.height

                Image {
                    id: playIcon
                    source: image
                    width: parent.height * 0.8
                    height: parent.height * 0.8
                    sourceSize.width: width
                    sourceSize.height: width
                    fillMode: Image.PreserveAspectFit
                }

                Label {
                    id: mainLabel
                    text: title
                    width: parent.width
                    elide: "ElideRight"
                }
            }
        }
    }

    UpnpContentDirectoryModel {
        id: contentDirectoryModel
        browseFlag: 'BrowseDirectChildren'
        filter: '*'
        sortCriteria: ''
        contentDirectory: contentDirectoryService

        Component.onCompleted: {
            contentDirectoryModel.forceRefresh(rootId)
        }
    }

    GridView {
        id: contentDirectoryView
        anchors.fill: parent
        model: contentDirectoryModel

        delegate: mediaServerEntry

        focus: true
        contentWidth: parent.width
        contentHeight: parent.height

        cellHeight: Screen.pixelDensity * 15
        cellWidth: Screen.pixelDensity * 15
    }
}
