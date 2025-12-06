.pragma library
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

var background = "#1b1b1b"
var backgroundAlt = "#222222"
var surface = "#252525"
var surfaceBackground = "#1e1e1e"
var surfaceVariant = "#2a2a2a"
var surface0 = "#252525"
var surface1 = "#303030"
var surface2 = "#3a3a3a"
var mantle = "#181818"
var textPrimary = "#e0e0e0"
var textSecondary = "#a0a0a0"
var text = "#ffffff"
var subtext0 = "#b0b0b0"
var subtext1 = "#909090"
var border = "#404040"
var accent = "#96db59"
var primary = "#2fa3c6"
var selection = "#3d4d60"
var red = "#d55f5f"
var green = "#4caf50"
var yellow = "#ff9800"
var blue = "#2196f3"

// Layout
var radius = 8
var radiusS = 4
var radiusM = 8
var radiusL = 12
var spacingXS = 4
var spacingS = 8
var spacingM = 12
var spacingL = 16

// Typography
var fontHeader = 18
var fontSubtitle = 14
var fontBody = 12
var fontCaption = 11
var fontSizeSmall = 10
var fontSizeMedium = 12
var fontSizeLarge = 14

// Icon theme - matches Qt theme icons
// Icons are stored as qrc:/icons/{theme}/scalable/{name}.svg
// Default theme is pe_dark
var iconTheme = "pe_dark"

// Helper function to get icon path
function icon(name) {
    return "qrc:/icons/" + iconTheme + "/scalable/" + name + ".svg"
}

// Common icon names for QML usage
var icons = {
    // Toolbar
    "new": "new",
    "refresh": "refresh",
    "settings": "settings",
    "about": "about",
    "help": "help",
    "news": "news",
    "log": "log",
    "accounts": "accounts",
    "language": "language",
    "appearance": "appearance",
    
    // Instance actions
    "launch": "launch",
    "delete": "delete",
    "rename": "rename",
    "copy": "copy",
    "export": "export",
    "viewfolder": "viewfolder",
    "shortcut": "shortcut",
    "tag": "tag",
    
    // Instance settings
    "instanceSettings": "instance-settings",
    "java": "java",
    "minecraft": "minecraft",
    "loadermods": "loadermods",
    "coremods": "coremods",
    "jarmods": "jarmods",
    "resourcepacks": "resourcepacks",
    "shaderpacks": "shaderpacks",
    "datapacks": "datapacks",
    "worlds": "worlds",
    "screenshots": "screenshots",
    "notes": "notes",
    "server": "server",
    "customCommands": "custom-commands",
    
    // Settings pages
    "proxy": "proxy",
    "externaltools": "externaltools",
    "checkupdate": "checkupdate",
    
    // Status
    "statusGood": "status-good",
    "statusBad": "status-bad",
    "statusYellow": "status-yellow",
    "bug": "bug"
}

// Platform/Mod source icons - from multimc theme
var platformIcons = {
    "curseforge": "flame",
    "modrinth": "modrinth",
    "ftb": "ftb_logo",
    "atlauncher": "gear",
    "technic": "brick",
    "fabric": "fabricmc",
    "forge": "gear",
    "quilt": "quiltmc",
    "neoforge": "neoforged"
}

// Helper function to get platform icon path (from multimc theme)
function platformIcon(platform) {
    var iconName = platformIcons[platform.toLowerCase()] || "gear"
    return "qrc:/icons/multimc/scalable/instances/" + iconName + ".svg"
}

// Helper function to get instance icon (from multimc theme)
function instanceIconFromKey(key) {
    if (!key || key === "default" || key === "") {
        return "qrc:/icons/multimc/scalable/instances/grass.svg"
    }
    // Check if it's a known instance icon
    var knownIcons = ["bee", "brick", "chicken", "creeper", "diamond", "dirt", 
                      "enderman", "enderpearl", "fox", "gear", "gold", "grass",
                      "herobrine", "iron", "magitech", "meat", "netherstar", 
                      "planks", "skeleton", "squarecreeper", "steve", "stone", "tnt"]
    if (knownIcons.indexOf(key) >= 0) {
        return "qrc:/icons/multimc/scalable/instances/" + key + ".svg"
    }
    return "qrc:/icons/multimc/scalable/instances/grass.svg"
}

// Version type icons (for Minecraft versions)
function versionTypeIcon(type) {
    if (type === "release") return "qrc:/icons/multimc/scalable/instances/grass.svg"
    if (type === "snapshot") return "qrc:/icons/multimc/scalable/instances/enderpearl.svg"
    if (type === "old_beta") return icon("bug")
    if (type === "old_alpha") return "qrc:/icons/multimc/scalable/instances/chicken.svg"
    return "qrc:/icons/multimc/scalable/instances/grass.svg"
}

// Instance icon helper - uses IconList from C++
function instanceIcon(iconKey) {
    // Instance icons are managed by IconList C++ class
    // They use file:// protocol or fall back to default
    if (!iconKey || iconKey === "default" || iconKey === "") {
        return icon("minecraft")
    }
    // For custom icons, the path should come from IconList
    return ""
}
