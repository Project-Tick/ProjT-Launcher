# SPDX-License-Identifier: GPL-2.0
#
# Common build rules for all modules
#
# This file provides standard 'all' and 'clean' targets for modules.
# Each module Makefile should define:
#   lib      - Library name
#   lib-y    - Object files to build
#   includes-y - Include directories (optional)
#   ccflags-y  - Extra C compiler flags (optional)
#   cxxflags-y - Extra C++ compiler flags (optional)
#
# Then include this file at the end:
#   include $(srctree)/mk/module.mk

# Get module path relative to srctree
MODULE_PATH := $(patsubst $(srctree)/%,%,$(CURDIR))

# Default output directory
OBJDIR ?= $(KBUILD_OUTPUT)/obj/$(MODULE_PATH)
LIBDIR ?= $(KBUILD_OUTPUT)/lib

# Compiler settings
CC ?= gcc
CXX ?= g++
AR ?= ar
RANLIB ?= ranlib

# Base flags - adapt for MSVC vs GCC/Clang
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
BASE_CFLAGS := /nologo /W3 /EHsc /MD /O2
BASE_CXXFLAGS := $(BASE_CFLAGS) /std:c++17
else
BASE_CFLAGS := -O2 -g -fPIC -Wall
BASE_CXXFLAGS := $(BASE_CFLAGS) -std=c++17
endif

# Add include paths
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
INC_FLAGS := $(foreach dir,$(includes-y),/I$(srctree)/$(dir))
INC_FLAGS += /I$(srctree) /I$(KBUILD_OUTPUT)/include
else
INC_FLAGS := $(foreach dir,$(includes-y),-I$(srctree)/$(dir))
INC_FLAGS += -I$(srctree) -I$(KBUILD_OUTPUT)/include
endif

# Qt support - if module uses Qt, add Qt flags
ifdef qt-modules-y
QT_PREFIX ?= $(KBUILD_OUTPUT)/obj/qt/install
QT_PKG_CONFIG_PATH := $(QT_PREFIX)/lib/pkgconfig
QT_MODULES_PKG := $(foreach m,$(qt-modules-y),Qt6$(m))
QT_CFLAGS := $(shell PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --cflags $(QT_MODULES_PKG) 2>/dev/null || \
    echo "-I$(QT_PREFIX)/include $(foreach m,$(qt-modules-y),-I$(QT_PREFIX)/include/Qt$(m))")
QT_LIBS := $(shell PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --libs $(QT_MODULES_PKG) 2>/dev/null)
INC_FLAGS += $(QT_CFLAGS)
endif

# Final flags
CFLAGS := $(BASE_CFLAGS) $(INC_FLAGS) $(ccflags-y)
CXXFLAGS := $(BASE_CXXFLAGS) $(INC_FLAGS) $(cxxflags-y)
LDFLAGS ?=

# Object files with full path
OBJS := $(addprefix $(OBJDIR)/,$(lib-y))

# Library output
STATIC_LIB := $(LIBDIR)/lib$(lib).a

# Quiet/Verbose
ifeq ($(V),1)
Q :=
else
Q := @
endif

# ============================================================================
# TARGETS
# ============================================================================

.PHONY: all clean

all: $(STATIC_LIB)
	@echo "  Built $(lib)"

# Only define STATIC_LIB rule if not already defined by including Makefile
ifndef CUSTOM_STATIC_LIB_RULE
$(STATIC_LIB): $(OBJS) | $(LIBDIR)
ifneq ($(strip $(lib-y)),)
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
	@echo "  LIB     $@"
	$(Q)$(AR) /nologo /OUT:$@ $^
else
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@
endif
else
	@echo "  SKIP    $(lib) (header-only, no objects)"
	$(Q)touch $@
endif
endif

# Object compilation rules - search in current module directory
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
$(OBJDIR)/%.o: $(CURDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) /c /Fo$@ $<

$(OBJDIR)/%.o: $(CURDIR)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) /c /Fo$@ $<

$(OBJDIR)/%.o: $(CURDIR)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) /c /Fo$@ $<

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) /c /Fo$@ $<

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) /c /Fo$@ $<

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) /c /Fo$@ $<
else
$(OBJDIR)/%.o: $(CURDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(CURDIR)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(CURDIR)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) -c -o $@ $<
endif

# Directory creation
$(OBJDIR) $(LIBDIR):
	$(Q)mkdir -p $@

# Clean
clean:
	@echo "  CLEAN   $(lib)"
	$(Q)rm -rf $(OBJDIR)
	$(Q)rm -f $(STATIC_LIB)
