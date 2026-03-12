# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Advanced Toolchain Configuration
#
# This file provides comprehensive toolchain detection and configuration
# for multiple compilers: GCC, Clang, MSVC, MinGW, etc.

include mk/config.mk
include mk/platform.mk

# ============================================================================
# Toolchain Type Detection
# ============================================================================

# Toolchain type: gcc, clang, msvc, mingw, android-ndk, apple-clang
TOOLCHAIN ?= auto

# Cross-compile prefix (e.g., x86_64-w64-mingw32-)
CROSS_COMPILE := $(call cfg-unquote,$(CONFIG_CROSS_COMPILE))
SYSROOT := $(call cfg-unquote,$(CONFIG_SYSROOT))

# Config overrides
CC_CFG := $(call cfg-unquote,$(CONFIG_CC))
CXX_CFG := $(call cfg-unquote,$(CONFIG_CXX))
AR_CFG := $(call cfg-unquote,$(CONFIG_AR))
STRIP_CFG := $(call cfg-unquote,$(CONFIG_STRIP))

# ============================================================================
# Compiler Selection
# ============================================================================

# Set default compilers based on toolchain type and platform
ifeq ($(TOOLCHAIN),auto)
    # Auto-detect based on platform and available tools
    ifeq ($(HOST_OS),windows)
        ifeq ($(HOST_ENV),native)
            # Native Windows - prefer MSVC if available
            ifneq ($(shell where cl.exe 2>nul),)
                TOOLCHAIN := msvc
            else ifneq ($(shell where clang-cl.exe 2>nul),)
                TOOLCHAIN := clang-cl
            else ifneq ($(shell where gcc.exe 2>nul),)
                TOOLCHAIN := mingw
            endif
        else
            # MSYS2/MinGW/Cygwin
            TOOLCHAIN := mingw
        endif
    else ifeq ($(HOST_OS),macos)
        TOOLCHAIN := apple-clang
    else
        # Linux/BSD - check what's available
        ifneq ($(shell command -v clang 2>/dev/null),)
            TOOLCHAIN := clang
        else ifneq ($(shell command -v gcc 2>/dev/null),)
            TOOLCHAIN := gcc
        else
            $(error No suitable compiler found. Install GCC or Clang.)
        endif
    endif
endif

# ============================================================================
# GCC Toolchain
# ============================================================================

ifeq ($(TOOLCHAIN),gcc)
    ifneq ($(CROSS_COMPILE),)
        CC ?= $(CROSS_COMPILE)gcc
        CXX ?= $(CROSS_COMPILE)g++
        AR ?= $(CROSS_COMPILE)ar
        STRIP ?= $(CROSS_COMPILE)strip
        RANLIB ?= $(CROSS_COMPILE)ranlib
        NM ?= $(CROSS_COMPILE)nm
        OBJCOPY ?= $(CROSS_COMPILE)objcopy
        OBJDUMP ?= $(CROSS_COMPILE)objdump
        READELF ?= $(CROSS_COMPILE)readelf
    else
        CC ?= gcc
        CXX ?= g++
        AR ?= ar
        STRIP ?= strip
        RANLIB ?= ranlib
        NM ?= nm
        OBJCOPY ?= objcopy
        OBJDUMP ?= objdump
        READELF ?= readelf
    endif
    LD ?= $(CXX)
    AS ?= $(CC)
    TOOLCHAIN_ID := gcc
endif

# ============================================================================
# Clang/LLVM Toolchain
# ============================================================================

ifeq ($(TOOLCHAIN),clang)
    ifneq ($(CROSS_COMPILE),)
        CC ?= clang --target=$(patsubst %-,%,$(CROSS_COMPILE))
        CXX ?= clang++ --target=$(patsubst %-,%,$(CROSS_COMPILE))
        AR ?= llvm-ar
        STRIP ?= llvm-strip
        RANLIB ?= llvm-ranlib
        NM ?= llvm-nm
        OBJCOPY ?= llvm-objcopy
        OBJDUMP ?= llvm-objdump
        READELF ?= llvm-readelf
    else
        CC ?= clang
        CXX ?= clang++
        AR ?= llvm-ar
        STRIP ?= llvm-strip
        RANLIB ?= llvm-ranlib
        NM ?= llvm-nm
        OBJCOPY ?= llvm-objcopy
        OBJDUMP ?= llvm-objdump
        READELF ?= llvm-readelf
    endif
    LD ?= $(CXX)
    AS ?= $(CC)
    TOOLCHAIN_ID := clang
    
    # Use lld if available
    ifneq ($(shell command -v ld.lld 2>/dev/null),)
        LDFLAGS += -fuse-ld=lld
    endif
endif

# ============================================================================
# Apple Clang Toolchain
# ============================================================================

ifeq ($(TOOLCHAIN),apple-clang)
    CC ?= clang
    CXX ?= clang++
    AR ?= ar
    STRIP ?= strip
    RANLIB ?= ranlib
    NM ?= nm
    OBJCOPY ?= objcopy
    OBJDUMP ?= objdump
    LD ?= $(CXX)
    AS ?= $(CC)
    TOOLCHAIN_ID := apple-clang
    
    # macOS specific: universal binary support
    ifdef MACOS_UNIVERSAL
        CFLAGS += -arch x86_64 -arch arm64
        CXXFLAGS += -arch x86_64 -arch arm64
        LDFLAGS += -arch x86_64 -arch arm64
    endif
    
    # Deployment target
    MACOS_DEPLOYMENT_TARGET ?= 11.0
    CFLAGS += -mmacosx-version-min=$(MACOS_DEPLOYMENT_TARGET)
    CXXFLAGS += -mmacosx-version-min=$(MACOS_DEPLOYMENT_TARGET)
    LDFLAGS += -mmacosx-version-min=$(MACOS_DEPLOYMENT_TARGET)
endif

# ============================================================================
# MinGW Toolchain (for Windows targets)
# ============================================================================

ifeq ($(TOOLCHAIN),mingw)
    ifneq ($(CROSS_COMPILE),)
        CC ?= $(CROSS_COMPILE)gcc
        CXX ?= $(CROSS_COMPILE)g++
        AR ?= $(CROSS_COMPILE)ar
        STRIP ?= $(CROSS_COMPILE)strip
        RANLIB ?= $(CROSS_COMPILE)ranlib
        WINDRES ?= $(CROSS_COMPILE)windres
        DLLTOOL ?= $(CROSS_COMPILE)dlltool
    else
        CC ?= gcc
        CXX ?= g++
        AR ?= ar
        STRIP ?= strip
        RANLIB ?= ranlib
        WINDRES ?= windres
        DLLTOOL ?= dlltool
    endif
    LD ?= $(CXX)
    AS ?= $(CC)
    TOOLCHAIN_ID := mingw
    
    # Windows-specific flags
    CFLAGS += -DWIN32 -D_WIN32 -DWINDOWS
    CXXFLAGS += -DWIN32 -D_WIN32 -DWINDOWS
    
    # Unicode support
    CFLAGS += -DUNICODE -D_UNICODE
    CXXFLAGS += -DUNICODE -D_UNICODE
    
    # Stack size (8MB for Qt)
    LDFLAGS += -Wl,--stack,8388608
endif

# ============================================================================
# MSVC Toolchain (Native Windows)
# ============================================================================

ifeq ($(TOOLCHAIN),msvc)
    CC := cl.exe
    CXX := cl.exe
    AR := lib.exe
    LD := link.exe
    RC := rc.exe
    MT := mt.exe
    STRIP := 
    TOOLCHAIN_ID := msvc
    
    # MSVC doesn't use traditional flags
    CFLAGS :=
    CXXFLAGS :=
    LDFLAGS :=
    
    # MSVC-specific flags
    MSVC_CFLAGS := /nologo /W4 /EHsc /permissive-
    MSVC_CXXFLAGS := $(MSVC_CFLAGS) /std:c++20
    MSVC_LDFLAGS := /nologo /LTCG /MANIFEST:NO /STACK:8388608
    
    # Defines
    MSVC_DEFINES := /DWIN32 /D_WIN32 /DUNICODE /D_UNICODE
endif

# ============================================================================
# Clang-CL Toolchain (LLVM on Windows with MSVC ABI)
# ============================================================================

ifeq ($(TOOLCHAIN),clang-cl)
    CC := clang-cl.exe
    CXX := clang-cl.exe
    AR := llvm-lib.exe
    LD := lld-link.exe
    RC := llvm-rc.exe
    STRIP :=
    TOOLCHAIN_ID := clang-cl
    
    # Clang-CL uses MSVC-style flags
    MSVC_CFLAGS := /nologo /W4 /EHsc
    MSVC_CXXFLAGS := $(MSVC_CFLAGS) /std:c++20
    MSVC_LDFLAGS := /nologo
    MSVC_DEFINES := /DWIN32 /D_WIN32 /DUNICODE /D_UNICODE
endif

# ============================================================================
# Android NDK Toolchain
# ============================================================================

ifeq ($(TOOLCHAIN),android-ndk)
    ANDROID_NDK ?= $(ANDROID_NDK_HOME)
    ANDROID_API ?= 24
    ANDROID_ABI ?= arm64-v8a
    
    ifeq ($(ANDROID_ABI),arm64-v8a)
        ANDROID_TRIPLE := aarch64-linux-android
    else ifeq ($(ANDROID_ABI),armeabi-v7a)
        ANDROID_TRIPLE := armv7a-linux-androideabi
    else ifeq ($(ANDROID_ABI),x86_64)
        ANDROID_TRIPLE := x86_64-linux-android
    else ifeq ($(ANDROID_ABI),x86)
        ANDROID_TRIPLE := i686-linux-android
    endif
    
    ANDROID_TOOLCHAIN := $(ANDROID_NDK)/toolchains/llvm/prebuilt/$(HOST_OS)-$(HOST_ARCH)
    
    CC := $(ANDROID_TOOLCHAIN)/bin/$(ANDROID_TRIPLE)$(ANDROID_API)-clang
    CXX := $(ANDROID_TOOLCHAIN)/bin/$(ANDROID_TRIPLE)$(ANDROID_API)-clang++
    AR := $(ANDROID_TOOLCHAIN)/bin/llvm-ar
    STRIP := $(ANDROID_TOOLCHAIN)/bin/llvm-strip
    RANLIB := $(ANDROID_TOOLCHAIN)/bin/llvm-ranlib
    LD := $(CXX)
    TOOLCHAIN_ID := android-ndk
    
    CFLAGS += -DANDROID
    CXXFLAGS += -DANDROID
endif

# ============================================================================
# Emscripten (WebAssembly)
# ============================================================================

ifeq ($(TOOLCHAIN),emscripten)
    CC := emcc
    CXX := em++
    AR := emar
    RANLIB := emranlib
    LD := $(CXX)
    STRIP := 
    TOOLCHAIN_ID := emscripten
    
    EXE_SUFFIX := .js
    CFLAGS += -s WASM=1
    CXXFLAGS += -s WASM=1
endif

# ============================================================================
# Zig CC (Universal Cross-Compiler)
# ============================================================================

ifeq ($(TOOLCHAIN),zig)
    ZIG ?= zig
    ZIG_TARGET ?= native
    
    CC := $(ZIG) cc -target $(ZIG_TARGET)
    CXX := $(ZIG) c++ -target $(ZIG_TARGET)
    AR := $(ZIG) ar
    RANLIB := $(ZIG) ranlib
    LD := $(CXX)
    TOOLCHAIN_ID := zig
endif

# ============================================================================
# Apply Config Overrides
# ============================================================================

ifneq ($(CC_CFG),)
    CC := $(CC_CFG)
endif
ifneq ($(CXX_CFG),)
    CXX := $(CXX_CFG)
endif
ifneq ($(AR_CFG),)
    AR := $(AR_CFG)
endif
ifneq ($(STRIP_CFG),)
    STRIP := $(STRIP_CFG)
endif

# ============================================================================
# Sysroot Handling
# ============================================================================

ifneq ($(SYSROOT),)
    ifeq ($(TOOLCHAIN_ID),$(filter $(TOOLCHAIN_ID),gcc clang apple-clang mingw))
        CFLAGS += --sysroot=$(SYSROOT)
        CXXFLAGS += --sysroot=$(SYSROOT)
        LDFLAGS += --sysroot=$(SYSROOT)
    endif
endif

# ============================================================================
# Compiler Version Detection
# ============================================================================

# Get compiler version
ifeq ($(TOOLCHAIN_ID),$(filter $(TOOLCHAIN_ID),gcc mingw))
    COMPILER_VERSION := $(shell $(CC) -dumpversion 2>/dev/null)
    COMPILER_FULL_VERSION := $(shell $(CC) -dumpfullversion 2>/dev/null || echo $(COMPILER_VERSION))
else ifeq ($(TOOLCHAIN_ID),$(filter $(TOOLCHAIN_ID),clang apple-clang))
    COMPILER_VERSION := $(shell $(CC) --version 2>/dev/null | head -1 | sed 's/.*version \([0-9.]*\).*/\1/')
    COMPILER_FULL_VERSION := $(COMPILER_VERSION)
else ifeq ($(TOOLCHAIN_ID),msvc)
    # MSVC version detection is complex, skip for now
    COMPILER_VERSION := unknown
    COMPILER_FULL_VERSION := unknown
endif

# ============================================================================
# LTO Support
# ============================================================================

ifdef ENABLE_LTO
    ifeq ($(TOOLCHAIN_ID),$(filter $(TOOLCHAIN_ID),gcc mingw))
        CFLAGS += -flto
        CXXFLAGS += -flto
        LDFLAGS += -flto
        AR := gcc-ar
        RANLIB := gcc-ranlib
        ifneq ($(CROSS_COMPILE),)
            AR := $(CROSS_COMPILE)gcc-ar
            RANLIB := $(CROSS_COMPILE)gcc-ranlib
        endif
    else ifeq ($(TOOLCHAIN_ID),$(filter $(TOOLCHAIN_ID),clang apple-clang))
        CFLAGS += -flto=thin
        CXXFLAGS += -flto=thin
        LDFLAGS += -flto=thin
    else ifeq ($(TOOLCHAIN_ID),msvc)
        MSVC_CFLAGS += /GL
        MSVC_LDFLAGS += /LTCG
    endif
endif

# ============================================================================
# Debug/Release Builds
# ============================================================================

BUILD_TYPE := $(call cfg-unquote,$(CONFIG_BUILD_TYPE))
ifeq ($(BUILD_TYPE),)
    BUILD_TYPE := Debug
endif

ifeq ($(BUILD_TYPE),Debug)
    ifeq ($(TOOLCHAIN_ID),msvc)
        MSVC_CFLAGS += /Od /Zi /RTC1 /MDd
        MSVC_LDFLAGS += /DEBUG
    else
        CFLAGS += -O0 -g3 -DDEBUG
        CXXFLAGS += -O0 -g3 -DDEBUG
    endif
else ifeq ($(BUILD_TYPE),Release)
    ifeq ($(TOOLCHAIN_ID),msvc)
        MSVC_CFLAGS += /O2 /DNDEBUG /MD
    else
        CFLAGS += -O2 -DNDEBUG
        CXXFLAGS += -O2 -DNDEBUG
    endif
else ifeq ($(BUILD_TYPE),RelWithDebInfo)
    ifeq ($(TOOLCHAIN_ID),msvc)
        MSVC_CFLAGS += /O2 /Zi /DNDEBUG /MD
        MSVC_LDFLAGS += /DEBUG
    else
        CFLAGS += -O2 -g -DNDEBUG
        CXXFLAGS += -O2 -g -DNDEBUG
    endif
else ifeq ($(BUILD_TYPE),MinSizeRel)
    ifeq ($(TOOLCHAIN_ID),msvc)
        MSVC_CFLAGS += /O1 /DNDEBUG /MD
    else
        CFLAGS += -Os -DNDEBUG
        CXXFLAGS += -Os -DNDEBUG
    endif
endif

# ============================================================================
# Export
# ============================================================================

export CC CXX AR LD AS STRIP RANLIB NM OBJCOPY OBJDUMP READELF
export WINDRES DLLTOOL RC MT
export CFLAGS CXXFLAGS LDFLAGS
export MSVC_CFLAGS MSVC_CXXFLAGS MSVC_LDFLAGS MSVC_DEFINES
export TOOLCHAIN TOOLCHAIN_ID
export COMPILER_VERSION COMPILER_FULL_VERSION
export BUILD_TYPE

# ============================================================================
# Debug Output
# ============================================================================

ifdef TOOLCHAIN_DEBUG
$(info Toolchain Configuration:)
$(info   TOOLCHAIN=$(TOOLCHAIN) TOOLCHAIN_ID=$(TOOLCHAIN_ID))
$(info   CC=$(CC))
$(info   CXX=$(CXX))
$(info   AR=$(AR))
$(info   CFLAGS=$(CFLAGS))
$(info   CXXFLAGS=$(CXXFLAGS))
$(info   LDFLAGS=$(LDFLAGS))
$(info   COMPILER_VERSION=$(COMPILER_VERSION))
$(info   BUILD_TYPE=$(BUILD_TYPE))
endif
