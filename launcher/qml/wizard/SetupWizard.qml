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

    property var vm: ProjT.settingsVM
    property int currentPageIndex: 0

    // Wizard pages
    property var pages: [
        {
            id: "language",
            title: qsTr("Language"),
            component: languagePage
        },
        {
            id: "theme",
            title: qsTr("Theme"),
            component: themePage
        },
        {
            id: "java",
            title: qsTr("Java"),
            component: javaPage
        },
        {
            id: "autoJava",
            title: qsTr("Auto Java"),
            component: autoJavaPage
        },
        {
            id: "login",
            title: qsTr("Account"),
            component: loginPage
        }
    ]

    // Collected settings
    property string selectedLanguage: "en_US"
    property string selectedTheme: "system"
    property string javaPath: ""
    property bool autoDetectJava: true
    property bool autoDownloadJava: true

    header: Rectangle {
        height: 60
        color: ThemeColors.surface

        RowLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingM
            spacing: Theme.spacingS

            Repeater {
                model: pages

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
            text: pages[currentPageIndex].title
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
                    currentPageIndex--;
                }
            }
        }

        Button {
            text: currentPageIndex < pages.length - 1 ? qsTr("Next") : qsTr("Finish")
            highlighted: true
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            onClicked: {
                if (currentPageIndex < pages.length - 1) {
                    currentPageIndex++;
                } else {
                    finishSetup();
                }
            }
        }

        Button {
            text: qsTr("Skip")
            DialogButtonBox.buttonRole: DialogButtonBox.DestructiveRole
            onClicked: {
                setupWizard.close();
            }
        }
    }

    // Page container
    StackLayout {
        anchors.fill: parent
        currentIndex: currentPageIndex

        // Language page
        LanguageWizardPage {
            id: languagePage
            selectedLanguage: setupWizard.selectedLanguage
            onLanguageChanged: function (code) {
                setupWizard.selectedLanguage = code;
            }
        }

        // Theme page
        ThemeWizardPage {
            id: themePage
            selectedTheme: setupWizard.selectedTheme
            onThemeChanged: function (themeId) {
                setupWizard.selectedTheme = themeId;
            }
        }

        // Java page
        JavaWizardPage {
            id: javaPage
            javaPath: setupWizard.javaPath
            autoDetect: setupWizard.autoDetectJava
            onJavaPathChanged: function (path) {
                setupWizard.javaPath = path;
            }
            onAutoDetectChanged: function (enabled) {
                setupWizard.autoDetectJava = enabled;
            }
        }

        // Auto Java page
        AutoJavaWizardPage {
            id: autoJavaPage
            autoDownloadEnabled: setupWizard.autoDownloadJava
            onSettingsChanged: {
                setupWizard.autoDownloadJava = autoJavaPage.autoDownloadEnabled;
            }
        }

        // Login page
        LoginWizardPage {
            id: loginPage
        }
    }

    function finishSetup() {
        if (vm) {
            vm.applyWizardSettings({
                language: selectedLanguage,
                theme: selectedTheme,
                javaPath: javaPath,
                autoDetectJava: autoDetectJava,
                autoDownloadJava: autoDownloadJava
            });
        }
        setupWizard.accept();
    }
}
