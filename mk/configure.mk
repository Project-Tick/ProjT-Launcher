# ============================================================================
# mk/configure.mk - Template/configure file processing
# ============================================================================
#
# This file handles .in file conversion.
# Processes @VAR@ style substitutions using values from main Makefile.
#
# VERSION and BRANDING are defined at the top of the main Makefile,
# exactly like the Linux kernel.
#
# ============================================================================

ifndef _MK_CONFIGURE_INCLUDED
_MK_CONFIGURE_INCLUDED := 1

include $(srctree)/mk/config.mk

# ============================================================================
# Configuration Variables (from main Makefile, kernel-style)
# ============================================================================

# Native library names (platform-specific)
HOST_OS_LC := $(shell uname -s 2>/dev/null | tr '[:upper:]' '[:lower:]')
ifeq ($(HOST_OS_LC),darwin)
Launcher_GLFW_LIBRARY_NAME ?= libglfw.dylib
Launcher_OPENAL_LIBRARY_NAME ?= libopenal.dylib
else ifeq ($(HOST_OS_LC),windows)
Launcher_GLFW_LIBRARY_NAME ?= glfw.dll
Launcher_OPENAL_LIBRARY_NAME ?= OpenAL.dll
else
Launcher_GLFW_LIBRARY_NAME ?= libglfw.so
Launcher_OPENAL_LIBRARY_NAME ?= libopenal.so.1
endif

# URLs and API keys
Launcher_NEWS_RSS_URL ?= https://projecttick.org/product/projt-launcher/feed.xml
Launcher_NEWS_OPEN_URL ?= https://projecttick.org/product/projt-launcher/news
Launcher_TRANSLATIONS_URL ?= https://crowdin.com/project/projtlauncher
Launcher_TRANSLATION_FILES_URL ?= https://i18n.projecttick.org/
Launcher_MSA_CLIENT_ID ?= 3035382c-8f73-493a-b579-d182905c2864
Launcher_HELP_URL ?= https://projecttick.org/handbook/help-pages/%1
Launcher_HUB_HOME_URL ?= https://projecttick.org/p/projt-launcher/
Launcher_HUB_COMMUNITY_URL ?= https://projecttick.org/projtlauncher/discord
Launcher_HUB_SEARCH_URL ?= https://www.google.com/search?q=%1
Launcher_LOGIN_CALLBACK_URL ?= https://projecttick.org/projtlauncher/successful-login
Launcher_FMLLIBS_BASE_URL ?= https://files.projecttick.org/fmllibs/
Launcher_META_URL ?= https://meta.projecttick.org/
Launcher_IMGUR_CLIENT_ID ?= 5b97b0713fba4a3
Launcher_CURSEFORGE_API_KEY ?= \$$2a\$$10\$$S7KcKijbCj8mCHUQcn0tgOmtHg0kA8q9FI0niNJJ7knPq0INomzrG
Launcher_BUG_TRACKER_URL ?= https://github.com/Project-Tick/ProjT-Launcher/issues
Launcher_MATRIX_URL ?= https://projecttick.org/projtlauncher/matrix
Launcher_DISCORD_URL ?= https://projecttick.org/projtlauncher/discord
Launcher_SUBREDDIT_URL ?= https://projecttick.org/projtlauncher/reddit

# Version info comes from main Makefile (VERSION, PATCHLEVEL, SUBLEVEL, EXTRAVERSION)
Launcher_VERSION_MAJOR  := $(VERSION)
Launcher_VERSION_MINOR  := $(PATCHLEVEL)
Launcher_VERSION_PATCH  := $(SUBLEVEL)
Launcher_EXTRAVERSION   := $(EXTRAVERSION)
# VERSION_TWEAK - extract number from EXTRAVERSION (e.g. -1 -> 1)
Launcher_VERSION_TWEAK  := $(shell echo "$(EXTRAVERSION)" | sed 's/[^0-9]*//g')
ifeq ($(Launcher_VERSION_TWEAK),)
Launcher_VERSION_TWEAK  := 0
endif
LAUNCHER_VERSION        := $(PROJT_VERSION)
LAUNCHER_VERSION_FULL   := $(PROJT_VERSION_FULL)

# Branding - use values directly or fallback defaults
Launcher_CommonName     ?= $(if $(LAUNCHER_COMMONNAME),$(LAUNCHER_COMMONNAME),ProjTLauncher)
Launcher_Name           ?= $(if $(LAUNCHER_NAME),$(LAUNCHER_NAME),ProjTLauncher)
Launcher_DisplayName    ?= $(if $(LAUNCHER_DISPLAYNAME),$(LAUNCHER_DISPLAYNAME),ProjT Launcher)
Launcher_Copyright      ?= $(if $(LAUNCHER_COPYRIGHT),$(LAUNCHER_COPYRIGHT),© 2025-2026 Project Tick)
Launcher_Domain         ?= $(if $(LAUNCHER_DOMAIN),$(LAUNCHER_DOMAIN),projecttick.org)
Launcher_ConfigFile     ?= $(if $(LAUNCHER_CONFIGFILE),$(LAUNCHER_CONFIGFILE),projtlauncher.cfg)
Launcher_Git            ?= $(if $(LAUNCHER_GIT),$(LAUNCHER_GIT),https://github.com/Project-Tick/ProjT-Launcher)
Launcher_AppID          ?= $(if $(LAUNCHER_APPID),$(LAUNCHER_APPID),org.projecttick.ProjTLauncher)
Launcher_SVGFileName    ?= $(if $(LAUNCHER_SVGFILENAME),$(LAUNCHER_SVGFILENAME),org.projecttick.ProjTLauncher.svg)
Launcher_UserAgent      ?= $(if $(LAUNCHER_USERAGENT),$(LAUNCHER_USERAGENT),ProjTLauncher/$(VERSION).$(PATCHLEVEL).$(SUBLEVEL))
Launcher_BUILD_ARTIFACT ?= $(BUILD_ARTIFACT)
Launcher_UPDATER_GITHUB_REPO ?= $(if $(LAUNCHER_GITHUB_REPO),$(LAUNCHER_GITHUB_REPO),Project-Tick/ProjT-Launcher)
Launcher_APP_BINARY_NAME ?= projtlauncher

# Note: These URLs are already defined at top of file with ?= defaults
# Only override if main Makefile exports different values

# Build timestamp (generated at configure time)
Launcher_BUILD_TIMESTAMP := $(shell date -u +%Y-%m-%dT%H:%M:%SZ 2>/dev/null || echo "unknown")

# ============================================================================
# Platform Detection (from main Makefile TARGET_PLATFORM)
# ============================================================================

ifeq ($(TARGET_PLATFORM),linux)
    Launcher_BUILD_PLATFORM := Linux
    Launcher_COMPILER_TARGET_SYSTEM := Linux
else ifeq ($(TARGET_PLATFORM),windows)
    Launcher_BUILD_PLATFORM := Windows
    Launcher_COMPILER_TARGET_SYSTEM := Windows
else ifeq ($(TARGET_PLATFORM),macos)
    Launcher_BUILD_PLATFORM := macOS
    Launcher_COMPILER_TARGET_SYSTEM := Darwin
else
    Launcher_BUILD_PLATFORM := $(TARGET_PLATFORM)
    Launcher_COMPILER_TARGET_SYSTEM := $(TARGET_PLATFORM)
endif

# Architecture (from main Makefile TARGET_ARCH)
Launcher_COMPILER_TARGET_PROCESSOR := $(TARGET_ARCH)

# System version
ifeq ($(TARGET_PLATFORM),windows)
    # For Windows, we don't have uname -r
    Launcher_COMPILER_TARGET_SYSTEM_VERSION := 10.0
else
    Launcher_COMPILER_TARGET_SYSTEM_VERSION := $(shell uname -r 2>/dev/null || echo "unknown")
endif

# ============================================================================
# Compiler Detection (cross-platform safe)
# ============================================================================

ifeq ($(TARGET_PLATFORM),windows)
  ifeq ($(WINDOWS_TOOLCHAIN),msvc)
    Launcher_COMPILER_NAME := MSVC
    # MSVC version detection is complex, use a placeholder or detect via _MSC_VER
    Launcher_COMPILER_VERSION := 19.0
  else
    # MinGW
    Launcher_COMPILER_NAME := $(shell $(CC) -dumpmachine 2>/dev/null | cut -d'-' -f1 || echo "gcc")
    Launcher_COMPILER_VERSION := $(shell $(CC) -dumpversion 2>/dev/null || echo "unknown")
  endif
else
    # Unix-like (Linux, macOS)
    Launcher_COMPILER_NAME := $(shell basename $$($(CC) --version 2>/dev/null | head -1 | awk '{print $$1}') 2>/dev/null || echo "gcc")
    Launcher_COMPILER_VERSION := $(shell $(CC) -dumpversion 2>/dev/null || echo "unknown")
endif

# LIB_SUFFIX for 64-bit systems
LIB_SUFFIX := $(if $(filter x86_64 aarch64 arm64,$(TARGET_ARCH)),64,)

# ============================================================================
# Generated Headers Directory
# ============================================================================

GENERATED_DIR := $(OBJDIR)/generated
GENERATED_INCLUDE := $(GENERATED_DIR)/include

$(GENERATED_DIR):
	@mkdir -p $@

$(GENERATED_INCLUDE):
	@mkdir -p $@

# ============================================================================
# Substitution sed script
# ============================================================================

SED_SUBST = sed \
	-e 's|@Launcher_Name@|$(Launcher_Name)|g' \
	-e 's|@Launcher_APP_BINARY_NAME@|$(Launcher_APP_BINARY_NAME)|g' \
	-e 's|@Launcher_DisplayName@|$(Launcher_DisplayName)|g' \
	-e 's|@Launcher_Copyright@|$(Launcher_Copyright)|g' \
	-e 's|@Launcher_Domain@|$(Launcher_Domain)|g' \
	-e 's|@Launcher_ConfigFile@|$(Launcher_ConfigFile)|g' \
	-e 's|@Launcher_Git@|$(Launcher_Git)|g' \
	-e 's|@Launcher_AppID@|$(Launcher_AppID)|g' \
	-e 's|@Launcher_SVGFileName@|$(Launcher_SVGFileName)|g' \
	-e 's|@Launcher_UserAgent@|$(Launcher_UserAgent)|g' \
	-e 's|@Launcher_VERSION_MAJOR@|$(Launcher_VERSION_MAJOR)|g' \
	-e 's|@Launcher_VERSION_MINOR@|$(Launcher_VERSION_MINOR)|g' \
	-e 's|@Launcher_VERSION_PATCH@|$(Launcher_VERSION_PATCH)|g' \
	-e 's|@Launcher_VERSION_TWEAK@|$(Launcher_VERSION_TWEAK)|g' \
	-e 's|@Launcher_BUILD_PLATFORM@|$(Launcher_BUILD_PLATFORM)|g' \
	-e 's|@Launcher_BUILD_ARTIFACT@|$(Launcher_BUILD_ARTIFACT)|g' \
	-e 's|@Launcher_BUILD_TIMESTAMP@|$(Launcher_BUILD_TIMESTAMP)|g' \
	-e 's|@Launcher_UPDATER_GITHUB_REPO@|$(Launcher_UPDATER_GITHUB_REPO)|g' \
	-e 's|@Launcher_COMPILER_NAME@|$(Launcher_COMPILER_NAME)|g' \
	-e 's|@Launcher_COMPILER_VERSION@|$(Launcher_COMPILER_VERSION)|g' \
	-e 's|@Launcher_COMPILER_TARGET_SYSTEM@|$(Launcher_COMPILER_TARGET_SYSTEM)|g' \
	-e 's|@Launcher_COMPILER_TARGET_SYSTEM_VERSION@|$(Launcher_COMPILER_TARGET_SYSTEM_VERSION)|g' \
	-e 's|@Launcher_COMPILER_TARGET_PROCESSOR@|$(Launcher_COMPILER_TARGET_PROCESSOR)|g' \
	-e 's|@MACOSX_SPARKLE_UPDATE_PUBLIC_KEY@||g' \
	-e 's|@MACOSX_SPARKLE_UPDATE_FEED_URL@||g' \
	-e 's|@LIB_SUFFIX@|$(LIB_SUFFIX)|g' \
	-e 's|@VERSION@|$(LAUNCHER_VERSION)|g' \
	-e 's|@PROJECT_VERSION@|$(LAUNCHER_VERSION)|g' \
	-e 's|@Launcher_GIT_COMMIT@|$(Launcher_GIT_COMMIT)|g' \
	-e 's|@Launcher_GIT_TAG@|$(Launcher_GIT_TAG)|g' \
	-e 's|@Launcher_GIT_REFSPEC@|$(Launcher_GIT_REFSPEC)|g' \
	-e 's|@Launcher_NEWS_RSS_URL@|$(Launcher_NEWS_RSS_URL)|g' \
	-e 's|@Launcher_NEWS_OPEN_URL@|$(Launcher_NEWS_OPEN_URL)|g' \
	-e 's|@Launcher_TRANSLATIONS_URL@|$(Launcher_TRANSLATIONS_URL)|g' \
	-e 's|@Launcher_TRANSLATION_FILES_URL@|$(Launcher_TRANSLATION_FILES_URL)|g' \
	-e 's|@Launcher_GLFW_LIBRARY_NAME@|$(Launcher_GLFW_LIBRARY_NAME)|g' \
	-e 's|@Launcher_OPENAL_LIBRARY_NAME@|$(Launcher_OPENAL_LIBRARY_NAME)|g' \
	-e 's|@Launcher_MSA_CLIENT_ID@|$(Launcher_MSA_CLIENT_ID)|g' \
	-e 's|@Launcher_HELP_URL@|$(Launcher_HELP_URL)|g' \
	-e 's|@Launcher_HUB_HOME_URL@|$(Launcher_HUB_HOME_URL)|g' \
	-e 's|@Launcher_HUB_COMMUNITY_URL@|$(Launcher_HUB_COMMUNITY_URL)|g' \
	-e 's|@Launcher_HUB_SEARCH_URL@|$(Launcher_HUB_SEARCH_URL)|g' \
	-e 's|@Launcher_LOGIN_CALLBACK_URL@|$(Launcher_LOGIN_CALLBACK_URL)|g' \
	-e 's|@Launcher_FMLLIBS_BASE_URL@|$(Launcher_FMLLIBS_BASE_URL)|g' \
	-e 's|@Launcher_META_URL@|$(Launcher_META_URL)|g' \
	-e 's|@Launcher_IMGUR_CLIENT_ID@|$(Launcher_IMGUR_CLIENT_ID)|g' \
	-e 's|@Launcher_CURSEFORGE_API_KEY@|$(Launcher_CURSEFORGE_API_KEY)|g' \
	-e 's|@Launcher_BUG_TRACKER_URL@|$(Launcher_BUG_TRACKER_URL)|g' \
	-e 's|@Launcher_MATRIX_URL@|$(Launcher_MATRIX_URL)|g' \
	-e 's|@Launcher_DISCORD_URL@|$(Launcher_DISCORD_URL)|g' \
	-e 's|@Launcher_SUBREDDIT_URL@|$(Launcher_SUBREDDIT_URL)|g' \
	-e 's|\#cmakedefine01 Launcher_ENABLE_JAVA_DOWNLOADER|\#define Launcher_ENABLE_JAVA_DOWNLOADER $(if $(call cfg-yes,$(CONFIG_FEATURE_JAVA_DOWNLOADER)),1,0)|g'

# ============================================================================
# BuildConfig.cpp generation
# ============================================================================

BUILDCONFIG_IN := $(srctree)/buildconfig/BuildConfig.cpp.in
BUILDCONFIG_OUT := $(GENERATED_DIR)/BuildConfig.cpp

$(BUILDCONFIG_OUT): $(BUILDCONFIG_IN) $(KBUILD_OUTPUT)/.config | $(GENERATED_DIR)
	@echo "  GEN     $@"
	$(Q)$(SED_SUBST) $< > $@

# ============================================================================
# Launcher.in (launch script) generation
# ============================================================================

LAUNCHER_SCRIPT_IN := $(srctree)/launcher/Launcher.in
LAUNCHER_SCRIPT_OUT := $(OBJDIR)/bin/LauncherScript

$(LAUNCHER_SCRIPT_OUT): $(LAUNCHER_SCRIPT_IN) $(KBUILD_OUTPUT)/.config
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)$(SED_SUBST) $< > $@
	$(Q)chmod +x $@

# ============================================================================
# libpng pnglibconf.h
# ============================================================================

PNGLIBCONF_PREBUILT := $(srctree)/libpng/scripts/pnglibconf.h.prebuilt
PNGLIBCONF_OUT := $(GENERATED_INCLUDE)/pnglibconf.h

$(PNGLIBCONF_OUT): $(PNGLIBCONF_PREBUILT) | $(GENERATED_INCLUDE)
	@echo "  COPY    $@"
	$(Q)cp $< $@

# Also copy to libpng source dir for direct include
PNGLIBCONF_SRC := $(srctree)/libpng/pnglibconf.h

$(PNGLIBCONF_SRC): $(PNGLIBCONF_PREBUILT)
	@echo "  COPY    $@"
	$(Q)cp $< $@

# ============================================================================
# bzip2 bz_version.h
# ============================================================================

BZ_VERSION_IN := $(srctree)/bzip2/bz_version.h.in
BZ_VERSION_OUT := $(srctree)/bzip2/bz_version.h
BZ_VERSION := 1.0.8

$(BZ_VERSION_OUT): $(BZ_VERSION_IN)
	@echo "  GEN     $@"
	$(Q)sed -e 's|@BZ_VERSION@|$(BZ_VERSION)|g' $< > $@

# ============================================================================
# libnbtplusplus nbt_export.h

NBT_EXPORT_OUT := $(GENERATED_INCLUDE)/nbt_export.h
NBT_EXPORT_SRC := $(srctree)/libnbtplusplus/include/nbt_export.h

$(NBT_EXPORT_OUT): $(srctree)/scripts/gen-nbt-export.sh | $(GENERATED_INCLUDE)
	@echo "  GEN     $@"
	$(Q)$(srctree)/scripts/gen-nbt-export.sh $@

$(NBT_EXPORT_SRC): $(NBT_EXPORT_OUT)
	@echo "  COPY    $@"
	$(Q)cp $< $@

# ============================================================================
# libqrencode config.h (must be in libqrencode source dir)
# ============================================================================

QRENCODE_CONFIG_OUT := $(srctree)/libqrencode/config.h

$(QRENCODE_CONFIG_OUT): $(srctree)/scripts/gen-qrencode-config.sh
	@echo "  GEN     $@"
	$(Q)$(srctree)/scripts/gen-qrencode-config.sh $@

# ============================================================================
# cmark headers
# ============================================================================

CMARK_CONFIG_OUT := $(GENERATED_INCLUDE)/cmark_config.h
CMARK_EXPORT_OUT := $(GENERATED_INCLUDE)/cmark_export.h
CMARK_VERSION_OUT := $(GENERATED_INCLUDE)/cmark_version.h
CMARK_CONFIG_SRC := $(srctree)/cmark/src/cmark_config.h
CMARK_EXPORT_SRC := $(srctree)/cmark/src/cmark_export.h
CMARK_VERSION_SRC := $(srctree)/cmark/src/cmark_version.h

$(CMARK_CONFIG_OUT): $(srctree)/scripts/gen-cmark-config.sh | $(GENERATED_INCLUDE)
	@echo "  GEN     $@"
	$(Q)$(srctree)/scripts/gen-cmark-config.sh $@

$(CMARK_EXPORT_OUT): $(srctree)/scripts/gen-cmark-export.sh | $(GENERATED_INCLUDE)
	@echo "  GEN     $@"
	$(Q)$(srctree)/scripts/gen-cmark-export.sh $@

$(CMARK_VERSION_OUT): $(srctree)/scripts/gen-cmark-version.sh | $(GENERATED_INCLUDE)
	@echo "  GEN     $@"
	$(Q)$(srctree)/scripts/gen-cmark-version.sh $@

$(CMARK_CONFIG_SRC): $(CMARK_CONFIG_OUT)
	@echo "  COPY    $@"
	$(Q)cp $< $@

$(CMARK_EXPORT_SRC): $(CMARK_EXPORT_OUT)
	@echo "  COPY    $@"
	$(Q)cp $< $@

$(CMARK_VERSION_SRC): $(CMARK_VERSION_OUT)
	@echo "  COPY    $@"
	$(Q)cp $< $@

# ============================================================================
# All generated files
# ============================================================================

GENERATED_FILES := \
	$(BUILDCONFIG_OUT) \
	$(LAUNCHER_SCRIPT_OUT) \
	$(BZ_VERSION_OUT) \
	$(PNGLIBCONF_OUT) \
	$(PNGLIBCONF_SRC) \
	$(NBT_EXPORT_OUT) \
	$(NBT_EXPORT_SRC) \
	$(QRENCODE_CONFIG_OUT) \
	$(CMARK_CONFIG_OUT) \
	$(CMARK_EXPORT_OUT) \
	$(CMARK_VERSION_OUT) \
	$(CMARK_CONFIG_SRC) \
	$(CMARK_EXPORT_SRC) \
	$(CMARK_VERSION_SRC)

# ============================================================================
# Configure cleanup target (configure itself is in targets.mk)
# ============================================================================

.PHONY: configure-clean

configure-clean:
	$(Q)rm -rf $(GENERATED_DIR)
	$(Q)rm -f $(PNGLIBCONF_SRC) $(NBT_EXPORT_SRC) $(QRENCODE_CONFIG_OUT) $(BZ_VERSION_OUT) \
		$(CMARK_CONFIG_SRC) $(CMARK_EXPORT_SRC) $(CMARK_VERSION_SRC)
	@echo "Generated files cleaned."

# ============================================================================
# Export variables for other makefiles
# ============================================================================

export GENERATED_DIR
export GENERATED_INCLUDE
export LAUNCHER_VERSION
export LAUNCHER_VERSION_FULL

# Add generated include to CFLAGS
CFLAGS += -I$(GENERATED_INCLUDE)
CXXFLAGS += -I$(GENERATED_INCLUDE)

endif # _MK_CONFIGURE_INCLUDED
