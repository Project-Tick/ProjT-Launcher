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
import "../Theme.js" as Theme

Rectangle {
    id: languagePage
    color: ThemeColors.background

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM

        Label {
            text: qsTr("Select Language")
            font.pointSize: 12
            font.bold: true
            color: ThemeColors.text
        }

        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search languages...")
            selectByMouse: true
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.surface
            border.color: ThemeColors.border
            border.width: 1
            radius: Theme.radiusS

            ListView {
                id: languageList
                anchors.fill: parent
                anchors.margins: 1
                clip: true

                // Use TranslationsModel from backend
                model: translationsModel

                // Get selected index from model
                currentIndex: translationsModel ? translationsModel.selectedIndex().row : 0

                delegate: ItemDelegate {
                    id: langDelegate
                    width: languageList.width
                    highlighted: ListView.isCurrentItem

                    // Filter by search text
                    visible: {
                        if (searchField.text.length === 0)
                            return true;
                        var langName = model.display || "";
                        return langName.toLowerCase().indexOf(searchField.text.toLowerCase()) >= 0;
                    }
                    height: visible ? 40 : 0

                    contentItem: RowLayout {
                        spacing: Theme.spacingM

                        // Column 0: Language name
                        Label {
                            text: model.display || ""
                            color: langDelegate.highlighted ? ThemeColors.accent : ThemeColors.text
                            font.bold: langDelegate.highlighted
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        // Column 1: Completeness percentage (if available)
                        Label {
                            // Get completeness from column 1
                            text: {
                                if (translationsModel) {
                                    var idx = translationsModel.index(index, 1);
                                    return translationsModel.data(idx, Qt.DisplayRole) || "";
                                }
                                return "";
                            }
                            color: ThemeColors.textSecondary
                            font.pointSize: 9
                        }
                    }

                    background: Rectangle {
                        color: langDelegate.highlighted ? ThemeColors.highlight : (langDelegate.hovered ? Theme.hover : "transparent")
                    }

                    onClicked: {
                        languageList.currentIndex = index;
                        // Get the language key from UserRole
                        if (translationsModel) {
                            var langKey = translationsModel.data(translationsModel.index(index, 0), Qt.UserRole);
                            if (langKey) {
                                translationsModel.selectLanguage(langKey);
                                translationsModel.updateLanguage(langKey);
                            }
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }

        Label {
            text: qsTr("Translation completeness is shown on the right. Help us translate!")
            color: ThemeColors.textSecondary
            font.pointSize: 9
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Help translate on Weblate")
            onClicked: {
                Qt.openUrlExternally("https://hosted.weblate.org/engage/prismlauncher/");
            }
        }
    }
}
