// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team

pragma Singleton
import QtQuick 2.15

QtObject {
    id: theme

    // Auto-update connection
    property var _activeVM: (typeof ProjT !== "undefined" && ProjT && ProjT.themeVM) ? ProjT.themeVM : null
    
    property Connections _connections: Connections {
        target: theme._activeVM
        ignoreUnknownSignals: true
        function onThemeColorsChanged() {
            console.log("[ThemeColors] Theme changed signal received");
            theme.forceUpdate();
        }
    }

    // =========================================================================
    // STATE & LOGIC
    // =========================================================================
    
    // Internal trigger for updates
    property int _updateTrigger: 0
    
    // Helper to access C++ ViewModel
    function getThemeVM() {
        if (typeof themeVM !== "undefined" && themeVM !== null) return themeVM;
        try { if (typeof ProjT !== "undefined" && ProjT && ProjT.themeVM) return ProjT.themeVM; } catch (e) {}
        return null;
    }

    // Force update function
    function forceUpdate() { _updateTrigger++; }

    // Check if we are in dark mode based on window color luminance
    readonly property bool isDark: {
        var _ = _updateTrigger;
        var vm = getThemeVM();
        if (vm && vm.windowColor) {
            var c = vm.windowColor;
            // Calculate luminance to decide if dark or light theme logic applies
            // Standard formula: 0.299R + 0.587G + 0.114B
            var luminance = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            return luminance < 0.5; // If dark (< 0.5), use Dark Mode tokens
        }
        return true; // Default fallback
    }

    // =========================================================================
    // DESIGN TOKENS (PALETTE)
    // =========================================================================

    // --- Dark Palette (Professional Dark) ---
    readonly property var _darkPalette: QtObject {
        readonly property color bg: Qt.platform.os === "osx" ? "transparent" : "#121212"           // Deep matte background (transparent on macOS)
        readonly property color surface: Qt.platform.os === "osx" ? "#CC1E1E1E" : "#1E1E1E"      // Cards, Panels
        readonly property color surface2: Qt.platform.os === "osx" ? "#CC2D2D2D" : "#2D2D2D"     // Hover states, Secondary panels
        readonly property color surface3: "#383838"     // Borders, Dividers, Inputs
        readonly property color glassSurface: Qt.platform.os === "osx" ? "transparent" : "#1E1E1E"
        readonly property color glassSurface2: Qt.platform.os === "osx" ? Qt.rgba(1, 1, 1, 0.05) : "#2D2D2D"
        
        readonly property color text: "#FFFFFF"         // Primary Text (High Emphasis)
        readonly property color textMuted: "#A1A1AA"    // Secondary Text (Medium Emphasis)
        readonly property color textDisabled: "#52525B" // Disabled Text
        
        readonly property color border: "#333333"       // Subtle borders
        readonly property color borderFocus: "#60A5FA"  // Focus rings (Blue-400)
        
        readonly property color accent: "#3B82F6"       // Brand Primary (Blue-500)
        readonly property color accentHover: "#2563EB"  // Blue-600
        readonly property color accentText: "#FFFFFF"
        
        readonly property color danger: "#EF4444"       // Red-500
        readonly property color dangerHover: "#DC2626"
        readonly property color warning: "#F59E0B"      // Amber-500
        readonly property color success: "#10B981"      // Emerald-500
        
        readonly property color shadow: "#000000"
    }

    // --- Light Palette (Clean Light) ---
    readonly property var _lightPalette: QtObject {
        readonly property color bg: Qt.platform.os === "osx" ? "transparent" : "#F9FAFB"           // Gray-50 (transparent on macOS)
        readonly property color surface: Qt.platform.os === "osx" ? "#CCFFFFFF" : "#FFFFFF"      // White
        readonly property color surface2: Qt.platform.os === "osx" ? "#CCF3F4F6" : "#F3F4F6"     // Gray-100
        readonly property color surface3: "#E5E7EB"     // Gray-200
        readonly property color glassSurface: Qt.platform.os === "osx" ? "transparent" : "#FFFFFF"
        readonly property color glassSurface2: Qt.platform.os === "osx" ? Qt.rgba(0, 0, 0, 0.03) : "#F3F4F6"
        
        readonly property color text: "#111827"         // Gray-900
        readonly property color textMuted: "#6B7280"    // Gray-500
        readonly property color textDisabled: "#9CA3AF" // Gray-400
        
        readonly property color border: "#E5E7EB"       // Gray-200
        readonly property color borderFocus: "#3B82F6"  // Blue-500
        
        readonly property color accent: "#2563EB"       // Blue-600
        readonly property color accentHover: "#1D4ED8"  // Blue-700
        readonly property color accentText: "#FFFFFF"
        
        readonly property color danger: "#EF4444"
        readonly property color dangerHover: "#DC2626"
        readonly property color warning: "#F59E0B"
        readonly property color success: "#10B981"
        
        readonly property color shadow: "#9CA3AF"
    }

    // Active Palette Selection
    property var activePalette: isDark ? _darkPalette : _lightPalette

    // =========================================================================
    // EXPOSED COLORS (ALIASES)
    // =========================================================================
    
    readonly property color bg: activePalette.bg
    readonly property color surface: activePalette.surface
    readonly property color surface2: activePalette.surface2
    readonly property color surface3: activePalette.surface3
    readonly property color glassSurface: activePalette.glassSurface
    readonly property color glassSurface2: activePalette.glassSurface2
    
    readonly property color text: activePalette.text
    readonly property color textMuted: activePalette.textMuted
    readonly property color textDisabled: activePalette.textDisabled
    readonly property color textTitle: text // Alias
    readonly property color textSecondary: textMuted // Alias
    
    readonly property color border: activePalette.border
    readonly property color borderFocus: activePalette.borderFocus
    
    readonly property color accent: activePalette.accent
    readonly property color accentHover: activePalette.accentHover
    readonly property color accentText: activePalette.accentText
    
    readonly property color danger: activePalette.danger
    readonly property color dangerHover: activePalette.dangerHover
    readonly property color warning: activePalette.warning
    readonly property color success: activePalette.success
    readonly property color info: accent

    // Legacy Aliases (to prevent breaking existing code)
    readonly property color background: bg
    readonly property color backgroundAlt: surface
    readonly property color cardBackground: surface
    readonly property color cardBorder: border
    readonly property color bg1: bg
    readonly property color bg2: surface
    readonly property color bg3: surface2
    readonly property color toolBar: surface
    readonly property color separator: border
    
    readonly property color window: bg
    readonly property color windowText: text
    readonly property color base: surface
    readonly property color alternateBase: surface2
    readonly property color button: surface2
    readonly property color buttonText: text
    readonly property color highlight: accent
    readonly property color highlightedText: accentText
    readonly property color link: accent

    // =========================================================================
    // STATES & OPACITY
    // =========================================================================
    
    readonly property color hoverOverlay: Qt.rgba(1, 1, 1, isDark ? 0.08 : 0.05)
    readonly property color pressedOverlay: Qt.rgba(0, 0, 0, isDark ? 0.2 : 0.1)
    readonly property color focusRing: Qt.rgba(accent.r, accent.g, accent.b, 0.4)
    
    readonly property real disabledOpacity: 0.5
    readonly property real hoverOpacity: 0.08
    readonly property real pressedOpacity: 0.12

    // =========================================================================
    // TYPOGRAPHY
    // =========================================================================
    
    // Use system font stack preference
    readonly property string fontFamily: Qt.platform.os === "osx" ? "SF Pro Text" : (Qt.platform.os === "windows" ? "Segoe UI" : "Inter")
    readonly property string fontFamilyMono: Qt.platform.os === "osx" ? "SF Mono" : "Consolas"
    
    readonly property int fontSizeS: 11
    readonly property int fontSizeM: 13
    readonly property int fontSizeL: 16
    readonly property int fontSizeXL: 20
    readonly property int fontSizeXXL: 24

    readonly property font fontCaption: Qt.font({family: fontFamily, pixelSize: fontSizeS, weight: Font.Medium})
    readonly property font fontBody: Qt.font({family: fontFamily, pixelSize: fontSizeM, weight: Font.Normal})
    readonly property font fontBodyBold: Qt.font({family: fontFamily, pixelSize: fontSizeM, weight: Font.DemiBold})
    readonly property font fontTitle: Qt.font({family: fontFamily, pixelSize: fontSizeL, weight: Font.DemiBold})
    readonly property font fontHeader: Qt.font({family: fontFamily, pixelSize: fontSizeXL, weight: Font.Bold})

    // =========================================================================
    // SPACING & RADIUS
    // =========================================================================
    
    readonly property int spacingXS: 4
    readonly property int spacingS: 8
    readonly property int spacingM: 16
    readonly property int spacingL: 24
    readonly property int spacingXL: 32
    
    readonly property int radiusS: 4
    readonly property int radiusM: 8
    readonly property int radiusL: 12
    readonly property int radiusXL: 16
    readonly property int radiusPill: 999 

    // Legacy Aliases
    readonly property int radius: radiusM
    readonly property int fontHeaderSize: fontSizeXL
    readonly property int fontTitleSize: fontSizeL
    readonly property int fontBodySize: fontSizeM
    readonly property int fontCaptionSize: fontSizeS
}
