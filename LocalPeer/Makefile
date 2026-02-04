# SPDX-License-Identifier: BSD-3-Clause
# LocalPeer - Makefile
#
# Build rules for single-instance application library.

# Library name
lib := localpeer

# Define OBJDIR early for moc rules
OBJDIR := $(KBUILD_OUTPUT)/obj/LocalPeer

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

# Qt tools
QT_PREFIX ?= $(KBUILD_OUTPUT)/obj/qt/install
MOC := $(QT_PREFIX)/libexec/moc

# Add moc object to library (will be built by rules below)
# This must be done before include module.mk
EXTRA_OBJS := $(OBJDIR)/moc_LocalPeer.o

# MOC rules
$(OBJDIR)/moc_LocalPeer.cpp: $(CURDIR)/include/LocalPeer.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/LocalPeer/include $< -o $@

$(OBJDIR)/moc_LocalPeer.o: $(OBJDIR)/moc_LocalPeer.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(BASE_CXXFLAGS) $(QT_CFLAGS) -I$(srctree)/LocalPeer/include -c -o $@ $<

# Include common build rules
include $(srctree)/mk/module.mk

# Override library to include MOC objects
$(STATIC_LIB): $(OBJS) $(EXTRA_OBJS) | $(LIBDIR)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@
