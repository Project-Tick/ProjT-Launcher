# ============================================================================
# mk/configure.mk - Template/configure file processing
# ============================================================================
#
# This file handles .in file conversion based on Kconfig settings.
# Processes @VAR@ and ${VAR} style substitutions.
#
# ============================================================================

ifndef _MK_CONFIGURE_INCLUDED
_MK_CONFIGURE_INCLUDED := 1

include $(srctree)/mk/config.mk

# ============================================================================
# Configuration Variables from Kconfig
# ============================================================================

# Version info
LAUNCHER_VERSION := $(CONFIG_VERSION_MAJOR).$(CONFIG_VERSION_MINOR).$(CONFIG_VERSION_PATCH).$(CONFIG_VERSION_TWEAK)
LAUNCHER_VERSION_FULL := $(LAUNCHER_VERSION)$(CONFIG_VERSION_SUFFIX)

# Build info
Launcher_Name := $(CONFIG_LAUNCHER_NAME)
Launcher_APP_BINARY_NAME := $(CONFIG_LAUNCHER_APP_BINARY_NAME)
Launcher_DisplayName := $(CONFIG_LAUNCHER_DISPLAYNAME)
Launcher_Copyright := $(CONFIG_LAUNCHER_COPYRIGHT)
Launcher_Domain := $(CONFIG_LAUNCHER_DOMAIN)
Launcher_ConfigFile := $(CONFIG_LAUNCHER_CONFIGFILE)
Launcher_Git := $(CONFIG_LAUNCHER_GIT)
Launcher_AppID := $(CONFIG_LAUNCHER_APPID)
Launcher_SVGFileName := $(CONFIG_LAUNCHER_SVGFILENAME)
Launcher_UserAgent := $(subst @VERSION@,$(LAUNCHER_VERSION),$(CONFIG_LAUNCHER_USERAGENT))
Launcher_VERSION_MAJOR := $(CONFIG_VERSION_MAJOR)
Launcher_VERSION_MINOR := $(CONFIG_VERSION_MINOR)
Launcher_VERSION_PATCH := $(CONFIG_VERSION_PATCH)
Launcher_VERSION_TWEAK := $(CONFIG_VERSION_TWEAK)
Launcher_BUILD_ARTIFACT := $(CONFIG_BUILD_ARTIFACT)
Launcher_BUILD_TIMESTAMP := $(shell date -u +%Y-%m-%dT%H:%M:%SZ)
Launcher_UPDATER_GITHUB_REPO := $(CONFIG_UPDATER_GITHUB_REPO)

# Platform detection
ifeq ($(CONFIG_TARGET_AUTO),y)
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    Launcher_BUILD_PLATFORM := Linux
    Launcher_COMPILER_TARGET_SYSTEM := Linux
  else ifeq ($(UNAME_S),Darwin)
    Launcher_BUILD_PLATFORM := macOS
    Launcher_COMPILER_TARGET_SYSTEM := Darwin
  else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    Launcher_BUILD_PLATFORM := Windows
    Launcher_COMPILER_TARGET_SYSTEM := Windows
  else ifeq ($(findstring CYGWIN,$(UNAME_S)),CYGWIN)
    Launcher_BUILD_PLATFORM := Windows
    Launcher_COMPILER_TARGET_SYSTEM := Windows
  else
    Launcher_BUILD_PLATFORM := $(UNAME_S)
    Launcher_COMPILER_TARGET_SYSTEM := $(UNAME_S)
  endif
else ifeq ($(CONFIG_TARGET_LINUX),y)
  Launcher_BUILD_PLATFORM := Linux
  Launcher_COMPILER_TARGET_SYSTEM := Linux
else ifeq ($(CONFIG_TARGET_WINDOWS),y)
  Launcher_BUILD_PLATFORM := Windows
  Launcher_COMPILER_TARGET_SYSTEM := Windows
else ifeq ($(CONFIG_TARGET_MACOS),y)
  Launcher_BUILD_PLATFORM := macOS
  Launcher_COMPILER_TARGET_SYSTEM := Darwin
endif

# Compiler info
Launcher_COMPILER_TARGET_SYSTEM_VERSION := $(shell uname -r 2>/dev/null || echo "unknown")

ifeq ($(CONFIG_ARCH_AUTO),y)
  UNAME_M := $(shell uname -m)
  Launcher_COMPILER_TARGET_PROCESSOR := $(UNAME_M)
else ifeq ($(CONFIG_ARCH_X86_64),y)
  Launcher_COMPILER_TARGET_PROCESSOR := x86_64
else ifeq ($(CONFIG_ARCH_AARCH64),y)
  Launcher_COMPILER_TARGET_PROCESSOR := aarch64
else ifeq ($(CONFIG_ARCH_ARM),y)
  Launcher_COMPILER_TARGET_PROCESSOR := arm
else
  Launcher_COMPILER_TARGET_PROCESSOR := $(shell uname -m)
endif

# Detect compiler name and version
ifeq ($(findstring clang,$(CC)),clang)
  Launcher_COMPILER_NAME := Clang
  Launcher_COMPILER_VERSION := $(shell $(CC) --version 2>/dev/null | head -1 | sed 's/.*version \([0-9.]*\).*/\1/')
else ifeq ($(findstring gcc,$(CC)),gcc)
  Launcher_COMPILER_NAME := GCC
  Launcher_COMPILER_VERSION := $(shell $(CC) -dumpversion 2>/dev/null)
else
  Launcher_COMPILER_NAME := Unknown
  Launcher_COMPILER_VERSION := Unknown
endif

# macOS Sparkle (empty by default)
MACOSX_SPARKLE_UPDATE_PUBLIC_KEY :=
MACOSX_SPARKLE_UPDATE_FEED_URL :=

# LIB_SUFFIX for Launcher.in
ifeq ($(CONFIG_ARCH_X86_64),y)
  LIB_SUFFIX := 64
else ifeq ($(CONFIG_ARCH_AARCH64),y)
  LIB_SUFFIX := 64
else
  LIB_SUFFIX :=
endif

# ============================================================================
# Substitution function
# ============================================================================

# Create sed script for @VAR@ substitutions
define SED_SUBST_SCRIPT
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
-e 's|@MACOSX_SPARKLE_UPDATE_PUBLIC_KEY@|$(MACOSX_SPARKLE_UPDATE_PUBLIC_KEY)|g' \
-e 's|@MACOSX_SPARKLE_UPDATE_FEED_URL@|$(MACOSX_SPARKLE_UPDATE_FEED_URL)|g' \
-e 's|@LIB_SUFFIX@|$(LIB_SUFFIX)|g' \
-e 's|@VERSION@|$(LAUNCHER_VERSION)|g' \
-e 's|@PROJECT_VERSION@|$(LAUNCHER_VERSION)|g' \
-e 's|@PROJECT_VERSION_MAJOR@|$(Launcher_VERSION_MAJOR)|g' \
-e 's|@PROJECT_VERSION_MINOR@|$(Launcher_VERSION_MINOR)|g' \
-e 's|@PROJECT_VERSION_PATCH@|$(Launcher_VERSION_PATCH)|g'
endef

# Process a .in file to output
# Usage: $(call process_in,input.in,output)
define process_in
	@mkdir -p $(dir $(2))
	@echo "  GEN     $(2)"
	$(Q)sed $(SED_SUBST_SCRIPT) $(1) > $(2)
endef

# ============================================================================
# Generated Headers Directory
# ============================================================================

GENERATED_DIR := $(OBJDIR)/generated
GENERATED_INCLUDE := $(GENERATED_DIR)/include

# ============================================================================
# BuildConfig.cpp generation
# ============================================================================

BUILDCONFIG_IN := $(srctree)/buildconfig/BuildConfig.cpp.in
BUILDCONFIG_OUT := $(GENERATED_DIR)/BuildConfig.cpp

$(BUILDCONFIG_OUT): $(BUILDCONFIG_IN) $(srctree)/.config FORCE
	$(call process_in,$(BUILDCONFIG_IN),$(BUILDCONFIG_OUT))

# ============================================================================
# Launcher.in (launch script) generation
# ============================================================================

LAUNCHER_SCRIPT_IN := $(srctree)/launcher/Launcher.in
LAUNCHER_SCRIPT_OUT := $(OBJDIR)/bin/LauncherScript

$(LAUNCHER_SCRIPT_OUT): $(LAUNCHER_SCRIPT_IN) $(srctree)/.config FORCE
	$(call process_in,$(LAUNCHER_SCRIPT_IN),$(LAUNCHER_SCRIPT_OUT))
	$(Q)chmod +x $(LAUNCHER_SCRIPT_OUT)

# ============================================================================
# libpng pnglibconf.h
# ============================================================================
#
# libpng has a prebuilt header we can use directly.
# We just copy it to the generated include directory.
#

PNGLIBCONF_PREBUILT := $(srctree)/libpng/scripts/pnglibconf.h.prebuilt
PNGLIBCONF_OUT := $(GENERATED_INCLUDE)/pnglibconf.h

$(PNGLIBCONF_OUT): $(PNGLIBCONF_PREBUILT)
	@mkdir -p $(dir $@)
	@echo "  COPY    $@"
	$(Q)cp $< $@

# ============================================================================
# libnbtplusplus nbt_export.h
# ============================================================================
#
# Generate export header for libnbtplusplus.
# This mimics CMake's generate_export_header().
#

NBT_EXPORT_OUT := $(GENERATED_INCLUDE)/nbt_export.h

$(NBT_EXPORT_OUT): FORCE
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)cat > $@ << 'EXPORT_EOF'
#ifndef NBT_EXPORT_H
#define NBT_EXPORT_H

#ifdef NBT_STATIC_DEFINE
#  define NBT_EXPORT
#  define NBT_NO_EXPORT
#else
#  ifndef NBT_EXPORT
#    ifdef nbt___EXPORTS
        /* We are building this library */
#      define NBT_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define NBT_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef NBT_NO_EXPORT
#    define NBT_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef NBT_DEPRECATED
#  define NBT_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef NBT_DEPRECATED_EXPORT
#  define NBT_DEPRECATED_EXPORT NBT_EXPORT NBT_DEPRECATED
#endif

#ifndef NBT_DEPRECATED_NO_EXPORT
#  define NBT_DEPRECATED_NO_EXPORT NBT_NO_EXPORT NBT_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef NBT_NO_DEPRECATED
#    define NBT_NO_DEPRECATED
#  endif
#endif

#endif /* NBT_EXPORT_H */
EXPORT_EOF

# ============================================================================
# libqrencode config.h
# ============================================================================
#
# Generate config.h for libqrencode based on Kconfig options.
#

QRENCODE_CONFIG_OUT := $(GENERATED_INCLUDE)/qrencode_config.h

$(QRENCODE_CONFIG_OUT): $(srctree)/.config FORCE
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)cat > $@ << 'CONFIG_EOF'
/* Generated by Makefile from Kconfig */
#ifndef QRENCODE_CONFIG_H
#define QRENCODE_CONFIG_H

#define HAVE_LIBPNG 1
#define HAVE_PNG_H 1
#define MAJOR_VERSION 4
#define MICRO_VERSION 1
#define MINOR_VERSION 1
#define PACKAGE "qrencode"
#define PACKAGE_BUGREPORT ""
#define PACKAGE_NAME "qrencode"
#define PACKAGE_STRING "qrencode 4.1.1"
#define PACKAGE_TARNAME "qrencode"
#define PACKAGE_URL ""
#define PACKAGE_VERSION "4.1.1"
#define STDC_HEADERS 1
#define VERSION "4.1.1"
$(if $(CONFIG_LIBQRENCODE_THREAD_SAFETY),#define HAVE_PTHREAD 1,/* HAVE_PTHREAD disabled */)

#endif /* QRENCODE_CONFIG_H */
CONFIG_EOF

# ============================================================================
# gamemode build-config.h
# ============================================================================

GAMEMODE_CONFIG_IN := $(srctree)/gamemode/build-config.h.in
GAMEMODE_CONFIG_OUT := $(GENERATED_INCLUDE)/gamemode-config.h

$(GAMEMODE_CONFIG_OUT): $(GAMEMODE_CONFIG_IN) $(srctree)/.config FORCE
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)sed \
		-e 's|@GAMEMODE_VERSION@|$(CONFIG_GAMEMODE_VERSION)|g' \
		-e 's|#mesondefine .*||g' \
		$(GAMEMODE_CONFIG_IN) > $@

# ============================================================================
# cmark config.h and export.h
# ============================================================================

CMARK_CONFIG_OUT := $(GENERATED_INCLUDE)/cmark_config.h
CMARK_EXPORT_OUT := $(GENERATED_INCLUDE)/cmark_export.h
CMARK_VERSION_OUT := $(GENERATED_INCLUDE)/cmark_version.h

$(CMARK_CONFIG_OUT): FORCE
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)cat > $@ << 'CMARK_CONFIG_EOF'
#ifndef CMARK_CONFIG_H
#define CMARK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_STDBOOL_H 1
#define HAVE___BUILTIN_EXPECT 1
#define HAVE___ATTRIBUTE__ 1

#ifdef __cplusplus
}
#endif

#endif /* CMARK_CONFIG_H */
CMARK_CONFIG_EOF

$(CMARK_EXPORT_OUT): FORCE
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)cat > $@ << 'CMARK_EXPORT_EOF'
#ifndef CMARK_EXPORT_H
#define CMARK_EXPORT_H

#ifdef CMARK_STATIC_DEFINE
#  define CMARK_EXPORT
#  define CMARK_NO_EXPORT
#else
#  ifndef CMARK_EXPORT
#    define CMARK_EXPORT __attribute__((visibility("default")))
#  endif
#  ifndef CMARK_NO_EXPORT
#    define CMARK_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#endif /* CMARK_EXPORT_H */
CMARK_EXPORT_EOF

$(CMARK_VERSION_OUT): FORCE
	@mkdir -p $(dir $@)
	@echo "  GEN     $@"
	$(Q)cat > $@ << 'CMARK_VERSION_EOF'
#ifndef CMARK_VERSION_H
#define CMARK_VERSION_H

#define CMARK_VERSION ((0 << 24) | (31 << 16) | (1 << 8) | 0)
#define CMARK_VERSION_STRING "0.31.1"

#endif /* CMARK_VERSION_H */
CMARK_VERSION_EOF

# ============================================================================
# All generated files
# ============================================================================

GENERATED_FILES := \
	$(BUILDCONFIG_OUT) \
	$(LAUNCHER_SCRIPT_OUT) \
	$(PNGLIBCONF_OUT) \
	$(NBT_EXPORT_OUT) \
	$(QRENCODE_CONFIG_OUT) \
	$(CMARK_CONFIG_OUT) \
	$(CMARK_EXPORT_OUT) \
	$(CMARK_VERSION_OUT)

ifeq ($(CONFIG_LIB_GAMEMODE),y)
GENERATED_FILES += $(GAMEMODE_CONFIG_OUT)
endif

# ============================================================================
# Configure target
# ============================================================================

.PHONY: configure
configure: $(GENERATED_FILES)
	@echo "Configuration files generated."

.PHONY: configure-clean
configure-clean:
	$(Q)rm -rf $(GENERATED_DIR)
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
