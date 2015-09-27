import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView
    property UpnpContentDirectoryModel contentModel

    width: stackView.width
    height: stackView.height

    Component {
        id: rowDelegate

        Item {
            id: rowDelegateContent
            height: 80
            Rectangle {
                color: "#fff"
                anchors.fill: parent
            }
        }
    }

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

            Layout.preferredHeight: 80
            Layout.fillWidth: true

            onPlay: player.play()
            onPause: player.pause()
            onSeek: player.seek(position)
            onVolumeChanged: player.volume = volume
            onMutedChanged: player.muted = muted
        }

        TableView {
            id: contentDirectoryView

            model: DelegateModel {
                model: contentModel
                rootIndex: contentModel.indexFromId(rootId)

                delegate: AudioTrackDelegate {
                    height: 80
                    width: contentDirectoryView.width
                    title: if (model != undefined && model.title !== undefined)
                               model.title
                           else
                               ''
                    artist: if (model != undefined && model.artist !== undefined)
                                model.artist
                            else
                                ''
                    itemDecoration: if (model != undefined && model.image !== undefined)
                                        model.image
                                    else
                                        ''
                    duration: if (model != undefined && model.duration !== undefined)
                                  model.duration
                              else
                                  ''
                    trackRating: if (model != undefined && model.rating !== undefined)
                                     model.rating
                                 else
                                     ''
                    isPlaying: true
                }
            }

            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate

            onClicked: {
                player.source = contentModel.getUrl(model.modelIndex(row))
            }

            TableViewColumn {
                role: "title"
                title: "Title"
            }

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
