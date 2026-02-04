# SPDX-License-Identifier: BSD-3-Clause
# gamemode - Makefile
#
# Build rules for GameMode integration (Linux only).

# Only build on Linux
ifeq ($(TARGET_OS),linux)

# Library name
lib := gamemode_client

# We use the header-only client
# Source files - create a stub
lib-y := 

# Just provide include path for header-only usage
includes-y := gamemode/lib

# Compiler flags
ccflags-y := -DGAMEMODE_AUTO

endif # TARGET_OS == linux
