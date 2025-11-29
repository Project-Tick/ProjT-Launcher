// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "NavigationController.js" as Navigation

Rectangle {
    id: root
    color: "#1b1b1b"
    anchors.fill: parent
    property string initialRoute: shellState ? shellState.lastPageRoute : "instances"
    property int storedSidebarWidth: shellState ? shellState.sidebarWidth : 200

    function busyState() {
        return (ProjT.launcherVM && ProjT.launcherVM.busy) ||
               (ProjT.instancesVM && ProjT.instancesVM.busy) ||
               (ProjT.newsVM && ProjT.newsVM.busy) ||
               (ProjT.settingsVM && ProjT.settingsVM.busy)
    }

    ListModel {
        id: navModel
        ListElement { route: "instances"; title: qsTr("Instances"); source: "InstancePage.qml" }
        ListElement { route: "news"; title: qsTr("News"); source: "NewsPage.qml" }
        ListElement { route: "settings"; title: qsTr("Settings"); source: "SettingsPage.qml" }
        ListElement { route: "about"; title: qsTr("About"); source: "AboutPage.qml" }
    }

    Component.onCompleted: {
        Navigation.configure(stackView, navModel, busyState, function(route) {
            if (shellState) {
                shellState.lastPageRoute = route
            }
        })
        if (!Navigation.go(initialRoute)) {
            Navigation.go("instances")
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: sidebar
            color: "#222327"
            Layout.preferredWidth: storedSidebarWidth > 80 ? storedSidebarWidth : 200
            Layout.fillHeight: true
            onWidthChanged: {
                if (shellState && width > 0 && Math.abs(width - shellState.sidebarWidth) > 1) {
                    shellState.sidebarWidth = width
                }
            }
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Label {
                    text: ProjT.launcherVM ? ProjT.launcherVM.displayName : qsTr("ProjT Launcher")
                    color: "#f5f5f5"
                    font.pointSize: 14
                    font.bold: true
                    wrapMode: Text.WordWrap
                }
                Label {
                    text: ProjT.launcherVM ? ProjT.launcherVM.versionString : ""
                    color: "#b0bec5"
                    font.pointSize: 11
                    wrapMode: Text.WordWrap
                }

                ToolSeparator { Layout.fillWidth: true }

                Repeater {
                    model: navModel
                    delegate: Button {
                        text: title
                        checkable: true
                        checked: Navigation.currentRoute() === route
                        Layout.fillWidth: true
                        onClicked: Navigation.go(route)
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }

        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true
            onCurrentItemChanged: {
                if (shellState) {
                    shellState.lastPageRoute = Navigation.currentRoute()
                }
            }
        }
    }
}
