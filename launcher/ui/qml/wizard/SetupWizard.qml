// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme

Dialog {
    id: setupWizard
    title: qsTr("Welcome to ProjT Launcher")
    modal: true
    width: 600
    height: 500

    property int currentPageIndex: 0
    property var pageIds: []
    property var pages: []

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

    function pageTitle(pageId) {
        switch (pageId) {
        case "language":
            return qsTr("Language")
        case "theme":
            return qsTr("Theme")
        case "java":
            return qsTr("Java")
        case "autoJava":
            return qsTr("Auto Java")
        case "paste":
            return qsTr("Paste Service")
        case "login":
            return qsTr("Account")
        default:
            return ""
        }
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

    header: Rectangle {
        height: 60
        color: ThemeColors.surface

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingS

            Repeater {
                model: pages.length

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 4
                    radius: 2
                    color: index <= currentPageIndex ? ThemeColors.accent : ThemeColors.border

                    Behavior on color {
                        ColorAnimation {
                            duration: 200
                        }
                    }
                }
            }
        }

        Label {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: Theme.spacingS
            anchors.horizontalCenter: parent.horizontalCenter
            text: pages.length ? pageTitle(pages[currentPageIndex]) : ""
            font.bold: true
            color: ThemeColors.text
        }
    }

    footer: DialogButtonBox {
        Button {
            text: qsTr("Back")
            enabled: currentPageIndex > 0
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            onClicked: {
                if (currentPageIndex > 0) {
                    currentPageIndex--
                }
            }
        }

        Button {
            text: currentPageIndex < pages.length - 1 ? qsTr("Next") : qsTr("Finish")
            highlighted: true
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: {
                if (currentPageIndex < pages.length - 1) {
                    currentPageIndex++
                } else {
                    finishSetup()
                }
            }
        }

        Button {
            text: qsTr("Skip")
            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
            onClicked: {
                setupWizard.close()
            }
        }
    }

    // Page container
    StackLayout {
        anchors.fill: parent
        currentIndex: currentPageIndex

        Repeater {
            model: pages.length
            Loader {
                sourceComponent: pageComponent(pages[index])
            }
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
        setupWizard.accept()
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
