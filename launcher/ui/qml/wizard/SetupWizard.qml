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
import QtQuick.Window 2.15
import ProjTLauncher 1.0
import ProjTLauncher.Mac 1.0
import "../Theme.js" as Theme
import "../components"

Window {
    id: setupWizard
    title: qsTr("%1 Quick Setup").arg(ProjT.launcherVM ? ProjT.launcherVM.displayName : "ProjT Launcher")
    modality: Qt.ApplicationModal
    width: 620
    height: 660
    minimumWidth: 300
    minimumHeight: 450
    flags: Qt.Dialog
    visible: true
    color: Qt.platform.os === "osx" ? "#01000000" : ThemeColors.background

    MacVisualEffectView {
        anchors.fill: parent
        visible: Qt.platform.os === "osx"
        material: MacVisualEffectView.Sheet
        blendingMode: MacVisualEffectView.BehindWindow
    }

    property int currentPageIndex: 0
    property var pageIds: []
    property var pages: []
    property var pageItems: []
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
        pageItems = []
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
                        id: pageLoader
                        sourceComponent: pageComponent(pages[index])
                        onLoaded: {
                            setupWizard.pageItems[index] = item
                        }
                        onSourceComponentChanged: {
                            setupWizard.pageItems[index] = null
                        }
                    }
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

                    AppButton {
                        text: qsTr("Refresh")
                        visible: setupWizard.currentPageWantsRefresh()
                        enabled: !!setupWizard.currentPageItem()
                        onClicked: setupWizard.refreshCurrentPage()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    AppButton {
                        text: qsTr("< Back")
                        enabled: setupWizard.currentPageIndex > 0
                        onClicked: {
                            if (setupWizard.currentPageIndex > 0) {
                                setupWizard.currentPageIndex--;
                            }
                        }
                    }

                    AppButton {
                        text: setupWizard.currentPageIndex < setupWizard.pages.length - 1 ? qsTr("Next >") : qsTr("Finish")
                        variant: setupWizard.currentPageIndex === setupWizard.pages.length - 1 ? "primary" : "secondary"
                        onClicked: {
                            if (setupWizard.currentPageIndex < setupWizard.pages.length - 1) {
                                setupWizard.currentPageIndex++;
                            } else {
                                setupWizard.finishSetup();
                            }
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: updatePages()
    onPageIdsChanged: updatePages()

    function currentPageItem() {
        return pageItems && pageItems.length > currentPageIndex ? pageItems[currentPageIndex] : null
    }

    function currentPageWantsRefresh() {
        var page = currentPageItem()
        return page && page.wantsRefreshButton === true
    }

    function refreshCurrentPage() {
        var page = currentPageItem()
        if (page && page.refresh) {
            page.refresh()
        }
    }

    function handleLoginAccountAdded() {
        if (currentPageIndex >= pages.length - 1) {
            finishSetup()
            return
        }
        currentPageIndex++
    }

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
        LoginWizardPage {
            onAccountAddedSignal: setupWizard.handleLoginAccountAdded()
        }
    }
}
