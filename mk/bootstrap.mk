# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Self-Hosted Toolchain Bootstrap
#
# This module allows building GCC and/or LLVM from source for fully
# self-hosted builds. Useful for reproducible builds or exotic platforms.

include mk/config.mk
include mk/platform.mk

# ============================================================================
# Bootstrap Configuration
# ============================================================================

BOOTSTRAP_DIR := $(KBUILD_OUTPUT)/toolchain-bootstrap
BOOTSTRAP_SRC := $(BOOTSTRAP_DIR)/src
BOOTSTRAP_BUILD := $(BOOTSTRAP_DIR)/build
BOOTSTRAP_PREFIX := $(KBUILD_OUTPUT)/toolchain

# Versions
BINUTILS_VERSION ?= 2.42
GCC_VERSION ?= 14.2.0
LLVM_VERSION ?= 18.1.8
GMP_VERSION ?= 6.3.0
MPFR_VERSION ?= 4.2.1
MPC_VERSION ?= 1.3.1
ISL_VERSION ?= 0.26

# Download URLs
GNU_MIRROR ?= https://ftp.gnu.org/gnu
LLVM_GITHUB ?= https://github.com/llvm/llvm-project/releases/download

BINUTILS_URL := $(GNU_MIRROR)/binutils/binutils-$(BINUTILS_VERSION).tar.xz
GCC_URL := $(GNU_MIRROR)/gcc/gcc-$(GCC_VERSION)/gcc-$(GCC_VERSION).tar.xz
GMP_URL := $(GNU_MIRROR)/gmp/gmp-$(GMP_VERSION).tar.xz
MPFR_URL := $(GNU_MIRROR)/mpfr/mpfr-$(MPFR_VERSION).tar.xz
MPC_URL := $(GNU_MIRROR)/mpc/mpc-$(MPC_VERSION).tar.gz
ISL_URL := https://libisl.sourceforge.io/isl-$(ISL_VERSION).tar.xz
LLVM_URL := $(LLVM_GITHUB)/llvmorg-$(LLVM_VERSION)/llvm-project-$(LLVM_VERSION).src.tar.xz

# Target triple for cross-compilation
BOOTSTRAP_TARGET ?= $(HOST_ARCH)-$(HOST_OS)-gnu
ifneq ($(CROSS_COMPILE),)
    BOOTSTRAP_TARGET := $(patsubst %-,%,$(CROSS_COMPILE))
endif

# ============================================================================
# Directory Setup
# ============================================================================

$(BOOTSTRAP_DIR) $(BOOTSTRAP_SRC) $(BOOTSTRAP_BUILD) $(BOOTSTRAP_PREFIX):
	@mkdir -p $@

# ============================================================================
# Download Functions
# ============================================================================

define download-and-extract
@echo "Downloading $(1)..."
@mkdir -p $(BOOTSTRAP_SRC)
@if [ ! -f "$(BOOTSTRAP_SRC)/$(notdir $(2))" ]; then \
    curl -L -o "$(BOOTSTRAP_SRC)/$(notdir $(2))" "$(2)" || \
    wget -O "$(BOOTSTRAP_SRC)/$(notdir $(2))" "$(2)"; \
fi
@echo "Extracting $(1)..."
@cd $(BOOTSTRAP_SRC) && tar xf $(notdir $(2))
endef

# ============================================================================
# GCC Bootstrap (Stage 1: Build Dependencies)
# ============================================================================

# GMP
$(BOOTSTRAP_PREFIX)/.gmp-built: | $(BOOTSTRAP_SRC) $(BOOTSTRAP_BUILD) $(BOOTSTRAP_PREFIX)
	$(call download-and-extract,GMP,$(GMP_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/gmp
	cd $(BOOTSTRAP_BUILD)/gmp && \
		$(BOOTSTRAP_SRC)/gmp-$(GMP_VERSION)/configure \
		--prefix=$(BOOTSTRAP_PREFIX) \
		--disable-shared \
		--enable-static
	$(MAKE) -C $(BOOTSTRAP_BUILD)/gmp -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(BOOTSTRAP_BUILD)/gmp install
	@touch $@

# MPFR
$(BOOTSTRAP_PREFIX)/.mpfr-built: $(BOOTSTRAP_PREFIX)/.gmp-built
	$(call download-and-extract,MPFR,$(MPFR_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/mpfr
	cd $(BOOTSTRAP_BUILD)/mpfr && \
		$(BOOTSTRAP_SRC)/mpfr-$(MPFR_VERSION)/configure \
		--prefix=$(BOOTSTRAP_PREFIX) \
		--with-gmp=$(BOOTSTRAP_PREFIX) \
		--disable-shared \
		--enable-static
	$(MAKE) -C $(BOOTSTRAP_BUILD)/mpfr -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(BOOTSTRAP_BUILD)/mpfr install
	@touch $@

# MPC
$(BOOTSTRAP_PREFIX)/.mpc-built: $(BOOTSTRAP_PREFIX)/.mpfr-built
	$(call download-and-extract,MPC,$(MPC_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/mpc
	cd $(BOOTSTRAP_BUILD)/mpc && \
		$(BOOTSTRAP_SRC)/mpc-$(MPC_VERSION)/configure \
		--prefix=$(BOOTSTRAP_PREFIX) \
		--with-gmp=$(BOOTSTRAP_PREFIX) \
		--with-mpfr=$(BOOTSTRAP_PREFIX) \
		--disable-shared \
		--enable-static
	$(MAKE) -C $(BOOTSTRAP_BUILD)/mpc -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(BOOTSTRAP_BUILD)/mpc install
	@touch $@

# ISL (optional, for Graphite optimizations)
$(BOOTSTRAP_PREFIX)/.isl-built: $(BOOTSTRAP_PREFIX)/.gmp-built
	$(call download-and-extract,ISL,$(ISL_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/isl
	cd $(BOOTSTRAP_BUILD)/isl && \
		$(BOOTSTRAP_SRC)/isl-$(ISL_VERSION)/configure \
		--prefix=$(BOOTSTRAP_PREFIX) \
		--with-gmp-prefix=$(BOOTSTRAP_PREFIX) \
		--disable-shared \
		--enable-static
	$(MAKE) -C $(BOOTSTRAP_BUILD)/isl -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(BOOTSTRAP_BUILD)/isl install
	@touch $@

# ============================================================================
# GCC Bootstrap (Stage 2: Binutils)
# ============================================================================

$(BOOTSTRAP_PREFIX)/.binutils-built: | $(BOOTSTRAP_SRC) $(BOOTSTRAP_BUILD)
	$(call download-and-extract,Binutils,$(BINUTILS_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/binutils
	cd $(BOOTSTRAP_BUILD)/binutils && \
		$(BOOTSTRAP_SRC)/binutils-$(BINUTILS_VERSION)/configure \
		--prefix=$(BOOTSTRAP_PREFIX) \
		--target=$(BOOTSTRAP_TARGET) \
		--disable-multilib \
		--disable-nls \
		--disable-werror \
		--enable-gold \
		--enable-lto \
		--enable-plugins
	$(MAKE) -C $(BOOTSTRAP_BUILD)/binutils -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(BOOTSTRAP_BUILD)/binutils install
	@touch $@

# ============================================================================
# GCC Bootstrap (Stage 3: GCC itself)
# ============================================================================

GCC_DEPS := $(BOOTSTRAP_PREFIX)/.binutils-built \
            $(BOOTSTRAP_PREFIX)/.mpc-built \
            $(BOOTSTRAP_PREFIX)/.isl-built

$(BOOTSTRAP_PREFIX)/.gcc-built: $(GCC_DEPS)
	$(call download-and-extract,GCC,$(GCC_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/gcc
	cd $(BOOTSTRAP_BUILD)/gcc && \
		$(BOOTSTRAP_SRC)/gcc-$(GCC_VERSION)/configure \
		--prefix=$(BOOTSTRAP_PREFIX) \
		--target=$(BOOTSTRAP_TARGET) \
		--with-gmp=$(BOOTSTRAP_PREFIX) \
		--with-mpfr=$(BOOTSTRAP_PREFIX) \
		--with-mpc=$(BOOTSTRAP_PREFIX) \
		--with-isl=$(BOOTSTRAP_PREFIX) \
		--enable-languages=c,c++ \
		--disable-multilib \
		--disable-nls \
		--disable-libsanitizer \
		--enable-lto \
		--enable-plugin
	$(MAKE) -C $(BOOTSTRAP_BUILD)/gcc -j$(shell nproc 2>/dev/null || echo 4)
	$(MAKE) -C $(BOOTSTRAP_BUILD)/gcc install
	@touch $@

toolchain-gcc: $(BOOTSTRAP_PREFIX)/.gcc-built
	@echo "GCC $(GCC_VERSION) built successfully!"
	@echo "Add to PATH: export PATH=$(BOOTSTRAP_PREFIX)/bin:\$$PATH"

# ============================================================================
# LLVM Bootstrap
# ============================================================================

LLVM_CMAKE_FLAGS := \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$(BOOTSTRAP_PREFIX) \
    -DLLVM_ENABLE_PROJECTS="clang;lld;clang-tools-extra" \
    -DLLVM_ENABLE_RUNTIMES="compiler-rt;libcxx;libcxxabi;libunwind" \
    -DLLVM_TARGETS_TO_BUILD="X86;AArch64;ARM;RISCV;WebAssembly" \
    -DLLVM_ENABLE_LTO=OFF \
    -DLLVM_PARALLEL_LINK_JOBS=2 \
    -DCLANG_DEFAULT_CXX_STDLIB=libc++ \
    -DCLANG_DEFAULT_RTLIB=compiler-rt \
    -DCLANG_DEFAULT_LINKER=lld

$(BOOTSTRAP_PREFIX)/.llvm-built: | $(BOOTSTRAP_SRC) $(BOOTSTRAP_BUILD)
	$(call download-and-extract,LLVM,$(LLVM_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/llvm
	cd $(BOOTSTRAP_BUILD)/llvm && \
		cmake -G Ninja \
		$(LLVM_CMAKE_FLAGS) \
		$(BOOTSTRAP_SRC)/llvm-project-$(LLVM_VERSION).src/llvm
	cmake --build $(BOOTSTRAP_BUILD)/llvm -j$(shell nproc 2>/dev/null || echo 4)
	cmake --install $(BOOTSTRAP_BUILD)/llvm
	@touch $@

toolchain-llvm: $(BOOTSTRAP_PREFIX)/.llvm-built
	@echo "LLVM $(LLVM_VERSION) built successfully!"
	@echo "Add to PATH: export PATH=$(BOOTSTRAP_PREFIX)/bin:\$$PATH"

# ============================================================================
# Cross-Compiler Generation
# ============================================================================

# Generate a cross-compiler for a specific target
# Usage: make cross-toolchain TARGET=aarch64-linux-gnu

CROSS_TARGET ?= $(TARGET)

cross-toolchain-gcc:
	@echo "Building cross-compiler for $(CROSS_TARGET)..."
	$(MAKE) -f $(srctree)/Makefile toolchain-gcc BOOTSTRAP_TARGET=$(CROSS_TARGET)

cross-toolchain-llvm:
	@echo "Building LLVM cross-compiler for $(CROSS_TARGET)..."
	$(MAKE) -f $(srctree)/Makefile toolchain-llvm \
		LLVM_CMAKE_FLAGS="$(LLVM_CMAKE_FLAGS) -DLLVM_DEFAULT_TARGET_TRIPLE=$(CROSS_TARGET)"

# ============================================================================
# Convenience Targets
# ============================================================================

# Build both GCC and LLVM
toolchain-all: toolchain-gcc toolchain-llvm

# Use the bootstrapped toolchain
use-bootstrap-toolchain:
	@echo "export PATH=$(BOOTSTRAP_PREFIX)/bin:\$$PATH"
	@echo "export CC=$(BOOTSTRAP_PREFIX)/bin/gcc"
	@echo "export CXX=$(BOOTSTRAP_PREFIX)/bin/g++"

use-bootstrap-llvm:
	@echo "export PATH=$(BOOTSTRAP_PREFIX)/bin:\$$PATH"
	@echo "export CC=$(BOOTSTRAP_PREFIX)/bin/clang"
	@echo "export CXX=$(BOOTSTRAP_PREFIX)/bin/clang++"

# Clean bootstrap
toolchain-clean:
	$(Q)rm -rf $(BOOTSTRAP_DIR)

toolchain-distclean: toolchain-clean
	$(Q)rm -rf $(BOOTSTRAP_PREFIX)

# ============================================================================
# MinGW Cross-Compiler (Linux → Windows)
# ============================================================================

MINGW_VERSION ?= 12.0.0
MINGW_URL := https://github.com/mingw-w64/mingw-w64/archive/refs/tags/v$(MINGW_VERSION).tar.gz

toolchain-mingw: $(BOOTSTRAP_PREFIX)/.binutils-built
	@echo "Building MinGW-w64 cross-compiler..."
	$(call download-and-extract,MinGW-w64,$(MINGW_URL))
	@mkdir -p $(BOOTSTRAP_BUILD)/mingw-headers
	cd $(BOOTSTRAP_BUILD)/mingw-headers && \
		$(BOOTSTRAP_SRC)/mingw-w64-$(MINGW_VERSION)/mingw-w64-headers/configure \
		--prefix=$(BOOTSTRAP_PREFIX)/x86_64-w64-mingw32 \
		--host=x86_64-w64-mingw32
	$(MAKE) -C $(BOOTSTRAP_BUILD)/mingw-headers install
	# Continue with GCC build targeting mingw...
	@echo "MinGW headers installed. Complete GCC build with:"
	@echo "  make toolchain-gcc BOOTSTRAP_TARGET=x86_64-w64-mingw32"

# ============================================================================
# Help
# ============================================================================

toolchain-help:
	@echo "Toolchain Bootstrap Targets:"
	@echo "  toolchain-gcc      - Build GCC from source"
	@echo "  toolchain-llvm     - Build LLVM/Clang from source"
	@echo "  toolchain-all      - Build both GCC and LLVM"
	@echo "  toolchain-mingw    - Build MinGW cross-compiler"
	@echo "  cross-toolchain-gcc TARGET=<triple> - Build GCC cross-compiler"
	@echo "  cross-toolchain-llvm TARGET=<triple> - Build LLVM cross-compiler"
	@echo "  toolchain-clean    - Remove bootstrap build files"
	@echo "  toolchain-distclean - Remove everything including installed toolchain"
	@echo ""
	@echo "Versions:"
	@echo "  GCC_VERSION=$(GCC_VERSION)"
	@echo "  LLVM_VERSION=$(LLVM_VERSION)"
	@echo "  BINUTILS_VERSION=$(BINUTILS_VERSION)"

.PHONY: toolchain-gcc toolchain-llvm toolchain-all toolchain-mingw
.PHONY: cross-toolchain-gcc cross-toolchain-llvm
.PHONY: use-bootstrap-toolchain use-bootstrap-llvm
.PHONY: toolchain-clean toolchain-distclean toolchain-help
