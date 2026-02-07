# SPDX-License-Identifier: GPL-2.0
#
# ProjT Launcher - Main Makefile
#
# This is a build system for the ProjT Launcher project.
# It replaces CMake with pure Makefile + Kconfig.
#
# Usage:
#   make defconfig     - Load default configuration
#   make menuconfig    - Interactive configuration
#   make              - Build the project
#   make install      - Install to PREFIX
#   make clean        - Clean build output
#   make help         - Show all targets
#
# Cross-compilation:
#   make CROSS_COMPILE=x86_64-w64-mingw32- defconfig
#   make -j$(nproc)
#
# Out-of-tree build:
#   make O=/path/to/build defconfig
#   make O=/path/to/build -j$(nproc)

# ============================================================================
# VERSION AND BRANDING
# ============================================================================
# These values are extracted from CMakeLists.txt / program_info/CMakeLists.txt

VERSION = 0
PATCHLEVEL = 0
SUBLEVEL = 5
EXTRAVERSION = -1
NAME = ProjT Launcher

# Branding (from program_info/CMakeLists.txt)
LAUNCHER_COMMONNAME      = ProjTLauncher
LAUNCHER_NAME            = ProjTLauncher
LAUNCHER_DISPLAYNAME     = ProjT Launcher
LAUNCHER_COPYRIGHT       = © 2025-2026 Project Tick
LAUNCHER_DOMAIN          = projecttick.org
LAUNCHER_CONFIGFILE      = projtlauncher.cfg
LAUNCHER_GIT             = https://github.com/Project-Tick/ProjT-Launcher
LAUNCHER_APPID           = org.projecttick.ProjTLauncher
LAUNCHER_SVGFILENAME     = org.projecttick.ProjTLauncher.svg
LAUNCHER_USERAGENT       = ProjTLauncher/$(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
LAUNCHER_GITHUB_REPO     = Project-Tick/ProjT-Launcher
LAUNCHER_BUG_TRACKER_URL = https://github.com/Project-Tick/ProjT-Launcher/issues

# URLs (from CMakeLists.txt)
LAUNCHER_NEWS_RSS_URL    = https://projecttick.org/product/projt-launcher/feed.xml
LAUNCHER_META_URL        = https://meta.projecttick.org/
LAUNCHER_DISCORD_URL     = https://projecttick.org/projtlauncher/discord

# Build artifact identifier (for update system)
BUILD_ARTIFACT =

# Export all branding variables
export VERSION PATCHLEVEL SUBLEVEL EXTRAVERSION NAME
export LAUNCHER_COMMONNAME LAUNCHER_NAME LAUNCHER_DISPLAYNAME
export LAUNCHER_COPYRIGHT LAUNCHER_DOMAIN LAUNCHER_CONFIGFILE
export LAUNCHER_GIT LAUNCHER_APPID LAUNCHER_SVGFILENAME
export LAUNCHER_USERAGENT LAUNCHER_GITHUB_REPO BUILD_ARTIFACT
export LAUNCHER_BUG_TRACKER_URL LAUNCHER_NEWS_RSS_URL LAUNCHER_META_URL LAUNCHER_DISCORD_URL

# Computed version strings (matches CMake Launcher_VERSION_NAME)
PROJT_VERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
PROJT_VERSION_FULL = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)
export PROJT_VERSION PROJT_VERSION_FULL

# Force bash for recipe execution (PIPESTATUS, pipefail etc.)
# Use bash from PATH on Windows (MSYS2/Git Bash), /bin/bash on Unix
ifeq ($(OS),Windows_NT)
SHELL := bash
else
SHELL := /bin/bash
endif
.SHELLFLAGS := -c

# Prevent make from trying to remake this file
.PHONY: _all
_all: all

# ============================================================================
# Core Variables
# ============================================================================

# Source tree root
srctree := $(CURDIR)
export srctree

# Output directory (Kconfig-style O=)
O ?= build
KBUILD_OUTPUT := $(abspath $(O))
export KBUILD_OUTPUT

# Kbuild-style defaults
SRCARCH ?= projt
KBUILD_DEFCONFIG ?= projt_defconfig
export SRCARCH KBUILD_DEFCONFIG

# ============================================================================
# TARGET PLATFORM DETECTION
# ============================================================================

# Host OS detection
HOST_OS := $(shell uname -s 2>/dev/null || echo Windows)
ifeq ($(HOST_OS),Linux)
    HOST_PLATFORM := linux
else ifeq ($(HOST_OS),Darwin)
    HOST_PLATFORM := macos
else ifneq (,$(findstring MINGW,$(HOST_OS)))
    HOST_PLATFORM := windows
else ifneq (,$(findstring MSYS,$(HOST_OS)))
    HOST_PLATFORM := windows
else ifneq (,$(findstring CYGWIN,$(HOST_OS)))
    HOST_PLATFORM := windows
else ifeq ($(OS),Windows_NT)
    HOST_PLATFORM := windows
else
    HOST_PLATFORM := unknown
endif

# Target platform (can be overridden for cross-compilation)
# Values: linux, windows, macos
TARGET_PLATFORM ?= $(HOST_PLATFORM)

# Target architecture detection
HOST_ARCH := $(shell uname -m 2>/dev/null || echo x86_64)
ifeq ($(HOST_ARCH),x86_64)
    HOST_ARCH := x86_64
else ifeq ($(HOST_ARCH),amd64)
    HOST_ARCH := x86_64
else ifeq ($(HOST_ARCH),aarch64)
    HOST_ARCH := aarch64
else ifeq ($(HOST_ARCH),arm64)
    HOST_ARCH := aarch64
else ifeq ($(HOST_ARCH),i686)
    HOST_ARCH := i686
else ifeq ($(HOST_ARCH),i386)
    HOST_ARCH := i686
endif

TARGET_ARCH ?= $(HOST_ARCH)

export HOST_OS HOST_PLATFORM HOST_ARCH
export TARGET_PLATFORM TARGET_ARCH

# ============================================================================
# CROSS-COMPILATION SUPPORT
# ============================================================================

# Cross-compile prefix (e.g., x86_64-w64-mingw32-, aarch64-linux-gnu-)
CROSS_COMPILE ?=

# Windows toolchain selection: mingw or msvc
# Only relevant when TARGET_PLATFORM=windows
WINDOWS_TOOLCHAIN ?= mingw

# Detect if we're cross-compiling
ifneq ($(CROSS_COMPILE),)
    CROSS_COMPILING := 1
else ifeq ($(TARGET_PLATFORM),windows)
    ifneq ($(HOST_PLATFORM),windows)
        CROSS_COMPILING := 1
    endif
else ifneq ($(TARGET_PLATFORM),$(HOST_PLATFORM))
    CROSS_COMPILING := 1
endif

export CROSS_COMPILE WINDOWS_TOOLCHAIN CROSS_COMPILING

# ============================================================================
# TOOLCHAIN SELECTION
# ============================================================================

ifeq ($(TARGET_PLATFORM),windows)
  ifeq ($(WINDOWS_TOOLCHAIN),msvc)
    # Microsoft Visual C++ (Windows native or Wine)
    CC      = cl.exe
    CXX     = cl.exe
    LD      = link.exe
    AR      = lib.exe
    AS      = ml64.exe
    OBJCOPY = 
    STRIP   = 
    RC      = rc.exe
    MT      = mt.exe
    
    # MSVC-specific flags
    CFLAGS_BASE   = /nologo /W3 /EHsc /MD
    CXXFLAGS_BASE = /nologo /W3 /EHsc /MD /std:c++17
    LDFLAGS_BASE  = /nologo
    
    # Debug/Release
    ifeq ($(CONFIG_DEBUG),y)
        CFLAGS_BASE   += /Zi /Od /DDEBUG /D_DEBUG
        CXXFLAGS_BASE += /Zi /Od /DDEBUG /D_DEBUG
        LDFLAGS_BASE  += /DEBUG
    else
        CFLAGS_BASE   += /O2 /DNDEBUG
        CXXFLAGS_BASE += /O2 /DNDEBUG
        LDFLAGS_BASE  += /RELEASE
    endif
    
    # Output file extensions
    OBJ_EXT = .obj
    LIB_EXT = .lib
    DLL_EXT = .dll
    EXE_EXT = .exe
    
  else
    # MinGW-w64 (Cross-compile from Linux/macOS or native MSYS2)
    CC      = $(CROSS_COMPILE)gcc
    CXX     = $(CROSS_COMPILE)g++
    LD      = $(CROSS_COMPILE)g++
    AR      = $(CROSS_COMPILE)ar
    AS      = $(CROSS_COMPILE)as
    OBJCOPY = $(CROSS_COMPILE)objcopy
    STRIP   = $(CROSS_COMPILE)strip
    WINDRES = $(CROSS_COMPILE)windres
    
    # MinGW flags
    CFLAGS_BASE   = -Wall -Wextra
    CXXFLAGS_BASE = -Wall -Wextra -std=c++17
    LDFLAGS_BASE  = -static-libgcc -static-libstdc++
    
    # Debug/Release
    ifeq ($(CONFIG_DEBUG),y)
        CFLAGS_BASE   += -g -O0 -DDEBUG -D_DEBUG
        CXXFLAGS_BASE += -g -O0 -DDEBUG -D_DEBUG
    else
        CFLAGS_BASE   += -O2 -DNDEBUG
        CXXFLAGS_BASE += -O2 -DNDEBUG
        LDFLAGS_BASE  += -s
    endif
    
    # Output file extensions
    OBJ_EXT = .o
    LIB_EXT = .a
    DLL_EXT = .dll
    EXE_EXT = .exe
  endif
  
  # Windows-specific defines
  CFLAGS_PLATFORM   = -DWIN32 -D_WIN32 -DUNICODE -D_UNICODE
  CXXFLAGS_PLATFORM = -DWIN32 -D_WIN32 -DUNICODE -D_UNICODE
  
else ifeq ($(TARGET_PLATFORM),macos)
    # macOS (Apple Clang)
    CC      = $(CROSS_COMPILE)clang
    CXX     = $(CROSS_COMPILE)clang++
    LD      = $(CROSS_COMPILE)clang++
    AR      = $(CROSS_COMPILE)ar
    AS      = $(CROSS_COMPILE)as
    OBJCOPY = $(CROSS_COMPILE)objcopy
    STRIP   = $(CROSS_COMPILE)strip
    LIPO    = lipo
    INSTALL_NAME_TOOL = install_name_tool
    CODESIGN = codesign
    
    # macOS flags
    CFLAGS_BASE   = -Wall -Wextra
    CXXFLAGS_BASE = -Wall -Wextra -std=c++17
    LDFLAGS_BASE  = 
    
    # Debug/Release
    ifeq ($(CONFIG_DEBUG),y)
        CFLAGS_BASE   += -g -O0 -DDEBUG
        CXXFLAGS_BASE += -g -O0 -DDEBUG
    else
        CFLAGS_BASE   += -O2 -DNDEBUG
        CXXFLAGS_BASE += -O2 -DNDEBUG
    endif
    
    # macOS deployment target
    MACOSX_DEPLOYMENT_TARGET ?= 10.15
    CFLAGS_PLATFORM   = -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
    CXXFLAGS_PLATFORM = -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
    LDFLAGS_PLATFORM  = -mmacosx-version-min=$(MACOSX_DEPLOYMENT_TARGET)
    
    # Output file extensions
    OBJ_EXT = .o
    LIB_EXT = .a
    DLL_EXT = .dylib
    EXE_EXT =
    
else
    # Linux / Unix (GCC or Clang)
    CC      ?= $(CROSS_COMPILE)gcc
    CXX     ?= $(CROSS_COMPILE)g++
    LD      = $(CROSS_COMPILE)g++
    AR      = $(CROSS_COMPILE)ar
    AS      = $(CROSS_COMPILE)as
    OBJCOPY = $(CROSS_COMPILE)objcopy
    STRIP   = $(CROSS_COMPILE)strip
    
    # Linux flags
    CFLAGS_BASE   = -Wall -Wextra -fPIC
    CXXFLAGS_BASE = -Wall -Wextra -fPIC -std=c++17
    LDFLAGS_BASE  = 
    
    # Debug/Release
    ifeq ($(CONFIG_DEBUG),y)
        CFLAGS_BASE   += -g -O0 -DDEBUG
        CXXFLAGS_BASE += -g -O0 -DDEBUG
    else
        CFLAGS_BASE   += -O2 -DNDEBUG
        CXXFLAGS_BASE += -O2 -DNDEBUG
        LDFLAGS_BASE  += -s
    endif
    
    # Linux-specific
    CFLAGS_PLATFORM   = -D_GNU_SOURCE
    CXXFLAGS_PLATFORM = -D_GNU_SOURCE
    LDFLAGS_PLATFORM  = -Wl,--as-needed
    
    # Output file extensions
    OBJ_EXT = .o
    LIB_EXT = .a
    DLL_EXT = .so
    EXE_EXT =
endif

# Combine flags
CFLAGS   = $(CFLAGS_BASE) $(CFLAGS_PLATFORM)
CXXFLAGS = $(CXXFLAGS_BASE) $(CXXFLAGS_PLATFORM)
LDFLAGS  = $(LDFLAGS_BASE) $(LDFLAGS_PLATFORM)

export CC CXX LD AR AS OBJCOPY STRIP
export CFLAGS CXXFLAGS LDFLAGS
export OBJ_EXT LIB_EXT DLL_EXT EXE_EXT

# ============================================================================
# Kconfig Paths
# ============================================================================

KCONFIG_SRCDIR := $(srctree)/kconfig
KCONFIG_OBJDIR := $(KBUILD_OUTPUT)/kconfig
KCONFIG_CONFIG ?= $(KBUILD_OUTPUT)/.config
KCONFIG_AUTOCONFIG ?= $(KBUILD_OUTPUT)/include/config/auto.conf
KCONFIG_AUTOHEADER ?= $(KBUILD_OUTPUT)/include/generated/autoconf.h
KCONFIG_TRISTATE ?= $(KBUILD_OUTPUT)/include/config/tristate.conf
export KCONFIG_CONFIG KCONFIG_AUTOCONFIG KCONFIG_AUTOHEADER KCONFIG_TRISTATE

# Default defconfig
DEFCONFIG ?= defconfig

# ============================================================================
# Verbosity Control
# ============================================================================

V ?= 0
ifeq ($(V),0)
    Q := @
    MAKEFLAGS += --no-print-directory
else
    Q :=
endif
export Q V

kecho := $(if $(Q),@echo,@:)

# ============================================================================
# Host Tools
# ============================================================================

HOSTCC ?= cc
HOSTCXX ?= c++
HOSTLD ?= $(HOSTCC)
HOSTAR ?= ar
HOSTPKG_CONFIG ?= pkg-config
BISON ?= bison
FLEX ?= flex
PERL ?= perl
PYTHON3 ?= python3
INSTALL ?= install
MKDIR ?= mkdir -p
RM ?= rm -f
RMDIR ?= rm -rf
CP ?= cp
LN ?= ln -sf

export HOSTCC HOSTCXX HOSTLD HOSTAR HOSTPKG_CONFIG
export BISON FLEX PERL PYTHON3 INSTALL

HOSTCFLAGS ?= -O2 -g
HOSTCXXFLAGS ?= $(HOSTCFLAGS)
HOSTLDFLAGS ?=

export HOSTCFLAGS HOSTCXXFLAGS HOSTLDFLAGS

# ============================================================================
# Parallel Build & Speed Optimizations
# ============================================================================

# Auto-detect CPU count if not specified
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
JOBS ?= $(NPROC)

# Enable parallel make by default (like ninja)
MAKEFLAGS += -j$(JOBS)

# Disable built-in rules for speed (like ninja)
MAKEFLAGS += -r -R

# ccache - enabled by default if available
CCACHE := $(shell command -v ccache 2>/dev/null)
ifneq ($(CCACHE),)
  ifndef NO_CCACHE
    CC := ccache $(CC)
    CXX := ccache $(CXX)
    HOSTCC := ccache $(HOSTCC)
    HOSTCXX := ccache $(HOSTCXX)
  endif
endif

# sccache support (alternative to ccache)
ifndef CCACHE
  SCCACHE := $(shell command -v sccache 2>/dev/null)
  ifneq ($(SCCACHE),)
    ifndef NO_CCACHE
      CC := sccache $(CC)
      CXX := sccache $(CXX)
    endif
  endif
endif

# Use pipes instead of temp files (faster)
CFLAGS += -pipe
CXXFLAGS += -pipe

# Export job count for sub-makes
export JOBS

# ============================================================================
# Phony Targets Declaration
# ============================================================================

PHONY := _all all build prepare clean distclean help
PHONY += defconfig menuconfig xconfig gconfig nconfig oldconfig
PHONY += savedefconfig syncconfig listnewconfig olddefconfig
PHONY += kconfig-tools projt_defconfig
PHONY += install install-bin install-lib install-data uninstall
PHONY += test check fuzz
PHONY += package package-deb package-rpm package-appimage package-dmg
PHONY += toolchain-gcc toolchain-llvm toolchain-all toolchain-help
PHONY += subtrees info version listconfig

# ============================================================================
# Include Kconfig Build Rules
# ============================================================================

obj := $(KCONFIG_OBJDIR)
src := $(KCONFIG_SRCDIR)
export obj src
KCONFIG_EMBEDDED := 1
export KCONFIG_EMBEDDED

include $(KCONFIG_SRCDIR)/Makefile

# Ensure lexer can find parser.tab.h
HOSTCFLAGS_lexer.lex.o += -I$(obj)

# ============================================================================
# Include Auto-Generated Config
# ============================================================================

-include $(KCONFIG_AUTOCONFIG)

# ============================================================================
# Directory Targets
# ============================================================================

$(KCONFIG_OBJDIR):
	$(Q)$(MKDIR) $@

$(KBUILD_OUTPUT)/include/config $(KBUILD_OUTPUT)/include/generated:
	$(Q)$(MKDIR) $@

# ============================================================================
# Kconfig Tool Build Rules
# ============================================================================

# Parser/lexer generation
$(KCONFIG_OBJDIR)/parser.tab.c $(KCONFIG_OBJDIR)/parser.tab.h: $(KCONFIG_SRCDIR)/parser.y | $(KCONFIG_OBJDIR)
	$(Q)$(BISON) -d -t -o $(KCONFIG_OBJDIR)/parser.tab.c $<

$(KCONFIG_OBJDIR)/lexer.lex.c: $(KCONFIG_SRCDIR)/lexer.l $(KCONFIG_OBJDIR)/parser.tab.h | $(KCONFIG_OBJDIR)
	$(Q)$(FLEX) -o $@ $<

# Host compilation
KCONFIG_INCLUDES := -I$(KCONFIG_SRCDIR) -I$(KCONFIG_OBJDIR) -I$(KCONFIG_SRCDIR)/include

define host-cflags
$(HOSTCFLAGS) $(HOSTCFLAGS_$(patsubst $(KCONFIG_OBJDIR)/%,%,$(1))) $(KCONFIG_INCLUDES)
endef

define host-cxxflags
$(HOSTCXXFLAGS) $(HOSTCXXFLAGS_$(patsubst $(KCONFIG_OBJDIR)/%,%,$(1))) $(KCONFIG_INCLUDES)
endef

$(KCONFIG_OBJDIR)/%.o: $(KCONFIG_SRCDIR)/%.c | $(KCONFIG_OBJDIR)
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(HOSTCC) $(call host-cflags,$@) -c -o $@ $<

$(KCONFIG_OBJDIR)/%.o: $(KCONFIG_OBJDIR)/%.c | $(KCONFIG_OBJDIR)
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(HOSTCC) $(call host-cflags,$@) -c -o $@ $<

$(KCONFIG_OBJDIR)/%.o: $(KCONFIG_SRCDIR)/%.cc | $(KCONFIG_OBJDIR)
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(HOSTCXX) $(call host-cxxflags,$@) -c -o $@ $<

$(KCONFIG_OBJDIR)/%.o: $(KCONFIG_OBJDIR)/%.cc | $(KCONFIG_OBJDIR)
	$(Q)$(MKDIR) $(dir $@)
	$(Q)$(HOSTCXX) $(call host-cxxflags,$@) -c -o $@ $<

# Kconfig configuration scripts (ncurses detection, Qt detection, etc.)
$(KCONFIG_OBJDIR)/mconf-cflags $(KCONFIG_OBJDIR)/mconf-libs: $(KCONFIG_SRCDIR)/mconf-cfg.sh | $(KCONFIG_OBJDIR)
	$(Q)cd $(KCONFIG_OBJDIR) && $(KCONFIG_SRCDIR)/mconf-cfg.sh mconf-cflags mconf-libs

$(KCONFIG_OBJDIR)/nconf-cflags $(KCONFIG_OBJDIR)/nconf-libs: $(KCONFIG_SRCDIR)/nconf-cfg.sh | $(KCONFIG_OBJDIR)
	$(Q)cd $(KCONFIG_OBJDIR) && $(KCONFIG_SRCDIR)/nconf-cfg.sh nconf-cflags nconf-libs

$(KCONFIG_OBJDIR)/qconf-cflags $(KCONFIG_OBJDIR)/qconf-libs $(KCONFIG_OBJDIR)/qconf-bin: $(KCONFIG_SRCDIR)/qconf-cfg.sh | $(KCONFIG_OBJDIR)
	$(Q)cd $(KCONFIG_OBJDIR) && $(KCONFIG_SRCDIR)/qconf-cfg.sh qconf-cflags qconf-libs qconf-bin

$(KCONFIG_OBJDIR)/gconf-cflags $(KCONFIG_OBJDIR)/gconf-libs: $(KCONFIG_SRCDIR)/gconf-cfg.sh | $(KCONFIG_OBJDIR)
	$(Q)cd $(KCONFIG_OBJDIR) && $(KCONFIG_SRCDIR)/gconf-cfg.sh gconf-cflags gconf-libs

# Helper function to read config files
read-file = $(shell cat $(1) 2>/dev/null)

# Ncurses flags for menuconfig
HOSTLDLIBS_mconf = $(call read-file,$(KCONFIG_OBJDIR)/mconf-libs)
HOSTCFLAGS_mconf.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)
HOSTCFLAGS_lxdialog/checklist.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)
HOSTCFLAGS_lxdialog/inputbox.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)
HOSTCFLAGS_lxdialog/menubox.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)
HOSTCFLAGS_lxdialog/textbox.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)
HOSTCFLAGS_lxdialog/util.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)
HOSTCFLAGS_lxdialog/yesno.o = $(call read-file,$(KCONFIG_OBJDIR)/mconf-cflags)

# Ncurses flags for nconfig
HOSTLDLIBS_nconf = $(call read-file,$(KCONFIG_OBJDIR)/nconf-libs)
HOSTCFLAGS_nconf.o = $(call read-file,$(KCONFIG_OBJDIR)/nconf-cflags)
HOSTCFLAGS_nconf.gui.o = $(call read-file,$(KCONFIG_OBJDIR)/nconf-cflags)

# Link Kconfig tools
KCONFIG_PROGS := $(addprefix $(KCONFIG_OBJDIR)/,$(hostprogs))

$(KCONFIG_PROGS): | $(KCONFIG_OBJDIR)

# mconf depends on ncurses config
$(KCONFIG_OBJDIR)/mconf: | $(KCONFIG_OBJDIR)/mconf-libs
$(addprefix $(KCONFIG_OBJDIR)/,mconf.o $(lxdialog)): | $(KCONFIG_OBJDIR)/mconf-cflags

# nconf depends on ncurses config
$(KCONFIG_OBJDIR)/nconf: | $(KCONFIG_OBJDIR)/nconf-libs
$(addprefix $(KCONFIG_OBJDIR)/,nconf.o nconf.gui.o): | $(KCONFIG_OBJDIR)/nconf-cflags

define hostprog_link
$(KCONFIG_OBJDIR)/$(1): $$($(1)-objs:%=$(KCONFIG_OBJDIR)/%) $$($(1)-cxxobjs:%=$(KCONFIG_OBJDIR)/%)
	$$(Q)$$(if $$($(1)-cxxobjs),$(HOSTCXX),$(HOSTCC)) $(HOSTLDFLAGS) -o $$@ $$^ $$(HOSTLDLIBS_$(1))
endef

$(foreach p,$(hostprogs),$(eval $(call hostprog_link,$(p))))

kconfig-tools: $(KCONFIG_PROGS)

# ============================================================================
# Configuration Targets
# ============================================================================

# prepare: Generate auto.conf and autoconf.h from .config
# Uses lightweight syncconfig.sh instead of building kconfig tools.
# kconfig tools (menuconfig, defconfig, etc.) are only needed for
# interactive configuration and are built on-demand via their own targets.
prepare: | $(KBUILD_OUTPUT)/include/config $(KBUILD_OUTPUT)/include/generated
	@if [ ! -f "$(KCONFIG_CONFIG)" ]; then \
		echo "error: $(KCONFIG_CONFIG) not found."; \
		echo "Run ./configure (Unix) or configure.bat (Windows) first."; \
		exit 1; \
	fi
	@bash $(srctree)/scripts/syncconfig.sh $(KBUILD_OUTPUT)

# Override defconfig to use our custom defconfig file
projt_defconfig: $(KCONFIG_OBJDIR)/conf
	$(Q)$(KCONFIG_OBJDIR)/conf --defconfig=$(srctree)/$(DEFCONFIG) Kconfig

# Note: menuconfig, oldconfig, savedefconfig, etc. are provided by kconfig/Makefile

# ============================================================================
# Main Build Target (Recursive Only)
# ============================================================================

all: build

build: prepare
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk build

# Individual module builds
libs launcher java tests configure qt-build java-modules launcher-all:
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk $@

# Module-specific targets (e.g., make zlib, make launcher/ui)
%/:
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk $(patsubst %/,%,$@)

# ============================================================================
# Subtrees Target (delegated to targets.mk for proper sequencing)
# ============================================================================

subtrees: prepare
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk subtrees

# ============================================================================
# Toolchain Targets (Wrapper files in mk/)
# ============================================================================

# GCC toolchain (uses subtree if available, otherwise downloads)
toolchain-gcc:
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-gcc.mk \
		srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) all

# LLVM toolchain (uses subtree if available, otherwise downloads)
toolchain-llvm:
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-llvm.mk \
		srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) all

toolchain-all: toolchain-gcc toolchain-llvm

toolchain-clean:
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-gcc.mk clean 2>/dev/null || true
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-llvm.mk clean 2>/dev/null || true

toolchain-distclean:
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-gcc.mk distclean 2>/dev/null || true
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-llvm.mk distclean 2>/dev/null || true

toolchain-info:
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-gcc.mk info
	@echo ""
	$(Q)$(MAKE) -f $(srctree)/mk/toolchain-llvm.mk info

toolchain-help:
	@echo "Toolchain Targets"
	@echo "================="
	@echo ""
	@echo "  toolchain-gcc      - Build GCC (subtree or download)"
	@echo "  toolchain-llvm     - Build LLVM/Clang (subtree or download)"
	@echo "  toolchain-all      - Build both"
	@echo "  toolchain-clean    - Clean toolchain builds"
	@echo "  toolchain-distclean - Remove all toolchain files"
	@echo "  toolchain-info     - Show toolchain configuration"
	@echo ""
	@echo "Variables:"
	@echo "  GCC_VERSION=14.2.0   - GCC version to build"
	@echo "  LLVM_VERSION=18.1.8  - LLVM version to build"
	@echo "  TARGET=<triple>      - Cross-compilation target"
	@echo ""
	@echo "Subtree commands (keeps toolchain/ empty until you add):"
	@echo "  git subtree add --prefix=toolchain/gcc https://gcc.gnu.org/git/gcc.git releases/gcc-14 --squash"
	@echo "  git subtree add --prefix=toolchain/llvm https://github.com/llvm/llvm-project.git llvmorg-18.1.0 --squash"

# ============================================================================
# Installation Targets
# ============================================================================

PREFIX ?= /usr/local
DESTDIR ?=
BINDIR ?= $(PREFIX)/bin
LIBDIR ?= $(PREFIX)/lib
DATADIR ?= $(PREFIX)/share
DOCDIR ?= $(DATADIR)/doc/$(PROJECT_NAME)
MANDIR ?= $(DATADIR)/man

install: install-bin install-data
	@echo "Installed to $(DESTDIR)$(PREFIX)"

install-bin: build
	$(Q)$(INSTALL) -d $(DESTDIR)$(BINDIR)
	$(Q)$(INSTALL) -m 755 $(KBUILD_OUTPUT)/bin/projtlauncher$(EXE_SUFFIX) $(DESTDIR)$(BINDIR)/

install-data:
	$(Q)$(INSTALL) -d $(DESTDIR)$(DATADIR)/applications
	$(Q)$(INSTALL) -d $(DESTDIR)$(DATADIR)/icons/hicolor/scalable/apps
	$(Q)$(INSTALL) -d $(DESTDIR)$(DOCDIR)

uninstall:
	$(Q)$(RM) $(DESTDIR)$(BINDIR)/projtlauncher$(EXE_SUFFIX)
	$(Q)$(RMDIR) $(DESTDIR)$(DOCDIR)

# ============================================================================
# Testing Targets
# ============================================================================

test check: build
ifeq ($(CONFIG_BUILD_TESTS),y)
	@echo ""
	@echo "=== Running Tests ==="
	$(Q)$(MAKE) -f $(srctree)/mk/tests.mk srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) test
else
	@echo "Tests not enabled. Run 'make menuconfig' and enable BUILD_TESTS."
	@echo "Or run: ./configure --enable-tests"
endif

tests-build: build
ifeq ($(CONFIG_BUILD_TESTS),y)
	$(Q)$(MAKE) -f $(srctree)/mk/tests.mk srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) tests-build
else
	@echo "Tests not enabled."
endif

fuzz:
ifeq ($(CONFIG_BUILD_FUZZERS),y)
	$(Q)$(MAKE) -f $(srctree)/mk/fuzz.mk fuzz
else
	@echo "Fuzzers not enabled. Run 'make menuconfig' and enable BUILD_FUZZERS."
endif

# ============================================================================
# Packaging Targets
# ============================================================================

package: build
	$(Q)$(MAKE) -f $(srctree)/mk/package.mk package

package-deb package-rpm package-appimage package-dmg package-nsis:
	$(Q)$(MAKE) -f $(srctree)/mk/package.mk $@

# ============================================================================
# Utility Targets
# ============================================================================

listconfig: prepare
	@echo "Configuration Summary"
	@echo "====================="
	@echo "Config file: $(KCONFIG_CONFIG)"
	@echo "Build dir:   $(KBUILD_OUTPUT)"
	@echo ""
	@echo "Build Settings:"
	@echo "  BUILD_TYPE:          $(CONFIG_BUILD_TYPE)"
	@echo "  ENABLE_LTO:          $(CONFIG_ENABLE_LTO)"
	@echo ""
	@echo "Target:"
	@echo "  TARGET_OS:           $(TARGET_OS)"
	@echo "  TARGET_ARCH:         $(TARGET_ARCH)"
	@echo ""
	@echo "Toolchain:"
	@echo "  TOOLCHAIN:           $(TOOLCHAIN)"
	@echo "  CC:                  $(CC)"
	@echo "  CXX:                 $(CXX)"
	@echo ""
	@echo "Qt Settings:"
	@echo "  QT_VERSION_MAJOR:    $(CONFIG_QT_VERSION_MAJOR)"
	@echo "  USE_BUNDLED_QT:      $(CONFIG_USE_BUNDLED_QT)"
	@echo "  WITH_WEBENGINE:      $(CONFIG_LAUNCHER_WITH_WEBENGINE)"

info:
	@echo "=== $(NAME) ===" 
	@echo "Version:     $(PROJT_VERSION_FULL)"
	@echo "CommonName:  $(LAUNCHER_COMMONNAME)"
	@echo "DisplayName: $(LAUNCHER_DISPLAYNAME)"
	@echo "AppID:       $(LAUNCHER_APPID)"
	@echo "Domain:      $(LAUNCHER_DOMAIN)"
	@echo "Copyright:   $(LAUNCHER_COPYRIGHT)"
	@echo ""
	@echo "Build Information:"
	@echo "  Source:    $(srctree)"
	@echo "  Output:    $(KBUILD_OUTPUT)"
	@echo "  Platform:  $(TARGET_PLATFORM)"
	@echo "  Arch:      $(TARGET_ARCH)"
	@echo ""
	@echo "Toolchain:"
	@echo "  CC:        $(CC)"
	@echo "  CXX:       $(CXX)"
ifeq ($(TARGET_PLATFORM),windows)
	@echo "  Toolchain: $(WINDOWS_TOOLCHAIN)"
endif
ifdef CROSS_COMPILING
	@echo "  Cross:     yes ($(CROSS_COMPILE))"
endif

# Print version: "make kernelversion"
kernelversion projt-version:
	@echo "$(PROJT_VERSION)"

version:
	@echo "$(PROJT_VERSION_FULL)"

# ============================================================================
# Clean Targets
# ============================================================================

clean:
	$(Q)$(RMDIR) $(KBUILD_OUTPUT)/obj
	$(Q)$(RMDIR) $(KBUILD_OUTPUT)/lib
	$(Q)$(RMDIR) $(KBUILD_OUTPUT)/bin
	$(Q)$(RMDIR) $(KBUILD_OUTPUT)/generated
	$(Q)$(RMDIR) $(KBUILD_OUTPUT)/jars
	@echo "Cleaned build output"

distclean: clean
	$(Q)$(RMDIR) $(KBUILD_OUTPUT)
	@echo "Cleaned everything including configuration"

mrproper: distclean

# ============================================================================
# Help Target
# ============================================================================

help:
	@echo "$(NAME) Build System (v$(PROJT_VERSION_FULL))"
	@echo "==========================================="
	@echo ""
	@echo "Configuration targets:"
	@echo "  defconfig      - Load default configuration"
	@echo "  menuconfig     - Interactive ncurses configuration"
	@echo "  nconfig        - Alternative ncurses configuration"
	@echo "  xconfig        - Qt-based configuration (requires Qt)"
	@echo "  gconfig        - GTK-based configuration (requires GTK)"
	@echo "  oldconfig      - Update config with new options (prompts)"
	@echo "  olddefconfig   - Update config with new options (defaults)"
	@echo "  savedefconfig  - Save minimal config to defconfig"
	@echo "  listnewconfig  - List new config options"
	@echo ""
	@echo "Build targets:"
	@echo "  all            - Build everything (default)"
	@echo "  build          - Build the project"
	@echo "  configure      - Generate headers from .in files"
	@echo "  subtrees       - Build subtree dependencies"
	@echo "  qt-build       - Build Qt (if bundled)"
	@echo "  libs           - Build all libraries"
	@echo "  launcher-all   - Build launcher and submodules"
	@echo "  install        - Install to PREFIX (default: /usr/local)"
	@echo "  uninstall      - Remove installed files"
	@echo ""
	@echo "Testing targets:"
	@echo "  test / check   - Run tests"
	@echo "  fuzz           - Run fuzzers"
	@echo ""
	@echo "Packaging targets:"
	@echo "  package        - Build all configured packages"
	@echo "  package-deb    - Build .deb package"
	@echo "  package-rpm    - Build .rpm package"
	@echo "  package-appimage - Build AppImage"
	@echo "  package-dmg    - Build macOS DMG"
	@echo "  package-nsis   - Build Windows NSIS installer"
	@echo ""
	@echo "Toolchain (subtree or bootstrap):"
	@echo "  toolchain-gcc  - Build GCC (from subtree or download)"
	@echo "  toolchain-llvm - Build LLVM/Clang (from subtree or download)"
	@echo "  toolchain-all  - Build both GCC and LLVM"
	@echo "  toolchain-help - Show toolchain options"
	@echo ""
	@echo "Utility targets:"
	@echo "  info           - Show project and build information"
	@echo "  version        - Show version ($(PROJT_VERSION_FULL))"
	@echo "  listconfig     - Show configuration summary"
	@echo "  list-modules   - List all available modules"
	@echo "  clean          - Remove build artifacts"
	@echo "  distclean      - Remove everything including config"
	@echo "  help           - Show this help"
	@echo ""
	@echo "Build Variables:"
	@echo "  V=1                  - Verbose build output"
	@echo "  O=<dir>              - Out-of-tree build directory"
	@echo "  PREFIX=<path>        - Installation prefix (default: /usr/local)"
	@echo "  DESTDIR=<path>       - Staging directory for packages"
	@echo "  JOBS=<n>             - Parallel jobs (default: $(NPROC))"
	@echo ""
	@echo "Cross-Compilation:"
	@echo "  TARGET_PLATFORM=<os> - Target: linux, windows, macos"
	@echo "  TARGET_ARCH=<arch>   - Target: x86_64, aarch64, i686"
	@echo "  CROSS_COMPILE=<pfx>  - Toolchain prefix (e.g., x86_64-w64-mingw32-)"
	@echo ""
	@echo "Windows-specific:"
	@echo "  WINDOWS_TOOLCHAIN=mingw  - Use MinGW-w64 (default, cross-compile friendly)"
	@echo "  WINDOWS_TOOLCHAIN=msvc   - Use MSVC (Windows native only)"
	@echo ""
	@echo "Examples:"
	@echo "  # Native Linux build"
	@echo "  make defconfig && make -j\$$(nproc)"
	@echo ""
	@echo "  # Cross-compile for Windows (MinGW)"
	@echo "  make TARGET_PLATFORM=windows CROSS_COMPILE=x86_64-w64-mingw32- defconfig"
	@echo "  make -j\$$(nproc)"
	@echo ""
	@echo "  # Windows native build (MSVC, from VS Developer Command Prompt)"
	@echo "  make TARGET_PLATFORM=windows WINDOWS_TOOLCHAIN=msvc defconfig"
	@echo "  make -j\$$(nproc)"
	@echo ""
	@echo "  # macOS cross-compile (requires osxcross)"
	@echo "  make TARGET_PLATFORM=macos CROSS_COMPILE=x86_64-apple-darwin- defconfig"
	@echo "  make -j\$$(nproc)"
	@echo ""
	@echo "  # Out-of-tree build"
	@echo "  make O=../build-release menuconfig && make O=../build-release -j\$$(nproc)"

# ============================================================================
# Module List
# ============================================================================

list-modules:
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk list-modules

# ============================================================================
# CI Targets
# ============================================================================

# CI builds for individual components
PHONY += ci-bzip2 ci-cmark ci-quazip ci-libqrencode ci-javacheck ci-launcher
PHONY += ci-lint ci-prepare ci-release ci-all

# Bzip2 CI build
ci-bzip2: prepare
	@echo "=== CI: Building bzip2 ==="
	$(Q)cd $(srctree)/bzip2 && \
		mkdir -p builddir && cd builddir && \
		cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_SHARED_LIB=ON -DENABLE_STATIC_LIB=ON && \
		cmake --build . -j$(JOBS) && \
		ctest --output-on-failure || true

# CMark CI build
ci-cmark: prepare
	@echo "=== CI: Building cmark ==="
	$(Q)cd $(srctree)/cmark && \
		mkdir -p builddir && cd builddir && \
		cmake .. -DCMAKE_BUILD_TYPE=Release && \
		cmake --build . -j$(JOBS) && \
		ctest --output-on-failure || true

# Quazip CI build
ci-quazip: prepare
	@echo "=== CI: Building quazip ==="
	$(Q)cd $(srctree)/quazip && \
		mkdir -p builddir && cd builddir && \
		cmake .. -DCMAKE_BUILD_TYPE=Release && \
		cmake --build . -j$(JOBS) && \
		ctest --output-on-failure || true

# libqrencode CI build
ci-libqrencode: prepare
	@echo "=== CI: Building libqrencode ==="
	$(Q)cd $(srctree)/libqrencode && \
		mkdir -p builddir && cd builddir && \
		cmake .. -DCMAKE_BUILD_TYPE=Release && \
		cmake --build . -j$(JOBS)

# JavaCheck CI build
ci-javacheck: prepare
	@echo "=== CI: Building javacheck ==="
	$(Q)cd $(srctree)/javacheck && \
		$(MAKE) all

# Launcher CI build (full build)
ci-launcher: prepare
	@echo "=== CI: Building launcher ==="
	$(Q)$(MAKE) -f $(srctree)/Makefile all

# Lint target
ci-lint:
	@echo "=== CI: Running linters ==="
	$(Q)cd $(srctree) && \
		if command -v clang-tidy >/dev/null 2>&1; then \
			echo "Running clang-tidy..."; \
			find launcher -name '*.cpp' -o -name '*.h' | head -20 | xargs clang-tidy 2>/dev/null || true; \
		fi
	$(Q)cd $(srctree) && \
		if command -v cppcheck >/dev/null 2>&1; then \
			echo "Running cppcheck..."; \
			cppcheck --enable=warning,style --quiet launcher/ 2>/dev/null || true; \
		fi

# Prepare target (used by CI workflows)
ci-prepare: prepare
	@echo "=== CI: Prepared build environment ==="
	@echo "Platform: $(TARGET_PLATFORM)"
	@echo "Architecture: $(TARGET_ARCH)"
	@echo "Compiler: $(CXX)"
	@echo "Build directory: $(KBUILD_OUTPUT)"

# Release packaging
ci-release: all
	@echo "=== CI: Creating release artifacts ==="
ifeq ($(TARGET_PLATFORM),linux)
	$(Q)$(MAKE) -f $(srctree)/Makefile package-appimage || true
	$(Q)$(MAKE) -f $(srctree)/Makefile package-tar || true
endif
ifeq ($(TARGET_PLATFORM),macos)
	$(Q)$(MAKE) -f $(srctree)/Makefile package-dmg || true
endif
ifeq ($(TARGET_PLATFORM),windows)
	$(Q)$(MAKE) -f $(srctree)/Makefile package-zip || true
	$(Q)$(MAKE) -f $(srctree)/Makefile package-nsis || true
endif

# All CI targets
ci-all: ci-prepare ci-bzip2 ci-cmark ci-quazip ci-libqrencode ci-javacheck ci-launcher

# ============================================================================
# Forced Target
# ============================================================================

FORCE:

.PHONY: $(PHONY) FORCE libs launcher java tests list-modules
