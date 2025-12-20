// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0

WindowDialog {
    id: root

    title: qsTr("Select Release to Install")
    modal: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    property alias selectedVersion: versionsTree.currentIndex
    property string selectedChangelog: ""

    // Model to be set externally
    property var releasesModel: ListModel {}

    width: 468
    height: 385

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        // Explanation Label
        Label {
            id: explainLabel
            text: qsTr("Please select the release you wish to update to.")
            color: ThemeColors.text
            Layout.fillWidth: true
        }

        // Versions Tree
        TreeView {
            id: versionsTree
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 150

            alternatingRows: true
            clip: true

            model: releasesModel

            delegate: TreeViewDelegate {
                contentItem: RowLayout {
                    spacing: 8

                    Image {
                        source: model.icon || ""
                        sourceSize: Qt.size(16, 16)
                        visible: model.icon !== undefined
                    }

                    Label {
                        text: model.display || model.version || ""
                        color: ThemeColors.text
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Label {
                        text: model.date || ""
                        color: ThemeColors.textSecondary
                        font.pixelSize: 11
                    }
                }

                background: Rectangle {
                    color: row === versionsTree.currentIndex ? ThemeColors.highlight : (row % 2 === 0 ? ThemeColors.base : ThemeColors.alternateBase)
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex >= 0 && releasesModel.count > 0) {
                    let item = releasesModel.get(currentIndex);
                    if (item) {
                        root.selectedChangelog = item.changelog || "";
                    }
                }
            }
        }

        // Changelog Browser
        ScrollView {
            id: changelogScroll
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredHeight: 150

            TextArea {
                id: changelogTextBrowser
                readOnly: true
                textFormat: TextEdit.MarkdownText
                text: root.selectedChangelog || qsTr("Select a version to view its changelog.")
                color: ThemeColors.text
                wrapMode: TextEdit.Wrap

                background: Rectangle {
                    color: ThemeColors.base
                    border.color: ThemeColors.mid
                    border.width: 1
                    radius: 4
                }
            }
        }
    }

    background: Rectangle {
        color: ThemeColors.window
        border.color: ThemeColors.mid
        border.width: 1
        radius: 8
    }

    // Helper functions
    function setReleases(releases) {
        releasesModel.clear();
        for (let i = 0; i < releases.length; i++) {
            releasesModel.append(releases[i]);
        }
        if (releases.length > 0) {
            versionsTree.currentIndex = 0;
        }
    }

    function getSelectedRelease() {
        if (versionsTree.currentIndex >= 0 && releasesModel.count > 0) {
            return releasesModel.get(versionsTree.currentIndex);
        }
        return null;
    }
}
