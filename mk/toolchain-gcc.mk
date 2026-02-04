# SPDX-License-Identifier: GPL-2.0
#
# GCC Toolchain Build Wrapper
#
# This file builds GCC from source. It can use either:
# 1. A git subtree at toolchain/gcc (CONFIG_TOOLCHAIN_BUNDLED_GCC=y)
# 2. Downloaded sources to $(TOOLCHAIN_BUILD)/gcc-src
#
# Usage:
#   make -f mk/toolchain-gcc.mk [all|clean|distclean]
#
# Variables:
#   GCC_VERSION   - GCC version to build (default: 14.2.0)
#   TOOLCHAIN_PREFIX - Installation prefix
#   TARGET        - Cross-compilation target (optional)

-include $(KBUILD_OUTPUT)/.config

# Version and URLs
GCC_VERSION ?= 14.2.0
GCC_MIRROR ?= https://ftp.gnu.org/gnu/gcc
GCC_URL := $(GCC_MIRROR)/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz

# Directories
TOOLCHAIN_BUILD ?= $(KBUILD_OUTPUT)/toolchain
TOOLCHAIN_PREFIX ?= $(TOOLCHAIN_BUILD)/install
GCC_SRCDIR := $(TOOLCHAIN_BUILD)/gcc-src
GCC_BUILDDIR := $(TOOLCHAIN_BUILD)/gcc-build

# Check for subtree
ifeq ($(CONFIG_TOOLCHAIN_BUNDLED_GCC),y)
  ifneq ($(wildcard $(srctree)/toolchain/gcc/configure),)
    GCC_SRCDIR := $(srctree)/toolchain/gcc
    USE_SUBTREE := 1
  endif
endif

# Prerequisites (downloaded to toolchain build dir)
GMP_VERSION := 6.3.0
MPFR_VERSION := 4.2.1
MPC_VERSION := 1.3.1
ISL_VERSION := 0.26

# Parallel jobs
JOBS ?= $(shell nproc 2>/dev/null || echo 4)

# Target architecture (native or cross)
ifdef TARGET
  CONFIGURE_TARGET := --target=$(TARGET)
  PREFIX_SUFFIX := /$(TARGET)
else
  CONFIGURE_TARGET :=
  PREFIX_SUFFIX :=
endif

# Configure options
GCC_CONFIGURE_OPTS := \
	--prefix=$(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX) \
	--enable-languages=c,c++ \
	--disable-multilib \
	--disable-bootstrap \
	--disable-nls \
	--enable-lto \
	--enable-plugin \
	--with-system-zlib \
	$(CONFIGURE_TARGET)

# Quiet/Verbose
ifeq ($(V),1)
Q :=
REDIRECT :=
else
Q := @
REDIRECT := > /dev/null 2>&1
endif

.PHONY: all clean distclean download prerequisites configure build install

all: install

# Download GCC sources (skip if using subtree)
download: $(GCC_SRCDIR)/.downloaded

$(GCC_SRCDIR)/.downloaded:
ifndef USE_SUBTREE
	@echo "  DOWNLOAD  gcc-$(GCC_VERSION)"
	$(Q)mkdir -p $(TOOLCHAIN_BUILD)
	$(Q)cd $(TOOLCHAIN_BUILD) && \
		curl -fsSL $(GCC_URL) | tar xJ && \
		mv gcc-$(GCC_VERSION) gcc-src
	$(Q)touch $@
else
	@echo "  SUBTREE   Using toolchain/gcc"
	$(Q)mkdir -p $(dir $@)
	$(Q)touch $@
endif

# Download and setup prerequisites
prerequisites: $(GCC_SRCDIR)/.prereqs

$(GCC_SRCDIR)/.prereqs: $(GCC_SRCDIR)/.downloaded
	@echo "  PREREQS   Downloading GMP, MPFR, MPC, ISL"
	$(Q)cd $(GCC_SRCDIR) && \
		if [ ! -d gmp ]; then \
			curl -fsSL https://ftp.gnu.org/gnu/gmp/gmp-$(GMP_VERSION).tar.xz | tar xJ && \
			mv gmp-$(GMP_VERSION) gmp; \
		fi && \
		if [ ! -d mpfr ]; then \
			curl -fsSL https://ftp.gnu.org/gnu/mpfr/mpfr-$(MPFR_VERSION).tar.xz | tar xJ && \
			mv mpfr-$(MPFR_VERSION) mpfr; \
		fi && \
		if [ ! -d mpc ]; then \
			curl -fsSL https://ftp.gnu.org/gnu/mpc/mpc-$(MPC_VERSION).tar.gz | tar xz && \
			mv mpc-$(MPC_VERSION) mpc; \
		fi && \
		if [ ! -d isl ]; then \
			curl -fsSL https://libisl.sourceforge.io/isl-$(ISL_VERSION).tar.xz | tar xJ && \
			mv isl-$(ISL_VERSION) isl; \
		fi
	$(Q)touch $@

# Configure
configure: $(GCC_BUILDDIR)/.configured

$(GCC_BUILDDIR)/.configured: $(GCC_SRCDIR)/.prereqs
	@echo "  CONFIG    gcc-$(GCC_VERSION)"
	$(Q)mkdir -p $(GCC_BUILDDIR)
	$(Q)cd $(GCC_BUILDDIR) && \
		$(GCC_SRCDIR)/configure $(GCC_CONFIGURE_OPTS) $(REDIRECT)
	$(Q)touch $@

# Build
build: $(GCC_BUILDDIR)/.built

$(GCC_BUILDDIR)/.built: $(GCC_BUILDDIR)/.configured
	@echo "  BUILD     gcc-$(GCC_VERSION) (this may take a while...)"
	$(Q)$(MAKE) -C $(GCC_BUILDDIR) -j$(JOBS) $(REDIRECT)
	$(Q)touch $@

# Install
install: $(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)/.gcc-installed

$(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)/.gcc-installed: $(GCC_BUILDDIR)/.built
	@echo "  INSTALL   gcc -> $(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)"
	$(Q)$(MAKE) -C $(GCC_BUILDDIR) install $(REDIRECT)
	$(Q)touch $@

# Clean build directory
clean:
	@echo "  CLEAN     gcc build"
	$(Q)rm -rf $(GCC_BUILDDIR)

# Clean everything including downloads
distclean: clean
ifndef USE_SUBTREE
	@echo "  DISTCLEAN gcc sources"
	$(Q)rm -rf $(GCC_SRCDIR)
endif
	$(Q)rm -rf $(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)/bin/*gcc*
	$(Q)rm -rf $(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)/bin/*g++*
	$(Q)rm -rf $(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)/bin/*cpp*

# Show configuration
info:
	@echo "GCC Toolchain Configuration:"
	@echo "  Version:    $(GCC_VERSION)"
	@echo "  Source:     $(GCC_SRCDIR)"
ifdef USE_SUBTREE
	@echo "  Mode:       Subtree (toolchain/gcc)"
else
	@echo "  Mode:       Download"
endif
	@echo "  Build:      $(GCC_BUILDDIR)"
	@echo "  Install:    $(TOOLCHAIN_PREFIX)$(PREFIX_SUFFIX)"
ifdef TARGET
	@echo "  Target:     $(TARGET)"
endif
	@echo "  Jobs:       $(JOBS)"

.PHONY: info
