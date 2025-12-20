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

    property var vm: ProjT.launcherSettingsVM
    property var themeVM: ProjT.themeVM
    property bool showThemeGallery: true

    property var buttonStyleOptions: ["Icon only", "Text only", "Text beside icon", "Text under icon"]

    contentWidth: appearancePage.availableWidth
    contentHeight: contentLayout.implicitHeight + Theme.spacingM * 2

    ColumnLayout {
        id: contentLayout
        width: appearancePage.availableWidth - Theme.spacingM * 2
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: Theme.spacingM
        anchors.topMargin: Theme.spacingM
        spacing: Theme.spacingM

        // Theme Selection Mode Toggle
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingM

            Label {
                text: qsTr("Theme Selection:")
                color: ThemeColors.text
                font.bold: true
            }

            ThemedButton {
                text: showThemeGallery ? qsTr("Show Dropdown") : qsTr("Show Gallery")
                size: "small"
                onClicked: showThemeGallery = !showThemeGallery
            }

            Item {
                Layout.fillWidth: true
            }
        }

        // Platform Theme Information
        PlatformThemeInfo {
            Layout.fillWidth: true
            themeVM: appearancePage.themeVM
        }

        // Theme Gallery (Visual Selection)
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Available Themes")
            visible: showThemeGallery

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                ThemeGallery {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 400
                    themeVM: appearancePage.themeVM
                }
            }
        }

        // Theme Dropdown (Classic Selection)
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Theme")
            visible: !showThemeGallery

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM

                    Label {
                        text: qsTr("Application theme:")
                        color: ThemeColors.text
                    }

                    ComboBox {
                        id: themeCombo
                        Layout.fillWidth: true
                        model: themeVM ? themeVM.availableThemes : null
                        textRole: "name"
                        valueRole: "id"
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
                        displayText: themeVM && themeVM.currentTheme ? themeVM.currentTheme : qsTr("Default")
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: Theme.spacingM

                    Label {
                        text: qsTr("Icon theme:")
                        color: ThemeColors.text
                    }

                    ComboBox {
                        id: iconThemeCombo
                        Layout.fillWidth: true
                        model: themeVM ? themeVM.availableIconThemes : null
                        textRole: "name"
                        valueRole: "id"
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
                        displayText: themeVM && themeVM.currentIconTheme ? themeVM.currentIconTheme : qsTr("Default")
                    }
                }

                ThemedButton {
                    text: qsTr("Refresh themes")
                    size: "small"
                    onClicked: if (themeVM)
                        themeVM.refreshThemes()
                }

                // Current Theme Info
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: themeInfoLayout.implicitHeight + Theme.spacingM * 2
                    color: ThemeColors.surface
                    border.color: ThemeColors.border
                    radius: Theme.radiusS

                    ColumnLayout {
                        id: themeInfoLayout
                        anchors.fill: parent
                        anchors.margins: Theme.spacingM
                        spacing: Theme.spacingXS

                        Label {
                            text: qsTr("Current Theme: %1").arg(themeVM ? themeVM.currentThemeName : "")
                            color: ThemeColors.text
                            font.bold: true
                        }

                        Label {
                            text: themeVM ? themeVM.currentThemeTooltip : ""
                            color: ThemeColors.textSecondary
                            font.pointSize: Theme.fontCaption
                            wrapMode: Text.WordWrap
                            Layout.fillWidth: true
                            visible: text.length > 0
                        }

                        RowLayout {
                            spacing: Theme.spacingM

                            Label {
                                text: qsTr("Has Custom Stylesheet:")
                                color: ThemeColors.textSecondary
                                font.pointSize: Theme.fontCaption
                            }

                            Label {
                                text: themeVM && themeVM.hasStyleSheet ? qsTr("Yes") : qsTr("No")
                                color: themeVM && themeVM.hasStyleSheet ? ThemeColors.success : ThemeColors.textSecondary
                                font.pointSize: Theme.fontCaption
                            }
                        }

                        RowLayout {
                            spacing: Theme.spacingM
                            visible: themeVM && themeVM.qtTheme.length > 0

                            Label {
                                text: qsTr("Qt Style:")
                                color: ThemeColors.textSecondary
                                font.pointSize: Theme.fontCaption
                            }

                            Label {
                                text: themeVM ? themeVM.qtTheme : ""
                                color: ThemeColors.text
                                font.pointSize: Theme.fontCaption
                            }
                        }

                        // Platform info
                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: ThemeColors.border
                            Layout.topMargin: Theme.spacingXS
                            Layout.bottomMargin: Theme.spacingXS
                        }

                        Label {
                            text: {
                                if (Qt.platform.os === "windows")
                                    return qsTr("Platform: Windows");
                                if (Qt.platform.os === "osx")
                                    return qsTr("Platform: macOS");
                                if (Qt.platform.os === "linux")
                                    return qsTr("Platform: Linux");
                                return qsTr("Platform: %1").arg(Qt.platform.os);
                            }
                            color: ThemeColors.textSecondary
                            font.pointSize: Theme.fontCaption
                            font.italic: true
                        }
                    }
                }

                // Theme Preview
                ThemePreview {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                }
            }
        }

        // Icon Theme Gallery
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("Icon Themes")
            visible: showThemeGallery

            ColumnLayout {
                anchors.fill: parent
                spacing: Theme.spacingS

                IconThemeGallery {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 300
                    themeVM: appearancePage.themeVM
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
