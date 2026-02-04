# SPDX-License-Identifier: GPL-2.0
#
# ProjT Launcher - Main Makefile
#
# This is a Linux kernel-style build system for the ProjT Launcher project.
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

# Project info
PROJECT_NAME := ProjT-Launcher
PROJECT_VERSION := 0.0.5.1
export PROJECT_NAME PROJECT_VERSION

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
# Parallel Build
# ============================================================================

# Auto-detect CPU count if not specified
NPROC := $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
JOBS ?= $(NPROC)

# ccache support
ifdef USE_CCACHE
    CCACHE := $(shell command -v ccache 2>/dev/null)
    ifdef CCACHE
        CC := ccache $(CC)
        CXX := ccache $(CXX)
    endif
endif

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

# Link Kconfig tools
KCONFIG_PROGS := $(addprefix $(KCONFIG_OBJDIR)/,$(hostprogs))

$(KCONFIG_PROGS): | $(KCONFIG_OBJDIR)

define hostprog_link
$(KCONFIG_OBJDIR)/$(1): $$($(1)-objs:%=$(KCONFIG_OBJDIR)/%) $$($(1)-cxxobjs:%=$(KCONFIG_OBJDIR)/%)
	$$(Q)$$(if $$($(1)-cxxobjs),$(HOSTCXX),$(HOSTCC)) $(HOSTLDFLAGS) -o $$@ $$^ $$(HOSTLDLIBS_$(1))
endef

$(foreach p,$(hostprogs),$(eval $(call hostprog_link,$(p))))

kconfig-tools: $(KCONFIG_PROGS)

# ============================================================================
# Configuration Targets
# ============================================================================

prepare: $(KCONFIG_OBJDIR)/conf | $(KBUILD_OUTPUT)/include/config $(KBUILD_OUTPUT)/include/generated
	@if [ ! -f "$(KCONFIG_CONFIG)" ]; then \
		$(MAKE) -f $(srctree)/Makefile defconfig; \
	fi
	$(Q)$(MAKE) -f $(srctree)/Makefile syncconfig

projt_defconfig: $(KCONFIG_OBJDIR)/conf
	$(Q)$(KCONFIG_OBJDIR)/conf --defconfig=$(srctree)/$(DEFCONFIG) Kconfig

defconfig: projt_defconfig

oldconfig: $(KCONFIG_OBJDIR)/conf
	$(Q)$(KCONFIG_OBJDIR)/conf --oldconfig Kconfig

olddefconfig: $(KCONFIG_OBJDIR)/conf
	$(Q)$(KCONFIG_OBJDIR)/conf --olddefconfig Kconfig

menuconfig: $(KCONFIG_OBJDIR)/mconf
	$(Q)$(KCONFIG_OBJDIR)/mconf Kconfig

nconfig: $(KCONFIG_OBJDIR)/nconf
	$(Q)$(KCONFIG_OBJDIR)/nconf Kconfig

xconfig: $(KCONFIG_OBJDIR)/qconf
	$(Q)$(KCONFIG_OBJDIR)/qconf Kconfig

gconfig: $(KCONFIG_OBJDIR)/gconf
	$(Q)$(KCONFIG_OBJDIR)/gconf Kconfig

savedefconfig: $(KCONFIG_OBJDIR)/conf
	$(Q)$(KCONFIG_OBJDIR)/conf --savedefconfig=$(KBUILD_OUTPUT)/defconfig Kconfig
	@echo "Saved minimal config to $(KBUILD_OUTPUT)/defconfig"

listnewconfig: $(KCONFIG_OBJDIR)/conf
	$(Q)$(KCONFIG_OBJDIR)/conf --listnewconfig Kconfig

# ============================================================================
# Main Build Target (Recursive Only)
# ============================================================================

all: build

build: prepare
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk build

# Individual module builds
libs launcher java tests:
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk $@

# Module-specific targets (e.g., make zlib, make launcher/ui)
%/:
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk $(patsubst %/,%,$@)

# ============================================================================
# Subtrees Target
# ============================================================================

subtrees: prepare
	$(Q)$(MAKE) -f $(srctree)/mk/subtrees.mk subtrees

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
	$(Q)$(INSTALL) -m 755 $(KBUILD_OUTPUT)/bin/projt-launcher$(EXE_SUFFIX) $(DESTDIR)$(BINDIR)/

install-data:
	$(Q)$(INSTALL) -d $(DESTDIR)$(DATADIR)/applications
	$(Q)$(INSTALL) -d $(DESTDIR)$(DATADIR)/icons/hicolor/scalable/apps
	$(Q)$(INSTALL) -d $(DESTDIR)$(DOCDIR)

uninstall:
	$(Q)$(RM) $(DESTDIR)$(BINDIR)/projt-launcher$(EXE_SUFFIX)
	$(Q)$(RMDIR) $(DESTDIR)$(DOCDIR)

# ============================================================================
# Testing Targets
# ============================================================================

test check: build
ifeq ($(CONFIG_BUILD_TESTS),y)
	$(Q)$(MAKE) -f $(srctree)/mk/tests.mk test
else
	@echo "Tests not enabled. Run 'make menuconfig' and enable BUILD_TESTS."
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
	@echo "Project: $(PROJECT_NAME) $(PROJECT_VERSION)"
	@echo "Source:  $(srctree)"
	@echo "Build:   $(KBUILD_OUTPUT)"
	@echo ""
	@$(MAKE) -f $(srctree)/Makefile listconfig

version:
	@echo "$(PROJECT_VERSION)"

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
	@echo "ProjT Launcher Build System"
	@echo "==========================="
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
	@echo "  build          - Build the project (monolithic)"
	@echo "  build-recursive - Build using per-directory Makefiles (Kbuild-style)"
	@echo "  subtrees       - Build subtree dependencies"
	@echo "  install        - Install to PREFIX (default: /usr/local)"
	@echo "  uninstall      - Remove installed files"
	@echo ""
	@echo "Module targets (recursive):"
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
	@echo "  list-modules   - List all available modules"
	@echo "  listconfig     - Show configuration summary"
	@echo "  info           - Show project information"
	@echo "  version        - Show version"
	@echo "  clean          - Remove build artifacts"
	@echo "  distclean      - Remove everything including config"
	@echo "  help           - Show this help"
	@echo ""
	@echo "Common variables:"
	@echo "  V=1            - Verbose build output"
	@echo "  O=<dir>        - Out-of-tree build directory"
	@echo "  CROSS_COMPILE= - Cross-compilation prefix"
	@echo "  PREFIX=        - Installation prefix"
	@echo "  DESTDIR=       - Staging directory for packages"
	@echo "  JOBS=          - Parallel jobs (default: $(NPROC))"
	@echo ""
	@echo "Examples:"
	@echo "  make defconfig && make -j$(NPROC)"
	@echo "  make zlib                         # Build single module"
	@echo "  make launcher/ui                  # Build launcher submodule"
	@echo "  make CROSS_COMPILE=x86_64-w64-mingw32- defconfig && make"
	@echo "  make O=../build-release menuconfig && make O=../build-release"

# ============================================================================
# Module List
# ============================================================================

list-modules:
	$(Q)$(MAKE) -f $(srctree)/mk/targets.mk list-modules

# ============================================================================
# Forced Target
# ============================================================================

FORCE:

.PHONY: $(PHONY) FORCE libs launcher java tests list-modules
