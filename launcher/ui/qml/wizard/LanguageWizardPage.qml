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

    property string selectedLanguage: translationsModel ? translationsModel.selectedLanguage() : "en_US"
    property bool wantsRefreshButton: true

    signal languageChanged(string langCode)

    function refresh() {
        if (translationsModel && translationsModel.downloadIndex) {
            translationsModel.downloadIndex();
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingS

        // Title
        Label {
            text: qsTr("Select Language")
            font.pixelSize: Theme.fontHeader
            font.bold: true
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("Choose your preferred language for the launcher interface.")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
            font.pixelSize: Theme.fontBody
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.surface
            border.color: ThemeColors.border
            radius: Theme.radiusS

            ListView {
                id: languageList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                model: translationsModel
                currentIndex: translationsModel ? translationsModel.selectedIndex().row : 0

                delegate: ItemDelegate {
                    width: languageList.width
                    height: 40
                    highlighted: ListView.isCurrentItem

                    contentItem: RowLayout {
                        spacing: Theme.spacingM

                        Label {
                            text: model.display || ""
                            color: ThemeColors.text
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
                            color: ThemeColors.textSecondary
                            font.pixelSize: Theme.fontCaption
                        }
                    }

                    onClicked: {
                        if (!translationsModel)
                            return;
                        languageList.currentIndex = index;
                        var langKey = translationsModel.data(translationsModel.index(index, 0), Qt.UserRole);
                        if (langKey) {
                            translationsModel.selectLanguage(langKey);
                            translationsModel.updateLanguage(langKey);
                            selectedLanguage = langKey;
                            languageChanged(langKey);
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }

        Label {
            text: qsTr("Don't see your language or the quality is poor? Help us with translations!")
            color: ThemeColors.textSecondary
            font.pixelSize: Theme.fontCaption
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("Help translate on Weblate")
            onClicked: {
                Qt.openUrlExternally("https://hosted.weblate.org/engage/prismlauncher/");
            }
        }

        CheckBox {
            text: qsTr("Use system locales")
            checked: false
            onCheckedChanged: {
                if (translationsModel && translationsModel.setUseSystemLocale) {
                    translationsModel.setUseSystemLocale(checked);
                }
            }
        }
    }

    Component.onCompleted: {
        if (translationsModel) {
            selectedLanguage = translationsModel.selectedLanguage();
        }
    }
}
