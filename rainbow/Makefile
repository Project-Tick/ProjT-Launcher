# SPDX-License-Identifier: LGPL-2.1
# rainbow - Makefile
#
# Build rules for Rainbow Qt theming library.

# Library name
lib := rainbow

# Source files
lib-y := \
	src/rainbow.o

# Include paths
includes-y := rainbow/include rainbow/src

# Qt MOC headers
moc-y := include/rainbow.h

# Qt dependencies
qt-modules-y := Core Gui

# Compiler flags
cxxflags-y := $(QT_CFLAGS)

# Include common build rules
include $(srctree)/mk/module.mk
