# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Platform Detection and Configuration
#
# This file detects the host and target platform, setting appropriate
# variables for the build system.

# ============================================================================
# Host Detection
# ============================================================================

# Detect host OS
UNAME_S := $(shell uname -s 2>/dev/null || echo Windows)
UNAME_M := $(shell uname -m 2>/dev/null || echo x86_64)
UNAME_R := $(shell uname -r 2>/dev/null || echo unknown)

# Normalize host OS
ifeq ($(UNAME_S),Linux)
    HOST_OS := linux
    HOST_OS_FAMILY := unix
else ifeq ($(UNAME_S),Darwin)
    HOST_OS := macos
    HOST_OS_FAMILY := unix
else ifeq ($(UNAME_S),FreeBSD)
    HOST_OS := freebsd
    HOST_OS_FAMILY := bsd
else ifeq ($(UNAME_S),OpenBSD)
    HOST_OS := openbsd
    HOST_OS_FAMILY := bsd
else ifeq ($(UNAME_S),NetBSD)
    HOST_OS := netbsd
    HOST_OS_FAMILY := bsd
else ifeq ($(UNAME_S),DragonFly)
    HOST_OS := dragonfly
    HOST_OS_FAMILY := bsd
else ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    HOST_OS := windows
    HOST_OS_FAMILY := windows
    HOST_ENV := mingw
else ifeq ($(findstring MSYS,$(UNAME_S)),MSYS)
    HOST_OS := windows
    HOST_OS_FAMILY := windows
    HOST_ENV := msys
else ifeq ($(findstring CYGWIN,$(UNAME_S)),CYGWIN)
    HOST_OS := windows
    HOST_OS_FAMILY := windows
    HOST_ENV := cygwin
else ifeq ($(UNAME_S),Windows)
    HOST_OS := windows
    HOST_OS_FAMILY := windows
    HOST_ENV := native
else ifeq ($(UNAME_S),Haiku)
    HOST_OS := haiku
    HOST_OS_FAMILY := haiku
else ifeq ($(UNAME_S),SunOS)
    HOST_OS := solaris
    HOST_OS_FAMILY := unix
else ifeq ($(UNAME_S),AIX)
    HOST_OS := aix
    HOST_OS_FAMILY := unix
else
    HOST_OS := unknown
    HOST_OS_FAMILY := unknown
endif

# Normalize host architecture
ifeq ($(UNAME_M),x86_64)
    HOST_ARCH := x86_64
else ifeq ($(UNAME_M),amd64)
    HOST_ARCH := x86_64
else ifeq ($(UNAME_M),i686)
    HOST_ARCH := x86
else ifeq ($(UNAME_M),i386)
    HOST_ARCH := x86
else ifeq ($(UNAME_M),aarch64)
    HOST_ARCH := aarch64
else ifeq ($(UNAME_M),arm64)
    HOST_ARCH := aarch64
else ifeq ($(UNAME_M),armv7l)
    HOST_ARCH := arm
else ifeq ($(UNAME_M),armv8l)
    HOST_ARCH := arm
else ifeq ($(findstring riscv64,$(UNAME_M)),riscv64)
    HOST_ARCH := riscv64
else ifeq ($(findstring riscv32,$(UNAME_M)),riscv32)
    HOST_ARCH := riscv32
else ifeq ($(UNAME_M),ppc64le)
    HOST_ARCH := ppc64le
else ifeq ($(UNAME_M),ppc64)
    HOST_ARCH := ppc64
else ifeq ($(UNAME_M),s390x)
    HOST_ARCH := s390x
else ifeq ($(UNAME_M),loongarch64)
    HOST_ARCH := loongarch64
else ifeq ($(UNAME_M),mips64)
    HOST_ARCH := mips64
else
    HOST_ARCH := $(UNAME_M)
endif

# ============================================================================
# Target Detection (defaults to host if not cross-compiling)
# ============================================================================

# If CROSS_COMPILE is set, try to detect target from it
ifneq ($(CROSS_COMPILE),)
    # Extract target triple from cross-compile prefix
    _CROSS_TRIPLE := $(patsubst %-,%,$(CROSS_COMPILE))
    
    # Detect target OS from triple
    ifneq ($(findstring mingw,$(_CROSS_TRIPLE)),)
        TARGET_OS := windows
        TARGET_OS_FAMILY := windows
    else ifneq ($(findstring windows,$(_CROSS_TRIPLE)),)
        TARGET_OS := windows
        TARGET_OS_FAMILY := windows
    else ifneq ($(findstring darwin,$(_CROSS_TRIPLE)),)
        TARGET_OS := macos
        TARGET_OS_FAMILY := unix
    else ifneq ($(findstring apple,$(_CROSS_TRIPLE)),)
        TARGET_OS := macos
        TARGET_OS_FAMILY := unix
    else ifneq ($(findstring linux,$(_CROSS_TRIPLE)),)
        TARGET_OS := linux
        TARGET_OS_FAMILY := unix
    else ifneq ($(findstring android,$(_CROSS_TRIPLE)),)
        TARGET_OS := android
        TARGET_OS_FAMILY := unix
    else ifneq ($(findstring freebsd,$(_CROSS_TRIPLE)),)
        TARGET_OS := freebsd
        TARGET_OS_FAMILY := bsd
    else ifneq ($(findstring openbsd,$(_CROSS_TRIPLE)),)
        TARGET_OS := openbsd
        TARGET_OS_FAMILY := bsd
    else ifneq ($(findstring netbsd,$(_CROSS_TRIPLE)),)
        TARGET_OS := netbsd
        TARGET_OS_FAMILY := bsd
    else ifneq ($(findstring haiku,$(_CROSS_TRIPLE)),)
        TARGET_OS := haiku
        TARGET_OS_FAMILY := haiku
    else ifneq ($(findstring wasm,$(_CROSS_TRIPLE)),)
        TARGET_OS := wasm
        TARGET_OS_FAMILY := web
    else
        TARGET_OS ?= $(HOST_OS)
        TARGET_OS_FAMILY ?= $(HOST_OS_FAMILY)
    endif
    
    # Detect target arch from triple
    ifneq ($(findstring x86_64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := x86_64
    else ifneq ($(findstring amd64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := x86_64
    else ifneq ($(findstring i686,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := x86
    else ifneq ($(findstring i386,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := x86
    else ifneq ($(findstring aarch64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := aarch64
    else ifneq ($(findstring arm64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := aarch64
    else ifneq ($(findstring armv7,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := arm
    else ifneq ($(findstring arm,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := arm
    else ifneq ($(findstring riscv64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := riscv64
    else ifneq ($(findstring riscv32,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := riscv32
    else ifneq ($(findstring powerpc64le,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := ppc64le
    else ifneq ($(findstring ppc64le,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := ppc64le
    else ifneq ($(findstring loongarch64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := loongarch64
    else ifneq ($(findstring wasm32,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := wasm32
    else ifneq ($(findstring wasm64,$(_CROSS_TRIPLE)),)
        TARGET_ARCH := wasm64
    else
        TARGET_ARCH ?= $(HOST_ARCH)
    endif
else
    # Native build
    TARGET_OS ?= $(HOST_OS)
    TARGET_OS_FAMILY ?= $(HOST_OS_FAMILY)
    TARGET_ARCH ?= $(HOST_ARCH)
endif

# Override from config if set
ifneq ($(call cfg-unquote,$(CONFIG_TARGET_OS)),)
    TARGET_OS := $(call cfg-unquote,$(CONFIG_TARGET_OS))
endif
ifneq ($(call cfg-unquote,$(CONFIG_TARGET_ARCH)),)
    TARGET_ARCH := $(call cfg-unquote,$(CONFIG_TARGET_ARCH))
endif

# ============================================================================
# Platform-Specific Variables
# ============================================================================

# Executable suffix
ifeq ($(TARGET_OS),windows)
    EXE_SUFFIX := .exe
    DLL_PREFIX :=
    DLL_SUFFIX := .dll
    LIB_PREFIX :=
    LIB_SUFFIX := .lib
    OBJ_SUFFIX := .obj
else ifeq ($(TARGET_OS),macos)
    EXE_SUFFIX :=
    DLL_PREFIX := lib
    DLL_SUFFIX := .dylib
    LIB_PREFIX := lib
    LIB_SUFFIX := .a
    OBJ_SUFFIX := .o
else
    EXE_SUFFIX :=
    DLL_PREFIX := lib
    DLL_SUFFIX := .so
    LIB_PREFIX := lib
    LIB_SUFFIX := .a
    OBJ_SUFFIX := .o
endif

# Shared library versioning
ifeq ($(TARGET_OS),linux)
    SONAME_FLAG = -Wl,-soname,$(@F)
else ifeq ($(TARGET_OS_FAMILY),bsd)
    SONAME_FLAG = -Wl,-soname,$(@F)
else ifeq ($(TARGET_OS),macos)
    SONAME_FLAG = -Wl,-install_name,@rpath/$(@F)
else
    SONAME_FLAG :=
endif

# Path separator
ifeq ($(TARGET_OS),windows)
    PATHSEP := ;
else
    PATHSEP := :
endif

# ============================================================================
# Platform Feature Detection
# ============================================================================

# Check for specific platform features
PLATFORM_HAS_EPOLL := $(if $(filter linux android,$(TARGET_OS)),y,)
PLATFORM_HAS_KQUEUE := $(if $(filter macos freebsd openbsd netbsd dragonfly,$(TARGET_OS)),y,)
PLATFORM_HAS_IOCP := $(if $(filter windows,$(TARGET_OS)),y,)
PLATFORM_HAS_DBUS := $(if $(filter linux,$(TARGET_OS)),y,)
PLATFORM_HAS_WAYLAND := $(if $(filter linux,$(TARGET_OS)),y,)
PLATFORM_HAS_X11 := $(if $(filter linux freebsd openbsd netbsd dragonfly,$(TARGET_OS)),y,)
PLATFORM_HAS_COCOA := $(if $(filter macos,$(TARGET_OS)),y,)
PLATFORM_HAS_WIN32 := $(if $(filter windows,$(TARGET_OS)),y,)

# ============================================================================
# Export Variables
# ============================================================================

export HOST_OS HOST_OS_FAMILY HOST_ARCH HOST_ENV
export TARGET_OS TARGET_OS_FAMILY TARGET_ARCH
export EXE_SUFFIX DLL_PREFIX DLL_SUFFIX LIB_PREFIX LIB_SUFFIX OBJ_SUFFIX
export SONAME_FLAG PATHSEP
export PLATFORM_HAS_EPOLL PLATFORM_HAS_KQUEUE PLATFORM_HAS_IOCP
export PLATFORM_HAS_DBUS PLATFORM_HAS_WAYLAND PLATFORM_HAS_X11
export PLATFORM_HAS_COCOA PLATFORM_HAS_WIN32

# ============================================================================
# Debug Output
# ============================================================================

ifdef PLATFORM_DEBUG
$(info Platform Detection:)
$(info   HOST_OS=$(HOST_OS) HOST_ARCH=$(HOST_ARCH) HOST_OS_FAMILY=$(HOST_OS_FAMILY))
$(info   TARGET_OS=$(TARGET_OS) TARGET_ARCH=$(TARGET_ARCH) TARGET_OS_FAMILY=$(TARGET_OS_FAMILY))
$(info   EXE_SUFFIX=$(EXE_SUFFIX) DLL_SUFFIX=$(DLL_SUFFIX))
endif
