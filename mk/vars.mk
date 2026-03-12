# SPDX-License-Identifier: GPL-2.0

include mk/host.mk

# Version
Launcher_VERSION_MAJOR ?= 0
Launcher_VERSION_MINOR ?= 0
Launcher_VERSION_PATCH ?= 5
Launcher_VERSION_TWEAK ?= 1
Launcher_VERSION_NAME ?= $(Launcher_VERSION_MAJOR).$(Launcher_VERSION_MINOR).$(Launcher_VERSION_PATCH).$(Launcher_VERSION_TWEAK)

# Identity
Launcher_CommonName ?= ProjTLauncher
Launcher_DisplayName ?= ProjT Launcher
Launcher_Name ?= $(Launcher_CommonName)
Launcher_AppID ?= org.projecttick.ProjTLauncher
Launcher_SVGFileName ?= $(Launcher_AppID).svg
Launcher_Domain ?= projecttick.org
Launcher_ConfigFile ?= projtlauncher.cfg
Launcher_Git ?= https://github.com/Project-Tick/ProjT-Launcher
Launcher_UserAgent ?= $(Launcher_CommonName)/$(Launcher_VERSION_NAME)

Launcher_Copyright ?= © 2025-2026 Project Tick\n© 2022-2025 Prism Launcher Contributors\n© 2021-2022 PolyMC Contributors\n© 2012-2021 MultiMC Contributors
Launcher_Copyright_Mac ?= © 2025-2026 Project Tick, © 2022-2025 Prism Launcher Contributors, © 2021-2022 PolyMC Contributors and © 2012-2021 MultiMC Contributors

Launcher_APP_BINARY_NAME ?= projtlauncher
Launcher_SVG ?= program_info/$(Launcher_SVGFileName)
Launcher_Branding_ICNS ?= program_info/projtlauncher.icns
Launcher_Branding_ICO ?= program_info/projtlauncher.ico
Launcher_Branding_WindowsRC ?= program_info/projtlauncher.rc
Launcher_Branding_LogoQRC ?= program_info/projtlauncher.qrc
Launcher_Portable_File ?= program_info/portable.txt
Launcher_Desktop ?= program_info/$(Launcher_AppID).desktop
Launcher_MetaInfo ?= program_info/$(Launcher_AppID).metainfo.xml
Launcher_mrpack_MIMEInfo ?= program_info/modrinth-mrpack-mime.xml

# URLs
Launcher_NEWS_RSS_URL ?= https://projecttick.org/product/projt-launcher/feed.xml
Launcher_NEWS_OPEN_URL ?= https://projecttick.org/product/projt-launcher/news
Launcher_HELP_URL ?= https://projecttick.org/handbook/help-pages/%1
Launcher_HUB_HOME_URL ?= https://projecttick.org/p/projt-launcher/
Launcher_HUB_COMMUNITY_URL ?= https://projecttick.org/projtlauncher/discord
Launcher_HUB_SEARCH_URL ?= https://www.google.com/search?q=%1
Launcher_LOGIN_CALLBACK_URL ?= https://projecttick.org/projtlauncher/successful-login
Launcher_FMLLIBS_BASE_URL ?= https://files.projecttick.org/fmllibs/
Launcher_META_URL ?= https://meta.projecttick.org/
Launcher_IMGUR_CLIENT_ID ?= 5b97b0713fba4a3
Launcher_MSA_CLIENT_ID ?= 3035382c-8f73-493a-b579-d182905c2864
Launcher_CURSEFORGE_API_KEY ?= $2a$10$S7KcKijbCj8mCHUQcn0tgOmtHg0kA8q9FI0niNJJ7knPq0INomzrG
Launcher_BUG_TRACKER_URL ?= https://github.com/Project-Tick/ProjT-Launcher/issues
Launcher_TRANSLATIONS_URL ?= https://crowdin.com/project/projtlauncher
Launcher_TRANSLATION_FILES_URL ?= https://i18n.projecttick.org/
Launcher_MATRIX_URL ?= https://projecttick.org/projtlauncher/matrix
Launcher_DISCORD_URL ?= https://projecttick.org/projtlauncher/discord
Launcher_SUBREDDIT_URL ?= https:/projecttick.org/projtlauncher/reddit

# Build info
Launcher_BUILD_PLATFORM ?= unknown
Launcher_BUILD_ARTIFACT ?=
Launcher_UPDATER_GITHUB_REPO ?= https://github.com/Project-Tick/ProjT-Launcher

# Sparkle (mac)
MACOSX_SPARKLE_UPDATE_PUBLIC_KEY ?=
MACOSX_SPARKLE_UPDATE_FEED_URL ?=

# Git metadata
Launcher_GIT_COMMIT ?= $(shell git rev-parse HEAD 2>/dev/null || echo GIT-NOTFOUND)
Launcher_GIT_TAG ?= $(shell git describe --tags --abbrev=0 2>/dev/null || echo GIT-NOTFOUND)
Launcher_GIT_REFSPEC ?= $(shell git symbolic-ref -q HEAD 2>/dev/null || echo GITDIR-NOTFOUND)

# Compiler info
Launcher_COMPILER_NAME ?= $(notdir $(CXX))
Launcher_COMPILER_VERSION ?= $(shell $(CXX) --version 2>/dev/null | head -n1)
Launcher_COMPILER_TARGET_SYSTEM ?= $(TARGET_OS)
Launcher_COMPILER_TARGET_SYSTEM_VERSION ?= $(shell uname -r 2>/dev/null || echo unknown)
Launcher_COMPILER_TARGET_PROCESSOR ?= $(TARGET_ARCH)

# Build timestamp
Launcher_BUILD_TIMESTAMP ?= $(shell date +%Y-%m-%d)

# Native libs
ifeq ($(TARGET_OS),darwin)
Launcher_GLFW_LIBRARY_NAME ?= libglfw.dylib
Launcher_OPENAL_LIBRARY_NAME ?= libopenal.dylib
else ifeq ($(TARGET_OS),windows)
Launcher_GLFW_LIBRARY_NAME ?= glfw.dll
Launcher_OPENAL_LIBRARY_NAME ?= OpenAL.dll
else
Launcher_GLFW_LIBRARY_NAME ?= libglfw.so
Launcher_OPENAL_LIBRARY_NAME ?= libopenal.so
endif

# Feature toggles
Launcher_ENABLE_JAVA_DOWNLOADER ?= $(if $(call cfg-yes,$(CONFIG_ENABLE_JAVA_DOWNLOADER)),1,0)

export Launcher_VERSION_MAJOR Launcher_VERSION_MINOR Launcher_VERSION_PATCH Launcher_VERSION_TWEAK Launcher_VERSION_NAME
export Launcher_CommonName Launcher_DisplayName Launcher_Name Launcher_AppID Launcher_SVGFileName
export Launcher_Domain Launcher_ConfigFile Launcher_Git Launcher_UserAgent
export Launcher_Copyright Launcher_Copyright_Mac Launcher_APP_BINARY_NAME Launcher_SVG
export Launcher_Branding_ICNS Launcher_Branding_ICO Launcher_Branding_WindowsRC Launcher_Branding_LogoQRC Launcher_Portable_File
export Launcher_Desktop Launcher_MetaInfo Launcher_mrpack_MIMEInfo
export Launcher_NEWS_RSS_URL Launcher_NEWS_OPEN_URL Launcher_HELP_URL Launcher_HUB_HOME_URL Launcher_HUB_COMMUNITY_URL Launcher_HUB_SEARCH_URL
export Launcher_LOGIN_CALLBACK_URL Launcher_FMLLIBS_BASE_URL Launcher_META_URL Launcher_IMGUR_CLIENT_ID Launcher_MSA_CLIENT_ID Launcher_CURSEFORGE_API_KEY
export Launcher_BUG_TRACKER_URL Launcher_TRANSLATIONS_URL Launcher_TRANSLATION_FILES_URL Launcher_MATRIX_URL Launcher_DISCORD_URL Launcher_SUBREDDIT_URL
export Launcher_BUILD_PLATFORM Launcher_BUILD_ARTIFACT Launcher_UPDATER_GITHUB_REPO
export MACOSX_SPARKLE_UPDATE_PUBLIC_KEY MACOSX_SPARKLE_UPDATE_FEED_URL
export Launcher_GIT_COMMIT Launcher_GIT_TAG Launcher_GIT_REFSPEC
export Launcher_COMPILER_NAME Launcher_COMPILER_VERSION Launcher_COMPILER_TARGET_SYSTEM Launcher_COMPILER_TARGET_SYSTEM_VERSION Launcher_COMPILER_TARGET_PROCESSOR
export Launcher_BUILD_TIMESTAMP Launcher_GLFW_LIBRARY_NAME Launcher_OPENAL_LIBRARY_NAME
export Launcher_ENABLE_JAVA_DOWNLOADER
