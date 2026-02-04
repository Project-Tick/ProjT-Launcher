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

# Base flags
BASE_CFLAGS := -O2 -g -fPIC -Wall
BASE_CXXFLAGS := $(BASE_CFLAGS) -std=c++17

# Add include paths
INC_FLAGS := $(foreach dir,$(includes-y),-I$(srctree)/$(dir))
INC_FLAGS += -I$(srctree) -I$(KBUILD_OUTPUT)/include

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

$(STATIC_LIB): $(OBJS) | $(LIBDIR)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@

# Object compilation rules - search in current module directory
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

# Also try srctree-relative paths for modules that specify paths that way
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

# Directory creation
$(OBJDIR) $(LIBDIR):
	$(Q)mkdir -p $@

# Clean
clean:
	@echo "  CLEAN   $(lib)"
	$(Q)rm -rf $(OBJDIR)
	$(Q)rm -f $(STATIC_LIB)
