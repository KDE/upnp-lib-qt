import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView
    property UpnpContentDirectoryModel contentModel

    GridView {
        id: contentDirectoryView
        anchors.fill: parent
        snapMode: GridView.SnapToRow
        cacheBuffer: 0
        displayMarginBeginning: 0
        displayMarginEnd: 0

        model: DelegateModel {
            id: delegateContentModel
            model: contentModel
            rootIndex: contentModel.indexFromId(rootId)

            delegate: Item {
                id: mediaServerEntry

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
                                               item: Qt.resolvedUrl("mediaPlayer.qml"),
                                               properties: {
                                                   'audioUrl': '',
                                                   'stackView': stackView,
                                                   'contentModel': contentModel
                                               }
                                           })
                        }
                        else if (itemClass == UpnpContentDirectoryModel.Album)
                        {
                            stackView.push({
                                               item: Qt.resolvedUrl("mediaAlbumView.qml"),
                                               properties: {
                                                   'contentDirectoryService': contentDirectoryService,
                                                   'rootId': contentModel.objectIdByIndex(delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex)),
                                                   'stackView': stackView,
                                                   'contentModel': contentModel
                                               }
                                           })
                        }
                        else
                        {
                            stackView.push({
                                               item: Qt.resolvedUrl("mediaServerListing.qml"),
                                               properties: {
                                                   'contentDirectoryService': contentDirectoryService,
                                                   'rootId': contentModel.objectIdByIndex(delegateContentModel.modelIndex(mediaServerEntry.DelegateModel.itemsIndex)),
                                                   'stackView': stackView,
                                                   'contentModel': contentModel
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
                        text: if (title != undefined)
                                  title
                              else
                                  ''
                        width: parent.width
                        elide: "ElideRight"
                    }
                }
            }
        }

        focus: true
        contentWidth: parent.width
        contentHeight: parent.height
    }
}
