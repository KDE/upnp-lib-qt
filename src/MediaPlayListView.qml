import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import QtQml.Models 2.1
import org.mgallien.QmlExtension 1.0
import QtMultimedia 5.4

Rectangle {
    property UpnpControlMediaServer mediaServerDevice
    property StackView parentStackView
    property UpnpControlConnectionManager connectionManager
    property Audio player
    property MediaPlayerControl playControl

    color: 'black'
}

