# SPDX-License-Identifier: GPL-3.0
# systeminfo - Makefile
#
# Build rules for system information library.

# Library name
lib := systeminfo

# Source files (platform-specific selection)
ifeq ($(TARGET_OS),windows)
    lib-y := src/sys_win32.o
else ifeq ($(TARGET_OS),macos)
    lib-y := src/sys_apple.o
else
    # Linux, BSD, etc.
    lib-y := src/sys_unix.o src/distroutils.o
endif

# Include paths
includes-y := systeminfo/include systeminfo/src

# Qt dependencies
qt-modules-y := Core Gui Network

# Compiler flags
cxxflags-y := $(QT_CFLAGS)

# Platform-specific flags
ifeq ($(TARGET_OS),macos)
    LDFLAGS += -framework CoreServices -framework IOKit
endif
