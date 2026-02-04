# SPDX-License-Identifier: BSD-3-Clause
# LocalPeer - Makefile
#
# Build rules for single-instance application library.

# Library name
lib := localpeer

# Source files
lib-y := \
	src/LocalPeer.o \
	src/LockedFile.o

# Platform-specific sources
ifeq ($(TARGET_OS),windows)
    lib-y += src/LockedFile_win.o
else
    lib-y += src/LockedFile_unix.o
endif

# Include paths
includes-y := LocalPeer/include LocalPeer/src

# Qt MOC headers
moc-y := include/LocalPeer.h

# Qt dependencies
qt-modules-y := Core Network

# Compiler flags
cxxflags-y := $(QT_CFLAGS)

# Include common build rules
include $(srctree)/mk/module.mk
