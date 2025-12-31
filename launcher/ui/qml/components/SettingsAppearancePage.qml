// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme
import "."

ScrollView {
    id: appearancePage
    clip: true
    contentWidth: availableWidth
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    property var vm: ProjT.launcherSettingsVM
    property var themeVM: ProjT.themeVM
    property var buttonStyleOptions: ["Icon only", "Text only", "Text beside icon", "Text under icon"]

    Rectangle {
        width: appearancePage.availableWidth
        implicitHeight: mainColumn.implicitHeight + 40
        color: "transparent"

        ColumnLayout {
            id: mainColumn
            width: Math.min(parent.width - 40, 700)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: 20
            spacing: ThemeColors.spacingL

            // === Theme Settings ===
            SettingsSection {
                Layout.fillWidth: true
                title: qsTr("Visual Style")
                iconSource: Theme.icon("appearance")

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: ThemeColors.spacingM

                    // Theme Row
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: ThemeColors.spacingM
                        
                        Label {
                            text: qsTr("Theme:")
                            color: ThemeColors.textSecondary
                            font: ThemeColors.fontBody
                            Layout.preferredWidth: 100
                        }
                        
                        ComboBox {
                            id: themeCombo
                            Layout.fillWidth: true
                            model: themeVM ? themeVM.availableThemes : null
                            textRole: "name"
                            valueRole: "themeId"
                            
                            // Bind currentIndex to the actual current theme form VM
                            currentIndex: indexOfValue(themeVM ? themeVM.currentTheme : "")
                            
                            onActivated: {
                                if (themeVM) {
                                   themeVM.setCurrentTheme(currentValue);
                                   // Force UI refresh handled by ThemeColors connection
                                }
                            }
                        }
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: ThemeColors.separator }

                    // Icon Theme Row
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: ThemeColors.spacingM
                        
                        Label {
                            text: qsTr("Icon Theme:")
                            color: ThemeColors.textSecondary
                            font: ThemeColors.fontBody
                            Layout.preferredWidth: 100
                        }

                        ComboBox {
                            Layout.fillWidth: true
                            model: themeVM ? themeVM.availableIconThemes : null
                            textRole: "name"
                            valueRole: "id"
                            currentIndex: indexOfValue(themeVM ? themeVM.currentIconTheme : "")
                            onActivated: if (themeVM) themeVM.setCurrentIconTheme(currentValue)
                        }

                        AppButton {
                            text: qsTr("Open Folder")
                            size: "small"
                            variant: "secondary"
                            // iconSource: Theme.icon("folder")
                            onClicked: if (themeVM) themeVM.openIconThemesFolder()
                        }
                    }

                    // Showcase of Buttons (New System)
                    Label { 
                        text: "Theme System Preview (New Components)"
                        color: ThemeColors.accent
                        font: ThemeColors.fontCaption
                        Layout.topMargin: 20
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        AppButton { text: "Primary Action"; variant: "primary" }
                        AppButton { text: "Secondary"; variant: "secondary" }
                        AppButton { text: "Ghost"; variant: "ghost" }
                        AppButton { text: "Danger"; variant: "danger" }
                    }
                    
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        AppButton { text: "Small"; size: "small"; variant: "primary" }
                        AppButton { text: "Disabled"; variant: "primary"; enabled: false }
                        AppButton { text: "Icon"; iconSource: Theme.icon("settings"); variant: "secondary" }
                    }
                    
                    AppTextField {
                        Layout.fillWidth: true
                        placeholderText: "New AppTextField component..."
                    }
                }
            }
        }
    }
}
