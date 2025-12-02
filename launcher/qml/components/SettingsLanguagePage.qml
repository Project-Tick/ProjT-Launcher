// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  Language settings page
 */
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../Theme.js" as Theme

Rectangle {
    id: languagePage
    color: Theme.background
    
    property var vm: launcherSettingsVM
    
    // Static language list (could come from vm.availableLanguages in future)
    property var languageModel: [
        { code: "en_US", name: "English (United States)", percent: 100 },
        { code: "tr_TR", name: "Türkçe (Turkish)", percent: 98 },
        { code: "de_DE", name: "Deutsch (German)", percent: 95 },
        { code: "fr_FR", name: "Français (French)", percent: 92 },
        { code: "es_ES", name: "Español (Spanish)", percent: 90 },
        { code: "pt_BR", name: "Português (Brazilian)", percent: 88 },
        { code: "ru_RU", name: "Русский (Russian)", percent: 85 },
        { code: "zh_CN", name: "简体中文 (Simplified Chinese)", percent: 82 },
        { code: "zh_TW", name: "繁體中文 (Traditional Chinese)", percent: 78 },
        { code: "ja_JP", name: "日本語 (Japanese)", percent: 75 },
        { code: "ko_KR", name: "한국어 (Korean)", percent: 72 },
        { code: "it_IT", name: "Italiano (Italian)", percent: 70 },
        { code: "pl_PL", name: "Polski (Polish)", percent: 68 },
        { code: "nl_NL", name: "Nederlands (Dutch)", percent: 65 },
        { code: "uk_UA", name: "Українська (Ukrainian)", percent: 60 }
    ]
    
    function findCurrentLanguageIndex() {
        var currentLang = vm ? vm.currentLanguage : "en_US"
        for (var i = 0; i < languageModel.length; i++) {
            if (languageModel[i].code === currentLang) {
                return i
            }
        }
        return 0
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingM
        spacing: Theme.spacingM
        
        Label {
            text: qsTr("Select Language")
            font.pointSize: 12
            font.bold: true
            color: Theme.textPrimary
        }
        
        TextField {
            id: searchField
            Layout.fillWidth: true
            placeholderText: qsTr("Search languages...")
            selectByMouse: true
        }
        
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surfaceBackground
            border.color: Theme.border
            border.width: 1
            radius: Theme.radiusS
            
            ListView {
                id: languageList
                anchors.fill: parent
                anchors.margins: 1
                clip: true
                
                model: languageModel.filter(function(item) {
                    return searchField.text.length === 0 || 
                           item.name.toLowerCase().indexOf(searchField.text.toLowerCase()) >= 0
                })
                
                currentIndex: findCurrentLanguageIndex()
                
                delegate: ItemDelegate {
                    width: languageList.width
                    height: 40
                    highlighted: languageList.currentIndex === index
                    
                    contentItem: RowLayout {
                        spacing: Theme.spacingM
                        
                        Label {
                            text: modelData.name
                            color: Theme.textPrimary
                            Layout.fillWidth: true
                        }
                        
                        Label {
                            text: modelData.percent + "%"
                            color: {
                                if (modelData.percent >= 90) return Theme.success
                                if (modelData.percent >= 70) return Theme.warning
                                return Theme.textSecondary
                            }
                            font.pointSize: 9
                        }
                    }
                    
                    onClicked: {
                        languageList.currentIndex = index
                        if (vm) {
                            vm.currentLanguage = modelData.code
                        }
                    }
                }
                
                ScrollBar.vertical: ScrollBar {}
            }
        }
        
        Label {
            text: qsTr("Translation completeness is shown on the right. Help us translate at Weblate!")
            color: Theme.textSecondary
            font.pointSize: 9
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        
        Button {
            text: qsTr("Help translate on Weblate")
            onClicked: {
                Qt.openUrlExternally("https://hosted.weblate.org/engage/prismlauncher/")
            }
        }
    }
}
