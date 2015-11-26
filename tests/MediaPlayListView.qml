import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQuick.Window 2.2
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Item {
    property UpnpControlMediaServer mediaServerDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager
    property MediaPlayerControl playControl
    property MediaPlayList playListModel

    Component {
        id: rowDelegate

        Item {
            id: rowDelegateContent
            height: Screen.pixelDensity * 20.
            Rectangle {
                color: "#fff"
                anchors.fill: parent
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        TableView {
            id: playListView

            model: DelegateModel {
                model: playListModel

                delegate: AudioTrackDelegate {
                    height: Screen.pixelDensity * 20.
                    width: playListView.width
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
                    isPlaying: model.isPlaying
                }
            }

            headerVisible: false
            frameVisible: false
            focus: true
            rowDelegate: rowDelegate

            TableViewColumn {
                role: "title"
                title: "Title"
            }

            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}

