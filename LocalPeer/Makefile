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

# Qt tools - use exported values from mk/qt.mk
MOC := $(QT_MOC)
ifeq ($(MOC),)
# Fallback to PATH if parent did not export a tool path.
MOC := moc
endif

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

# Mark that we have custom STATIC_LIB rule
CUSTOM_STATIC_LIB_RULE := 1

# Include common build rules
include $(srctree)/mk/module.mk

# Custom library rule to include MOC objects
$(STATIC_LIB): $(OBJS) $(EXTRA_OBJS) | $(LIBDIR)
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
	@echo "  LIB     $@"
	$(Q)$(AR) /nologo /OUT:$@ $^
else
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
ifneq ($(strip $(RANLIB)),)
	$(Q)$(RANLIB) $@
endif
endif
