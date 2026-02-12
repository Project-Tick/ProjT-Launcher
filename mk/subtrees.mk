# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Subtree Wrapper System
#
# This file provides build rules for git subtrees without modifying them.
# Each subtree is built using our Makefile rules while keeping source intact.
#
# SUBTREES (from .github/subtrees.txt):
#   zlib, tomlplusplus, json, qt/*
# DO NOT modify files in these directories!

include $(srctree)/mk/config.mk

# Quiet/Verbose
ifeq ($(V),1)
Q :=
else
Q := @
endif

# Speed optimizations
MAKEFLAGS += -r -R

# Base compiler flags (with -pipe for speed)
CFLAGS ?= -O2 -g -fPIC -Wall -pipe
CXXFLAGS ?= $(CFLAGS) -std=c++17

# ============================================================================
# Zlib Subtree Wrapper
# ============================================================================

ifneq ($(wildcard $(srctree)/zlib/adler32.c),)
ZLIB_DIR := $(srctree)/zlib
else ifneq ($(wildcard $(srctree)/toolchain/gcc/zlib/adler32.c),)
ZLIB_DIR := $(srctree)/toolchain/gcc/zlib
else
$(error zlib sources not found at '$(srctree)/zlib' or '$(srctree)/toolchain/gcc/zlib')
endif
ZLIB_OBJDIR := $(OBJDIR)/zlib
ZLIB_STATIC_LIB := $(LIBDIR)/libz$(LIB_EXT)

# On Windows+MinGW, prefer cwd-relative paths. This avoids /d vs D:/ path
# translation mismatches between shells/toolchains on CI.
ifeq ($(TARGET_OS),windows)
ifeq ($(WINDOWS_TOOLCHAIN),mingw)
ifneq ($(wildcard zlib/adler32.c),)
ZLIB_DIR_REL := zlib
else ifneq ($(wildcard toolchain/gcc/zlib/adler32.c),)
ZLIB_DIR_REL := toolchain/gcc/zlib
else
ZLIB_DIR_REL := $(ZLIB_DIR)
endif
ZLIB_OBJDIR_REL := $(patsubst $(srctree)/%,%,$(ZLIB_OBJDIR))
ifeq ($(ZLIB_OBJDIR_REL),$(ZLIB_OBJDIR))
ZLIB_OBJDIR_REL := build/obj/zlib
endif
else
ZLIB_DIR_REL := $(ZLIB_DIR)
ZLIB_OBJDIR_REL := $(ZLIB_OBJDIR)
endif
else
ZLIB_DIR_REL := $(ZLIB_DIR)
ZLIB_OBJDIR_REL := $(ZLIB_OBJDIR)
endif

ifeq ($(WINDOWS_TOOLCHAIN),msvc)
ZLIB_INCLUDES := /I$(ZLIB_DIR_REL)
ZLIB_DEFINES := /DHAVE_SYS_TYPES_H /DHAVE_STDINT_H /DHAVE_STDDEF_H /DZLIB_DLL
ZLIB_COMPILE_C = $(CC) $(ZLIB_CFLAGS) $(ZLIB_INCLUDES) /c /Fo$@ $<
ZLIB_AR = $(AR) /nologo /OUT:$@ $^
else
ZLIB_INCLUDES := -I$(ZLIB_DIR_REL)
ZLIB_DEFINES := -DHAVE_SYS_TYPES_H -DHAVE_STDINT_H -DHAVE_STDDEF_H
ifeq ($(TARGET_OS),windows)
    ZLIB_DEFINES += -DZLIB_DLL
else
    ZLIB_DEFINES += -DHAVE_UNISTD_H
endif
ZLIB_COMPILE_C = $(CC) $(ZLIB_CFLAGS) $(ZLIB_INCLUDES) -c -o $@ $<
ZLIB_AR = $(AR) rcs $@ $^
ZLIB_RANLIB = $(RANLIB) $@ 2>/dev/null || true
endif

ZLIB_SOURCES := \
    adler32.c \
    compress.c \
    crc32.c \
    deflate.c \
    gzclose.c \
    gzlib.c \
    gzread.c \
    gzwrite.c \
    infback.c \
    inffast.c \
    inflate.c \
    inftrees.c \
    trees.c \
    uncompr.c \
    zutil.c

ZLIB_OBJS := $(addprefix $(ZLIB_OBJDIR_REL)/,$(ZLIB_SOURCES:.c=$(OBJ_EXT)))

# Zlib build flags (minimal, library doesn't need many flags)
ZLIB_CFLAGS := $(CFLAGS) $(ZLIB_DEFINES)

$(ZLIB_OBJDIR_REL)/%$(OBJ_EXT): $(ZLIB_DIR_REL)/%.c | $(ZLIB_OBJDIR_REL)
	@mkdir -p $(@D)
	$(Q)$(ZLIB_COMPILE_C)

$(ZLIB_STATIC_LIB): $(ZLIB_OBJS)
	@mkdir -p $(@D)
	$(Q)$(ZLIB_AR)
ifneq ($(WINDOWS_TOOLCHAIN),msvc)
ifneq ($(strip $(RANLIB)),)
	$(Q)$(ZLIB_RANLIB)
endif
endif

# Shared library version of zlib (for tests to avoid symbol conflicts with Qt)
ifneq ($(WINDOWS_TOOLCHAIN),msvc)
ZLIB_SHARED_OBJS := $(addprefix $(ZLIB_OBJDIR_REL)/shared/,$(ZLIB_SOURCES:.c=.o))

ifeq ($(TARGET_PLATFORM),macos)
ZLIB_SHARED_LIB := $(LIBDIR)/libprojtZ.dylib
else
ZLIB_SHARED_LIB := $(LIBDIR)/libprojtZ.so.1
endif

$(ZLIB_OBJDIR_REL)/shared/%.o: $(ZLIB_DIR_REL)/%.c
	@mkdir -p $(@D)
	$(Q)$(CC) $(ZLIB_CFLAGS) -fPIC $(ZLIB_INCLUDES) -c -o $@ $<

$(ZLIB_SHARED_LIB): $(ZLIB_SHARED_OBJS)
	@mkdir -p $(@D)
ifeq ($(TARGET_PLATFORM),macos)
	$(Q)$(CC) -dynamiclib -Wl,-install_name,@rpath/libprojtZ.dylib -o $@ $^
else
	$(Q)$(CC) -shared -Wl,-soname,libprojtZ.so.1 -o $@ $^
	$(Q)ln -sf libprojtZ.so.1 $(LIBDIR)/libprojtZ.so
endif
endif

$(ZLIB_OBJDIR_REL):
	@mkdir -p $@

zlib: $(ZLIB_STATIC_LIB)

ifneq ($(WINDOWS_TOOLCHAIN),msvc)
zlib-shared: $(ZLIB_SHARED_LIB)
else
zlib-shared:
	@echo "  SKIP    zlib-shared (MSVC)"
endif

zlib-clean:
	$(Q)rm -rf $(ZLIB_OBJDIR) $(ZLIB_STATIC_LIB) $(LIBDIR)/libprojtZ.so* $(LIBDIR)/libprojtZ*.dylib

.PHONY: zlib zlib-shared zlib-clean

# ============================================================================
# tomlplusplus Subtree Wrapper (Header-Only, mostly config)
# ============================================================================

TOMLPP_DIR := $(srctree)/tomlplusplus
TOMLPP_INCLUDES := -I$(TOMLPP_DIR)/include

# tomlplusplus is header-only, but we create a stamp file for dependency tracking
$(KBUILD_OUTPUT)/.tomlplusplus-stamp: $(wildcard $(TOMLPP_DIR)/include/toml++/*.hpp)
	@mkdir -p $(@D)
	@touch $@

tomlplusplus: $(KBUILD_OUTPUT)/.tomlplusplus-stamp

.PHONY: tomlplusplus

# ============================================================================
# JSON Subtree Wrapper (Header-Only)
# ============================================================================

JSON_DIR := $(srctree)/json
JSON_INCLUDES := -I$(JSON_DIR)/include

$(KBUILD_OUTPUT)/.json-stamp: $(wildcard $(JSON_DIR)/include/nlohmann/*.hpp)
	@mkdir -p $(@D)
	@touch $@

json: $(KBUILD_OUTPUT)/.json-stamp

.PHONY: json

# ============================================================================
# Qt Subtree Wrapper (Complex - Needs Special Handling)
# ============================================================================

# Qt is very complex and has its own build system (qmake/cmake)
# We provide a wrapper that:
# 1. Configures Qt if not already done
# 2. Builds Qt using its own system
# 3. Provides variables for our build to use

QT_DIR := $(srctree)/qt
QT_BUILD_DIR := $(KBUILD_OUTPUT)/qt-build
QT_PREFIX := $(KBUILD_OUTPUT)/qt-install

# Check if we should use bundled Qt
USE_BUNDLED_QT := $(call cfg-yes,$(CONFIG_QT_BUNDLED))

ifeq ($(USE_BUNDLED_QT),y)

# Qt modules we need (from Kconfig)
QT_MODULES := qtbase

ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTNETWORKAUTH)),y)
    QT_MODULES += qtnetworkauth
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTIMAGEFORMATS)),y)
    QT_MODULES += qtimageformats
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTPOSITIONING)),y)
    QT_MODULES += qtpositioning
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTSERIALPORT)),y)
    QT_MODULES += qtserialport
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTWEBCHANNEL)),y)
    QT_MODULES += qtwebchannel
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTDECLARATIVE)),y)
    QT_MODULES += qtdeclarative
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_MODULE_QTWEBENGINE)),y)
    # QtWebEngine requires qtdeclarative (Qt Quick/QML)
    QT_MODULES += qtdeclarative qtwebengine
endif
ifeq ($(call cfg-yes,$(CONFIG_QT_PLATFORM_WAYLAND)),y)
    QT_MODULES += qtwayland
endif

# Qt base configuration
QT_CONFIGURE_FLAGS := \
    -prefix $(QT_PREFIX) \
    -opensource \
    -confirm-license \
    -nomake examples \
    -nomake tests

# Build type
ifeq ($(call cfg-yes,$(CONFIG_QT_DEBUG_BUILD)),y)
    QT_CONFIGURE_FLAGS += -debug
else
    QT_CONFIGURE_FLAGS += -release
endif

# Static/Shared
ifeq ($(call cfg-yes,$(CONFIG_QT_STATIC)),y)
    QT_CONFIGURE_FLAGS += -static
else
    QT_CONFIGURE_FLAGS += -shared
endif

# Features from Kconfig
ifneq ($(call cfg-yes,$(CONFIG_QT_FEATURE_SQL)),y)
    QT_CONFIGURE_FLAGS += -no-feature-sql
endif

ifeq ($(call cfg-yes,$(CONFIG_QT_FEATURE_OPENGL)),y)
    QT_CONFIGURE_FLAGS += -opengl desktop
endif

ifeq ($(call cfg-yes,$(CONFIG_QT_FEATURE_OPENSSL)),y)
    QT_CONFIGURE_FLAGS += -openssl-linked
endif

ifeq ($(call cfg-yes,$(CONFIG_QT_FEATURE_ICU)),y)
    QT_CONFIGURE_FLAGS += -icu
else
    QT_CONFIGURE_FLAGS += -no-icu
endif

ifeq ($(call cfg-yes,$(CONFIG_QT_FEATURE_ZSTD)),y)
    QT_CONFIGURE_FLAGS += -zstd
endif

ifneq ($(call cfg-yes,$(CONFIG_QT_FEATURE_DBUS)),y)
    QT_CONFIGURE_FLAGS += -no-dbus
endif

# Platform-specific configuration
ifeq ($(TARGET_OS),linux)
    # X11/XCB support
    ifeq ($(call cfg-yes,$(CONFIG_QT_PLATFORM_XCB)),y)
        QT_CONFIGURE_FLAGS += -xcb -xcb-xlib -bundled-xcb-xinput
    else
        QT_CONFIGURE_FLAGS += -no-xcb
    endif
    
    # Wayland support
    ifeq ($(call cfg-yes,$(CONFIG_QT_PLATFORM_WAYLAND)),y)
        QT_CONFIGURE_FLAGS += -feature-wayland-client
    endif
    
    # EGLFS support
    ifeq ($(call cfg-yes,$(CONFIG_QT_PLATFORM_EGLFS)),y)
        QT_CONFIGURE_FLAGS += -eglfs
    else
        QT_CONFIGURE_FLAGS += -no-eglfs
    endif
    
    # Linux Framebuffer
    ifeq ($(call cfg-yes,$(CONFIG_QT_PLATFORM_LINUXFB)),y)
        QT_CONFIGURE_FLAGS += -linuxfb
    else
        QT_CONFIGURE_FLAGS += -no-linuxfb
    endif
    
else ifeq ($(TARGET_OS),windows)
    QT_CONFIGURE_FLAGS += -platform win32-g++
else ifeq ($(TARGET_OS),macos)
    QT_CONFIGURE_FLAGS += -platform macx-clang
endif

# Configure Qt
$(QT_BUILD_DIR)/.configured: | $(QT_BUILD_DIR)
	@echo "Configuring Qt..."
	cd $(QT_BUILD_DIR) && $(QT_DIR)/qtbase/configure $(QT_CONFIGURE_FLAGS)
	@touch $@

# Build Qt
$(QT_PREFIX)/.built: $(QT_BUILD_DIR)/.configured
	@echo "Building Qt..."
	$(MAKE) -C $(QT_BUILD_DIR) -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(QT_BUILD_DIR) install
	@touch $@

$(QT_BUILD_DIR):
	@mkdir -p $@

qt-bundled: $(QT_PREFIX)/.built

qt-bundled-clean:
	$(Q)rm -rf $(QT_BUILD_DIR) $(QT_PREFIX)

.PHONY: qt-bundled qt-bundled-clean

# Set Qt variables for bundled build
QT_CFLAGS := -I$(QT_PREFIX)/include
QT_LIBS := -L$(QT_PREFIX)/lib
QT_MOC := $(QT_PREFIX)/libexec/moc
QT_UIC := $(QT_PREFIX)/libexec/uic
QT_RCC := $(QT_PREFIX)/libexec/rcc

else
# Use system Qt
include mk/qt-tools.mk
endif

# ============================================================================
# Export Subtree Variables
# ============================================================================

export ZLIB_INCLUDES ZLIB_OBJDIR
export TOMLPP_INCLUDES
export JSON_INCLUDES
export QT_CFLAGS QT_LIBS QT_MOC QT_UIC QT_RCC

# ============================================================================
# Subtree Meta-Targets
# ============================================================================

SUBTREE_LIBS :=

ifeq ($(call cfg-yes,$(CONFIG_LIB_ZLIB)),y)
    SUBTREE_LIBS += $(ZLIB_STATIC_LIB)
endif

subtrees: $(SUBTREE_LIBS) tomlplusplus json
	@echo "Subtrees built."

subtrees-clean: zlib-clean
	$(Q)rm -f $(KBUILD_OUTPUT)/.tomlplusplus-stamp $(KBUILD_OUTPUT)/.json-stamp

.PHONY: subtrees subtrees-clean
