#!/bin/bash
# ProjT Launcher - .in File Transformer
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick
#
# This script transforms .in template files by replacing @VAR@ placeholders
# with actual values from mk/config.mk
#
# Usage: ./scripts/configure-in.sh <input.in> <output>

set -e

if [ $# -lt 2 ]; then
    echo "Usage: $0 <input.in> <output>"
    echo "       $0 --all    # Process all .in files"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SRCDIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
CONFIG_FILE="${SRCDIR}/mk/config.mk"
BUILDDIR="${SRCDIR}/build"

# Check if config.mk exists
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: ${CONFIG_FILE} not found. Run ./configure first."
    exit 1
fi

# Source variables from config.mk (convert make syntax to shell)
eval $(sed -e 's/ := /=/' -e 's/ = /=/' -e '/^#/d' -e '/^$/d' -e 's/\$$/\\$/g' "$CONFIG_FILE" | \
       grep -E '^[A-Z_]+=' | \
       sed -e 's/=\(.*\)/="\1"/')

# Additional variables that may not be in config.mk
LAUNCHER_SVGFILENAME="${LAUNCHER_SVGFILENAME:-org.projecttick.ProjTLauncher.svg}"
LAUNCHER_USERAGENT="${LAUNCHER_NAME}/${VERSION_NAME}"

# cmakedefine handling - convert to 0 or 1
if [ "${JAVA_DOWNLOADER_ENABLED:-1}" = "1" ]; then
    LAUNCHER_ENABLE_JAVA_DOWNLOADER="1"
else
    LAUNCHER_ENABLE_JAVA_DOWNLOADER="0"
fi

# Function to transform a single .in file
transform_file() {
    local input="$1"
    local output="$2"
    
    if [ ! -f "$input" ]; then
        echo "Error: Input file not found: $input"
        return 1
    fi
    
    # Create output directory if needed
    mkdir -p "$(dirname "$output")"
    
    # Apply substitutions
    sed \
        -e "s|@Launcher_Name@|${LAUNCHER_NAME}|g" \
        -e "s|@Launcher_APP_BINARY_NAME@|${LAUNCHER_APP_BINARY_NAME}|g" \
        -e "s|@Launcher_DisplayName@|${LAUNCHER_DISPLAYNAME}|g" \
        -e "s|@Launcher_Copyright@|${LAUNCHER_COPYRIGHT}|g" \
        -e "s|@Launcher_Domain@|${LAUNCHER_DOMAIN}|g" \
        -e "s|@Launcher_ConfigFile@|${LAUNCHER_CONFIGFILE}|g" \
        -e "s|@Launcher_Git@|${LAUNCHER_GIT}|g" \
        -e "s|@Launcher_AppID@|${LAUNCHER_APPID}|g" \
        -e "s|@Launcher_SVGFileName@|${LAUNCHER_SVGFILENAME}|g" \
        -e "s|@Launcher_UserAgent@|${LAUNCHER_USERAGENT}|g" \
        -e "s|@Launcher_VERSION_MAJOR@|${VERSION_MAJOR}|g" \
        -e "s|@Launcher_VERSION_MINOR@|${VERSION_MINOR}|g" \
        -e "s|@Launcher_VERSION_PATCH@|${VERSION_PATCH}|g" \
        -e "s|@Launcher_VERSION_TWEAK@|${VERSION_TWEAK}|g" \
        -e "s|@Launcher_VERSION_NAME@|${VERSION_NAME}|g" \
        -e "s|@Launcher_VERSION_NAME4@|${VERSION_NAME}|g" \
        -e "s|@Launcher_VERSION_NAME4_COMMA@|${VERSION_MAJOR},${VERSION_MINOR},${VERSION_PATCH},${VERSION_TWEAK}|g" \
        -e "s|@Launcher_BUILD_PLATFORM@|${BUILD_PLATFORM}|g" \
        -e "s|@Launcher_BUILD_ARTIFACT@|${BUILD_ARTIFACT}|g" \
        -e "s|@Launcher_BUILD_TIMESTAMP@|${BUILD_TIMESTAMP}|g" \
        -e "s|@Launcher_UPDATER_GITHUB_REPO@|${UPDATER_GITHUB_REPO}|g" \
        -e "s|@Launcher_COMPILER_NAME@|${COMPILER_NAME}|g" \
        -e "s|@Launcher_COMPILER_VERSION@|${COMPILER_VERSION}|g" \
        -e "s|@Launcher_COMPILER_TARGET_SYSTEM@|${COMPILER_TARGET_SYSTEM}|g" \
        -e "s|@Launcher_COMPILER_TARGET_SYSTEM_VERSION@|${COMPILER_TARGET_SYSTEM_VERSION}|g" \
        -e "s|@Launcher_COMPILER_TARGET_PROCESSOR@|${COMPILER_TARGET_PROCESSOR}|g" \
        -e "s|@MACOSX_SPARKLE_UPDATE_PUBLIC_KEY@||g" \
        -e "s|@MACOSX_SPARKLE_UPDATE_FEED_URL@||g" \
        -e "s|@Launcher_GIT_COMMIT@|${GIT_COMMIT}|g" \
        -e "s|@Launcher_GIT_TAG@|${GIT_TAG}|g" \
        -e "s|@Launcher_GIT_REFSPEC@|${GIT_REFSPEC}|g" \
        -e "s|@Launcher_NEWS_RSS_URL@|${NEWS_RSS_URL}|g" \
        -e "s|@Launcher_NEWS_OPEN_URL@|${NEWS_OPEN_URL}|g" \
        -e "s|@Launcher_HELP_URL@|${HELP_URL}|g" \
        -e "s|@Launcher_LOGIN_CALLBACK_URL@|${LOGIN_CALLBACK_URL}|g" \
        -e "s|@Launcher_IMGUR_CLIENT_ID@|${IMGUR_CLIENT_ID}|g" \
        -e "s|@Launcher_MSA_CLIENT_ID@|${MSA_CLIENT_ID}|g" \
        -e "s|@Launcher_CURSEFORGE_API_KEY@|${FLAME_API_KEY}|g" \
        -e "s|@Launcher_META_URL@|${META_URL}|g" \
        -e "s|@Launcher_FMLLIBS_BASE_URL@|${FMLLIBS_BASE_URL}|g" \
        -e "s|@Launcher_GLFW_LIBRARY_NAME@|${GLFW_LIBRARY_NAME}|g" \
        -e "s|@Launcher_OPENAL_LIBRARY_NAME@|${OPENAL_LIBRARY_NAME}|g" \
        -e "s|@Launcher_BUG_TRACKER_URL@|${BUG_TRACKER_URL}|g" \
        -e "s|@Launcher_TRANSLATIONS_URL@|${TRANSLATIONS_URL}|g" \
        -e "s|@Launcher_TRANSLATION_FILES_URL@|${TRANSLATION_FILES_URL}|g" \
        -e "s|@Launcher_MATRIX_URL@|${MATRIX_URL}|g" \
        -e "s|@Launcher_DISCORD_URL@|${DISCORD_URL}|g" \
        -e "s|@Launcher_SUBREDDIT_URL@|${SUBREDDIT_URL}|g" \
        -e "s|@Launcher_Branding_ICO@|projtlauncher.ico|g" \
        -e "s|@Launcher_Branding_WindowsRC@|projtlauncher.rc|g" \
        -e "s|@Launcher_Branding_LogoQRC@|projtlauncher.qrc|g" \
        -e "s|@Launcher_Project_Source_Dir@|${SRCDIR}|g" \
        -e "s|@BINARY_DEST_DIR@|bin|g" \
        -e "s|@LIBRARY_DEST_DIR@|lib|g" \
        -e "s|@JARS_DEST_DIR@|jars|g" \
        -e "s|@Launcher_ENABLE_JAVA_DOWNLOADER@|${LAUNCHER_ENABLE_JAVA_DOWNLOADER}|g" \
        "$input" > "$output.tmp"
    
    # Handle #cmakedefine01 lines (convert to #define with 0 or 1)
    sed -i.bak \
        -e "s|^#cmakedefine01 Launcher_ENABLE_JAVA_DOWNLOADER|#define Launcher_ENABLE_JAVA_DOWNLOADER ${LAUNCHER_ENABLE_JAVA_DOWNLOADER}|g" \
        "$output.tmp"
    
    # Remove backup file created by sed -i
    rm -f "$output.tmp.bak"
    
    # Move to final location
    mv "$output.tmp" "$output"
    
    echo "Generated: $output"
}

# Process all .in files
process_all() {
    echo "Processing all .in files..."
    
    # Project-specific .in files
    local in_files=(
        "buildconfig/BuildConfig.cpp.in:${BUILDDIR}/gen/BuildConfig.cpp"
        "program_info/org.projecttick.ProjTLauncher.desktop.in:${BUILDDIR}/gen/org.projecttick.ProjTLauncher.desktop"
        "program_info/org.projecttick.ProjTLauncher.metainfo.xml.in:${BUILDDIR}/gen/org.projecttick.ProjTLauncher.metainfo.xml"
        "program_info/projtlauncher.manifest.in:${BUILDDIR}/gen/projtlauncher.manifest"
        "program_info/projtlauncher.qrc.in:${BUILDDIR}/gen/projtlauncher.qrc"
        "program_info/projtlauncher.rc.in:${BUILDDIR}/gen/projtlauncher.rc"
        "program_info/win_install.nsi.in:${BUILDDIR}/gen/win_install.nsi"
        "launcher/Launcher.in:${BUILDDIR}/gen/Launcher"
    )
    
    for mapping in "${in_files[@]}"; do
        local input="${SRCDIR}/${mapping%%:*}"
        local output="${mapping#*:}"
        
        if [ -f "$input" ]; then
            transform_file "$input" "$output"
        else
            echo "Warning: Input file not found: $input"
        fi
    done
    
    echo ""
    echo "All .in files processed successfully."
}

# Main logic
if [ "$1" = "--all" ]; then
    process_all
else
    transform_file "$1" "$2"
fi
