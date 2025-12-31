// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import ProjTLauncher 1.0
import "../Theme.js" as Theme
import "../components"

WindowDialog {
    id: msaLoginDialog
    title: qsTr("Add Microsoft Account")
    modal: true
    closePolicy: Popup.NoAutoClose
    width: 440
    height: 480
    standardButtons: Dialog.Cancel
    
    // WindowDialog uses ThemeColors.bg1/surface automatically

    property var vm: typeof ProjT !== "undefined" && ProjT ? ProjT.accountsVM : null
    property string loginUrl: ""
    property string userCode: ""
    property string qrCodeData: ""
    property string statusText: ""
    property int topPanelState: 0  // 0: loading, 1: button ready
    property int bottomPanelState: 0  // 0: loading, 1: code ready

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Top StackedWidget - Login Button or Loading
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 80

            // Loading state
            ColumnLayout {
                anchors.centerIn: parent
                visible: topPanelState === 0
                spacing: ThemeColors.spacingS

                Label {
                    text: qsTr("Please wait...")
                    font.pointSize: 16
                    font.bold: true
                    color: ThemeColors.text
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: statusText || qsTr("Initializing...")
                    color: ThemeColors.textSecondary
                    Layout.alignment: Qt.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }

            // Button state
            AppButton {
                anchors.centerIn: parent
                visible: topPanelState === 1
                text: qsTr("Sign in with Microsoft")
                variant: "primary"
                width: 250
                onClicked: {
                    if (vm) {
                        topPanelState = 0;
                        bottomPanelState = 0;
                        vm.addMicrosoftAccount();
                    }
                }
            }
        }

        // "Or" separator with lines
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: ThemeColors.spacingM
            Layout.bottomMargin: ThemeColors.spacingM
            spacing: ThemeColors.spacingM
            visible: topPanelState === 1 // Only show if button visible

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: ThemeColors.border
            }

            Label {
                text: qsTr("Or")
                font.pointSize: 16
                color: ThemeColors.textSecondary
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: ThemeColors.border
            }
        }

        // Bottom StackedWidget - QR Code/Device Code or Loading
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Loading state
            ColumnLayout {
                anchors.centerIn: parent
                visible: bottomPanelState === 0
                spacing: ThemeColors.spacingS

                Label {
                    text: qsTr("Please wait...")
                    font.pointSize: 16
                    font.bold: true
                    color: ThemeColors.text
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    text: statusText || qsTr("Waiting for device code...")
                    color: ThemeColors.textSecondary
                    Layout.alignment: Qt.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                BusyIndicator {
                    Layout.alignment: Qt.AlignHCenter
                    running: bottomPanelState === 0
                    // Customizing BusyIndicator color is complex due to default style, 
                    // but typically follows accent in Fusion/Material.
                }
            }

            // Code ready state
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: ThemeColors.spacingM
                visible: bottomPanelState === 1
                spacing: ThemeColors.spacingM

                // QR Code
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.preferredWidth: 156
                    Layout.preferredHeight: 156
                    color: "white"
                    radius: 4

                    Image {
                        id: qrImage
                        anchors.fill: parent
                        anchors.margins: 4
                        source: qrCodeData || ""
                        fillMode: Image.PreserveAspectFit
                        visible: qrCodeData !== ""
                    }

                    Label {
                        anchors.centerIn: parent
                        text: qsTr("QR Code")
                        color: "#000000" // Always black on white paper
                        visible: qrCodeData === ""
                    }
                }

                // Device Code with Copy button
                RowLayout {
                    Layout.alignment: Qt.AlignHCenter
                    spacing: ThemeColors.spacingS

                    Label {
                        id: codeLabel
                        text: userCode || "--------"
                        font.pointSize: 30
                        font.bold: true
                        color: ThemeColors.accent

                        MouseArea {
                            anchors.fill: parent
                            cursorShape: Qt.IBeamCursor
                        }
                    }

                    AppButton {
                        id: copyCodeBtn
                        variant: "ghost"
                        iconSource: Theme.icon("edit-copy")
                        size: "small"
                        ToolTip.visible: hovered
                        ToolTip.text: qsTr("Copy code to clipboard")
                        onClicked: {
                            if (vm && userCode) {
                                vm.copyCodeToClipboard(userCode);
                            }
                        }
                    }
                }

                // Info message
                Label {
                    Layout.fillWidth: true
                    text: qsTr("Open <a href=\"%1\" style=\"color: %2\">%1</a> and enter the code above to sign in.").arg(loginUrl || "https://microsoft.com/link").arg(ThemeColors.accent)
                    color: ThemeColors.textSecondary
                    wrapMode: Text.WordWrap
                    horizontalAlignment: Text.AlignHCenter
                    onLinkActivated: link => Qt.openUrlExternally(link)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    Connections {
        target: vm

        function onLoginUrlReady(url, code) {
            loginUrl = url;
            userCode = code;
            bottomPanelState = 1;
            topPanelState = 1;
        }

        function onQrCodeReady(qrData) {
            qrCodeData = qrData;
        }

        function onLoginStatusChanged(status) {
            statusText = status;
        }

        function onLoginStarted() {
            statusText = qsTr("Authenticating...");
        }

        function onLoginFinished(success, message) {
            if (success) {
                statusText = qsTr("Account added successfully!");
                closeTimer.start();
            } else {
                topPanelState = 1;
                bottomPanelState = 1;
                errorDialog.message = message;
                errorDialog.open();
            }
        }
    }

    Timer {
        id: closeTimer
        interval: 1500
        onTriggered: msaLoginDialog.accept()
    }

    Dialog {
        id: errorDialog
        title: qsTr("Login Error")
        modal: true
        standardButtons: Dialog.Ok
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        // Use standard dialog styling or custom content
        topPadding: 20
        bottomPadding: 20
        
        property string message: ""

        Label {
            text: errorDialog.message
            color: ThemeColors.danger
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }

    onOpened: {
        topPanelState = 0;
        bottomPanelState = 0;
        statusText = "";
        loginUrl = "";
        userCode = "";
        qrCodeData = "";
        if (vm)
            vm.addMicrosoftAccount();
    }

    onRejected: {
        if (vm)
            vm.cancelLogin();
    }
}
