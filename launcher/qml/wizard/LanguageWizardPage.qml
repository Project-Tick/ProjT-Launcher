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

Rectangle {
    id: languagePage
    color: ThemeColors.background

    property var vm: ProjT.settingsVM
    property string selectedLanguage: "en_US"

    signal languageChanged(string langCode)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Title
        Label {
            text: qsTr("Select Language")
            font.pixelSize: 18
            font.bold: true
            color: ThemeColors.text
        }

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("Choose your preferred language for the launcher interface.")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Language list
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.backgroundAlt
            border.color: ThemeColors.border
            radius: Theme.radiusS

            ListView {
                id: languageList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                model: vm ? vm.languageList : [
                    {
                        code: "en_US",
                        name: "English (US)"
                    },
                    {
                        code: "en_GB",
                        name: "English (UK)"
                    },
                    {
                        code: "de_DE",
                        name: "Deutsch"
                    },
                    {
                        code: "es_ES",
                        name: "Español"
                    },
                    {
                        code: "fr_FR",
                        name: "Français"
                    },
                    {
                        code: "it_IT",
                        name: "Italiano"
                    },
                    {
                        code: "ja_JP",
                        name: "日本語"
                    },
                    {
                        code: "ko_KR",
                        name: "한국어"
                    },
                    {
                        code: "pt_BR",
                        name: "Português (Brasil)"
                    },
                    {
                        code: "ru_RU",
                        name: "Русский"
                    },
                    {
                        code: "tr_TR",
                        name: "Türkçe"
                    },
                    {
                        code: "zh_CN",
                        name: "简体中文"
                    },
                    {
                        code: "zh_TW",
                        name: "繁體中文"
                    }
                ]

                delegate: ItemDelegate {
                    width: languageList.width
                    height: 40
                    highlighted: modelData.code === selectedLanguage

                    contentItem: RowLayout {
                        spacing: Theme.spacingM

                        Label {
                            text: modelData.name || modelData
                            color: ThemeColors.text
                            Layout.fillWidth: true
                        }

                        Label {
                            text: modelData.code || ""
                            color: ThemeColors.textSecondary
                            font.pixelSize: 11
                        }

                        Label {
                            text: "✓"
                            color: ThemeColors.accent
                            visible: modelData.code === selectedLanguage
                            font.bold: true
                        }
                    }

                    onClicked: {
                        selectedLanguage = modelData.code;
                        languageChanged(selectedLanguage);
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }
    }
}
