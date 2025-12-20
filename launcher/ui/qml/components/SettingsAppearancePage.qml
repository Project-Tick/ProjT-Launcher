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

ScrollView {
    id: appearancePage
    clip: true
    Layout.fillWidth: true
    Layout.fillHeight: true

    property var vm: ProjT.launcherSettingsVM
    property var themeVM: ProjT.themeVM
    property var buttonStyleOptions: ["Icon only", "Text only", "Text beside icon", "Text under icon"]

    contentWidth: Math.max(appearancePage.availableWidth, contentLayout.implicitWidth + Theme.spacingM * 2)
    contentHeight: contentLayout.implicitHeight + Theme.spacingM * 2

    Component.onCompleted: {
        if (themeVM && themeVM.currentIconTheme) {
            Theme.iconTheme = themeVM.currentIconTheme;
        }
    }

    Connections {
        target: themeVM
        function onCurrentIconThemeChanged() {
            if (themeVM && themeVM.currentIconTheme) {
                Theme.iconTheme = themeVM.currentIconTheme;
            }
        }
    }

    ColumnLayout {
        id: contentLayout
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.leftMargin: Theme.spacingM
        anchors.rightMargin: Theme.spacingM
        anchors.topMargin: Theme.spacingM
        spacing: Theme.spacingM

        // Theming (old UI style)
        GroupBox {
            Layout.fillWidth: true
            title: ""

            GridLayout {
                columns: 4
                columnSpacing: Theme.spacingM
                rowSpacing: Theme.spacingS
                anchors.fill: parent
                anchors.margins: Theme.spacingS

                Label {
                    text: qsTr("Theme:")
                }

                ComboBox {
                    id: themeCombo
                    Layout.fillWidth: true
                    model: themeVM ? themeVM.availableThemes : null
                    textRole: "name"
                    valueRole: "themeId"
                    currentIndex: {
                        if (!themeVM || !themeVM.availableThemes || !themeVM.currentTheme)
                            return 0;
                        for (var i = 0; i < themeVM.availableThemes.rowCount(); i++) {
                            var idx = themeVM.availableThemes.index(i, 0);
                            if (themeVM.availableThemes.data(idx, Qt.UserRole + 1) === themeVM.currentTheme) {
                                return i;
                            }
                        }
                        return 0;
                    }
                    onActivated: {
                        if (themeVM && themeVM.availableThemes) {
                            var idx = themeVM.availableThemes.index(currentIndex, 0);
                            var themeId = themeVM.availableThemes.data(idx, Qt.UserRole + 1);
                            themeVM.setCurrentTheme(themeId);
                            themeVM.applyTheme();
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Open Folder")
                    size: "small"
                    onClicked: if (themeVM)
                        themeVM.openWidgetThemesFolder()
                }

                Item {
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("&Icons:")
                }

                ComboBox {
                    id: iconThemeCombo
                    Layout.fillWidth: true
                    model: themeVM ? themeVM.availableIconThemes : null
                    textRole: "name"
                    valueRole: "themeId"
                    currentIndex: {
                        if (!themeVM || !themeVM.availableIconThemes || !themeVM.currentIconTheme)
                            return 0;
                        for (var i = 0; i < themeVM.availableIconThemes.rowCount(); i++) {
                            var idx = themeVM.availableIconThemes.index(i, 0);
                            if (themeVM.availableIconThemes.data(idx, Qt.UserRole + 1) === themeVM.currentIconTheme) {
                                return i;
                            }
                        }
                        return 0;
                    }
                    onActivated: {
                        if (themeVM && themeVM.availableIconThemes) {
                            var idx = themeVM.availableIconThemes.index(currentIndex, 0);
                            var iconThemeId = themeVM.availableIconThemes.data(idx, Qt.UserRole + 1);
                            themeVM.setCurrentIconTheme(iconThemeId);
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Open Folder")
                    size: "small"
                    onClicked: if (themeVM)
                        themeVM.openIconThemesFolder()
                }

                Item {
                    Layout.fillWidth: true
                }

                Label {
                    text: qsTr("&Cat Pack:")
                    visible: themeVM && themeVM.availableCatPacks && themeVM.availableCatPacks.rowCount() > 0
                }

                ComboBox {
                    id: catPackCombo
                    Layout.fillWidth: true
                    visible: themeVM && themeVM.availableCatPacks && themeVM.availableCatPacks.rowCount() > 0
                    model: themeVM ? themeVM.availableCatPacks : null
                    textRole: "name"
                    valueRole: "catId"
                    currentIndex: {
                        if (!themeVM || !themeVM.availableCatPacks || !themeVM.currentCatPack)
                            return 0;
                        for (var i = 0; i < themeVM.availableCatPacks.rowCount(); i++) {
                            var idx = themeVM.availableCatPacks.index(i, 0);
                            if (themeVM.availableCatPacks.data(idx, Qt.UserRole + 1) === themeVM.currentCatPack) {
                                return i;
                            }
                        }
                        return 0;
                    }
                    onActivated: {
                        if (themeVM && themeVM.availableCatPacks) {
                            var idx = themeVM.availableCatPacks.index(currentIndex, 0);
                            var catId = themeVM.availableCatPacks.data(idx, Qt.UserRole + 1);
                            themeVM.setCurrentCatPack(catId);
                        }
                    }
                }

                ThemedButton {
                    text: qsTr("Open Folder")
                    size: "small"
                    visible: themeVM && themeVM.availableCatPacks && themeVM.availableCatPacks.rowCount() > 0
                    onClicked: if (themeVM)
                        themeVM.openCatPacksFolder()
                }

                Item {
                    Layout.fillWidth: true
                }

                Item {
                    Layout.fillWidth: true
                }

                ThemedButton {
                    text: qsTr("Reload All")
                    size: "small"
                    onClicked: if (themeVM)
                        themeVM.refreshThemes()
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        // Toolbar
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Toolbar")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Show toolbar text labels")
                    checked: vm ? vm.showToolbarText : true
                    onCheckedChanged: if (vm)
                        vm.showToolbarText = checked
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM

                    Label {
                        text: qsTr("Button style:")
                        color: ThemeColors.text
                    }

                    ComboBox {
                        id: buttonStyleCombo
                        Layout.fillWidth: true
                        model: buttonStyleOptions
                        currentIndex: vm && vm.buttonStyle !== undefined ? vm.buttonStyle : 3
                        onActivated: if (vm && vm.buttonStyle !== undefined)
                            vm.buttonStyle = currentIndex
                        // Note: buttonStyle property needs to be added to LauncherSettingsViewModel
                    }
                }
            }
        }

        // Instance list
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Instance List")

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                CheckBox {
                    text: qsTr("Show instance icons")
                    checked: vm ? vm.instanceListIcons : true
                    onCheckedChanged: if (vm)
                        vm.instanceListIcons = checked
                }

                CheckBox {
                    text: qsTr("Show instance status light")
                    checked: vm ? vm.showInstanceStatusLight : true
                    onCheckedChanged: if (vm)
                        vm.showInstanceStatusLight = checked
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM

                    Label {
                        text: qsTr("Icon size:")
                        color: ThemeColors.text
                    }

                    Slider {
                        id: iconSizeSlider
                        Layout.fillWidth: true
                        from: 32
                        to: 128
                        value: 48
                        stepSize: 8
                    }

                    Label {
                        text: iconSizeSlider.value + "px"
                        color: ThemeColors.textSecondary
                    }
                }
            }
        }

        // Cat
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Cat")

            CheckBox {
                text: qsTr("Catify the launcher")
                checked: vm ? vm.enableCat : false
                onCheckedChanged: if (vm)
                    vm.enableCat = checked
            }
        }

        Item {
            height: Theme.spacingL
        }
    }
}
