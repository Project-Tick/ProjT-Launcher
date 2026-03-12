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

# On Windows+MinGW, normalize key paths to Windows form (D:/...) to avoid
# MSYS path conversion mismatches in native toolchain components.
MODULE_SRCDIR := $(CURDIR)
MODULE_LIBSRCDIR := $(srctree)/$(lib)
MODULE_ROOT := $(srctree)
ifeq ($(TARGET_OS),windows)
ifeq ($(WINDOWS_TOOLCHAIN),mingw)
CYGPATH := $(shell command -v cygpath 2>/dev/null)
ifneq ($(strip $(CYGPATH)),)
OBJDIR := $(shell cygpath -m "$(OBJDIR)")
LIBDIR := $(shell cygpath -m "$(LIBDIR)")
MODULE_SRCDIR := $(shell cygpath -m "$(MODULE_SRCDIR)")
MODULE_LIBSRCDIR := $(shell cygpath -m "$(MODULE_LIBSRCDIR)")
MODULE_ROOT := $(shell cygpath -m "$(MODULE_ROOT)")
endif
endif
endif

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
BASE_CXXFLAGS := $(BASE_CFLAGS) /std:c++17 /Zc:__cplusplus /permissive-
else
BASE_CFLAGS := -O2 -g -fPIC -Wall
BASE_CXXFLAGS := $(BASE_CFLAGS) -std=c++17
endif

# Add include paths
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
INC_FLAGS := $(foreach dir,$(includes-y),/I$(MODULE_ROOT)/$(dir))
INC_FLAGS += /I$(MODULE_ROOT) /I$(KBUILD_OUTPUT)/include
else
INC_FLAGS := $(foreach dir,$(includes-y),-I$(MODULE_ROOT)/$(dir))
INC_FLAGS += -I$(MODULE_ROOT) -I$(KBUILD_OUTPUT)/include
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
QT_IMPORTED_CFLAGS := $(strip $(QT_CFLAGS))
QT_IMPORTED_LIBS := $(strip $(QT_LIBS))
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
ifneq ($(QT_IMPORTED_CFLAGS),)
QT_MODULE_CFLAGS := $(QT_IMPORTED_CFLAGS)
else
QT_MODULE_CFLAGS := /I$(QT_PREFIX)/include $(foreach m,$(qt-modules-y),/I$(QT_PREFIX)/include/Qt$(m))
endif
QT_MODULE_LIBS := $(QT_IMPORTED_LIBS)
else
QT_MODULE_CFLAGS := $(strip $(shell \
	PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --cflags $(QT_MODULES_PKG) 2>/dev/null || \
	pkg-config --cflags $(QT_MODULES_PKG) 2>/dev/null))
QT_MODULE_LIBS := $(strip $(shell \
	PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --libs $(QT_MODULES_PKG) 2>/dev/null || \
	pkg-config --libs $(QT_MODULES_PKG) 2>/dev/null))
ifeq ($(strip $(QT_MODULE_CFLAGS)),)
  QT_HEADERS_DIR := $(strip $(shell \
    if [ -x "$(QT_PREFIX)/bin/qtpaths6" ]; then \
      "$(QT_PREFIX)/bin/qtpaths6" --query QT_INSTALL_HEADERS 2>/dev/null; \
    elif [ -x "$(QT_PREFIX)/bin/qtpaths" ]; then \
      "$(QT_PREFIX)/bin/qtpaths" --query QT_INSTALL_HEADERS 2>/dev/null; \
    elif command -v qtpaths6 >/dev/null 2>&1; then \
      qtpaths6 --query QT_INSTALL_HEADERS 2>/dev/null; \
    elif command -v qtpaths >/dev/null 2>&1; then \
      qtpaths --query QT_INSTALL_HEADERS 2>/dev/null; \
    fi))
  QT_HEADERS_OK := $(strip $(shell \
    ok=1; h="$(QT_HEADERS_DIR)"; \
    [ -n "$$h" ] && [ -d "$$h" ] || ok=0; \
    for m in $(qt-modules-y); do \
      [ -d "$$h/Qt$$m" ] || { ok=0; break; }; \
    done; \
    [ "$$ok" -eq 1 ] && echo y))
  ifeq ($(QT_HEADERS_OK),y)
    QT_MODULE_CFLAGS := -I$(QT_HEADERS_DIR) $(foreach m,$(qt-modules-y),-I$(QT_HEADERS_DIR)/Qt$(m))
  endif
endif
ifeq ($(strip $(QT_MODULE_CFLAGS)),)
  QT_SYS_PREFIX := $(strip $(shell \
    for d in "$(QT_PREFIX)" "$(QT_PREFIX)"/* "$(QT_PREFIX)"/*/* /usr/include/qt6 /usr/include/*-linux-gnu/qt6 /usr/lib/qt6 /usr/lib64/qt6 /usr/lib/*-linux-gnu/qt6 /usr/local/qt6 /usr/local/lib/qt6 /opt/qt6 "$$HOME/Qt"/*/* ; do \
      [ -d "$$d" ] || continue; \
      if [ -d "$$d/include" ]; then h="$$d/include"; \
      elif [ -d "$$d/QtCore" ]; then h="$$d"; \
      else continue; fi; \
      ok=1; \
      for m in $(qt-modules-y); do \
        [ -d "$$h/Qt$$m" ] || { ok=0; break; }; \
      done; \
      if [ "$$ok" -eq 1 ]; then echo "$$d"; break; fi; \
    done))
  ifneq ($(strip $(QT_SYS_PREFIX)),)
    ifneq ($(wildcard $(QT_SYS_PREFIX)/include/QtCore),)
      QT_MODULE_CFLAGS := -I$(QT_SYS_PREFIX)/include $(foreach m,$(qt-modules-y),-I$(QT_SYS_PREFIX)/include/Qt$(m))
    else
      QT_MODULE_CFLAGS := -I$(QT_SYS_PREFIX) $(foreach m,$(qt-modules-y),-I$(QT_SYS_PREFIX)/Qt$(m))
    endif
  else ifneq ($(QT_IMPORTED_CFLAGS),)
    QT_MODULE_CFLAGS := $(QT_IMPORTED_CFLAGS)
  else
    QT_MODULE_CFLAGS := -I$(QT_PREFIX)/include $(foreach m,$(qt-modules-y),-I$(QT_PREFIX)/include/Qt$(m))
  endif
endif
ifneq ($(strip $(QT_MODULE_CFLAGS)),)
ifneq ($(strip $(QT_IMPORTED_LIBS)),)
ifeq ($(strip $(QT_MODULE_LIBS)),)
QT_MODULE_LIBS := $(QT_IMPORTED_LIBS)
endif
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

$(OBJDIR)/%.o: $(MODULE_SRCDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_C)

$(OBJDIR)/%.o: $(MODULE_SRCDIR)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

$(OBJDIR)/%.o: $(MODULE_SRCDIR)/%.cc | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

$(OBJDIR)/%.o: $(MODULE_LIBSRCDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_C)

$(OBJDIR)/%.o: $(MODULE_LIBSRCDIR)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(COMPILE_CXX)

$(OBJDIR)/%.o: $(MODULE_LIBSRCDIR)/%.cc | $(OBJDIR)
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
