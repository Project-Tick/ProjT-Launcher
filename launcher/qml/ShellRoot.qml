// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import ProjTLauncher 1.0
import "components"

Rectangle {
    id: root
    color: "#1b1b1b"
    anchors.fill: parent
    property int storedSidebarWidth: shellState ? shellState.sidebarWidth : 200

    property var navEntries: [
        { title: qsTr("Instances"), page: LauncherViewModelEnums.Page.Instances, source: "InstancePage.qml" },
        { title: qsTr("News"), page: LauncherViewModelEnums.Page.News, source: "NewsPage.qml" },
        { title: qsTr("Settings"), page: LauncherViewModelEnums.Page.Settings, source: "SettingsPage.qml" },
        { title: qsTr("Logs"), page: LauncherViewModelEnums.Page.Logs, source: "LogsPage.qml" },
        { title: qsTr("About"), page: LauncherViewModelEnums.Page.About, source: "AboutPage.qml" }
    ]

    function pageSource(page) {
        switch (page) {
        case LauncherViewModelEnums.Page.News:
            return "NewsPage.qml"
        case LauncherViewModelEnums.Page.Settings:
            return "SettingsPage.qml"
        case LauncherViewModelEnums.Page.About:
            return "AboutPage.qml"
        case LauncherViewModelEnums.Page.Logs:
            return "LogsPage.qml"
        case LauncherViewModelEnums.Page.Instances:
        default:
            return "InstancePage.qml"
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            id: sidebar
            color: "#222327"
            width: 180
            Layout.preferredWidth: 180
            Layout.fillHeight: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

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
                    model: navEntries
                    delegate: Button {
                        text: modelData.title
                        checkable: true
                        property int targetPage: modelData.page
                        checked: ProjT.launcherVM && ProjT.launcherVM.currentPage === targetPage
                        implicitHeight: 40
                        implicitWidth: (contentItem ? contentItem.implicitWidth : 96) + 24
                        Layout.preferredWidth: implicitWidth
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        onClicked: {
                            if (ProjT.launcherVM) {
                                ProjT.launcherVM.currentPage = targetPage
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }

        Loader {
            id: pageLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: pageSource(ProjT.launcherVM ? ProjT.launcherVM.currentPage : LauncherViewModelEnums.Page.Instances)
        }
    }
}
