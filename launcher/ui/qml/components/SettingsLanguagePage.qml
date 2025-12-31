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
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme
import "."

Rectangle {
    id: languagePage
    color: "transparent"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: ThemeColors.spacingM

        SettingsSection {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Language Selection")
            iconSource: Theme.icon("language")

            ColumnLayout {
                spacing: ThemeColors.spacingM
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextField {
                    id: searchField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Search languages...")
                    selectByMouse: true
                    leftPadding: 32
                    background: Rectangle {
                        color: ThemeColors.bg1
                        radius: ThemeColors.radius
                        border.color: searchField.activeFocus ? ThemeColors.accent : ThemeColors.border
                    }
                    Text {
                        text: "🔍"
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        anchors.verticalCenter: parent.verticalCenter
                        color: ThemeColors.textSecondary
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: ThemeColors.bg1
                    border.color: ThemeColors.border
                    radius: ThemeColors.radius

                    ListView {
                        id: languageList
                        anchors.fill: parent
                        anchors.margins: 4
                        clip: true
                        model: translationsModel
                        spacing: 2
                        currentIndex: translationsModel ? translationsModel.selectedIndex().row : 0

                        delegate: ItemDelegate {
                            id: langDelegate
                            width: languageList.width
                            height: visible ? 40 : 0
                            highlighted: ListView.isCurrentItem
                            
                            visible: {
                                if (searchField.text.length === 0) return true;
                                var langName = model.display || "";
                                return langName.toLowerCase().indexOf(searchField.text.toLowerCase()) >= 0;
                            }

                            background: Rectangle {
                                color: langDelegate.highlighted ? ThemeColors.surfaceHighlight : (langDelegate.hovered ? ThemeColors.bg3 : "transparent")
                                radius: ThemeColors.radiusS
                                border.color: langDelegate.highlighted ? ThemeColors.border : "transparent"
                            }

                            contentItem: RowLayout {
                                spacing: ThemeColors.spacingM
                                Label {
                                    text: (model.display !== undefined ? model.display : "") || (model.name !== undefined ? model.name : "") || ""
                                    color: langDelegate.highlighted ? ThemeColors.textTitle : ThemeColors.text
                                    font.weight: langDelegate.highlighted ? Font.DemiBold : Font.Normal
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                }
                                Label {
                                    text: {
                                        if (translationsModel) {
                                            var idx = translationsModel.index(index, 1);
                                            return translationsModel.data(idx, Qt.DisplayRole) || "";
                                        }
                                        return "";
                                    }
                                    color: langDelegate.highlighted ? ThemeColors.text : ThemeColors.textSecondary
                                    font.pixelSize: 11
                                }
                            }

                            onClicked: {
                                languageList.currentIndex = index;
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

                RowLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingS

                    Text {
                        text: "💡"
                        font.pixelSize: 14
                    }
                    
                    Label {
                        text: qsTr("Translation completeness is shown on the right. Help us translate!")
                        color: ThemeColors.textSecondary
                        font.pixelSize: 11
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                    }

                    ThemedButton {
                        text: qsTr("Help Translate")
                        onClicked: Qt.openUrlExternally("https://hosted.weblate.org/engage/prismlauncher/")
                    }
                }
            }
        }
    }
}

