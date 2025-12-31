// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme
import "../components" // For AppButton, AppTextField

WindowDialog {
    id: newInstanceDialog
    title: qsTr("New Instance")
    modal: true
    width: 850
    height: 600
    standardButtons: Dialog.NoButton
    
    // Dynamic background
    // WindowDialog uses ThemeColors.bg1 automatically

    property var vm: ProjT ? ProjT.instancesVM : null
    property var versionsVM: ProjT ? ProjT.newInstanceVM : null
    property string currentPage: "vanilla"

    // Theme binding
    property var themeVM: ProjT.themeVM
    property int _themeUpdateCount: 0

    Connections {
        target: themeVM
        function onThemeColorsChanged() {
            newInstanceDialog._themeUpdateCount++;
        }
    }

    // Custom Header
    header: Rectangle {
        height: 50
        color: ThemeColors.surface // Sidebar extends to header mostly in modern apps, but let's keep it clean
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            
            Label {
                text: newInstanceDialog.title
                color: ThemeColors.textTitle
                font.bold: true
                font.pixelSize: 16
                Layout.fillWidth: true
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            width: parent.width
            height: 1
            color: ThemeColors.border
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left sidebar
        Rectangle {
            Layout.preferredWidth: 220
            Layout.fillHeight: true
            color: ThemeColors.surface // Sidebar/Panel color

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 4

                Label {
                    text: qsTr("SOURCE")
                    color: ThemeColors.textMuted
                    font: ThemeColors.fontCaption
                    Layout.fillWidth: true
                    Layout.bottomMargin: 8
                    Layout.leftMargin: 8
                }

                // Navigation items
                Repeater {
                    model: [
                        { id: "vanilla", name: qsTr("Vanilla"), icon: "minecraft" },
                        { id: "curseforge", name: "CurseForge", icon: "centralmods" },
                        { id: "modrinth", name: "Modrinth", icon: "loadermods" },
                        { id: "atlauncher", name: "ATLauncher", icon: "server" },
                        { id: "ftb", name: "FTB", icon: "server" },
                        { id: "technic", name: "Technic", icon: "server" },
                        { id: "import", name: qsTr("Import Zip"), icon: "viewfolder" }
                    ]

                    delegate: AbstractButton {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        
                        property bool isActive: currentPage === modelData.id
                        
                        background: Rectangle {
                            color: parent.isActive ? ThemeColors.surface2 : (parent.hovered ? ThemeColors.hoverOverlay : "transparent")
                            radius: 6
                            border.width: parent.isActive ? 1 : 0
                            border.color: ThemeColors.border
                        }

                        contentItem: RowLayout {
                            spacing: 12
                            anchors.left: parent.left
                            anchors.leftMargin: 12
                            
                            // SVG Icon
                            Image {
                                source: Theme.icon(modelData.icon)
                                sourceSize: Qt.size(18, 18)
                                width: 18
                                height: 18
                                Layout.alignment: Qt.AlignVCenter
                                opacity: parent.parent.isActive ? 1.0 : 0.7
                            }
                            
                            Label {
                                text: modelData.name
                                color: parent.parent.isActive ? ThemeColors.text : ThemeColors.textSecondary
                                font.weight: parent.parent.isActive ? Font.DemiBold : Font.Normal
                                font: ThemeColors.fontBody
                                Layout.fillWidth: true
                            }
                        }

                        onClicked: currentPage = modelData.id
                        hoverEnabled: true
                        
                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: parent.clicked()
                            hoverEnabled: true
                            onEntered: parent.hovered = true
                            onExited: parent.hovered = false
                        }
                    }
                }

                Item { Layout.fillHeight: true }
            }
            
            Rectangle {
                anchors.right: parent.right
                width: 1
                height: parent.height
                color: ThemeColors.border
            }
        }

        // Main content
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: ThemeColors.bg // Main background

            StackLayout {
                anchors.fill: parent
                anchors.margins: 24
                currentIndex: {
                    switch (currentPage) {
                        case "vanilla": return 0;
                        case "curseforge": return 1;
                        case "modrinth": return 2;
                        case "atlauncher": return 3;
                        case "ftb": return 4;
                        case "technic": return 5;
                        case "import": return 6;
                        default: return 0;
                    }
                }

                VanillaPage {}
                
                // Placesholders
                ModpackBrowserPage { source: "curseforge"; sourceName: "CurseForge"; sourceColor: "#f16436" }
                ModpackBrowserPage { source: "modrinth"; sourceName: "Modrinth"; sourceColor: "#1bd96a" }
                ModpackBrowserPage { source: "atlauncher"; sourceName: "ATLauncher"; sourceColor: "#1e8e3e" }
                ModpackBrowserPage { source: "ftb"; sourceName: "FTB"; sourceColor: "#f44336" }
                ModpackBrowserPage { source: "technic"; sourceName: "Technic"; sourceColor: "#2196f3" }
                
                ImportPage {}
            }
        }
    }

    // Components
    component VanillaPage: ColumnLayout {
        spacing: 20

        RowLayout {
            spacing: 16
            Image {
                source: Theme.icon("minecraft")
                sourceSize: Qt.size(32, 32)
            }
            ColumnLayout {
                spacing: 2
                Label {
                    text: qsTr("Create Vanilla Instance")
                    color: ThemeColors.textTitle
                    font: ThemeColors.fontTitle
                }
                Label {
                    text: qsTr("Customize your installation")
                    color: ThemeColors.textMuted
                    font: ThemeColors.fontCaption
                }
            }
        }

        GridLayout {
            columns: 2
            rowSpacing: 16
            columnSpacing: 16
            Layout.fillWidth: true

            Label { text: qsTr("Name"); color: ThemeColors.text; font: ThemeColors.fontBodyBold; Layout.alignment: Qt.AlignVCenter }
            AppTextField {
                id: vanillaNameField
                Layout.fillWidth: true
                placeholderText: qsTr("My New Instance")
            }

            Label { text: qsTr("Group"); color: ThemeColors.text; font: ThemeColors.fontBodyBold; Layout.alignment: Qt.AlignVCenter }
            AppComboBox {
                id: vanillaGroupCombo
                Layout.fillWidth: true
                editable: true
                model: vm ? vm.groupList : []
                // TODO: Themify ComboBox
            }
        }

        SettingsSection {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title: qsTr("Minecraft Version")
            iconSource: Theme.icon("minecraft")

            ColumnLayout {
                anchors.fill: parent
                spacing: 12
                
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 16
                    AppCheckBox { text: qsTr("Releases"); checked: versionsVM ? versionsVM.showReleases : true; onCheckedChanged: if(versionsVM) versionsVM.showReleases = checked }
                    AppCheckBox { text: qsTr("Snapshots"); checked: versionsVM ? versionsVM.showSnapshots : false; onCheckedChanged: if(versionsVM) versionsVM.showSnapshots = checked }
                    AppCheckBox { text: qsTr("Old"); checked: versionsVM ? versionsVM.showOldVersions : false; onCheckedChanged: if(versionsVM) versionsVM.showOldVersions = checked }
                    Item { Layout.fillWidth: true }
                    AppButton { text: qsTr("Refresh"); size: "small"; onClicked: if(versionsVM) versionsVM.loadMinecraftVersions() }
                }

                AppTextField {
                    id: versionSearchField
                    Layout.fillWidth: true
                    placeholderText: qsTr("Search versions...")
                }

                ListView {
                    id: versionList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: versionsVM ? versionsVM.minecraftVersionsModel : []
                    property string selectedVersion: ""

                     delegate: ItemDelegate {
                        width: versionList.width
                        height: visible ? 36 : 0
                        highlighted: versionText === versionList.selectedVersion
                        
                        property string versionText: model.version || model.versionId || ""
                        property string versionType: model.type || ""
                        
                        visible: versionSearchField.text.length === 0 || versionText.toLowerCase().includes(versionSearchField.text.toLowerCase())

                        background: Rectangle {
                            color: highlighted ? ThemeColors.surface2 : (hovered ? ThemeColors.hoverOverlay : "transparent")
                            radius: 4
                            border.width: highlighted ? 1 : 0
                            border.color: ThemeColors.accent
                        }

                        contentItem: RowLayout {
                            spacing: 10
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            
                            Rectangle {
                                width: 8; height: 8; radius: 4
                                color: versionType === "release" ? ThemeColors.success : ThemeColors.warning
                            }
                            
                            Label {
                                text: versionText
                                color: highlighted ? ThemeColors.text : ThemeColors.text
                                font.bold: highlighted
                            }
                        }
                        
                        onClicked: {
                            versionList.selectedVersion = versionText;
                            if (versionsVM) versionsVM.selectedMinecraftVersion = versionText;
                        }
                    }
                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: 12
            
            Item { Layout.fillWidth: true }
            
            AppButton {
                text: qsTr("Cancel")
                variant: "ghost"
                onClicked: newInstanceDialog.reject()
            }
            
            AppButton {
                text: qsTr("Create")
                variant: "primary"
                enabled: vanillaNameField.text.length > 0 && versionList.selectedVersion.length > 0
                onClicked: {
                    if (vm) {
                        vm.createNewInstance(vanillaNameField.text, vanillaGroupCombo.editText, versionList.selectedVersion);
                        newInstanceDialog.accept();
                    }
                }
            }
        }
    }

    component ModpackBrowserPage: ColumnLayout {
        property string source: ""
        property string sourceName: ""
        property string sourceColor: ThemeColors.accent

        spacing: 20

        RowLayout {
            spacing: 16
            Rectangle {
                width: 32; height: 32; radius: 8; color: sourceColor
                Label { anchors.centerIn: parent; text: sourceName.charAt(0); color: "white"; font.bold: true }
            }
            Label {
                text: qsTr("Browse %1").arg(sourceName)
                color: ThemeColors.textTitle
                font: ThemeColors.fontTitle
            }
        }

        RowLayout {
            Layout.fillWidth: true
            AppTextField {
                Layout.fillWidth: true
                placeholderText: qsTr("Search %1...").arg(sourceName)
            }
            AppButton { text: qsTr("Search"); variant: "primary" }
        }

        Rectangle {
            Layout.fillWidth: true; Layout.fillHeight: true
            color: ThemeColors.surface2; radius: 8
            border.color: ThemeColors.border
            border.width: 1
            
            ColumnLayout {
                anchors.centerIn: parent
                spacing: 12
                Image { 
                    source: Theme.icon("centralmods")
                    sourceSize: Qt.size(48, 48)
                    opacity: 0.5
                    Layout.alignment: Qt.AlignHCenter
                }
                Label {
                    text: qsTr("Integration Coming Soon")
                    color: ThemeColors.textSecondary
                }
            }
        }
        
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            AppButton { text: qsTr("Cancel"); onClicked: newInstanceDialog.reject() }
        }
    }

    component ImportPage: ColumnLayout {
        spacing: 20
        
        Label {
            text: qsTr("Import Instance")
            color: ThemeColors.textTitle
             font: ThemeColors.fontTitle
        }

        SettingsSection {
            Layout.fillWidth: true
            title: qsTr("Source")
            iconSource: Theme.icon("viewfolder")
            
            ColumnLayout {
                spacing: 12
                Layout.fillWidth: true
                
                Label { text: qsTr("Local File (.zip, .mrpack)"); color: ThemeColors.textMuted }
                RowLayout {
                   AppTextField { id: importPathField; Layout.fillWidth: true; placeholderText: qsTr("Select file..."); readOnly: true }
                   AppButton { text: qsTr("Browse"); onClicked: openImportFileDialog() }
                }

                Rectangle { height: 1; color: ThemeColors.border; Layout.fillWidth: true; Layout.margins: 8 }

                Label { text: qsTr("Or Direct URL"); color: ThemeColors.textMuted }
                AppTextField { id: importUrlField; Layout.fillWidth: true; placeholderText: "https://example.com/modpack.zip" }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            AppButton { text: qsTr("Cancel"); variant: "ghost"; onClicked: newInstanceDialog.reject() }
            AppButton { 
                text: qsTr("Import")
                variant: "primary"
                enabled: importPathField.text.length > 0 || importUrlField.text.length > 0
                onClicked: {
                    if (vm) {
                        if (importUrlField.text.length > 0) vm.importFromUrl(importUrlField.text, "Imported Instance", "Import");
                        else vm.importFromFile(importPathField.text, "Imported Instance", "Import");
                        newInstanceDialog.accept();
                    }
                }
            }
        }

        function openImportFileDialog() {
            if (ProjT && ProjT.launcherVM) {
                var path = ProjT.launcherVM.browseForFile(qsTr("Select Modpack"), "Modpack files (*.zip *.mrpack);;All files (*)");
                if (path && path.length > 0) importPathField.text = path;
            }
        }
    }
}
