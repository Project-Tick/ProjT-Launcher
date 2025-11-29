import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    width: 280
    height: 100
    color: "#00000000"
    border.color: "#40FFFFFF"
    radius: 4

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 4

        Text {
            text: launcherViewModel ? launcherViewModel.displayName : ""
            font.bold: true
            color: "white"
            wrapMode: Text.WordWrap
        }

        Text {
            text: launcherViewModel ? launcherViewModel.versionString : ""
            color: "#CCCCCC"
            wrapMode: Text.WordWrap
        }

        Text {
            text: launcherViewModel && launcherViewModel.busy ? qsTr("Status: Busy") : qsTr("Status: Idle")
            color: launcherViewModel && launcherViewModel.busy ? "#FFB74D" : "#A5D6A7"
            font.pointSize: 10
        }
    }
}
