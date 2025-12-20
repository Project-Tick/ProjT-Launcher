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

pragma Singleton
import QtQuick 2.15

QtObject {
    id: themeColors

    // Internal trigger that forces all bindings to update when theme changes
    property int _updateTrigger: 0

    // Safe getter for themeVM - uses direct context property 'themeVM' which is more reliable
    function getThemeVM() {
        // Try direct context property first (most reliable)
        if (typeof themeVM !== "undefined" && themeVM !== null) {
            return themeVM;
        }
        // Try ProjT.themeVM as fallback
        try {
            if (typeof ProjT !== "undefined" && ProjT && ProjT.themeVM) {
                return ProjT.themeVM;
            }
        } catch (e) {
            console.log("[ThemeColors] Error accessing ProjT.themeVM:", e);
        }
        return null;
    }

    // Default fallback colors (dark theme)
    readonly property color _defaultWindow: "#1e1e1e"
    readonly property color _defaultWindowText: "#e0e0e0"
    readonly property color _defaultBase: "#222222"
    readonly property color _defaultAlternateBase: "#2a2a2a"
    readonly property color _defaultText: "#ffffff"
    readonly property color _defaultButton: "#303030"
    readonly property color _defaultButtonText: "#ffffff"
    readonly property color _defaultHighlight: "#96db59"
    readonly property color _defaultHighlightedText: "#000000"
    readonly property color _defaultLink: "#2fa3c6"

    // Primary colors from QPalette - use _updateTrigger to force binding updates
    readonly property color window: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.windowColor : _defaultWindow;
    }
    readonly property color windowText: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.windowTextColor : _defaultWindowText;
    }
    readonly property color base: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.baseColor : _defaultBase;
    }
    readonly property color alternateBase: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.alternateBaseColor : _defaultAlternateBase;
    }
    readonly property color text: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.textColor : _defaultText;
    }
    readonly property color button: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.buttonColor : _defaultButton;
    }
    readonly property color buttonText: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.buttonTextColor : _defaultButtonText;
    }
    readonly property color highlight: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.highlightColor : _defaultHighlight;
    }
    readonly property color highlightedText: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.highlightedTextColor : _defaultHighlightedText;
    }
    readonly property color link: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.linkColor : _defaultLink;
    }

    // Semantic aliases for better readability - ALL use _updateTrigger
    readonly property color background: {
        var _ = _updateTrigger;
        return window;
    }
    readonly property color backgroundAlt: {
        var _ = _updateTrigger;
        return alternateBase;
    }
    readonly property color surface: {
        var _ = _updateTrigger;
        return base;
    }
    readonly property color toolBar: {
        var _ = _updateTrigger;
        return Qt.darker(window, 1.05);
    }
    readonly property color textPrimary: {
        var _ = _updateTrigger;
        return text;
    }
    readonly property color textSecondary: {
        var _ = _updateTrigger;
        return Qt.darker(text, 1.3);
    }
    readonly property color accent: {
        var _ = _updateTrigger;
        return highlight;
    }
    readonly property color primary: {
        var _ = _updateTrigger;
        return link;
    }

    // Additional derived colors - ALL use _updateTrigger
    readonly property color border: {
        var _ = _updateTrigger;
        return Qt.darker(window, 1.2);
    }
    readonly property color hover: {
        var _ = _updateTrigger;
        return Qt.lighter(button, 1.1);
    }
    readonly property color pressed: {
        var _ = _updateTrigger;
        return Qt.darker(button, 1.1);
    }
    readonly property color disabled: {
        var _ = _updateTrigger;
        return Qt.rgba(text.r, text.g, text.b, 0.3);
    }

    // Fade color from theme (used for overlays and fade effects)
    readonly property color fade: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.fadeColor : Qt.rgba(0, 0, 0, 0.5);
    }
    readonly property real fadeOpacity: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        return vm ? vm.fadeAmount : 0.5;
    }

    // Status colors (can be customized per theme in future)
    readonly property color success: "#4caf50"
    readonly property color warning: "#ff9800"
    readonly property color error: "#d55f5f"
    readonly property color info: {
        var _ = _updateTrigger;
        return link;
    }

    // Layout constants (not theme-dependent)
    readonly property int radius: 8
    readonly property int radiusS: 4
    readonly property int radiusM: 8
    readonly property int radiusL: 12
    readonly property int spacingXS: 4
    readonly property int spacingS: 8
    readonly property int spacingM: 12
    readonly property int spacingL: 16

    // Typography
    readonly property int fontHeader: 18
    readonly property int fontSubtitle: 14
    readonly property int fontBody: 12
    readonly property int fontCaption: 11

    // Force update function - can be called from C++ or QML
    function forceUpdate() {
        console.log("[ThemeColors] Force update triggered");
        _updateTrigger++;
    }
}
