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
# Ignore inherited OBJDIR from parent environment unless module Makefile
# explicitly sets it before including this file.
ifneq ($(filter file override,$(origin OBJDIR)),)
  # Keep module-defined OBJDIR.
else
  OBJDIR := $(KBUILD_OUTPUT)/obj/$(MODULE_PATH)
endif
LIBDIR ?= $(KBUILD_OUTPUT)/lib

# Compiler settings
CC ?= gcc
CXX ?= g++
AR ?= ar
RANLIB ?= ranlib

# Preserve top-level toolchain/config flags passed by parent make.
PARENT_CFLAGS := $(strip $(CFLAGS))
PARENT_CXXFLAGS := $(strip $(CXXFLAGS))
PARENT_LDFLAGS := $(strip $(LDFLAGS))

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
cfg-unquote = $(strip $(subst ",,$(1)))
QT_PREFIX ?= $(call cfg-unquote,$(CONFIG_QT_PREFIX))
ifeq ($(strip $(QT_PREFIX)),)
QT_PREFIX := $(KBUILD_OUTPUT)/obj/qt/install
endif
QT_PKG_CONFIG_PATH := $(QT_PREFIX)/lib/pkgconfig
QT_MODULES_PKG := $(foreach m,$(qt-modules-y),Qt6$(m))
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
QT_MODULE_CFLAGS := /I$(QT_PREFIX)/include $(foreach m,$(qt-modules-y),/I$(QT_PREFIX)/include/Qt$(m))
QT_MODULE_LIBS :=
else
QT_MODULE_CFLAGS := $(strip $(shell \
	PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --cflags $(QT_MODULES_PKG) 2>/dev/null || \
	pkg-config --cflags $(QT_MODULES_PKG) 2>/dev/null))
QT_MODULE_LIBS := $(strip $(shell \
	PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --libs $(QT_MODULES_PKG) 2>/dev/null || \
	pkg-config --libs $(QT_MODULES_PKG) 2>/dev/null))
ifeq ($(strip $(QT_MODULE_CFLAGS)),)
  QT_SYS_PREFIX := $(strip $(shell \
    for d in "$(QT_PREFIX)" /usr/include/qt6 /usr/include/*-linux-gnu/qt6 /usr/lib/qt6 /usr/lib64/qt6 /usr/local/qt6 /opt/qt6 ; do \
      if [ -d "$$d/include/QtCore" ]; then echo "$$d"; break; fi; \
      if [ -d "$$d/QtCore" ]; then echo "$$d"; break; fi; \
    done))
  ifneq ($(strip $(QT_SYS_PREFIX)),)
    ifneq ($(wildcard $(QT_SYS_PREFIX)/include/QtCore),)
      QT_MODULE_CFLAGS := -I$(QT_SYS_PREFIX)/include $(foreach m,$(qt-modules-y),-I$(QT_SYS_PREFIX)/include/Qt$(m))
    else
      QT_MODULE_CFLAGS := -I$(QT_SYS_PREFIX) $(foreach m,$(qt-modules-y),-I$(QT_SYS_PREFIX)/Qt$(m))
    endif
  else ifneq ($(strip $(QT_CFLAGS)),)
    QT_MODULE_CFLAGS := $(QT_CFLAGS)
  else
    QT_MODULE_CFLAGS := -I$(QT_PREFIX)/include $(foreach m,$(qt-modules-y),-I$(QT_PREFIX)/include/Qt$(m))
  endif
endif
endif
QT_CFLAGS := $(QT_MODULE_CFLAGS)
QT_LIBS := $(if $(strip $(QT_MODULE_LIBS)),$(QT_MODULE_LIBS),$(QT_LIBS))
INC_FLAGS += $(QT_MODULE_CFLAGS)
endif

# Final flags
override CFLAGS := $(strip $(BASE_CFLAGS) $(PARENT_CFLAGS) $(INC_FLAGS) $(ccflags-y))
override CXXFLAGS := $(strip $(BASE_CXXFLAGS) $(PARENT_CXXFLAGS) $(INC_FLAGS) $(cxxflags-y))
override LDFLAGS := $(strip $(PARENT_LDFLAGS))

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
ifneq ($(strip $(RANLIB)),)
	$(Q)$(RANLIB) $@
endif
endif
else
	@echo "  SKIP    $(lib) (header-only, no objects)"
	$(Q)touch $@
endif
endif

# Object compilation rules - MSVC vs GCC/Clang
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
COMPILE_C   = $(CC) $(CFLAGS) /c /Fo$@ $<
COMPILE_CXX = $(CXX) $(CXXFLAGS) /c /Fo$@ $<
else
COMPILE_C   = $(CC) $(CFLAGS) -c -o $@ $<
COMPILE_CXX = $(CXX) $(CXXFLAGS) -c -o $@ $<
endif

$(OBJDIR)/%.o: $(CURDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_C)

$(OBJDIR)/%.o: $(CURDIR)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

$(OBJDIR)/%.o: $(CURDIR)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_C)

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

$(OBJDIR)/%.o: $(srctree)/$(lib)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

# Directory creation
$(OBJDIR) $(LIBDIR):
	$(Q)mkdir -p $@

# Clean
clean:
	@echo "  CLEAN   $(lib)"
	$(Q)rm -rf $(OBJDIR)
	$(Q)rm -f $(STATIC_LIB)
