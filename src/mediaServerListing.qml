import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import org.mgallien.QmlExtension 1.0

Item {
    property UpnpControlContentDirectory contentDirectoryService
    property string rootId
    property StackView stackView

    width: parent.width
    height: parent.height

    Loader {
        anchors.fill: parent

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

        Button {
            id: backButton
            visible:(rootId != '0')
            height: if (rootId == '0')
                        0
                    else
                        25
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            onClicked: stackView.pop()
            text: 'Back'
        }

        TableView {
            id: contentDirectoryView
            anchors.top: backButton.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            model: contentDirectoryModel

            TableViewColumn {
                role: "title"
                title: "Title"
                width: 100
            }

            TableViewColumn {
                role: "count"
                title: "Count"
                width: 50
            }

            TableViewColumn {
                role: "class"
                title: "Class"
                width: 50
            }

            onClicked:
            {
                stackView.push({
                                   item: Qt.resolvedUrl("mediaServerListing.qml"),
                                   properties: {
                                       'contentDirectoryService': contentDirectoryService,
                                       'rootId': contentDirectoryModel.objectIdByRow(row),
                                       'stackView': stackView
                                   }
                               })
            }
        }
    }
}
