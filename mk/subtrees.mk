# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Subtree Wrapper System
#
# This file provides build rules for git subtrees without modifying them.
# Each subtree is built using our Makefile rules while keeping source intact.

include mk/config.mk

# ============================================================================
# Zlib Subtree Wrapper
# ============================================================================

ZLIB_DIR := $(srctree)/zlib
ZLIB_OBJDIR := $(OBJDIR)/zlib
ZLIB_INCLUDES := -I$(ZLIB_DIR)

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

ZLIB_OBJS := $(addprefix $(ZLIB_OBJDIR)/,$(ZLIB_SOURCES:.c=.o))

# Zlib build flags (minimal, library doesn't need many flags)
ZLIB_CFLAGS := $(CFLAGS) -DHAVE_SYS_TYPES_H -DHAVE_STDINT_H -DHAVE_STDDEF_H
ifeq ($(TARGET_OS),windows)
    ZLIB_CFLAGS += -DZLIB_DLL
else
    ZLIB_CFLAGS += -DHAVE_UNISTD_H
endif

$(ZLIB_OBJDIR)/%.o: $(ZLIB_DIR)/%.c | $(ZLIB_OBJDIR)
	@mkdir -p $(@D)
	$(Q)$(CC) $(ZLIB_CFLAGS) $(ZLIB_INCLUDES) -c -o $@ $<

$(LIBDIR)/libz.a: $(ZLIB_OBJS)
	@mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@ 2>/dev/null || true

$(ZLIB_OBJDIR):
	@mkdir -p $@

zlib: $(LIBDIR)/libz.a

zlib-clean:
	$(Q)rm -rf $(ZLIB_OBJDIR) $(LIBDIR)/libz.a

.PHONY: zlib zlib-clean

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
USE_BUNDLED_QT := $(call cfg-yes,$(CONFIG_USE_BUNDLED_QT))

ifeq ($(USE_BUNDLED_QT),y)

# Qt modules we need
QT_MODULES := qtbase

ifeq ($(call cfg-yes,$(CONFIG_LAUNCHER_WITH_WEBENGINE)),y)
    QT_MODULES += qtwebengine qtwebchannel qtpositioning
endif

# Qt configuration
QT_CONFIGURE_FLAGS := \
    -prefix $(QT_PREFIX) \
    -release \
    -opensource \
    -confirm-license \
    -nomake examples \
    -nomake tests \
    -no-feature-sql \
    -static

ifeq ($(TARGET_OS),linux)
    QT_CONFIGURE_FLAGS += -xcb -opengl desktop
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

ifeq ($(call cfg-yes,$(CONFIG_MOD_ZLIB)),y)
    SUBTREE_LIBS += $(LIBDIR)/libz.a
endif

subtrees: $(SUBTREE_LIBS) tomlplusplus json
	@echo "Subtrees built."

subtrees-clean: zlib-clean
	$(Q)rm -f $(KBUILD_OUTPUT)/.tomlplusplus-stamp $(KBUILD_OUTPUT)/.json-stamp

.PHONY: subtrees subtrees-clean
