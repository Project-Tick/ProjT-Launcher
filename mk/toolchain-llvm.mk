# SPDX-License-Identifier: GPL-2.0
#
# LLVM/Clang Toolchain Build Wrapper
#
# This file builds LLVM/Clang from source. It can use either:
# 1. A git subtree at toolchain/llvm (CONFIG_TOOLCHAIN_BUNDLED_LLVM=y)
# 2. Downloaded sources to $(TOOLCHAIN_BUILD)/llvm-src
#
# Usage:
#   make -f mk/toolchain-llvm.mk [all|clean|distclean]
#
# Variables:
#   LLVM_VERSION  - LLVM version to build (default: 18.1.8)
#   TOOLCHAIN_PREFIX - Installation prefix
#   TARGET        - Cross-compilation target (optional)

-include $(KBUILD_OUTPUT)/.config

# Version and URLs
LLVM_VERSION ?= 18.1.8
LLVM_GITHUB := https://github.com/llvm/llvm-project/releases/download
LLVM_URL := $(LLVM_GITHUB)/llvmorg-$(LLVM_VERSION)/llvm-project-$(LLVM_VERSION).src.tar.xz

# Directories
TOOLCHAIN_BUILD ?= $(KBUILD_OUTPUT)/toolchain
TOOLCHAIN_PREFIX ?= $(TOOLCHAIN_BUILD)/install
LLVM_SRCDIR := $(TOOLCHAIN_BUILD)/llvm-src
LLVM_BUILDDIR := $(TOOLCHAIN_BUILD)/llvm-build

# Check for subtree
ifeq ($(CONFIG_TOOLCHAIN_BUNDLED_LLVM),y)
  ifneq ($(wildcard $(srctree)/toolchain/llvm/llvm/CMakeLists.txt),)
    LLVM_SRCDIR := $(srctree)/toolchain/llvm
    USE_SUBTREE := 1
  endif
endif

# Parallel jobs
JOBS ?= $(shell nproc 2>/dev/null || echo 4)

# Build type
CMAKE_BUILD_TYPE ?= Release

# Components to build
LLVM_PROJECTS := clang;lld

ifeq ($(CONFIG_TOOLCHAIN_LLVM_LIBCXX),y)
  LLVM_PROJECTS := $(LLVM_PROJECTS);libcxx;libcxxabi;libunwind
endif

ifeq ($(CONFIG_TOOLCHAIN_LLVM_COMPILER_RT),y)
  LLVM_RUNTIMES := compiler-rt
endif

# Target triple for cross-compilation
ifdef TARGET
  CMAKE_TARGET := -DLLVM_DEFAULT_TARGET_TRIPLE=$(TARGET)
else
  CMAKE_TARGET :=
endif

# CMake options
LLVM_CMAKE_OPTS := \
	-G Ninja \
	-S $(LLVM_SRCDIR)/llvm \
	-B $(LLVM_BUILDDIR) \
	-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
	-DCMAKE_INSTALL_PREFIX=$(TOOLCHAIN_PREFIX) \
	-DLLVM_ENABLE_PROJECTS="$(LLVM_PROJECTS)" \
	-DLLVM_ENABLE_RUNTIMES="$(LLVM_RUNTIMES)" \
	-DLLVM_TARGETS_TO_BUILD="X86;AArch64;ARM;WebAssembly" \
	-DLLVM_ENABLE_LTO=OFF \
	-DLLVM_INCLUDE_TESTS=OFF \
	-DLLVM_INCLUDE_EXAMPLES=OFF \
	-DLLVM_INCLUDE_BENCHMARKS=OFF \
	-DLLVM_INCLUDE_DOCS=OFF \
	-DLLVM_ENABLE_BINDINGS=OFF \
	-DLLVM_PARALLEL_LINK_JOBS=2 \
	-DCLANG_ENABLE_STATIC_ANALYZER=OFF \
	-DCLANG_ENABLE_ARCMT=OFF \
	$(CMAKE_TARGET)

# Quiet/Verbose
ifeq ($(V),1)
Q :=
REDIRECT :=
else
Q := @
REDIRECT := > /dev/null 2>&1
endif

.PHONY: all clean distclean download configure build install

all: install

# Download LLVM sources (skip if using subtree)
download: $(LLVM_SRCDIR)/.downloaded

$(LLVM_SRCDIR)/.downloaded:
ifndef USE_SUBTREE
	@echo "  DOWNLOAD  llvm-project-$(LLVM_VERSION)"
	$(Q)mkdir -p $(TOOLCHAIN_BUILD)
	$(Q)cd $(TOOLCHAIN_BUILD) && \
		curl -fsSL $(LLVM_URL) | tar xJ && \
		mv llvm-project-$(LLVM_VERSION).src llvm-src
	$(Q)touch $@
else
	@echo "  SUBTREE   Using toolchain/llvm"
	$(Q)mkdir -p $(dir $@)
	$(Q)touch $@
endif

# Configure with CMake
configure: $(LLVM_BUILDDIR)/.configured

$(LLVM_BUILDDIR)/.configured: $(LLVM_SRCDIR)/.downloaded
	@echo "  CONFIG    llvm-$(LLVM_VERSION)"
	$(Q)cmake $(LLVM_CMAKE_OPTS) $(REDIRECT)
	$(Q)touch $@

# Build
build: $(LLVM_BUILDDIR)/.built

$(LLVM_BUILDDIR)/.built: $(LLVM_BUILDDIR)/.configured
	@echo "  BUILD     llvm-$(LLVM_VERSION) (this may take a while...)"
	$(Q)cmake --build $(LLVM_BUILDDIR) -j$(JOBS) $(REDIRECT)
	$(Q)touch $@

# Install
install: $(TOOLCHAIN_PREFIX)/.llvm-installed

$(TOOLCHAIN_PREFIX)/.llvm-installed: $(LLVM_BUILDDIR)/.built
	@echo "  INSTALL   llvm -> $(TOOLCHAIN_PREFIX)"
	$(Q)cmake --install $(LLVM_BUILDDIR) $(REDIRECT)
	$(Q)touch $@

# Clean build directory
clean:
	@echo "  CLEAN     llvm build"
	$(Q)rm -rf $(LLVM_BUILDDIR)

# Clean everything including downloads
distclean: clean
ifndef USE_SUBTREE
	@echo "  DISTCLEAN llvm sources"
	$(Q)rm -rf $(LLVM_SRCDIR)
endif
	$(Q)rm -rf $(TOOLCHAIN_PREFIX)/bin/clang*
	$(Q)rm -rf $(TOOLCHAIN_PREFIX)/bin/llvm*
	$(Q)rm -rf $(TOOLCHAIN_PREFIX)/bin/lld*

# Show configuration
info:
	@echo "LLVM Toolchain Configuration:"
	@echo "  Version:    $(LLVM_VERSION)"
	@echo "  Source:     $(LLVM_SRCDIR)"
ifdef USE_SUBTREE
	@echo "  Mode:       Subtree (toolchain/llvm)"
else
	@echo "  Mode:       Download"
endif
	@echo "  Build:      $(LLVM_BUILDDIR)"
	@echo "  Install:    $(TOOLCHAIN_PREFIX)"
	@echo "  Projects:   $(LLVM_PROJECTS)"
ifdef LLVM_RUNTIMES
	@echo "  Runtimes:   $(LLVM_RUNTIMES)"
endif
ifdef TARGET
	@echo "  Target:     $(TARGET)"
endif
	@echo "  Jobs:       $(JOBS)"

# Cross-compilation presets
.PHONY: windows macos aarch64 wasm

windows:
	$(Q)$(MAKE) -f $(lastword $(MAKEFILE_LIST)) TARGET=x86_64-w64-mingw32 all

macos:
	$(Q)$(MAKE) -f $(lastword $(MAKEFILE_LIST)) TARGET=x86_64-apple-darwin all

aarch64:
	$(Q)$(MAKE) -f $(lastword $(MAKEFILE_LIST)) TARGET=aarch64-linux-gnu all

wasm:
	$(Q)$(MAKE) -f $(lastword $(MAKEFILE_LIST)) TARGET=wasm32-unknown-wasi all

.PHONY: info
