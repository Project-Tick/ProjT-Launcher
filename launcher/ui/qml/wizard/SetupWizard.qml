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

Window {
    id: setupWizard
    title: qsTr("%1 Quick Setup").arg(ProjT.launcherVM ? ProjT.launcherVM.displayName : "ProjT Launcher")
    modality: Qt.ApplicationModal
    width: 620
    height: 660
    implicitWidth: 620
    implicitHeight: 660
    minimumWidth: 300
    minimumHeight: 400
    flags: Qt.Dialog
    visible: true

    property int currentPageIndex: 0
    property var pageIds: []
    property var pages: []
    property string currentPageId: pages.length ? pages[currentPageIndex] : ""

    signal accepted
    signal rejected
    signal closed

    // Collected settings
    property string selectedLanguage: "en_US"
    property string selectedTheme: "system"
    property string javaPath: ""
    property bool autoDetectJava: true
    property bool autoDownloadJava: true
    property bool pasteUseDefault: true

    function updatePages() {
        var all = ["language", "theme", "java", "autoJava", "paste", "login"]
        pages = (pageIds && pageIds.length > 0) ? pageIds : all
        if (currentPageIndex >= pages.length)
            currentPageIndex = 0
    }

    function pageComponent(pageId) {
        switch (pageId) {
        case "language":
            return languagePageComponent
        case "theme":
            return themePageComponent
        case "java":
            return javaPageComponent
        case "autoJava":
            return autoJavaPageComponent
        case "paste":
            return pastePageComponent
        case "login":
            return loginPageComponent
        default:
            return null
        }
    }

    Rectangle {
        id: footerBar
        Layout.fillWidth: true
        Layout.preferredHeight: 52
        height: 52
        color: ThemeColors.surface

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 1
            color: ThemeColors.border
        }

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingS

            Button {
                text: qsTr("Refresh")
                visible: currentPageId === "language"
                enabled: translationsModel && translationsModel.downloadIndex
                onClicked: {
                    if (translationsModel && translationsModel.downloadIndex) {
                        translationsModel.downloadIndex();
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Cancel")
                onClicked: closeWizard(false)
            }

            Button {
                text: qsTr("< Back")
                enabled: currentPageIndex > 0
                onClicked: {
                    if (currentPageIndex > 0) {
                        currentPageIndex--;
                    }
                }
            }

            Button {
                text: currentPageIndex < pages.length - 1 ? qsTr("Next >") : qsTr("Finish")
                highlighted: true
                onClicked: {
                    if (currentPageIndex < pages.length - 1) {
                        currentPageIndex++;
                    } else {
                        finishSetup();
                    }
                }
            }
        }
    }

    // Page container
    Item {
        anchors.fill: parent
        anchors.margins: 0

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: currentPageIndex

                Repeater {
                    model: pages.length
                    Loader {
                        sourceComponent: pageComponent(pages[index])
                    }
                }
            }

            footerBar
        }
    }

    Component.onCompleted: updatePages()
    onPageIdsChanged: updatePages()

    function finishSetup() {
        if (App && App.applyWizardSettings) {
            App.applyWizardSettings({
                language: selectedLanguage,
                theme: selectedTheme,
                javaPath: javaPath,
                autoDetectJava: autoDetectJava,
                autoDownloadJava: autoDownloadJava,
                pasteUseDefault: pasteUseDefault
            })
        }
        closeWizard(true)
    }

    function skipSetup() {
        closeWizard(false)
    }

    function closeWizard(isAccepted) {
        if (isAccepted) {
            accepted()
        } else {
            rejected()
        }
        close()
    }

    onAccepted: {
        if (App && App.finishSetupWizard) {
            App.finishSetupWizard(0)
        }
    }
    onRejected: {
        if (App && App.finishSetupWizard) {
            App.finishSetupWizard(0)
        }
    }

    onClosing: {
        closed()
    }

    Component {
        id: languagePageComponent
        LanguageWizardPage {
            selectedLanguage: setupWizard.selectedLanguage
            onLanguageChanged: function (code) {
                setupWizard.selectedLanguage = code
            }
        }
    }

    Component {
        id: themePageComponent
        ThemeWizardPage {
            selectedTheme: setupWizard.selectedTheme
            onThemeChanged: function (themeId) {
                setupWizard.selectedTheme = themeId
            }
        }
    }

    Component {
        id: javaPageComponent
        JavaWizardPage {
            javaPath: setupWizard.javaPath
            autoDetect: setupWizard.autoDetectJava
            onJavaPathEdited: function (path) {
                setupWizard.javaPath = path
            }
            onAutoDetectToggled: function (enabled) {
                setupWizard.autoDetectJava = enabled
            }
        }
    }

    Component {
        id: autoJavaPageComponent
        AutoJavaWizardPage {
            id: autoJavaPage
            autoDownloadEnabled: setupWizard.autoDownloadJava
            onSettingsChanged: {
                setupWizard.autoDownloadJava = autoJavaPage.autoDownloadEnabled
            }
        }
    }

    Component {
        id: pastePageComponent
        PasteWizardPage {
            useDefaultService: setupWizard.pasteUseDefault
            onSettingsChanged: {
                setupWizard.pasteUseDefault = useDefaultService
            }
        }
    }

    Component {
        id: loginPageComponent
        LoginWizardPage {}
    }
}
