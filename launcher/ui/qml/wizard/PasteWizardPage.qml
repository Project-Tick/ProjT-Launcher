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

Rectangle {
    id: pastePage
    color: ThemeColors.background

    property bool useDefaultService: true

    signal settingsChanged

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingL
        spacing: Theme.spacingM

        // Description
        Label {
            Layout.fillWidth: true
            text: qsTr("The default paste service has changed to mclo.gs, please choose what you want to do with your settings.")
            color: ThemeColors.text
            wrapMode: Text.WordWrap
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: ThemeColors.border
        }

        // Options
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingS

            RadioButton {
                text: qsTr("Use new default service")
                checked: useDefaultService
                onCheckedChanged: {
                    if (checked) {
                        useDefaultService = true;
                        settingsChanged();
                    }
                }
            }

            RadioButton {
                text: qsTr("Keep previous settings")
                checked: !useDefaultService
                onCheckedChanged: {
                    if (checked) {
                        useDefaultService = false;
                        settingsChanged();
                    }
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
