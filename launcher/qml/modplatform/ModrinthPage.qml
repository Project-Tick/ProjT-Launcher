// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Rectangle {
    id: modrinthPage
    color: ThemeColors.background

    property var vm: typeof ProjT !== "undefined" ? ProjT.modrinthVM : null

    Component.onCompleted: {
        if (vm)
            vm.refresh();
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // Search row
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            Button {
                text: qsTr("Filter options")
                checkable: true
                checked: filterPanel.visible
                onClicked: filterPanel.visible = !filterPanel.visible
            }

            TextField {
                id: searchField
                Layout.fillWidth: true
                placeholderText: qsTr("Search and filter...")
                text: vm ? vm.searchTerm : ""
                onTextChanged: {
                    if (vm)
                        vm.search(text);
                }
                onAccepted: {
                    if (vm)
                        vm.refresh();
                }
            }
        }

        // Main content splitter
        SplitView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Horizontal

            // Filter panel (hidden by default)
            Rectangle {
                id: filterPanel
                visible: false
                SplitView.preferredWidth: 200
                SplitView.minimumWidth: 150
                SplitView.maximumWidth: 300
                color: ThemeColors.surface

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingS
                    spacing: Theme.spacingS

                    Label {
                        text: qsTr("Categories")
                        font.bold: true
                        color: ThemeColors.text
                    }

                    ComboBox {
                        Layout.fillWidth: true
                        model: vm ? vm.categories : [qsTr("All Categories")]
                        currentIndex: vm ? vm.selectedCategoryIndex : 0
                        onCurrentIndexChanged: {
                            if (vm && currentIndex !== vm.selectedCategoryIndex) {
                                vm.selectedCategoryIndex = currentIndex;
                            }
                        }
                    }

                    Label {
                        text: qsTr("Loader")
                        font.bold: true
                        color: ThemeColors.text
                    }

                    ComboBox {
                        Layout.fillWidth: true
                        model: vm ? vm.loaders : []
                        currentIndex: vm ? vm.selectedLoaderIndex : 0
                        onCurrentIndexChanged: {
                            if (vm && currentIndex !== vm.selectedLoaderIndex) {
                                vm.selectedLoaderIndex = currentIndex;
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            // Pack list
            ListView {
                id: packsList
                SplitView.fillWidth: true
                SplitView.minimumWidth: 250
                clip: true
                model: vm ? vm.packsModel : null
                currentIndex: vm ? vm.selectedPackIndex : -1

                ScrollBar.vertical: ScrollBar {}

                delegate: ItemDelegate {
                    width: packsList.width
                    height: 58
                    highlighted: ListView.isCurrentItem

                    background: Rectangle {
                        color: highlighted ? ThemeColors.primary : (index % 2 === 0 ? "transparent" : ThemeColors.backgroundAlt)
                        opacity: highlighted ? 0.2 : 0.3
                    }

                    onClicked: {
                        if (vm)
                            vm.selectPack(index);
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 4
                        spacing: 8

                        Image {
                            Layout.preferredWidth: 48
                            Layout.preferredHeight: 48
                            source: model.iconUrl || ""
                            fillMode: Image.PreserveAspectFit

                            Rectangle {
                                anchors.fill: parent
                                visible: parent.status !== Image.Ready
                                color: ThemeColors.success
                                radius: 4

                                Label {
                                    anchors.centerIn: parent
                                    text: "M"
                                    color: "white"
                                    font.bold: true
                                    font.pointSize: 16
                                }
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2

                            Label {
                                text: model.name || ""
                                color: ThemeColors.text
                                font.bold: true
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }

                            Label {
                                text: model.description ? model.description.substring(0, 100) : ""
                                color: ThemeColors.textSecondary
                                font.pointSize: 9
                                elide: Text.ElideRight
                                Layout.fillWidth: true
                            }
                        }
                    }
                }

                BusyIndicator {
                    anchors.centerIn: parent
                    running: vm ? vm.isLoading : false
                    visible: running && packsList.count === 0
                }

                Label {
                    anchors.centerIn: parent
                    visible: packsList.count === 0 && !(vm && vm.isLoading)
                    text: qsTr("No modpacks found. Search for modpacks on Modrinth.")
                    color: ThemeColors.textSecondary
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // Pack description
            ScrollView {
                id: descriptionView
                SplitView.preferredWidth: parent.width * 0.4
                SplitView.minimumWidth: 250
                clip: true

                TextArea {
                    id: packDescription
                    readOnly: true
                    textFormat: TextArea.RichText
                    wrapMode: TextArea.Wrap
                    color: ThemeColors.text

                    text: {
                        if (!vm || !vm.selectedPack || !vm.selectedPack.name) {
                            return "<i>" + qsTr("Select a modpack to view details") + "</i>";
                        }

                        var html = "";
                        var pack = vm.selectedPack;

                        // Name with link
                        if (pack.websiteUrl) {
                            html += '<a href="' + pack.websiteUrl + '">' + pack.name + '</a>';
                        } else {
                            html += "<b>" + pack.name + "</b>";
                        }

                        // Author
                        if (pack.authors) {
                            html += "<br>" + qsTr(" by ") + pack.authors;
                        }

                        // Archived status
                        if (pack.status === "archived") {
                            html += "<br><br><b style='color: orange;'>" + qsTr("This project has been archived. It will not receive any further updates unless the author decides to unarchive the project.") + "</b>";
                        }

                        // External links
                        var hasLinks = pack.issuesUrl || pack.sourceUrl || pack.wikiUrl || pack.discordUrl;
                        if (hasLinks) {
                            html += "<br><br>" + qsTr("External links:") + "<br>";
                            if (pack.issuesUrl)
                                html += "- " + qsTr("Issues:") + ' <a href="' + pack.issuesUrl + '">' + pack.issuesUrl + '</a><br>';
                            if (pack.wikiUrl)
                                html += "- " + qsTr("Wiki:") + ' <a href="' + pack.wikiUrl + '">' + pack.wikiUrl + '</a><br>';
                            if (pack.sourceUrl)
                                html += "- " + qsTr("Source:") + ' <a href="' + pack.sourceUrl + '">' + pack.sourceUrl + '</a><br>';
                            if (pack.discordUrl)
                                html += "- " + qsTr("Discord:") + ' <a href="' + pack.discordUrl + '">' + pack.discordUrl + '</a><br>';
                        }

                        html += "<hr>";

                        // Body or description
                        if (pack.body) {
                            html += pack.body;
                        } else if (pack.description) {
                            html += pack.description;
                        }

                        return html;
                    }

                    onLinkActivated: function (link) {
                        Qt.openUrlExternally(link);
                    }
                }
            }
        }

        // Bottom bar
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            ComboBox {
                id: sortCombo
                Layout.preferredWidth: 180
                model: vm ? vm.sortOptions : []
                currentIndex: vm ? vm.sortIndex : 0
                onCurrentIndexChanged: {
                    if (vm && currentIndex !== vm.sortIndex) {
                        vm.sortIndex = currentIndex;
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Version selected:")
                color: ThemeColors.text
            }

            ComboBox {
                id: versionCombo
                Layout.preferredWidth: 200
                model: vm ? vm.selectedPackVersions : []
                currentIndex: vm ? vm.selectedVersionIndex : -1
                enabled: count > 0
                onCurrentIndexChanged: {
                    if (vm && currentIndex >= 0 && currentIndex !== vm.selectedVersionIndex) {
                        vm.selectVersion(currentIndex);
                    }
                }
            }

            Button {
                text: qsTr("Install")
                enabled: vm && vm.selectedPackVersions && vm.selectedPackVersions.length > 0
                onClicked: {
                    if (vm) {
                        vm.installSelected("", "");
                    }
                }
            }
        }
    }
}
