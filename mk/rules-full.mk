# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Comprehensive Build Rules
#
# This file provides build rules, pattern rules, and helper macros
# for the entire build system.

include mk/toolchain-full.mk
include mk/qt-tools.mk

# ============================================================================
# Directory Layout
# ============================================================================

OBJDIR := $(KBUILD_OUTPUT)/obj
LIBDIR := $(KBUILD_OUTPUT)/lib
BINDIR := $(KBUILD_OUTPUT)/bin
JARDIR := $(KBUILD_OUTPUT)/jars
GEN_DIR := $(KBUILD_OUTPUT)/generated
DEPDIR := $(KBUILD_OUTPUT)/deps

# Include paths for generated files
CPPFLAGS += -I$(KBUILD_OUTPUT)/include -I$(KBUILD_OUTPUT)/include/generated
CPPFLAGS += -I$(GEN_DIR)

# ============================================================================
# Path Utilities
# ============================================================================

# Convert source path to relative
src-rel = $(patsubst $(srctree)/%,%,$(1))

# Source -> object mapping (handles both srctree and generated sources)
src-to-obj = $(foreach s,$(1),\
	$(if $(filter $(KBUILD_OUTPUT)/%,$(s)),\
		$(OBJDIR)/$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.cxx,%.o,$(patsubst %.mm,%.o,$(patsubst %.m,%.o,$(patsubst $(KBUILD_OUTPUT)/%,%,$(s))))))))),\
		$(OBJDIR)/$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.cxx,%.o,$(patsubst %.mm,%.o,$(patsubst %.m,%.o,$(call src-rel,$(s))))))))\
	))

# Header -> moc source
hdr-to-moc = $(patsubst %.h,$(GEN_DIR)/moc_%.cpp,$(notdir $(1)))

# UI -> header
ui-to-hdr = $(patsubst %.ui,$(GEN_DIR)/ui_%.h,$(notdir $(1)))

# QRC -> source
qrc-to-cpp = $(patsubst %.qrc,$(GEN_DIR)/qrc_%.cpp,$(notdir $(1)))

# ============================================================================
# Pattern Rules - C Files
# ============================================================================

$(OBJDIR)/%.o: $(srctree)/%.c
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(CC) $(CPPFLAGS) $(CFLAGS) $($*_CFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@,\
		$(CC) $(MSVC_CFLAGS) $(MSVC_DEFINES) /Fo$@ /c $<)

$(OBJDIR)/%.o: $(KBUILD_OUTPUT)/%.c
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(CC) $(CPPFLAGS) $(CFLAGS) $($*_CFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@,\
		$(CC) $(MSVC_CFLAGS) $(MSVC_DEFINES) /Fo$@ /c $<)

# ============================================================================
# Pattern Rules - C++ Files
# ============================================================================

$(OBJDIR)/%.o: $(srctree)/%.cpp
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_CXXFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@,\
		$(CXX) $(MSVC_CXXFLAGS) $(MSVC_DEFINES) /Fo$@ /c $<)

$(OBJDIR)/%.o: $(srctree)/%.cc
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_CXXFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@,\
		$(CXX) $(MSVC_CXXFLAGS) $(MSVC_DEFINES) /Fo$@ /c $<)

$(OBJDIR)/%.o: $(srctree)/%.cxx
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_CXXFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@,\
		$(CXX) $(MSVC_CXXFLAGS) $(MSVC_DEFINES) /Fo$@ /c $<)

$(OBJDIR)/%.o: $(KBUILD_OUTPUT)/%.cpp
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(CXX) $(CPPFLAGS) $(CXXFLAGS) $($*_CXXFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@,\
		$(CXX) $(MSVC_CXXFLAGS) $(MSVC_DEFINES) /Fo$@ /c $<)

# ============================================================================
# Pattern Rules - Objective-C/C++ (macOS)
# ============================================================================

$(OBJDIR)/%.o: $(srctree)/%.m
	@mkdir -p $(dir $@)
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJCFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@

$(OBJDIR)/%.o: $(srctree)/%.mm
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OBJCXXFLAGS) -MMD -MP -MF $(DEPDIR)/$*.d -c $< -o $@

# ============================================================================
# Pattern Rules - Windows Resources
# ============================================================================

ifeq ($(TARGET_OS),windows)
$(OBJDIR)/%.res: $(srctree)/%.rc
	@mkdir -p $(dir $@)
	$(Q)$(if $(TOOLCHAIN_ID:msvc=),\
		$(WINDRES) -i $< -o $@,\
		$(RC) /fo$@ $<)

$(OBJDIR)/%.o: $(OBJDIR)/%.res
	@mkdir -p $(dir $@)
	$(Q)$(if $(WINDRES),$(WINDRES) -i $< -o $@,cp $< $@)
endif

# ============================================================================
# Qt Tool Rules
# ============================================================================

# MOC: Meta-Object Compiler
$(GEN_DIR)/moc_%.cpp: $(srctree)/%.h | $(GEN_DIR)
	@mkdir -p $(dir $@)
	$(Q)$(QT_MOC) $(QT_MOC_FLAGS) $< -o $@

$(GEN_DIR)/%_moc.cpp: $(srctree)/%.cpp | $(GEN_DIR)
	@mkdir -p $(dir $@)
	$(Q)$(QT_MOC) $(QT_MOC_FLAGS) $< -o $@

# UIC: User Interface Compiler
$(GEN_DIR)/ui_%.h: $(srctree)/%.ui | $(GEN_DIR)
	@mkdir -p $(dir $@)
	$(Q)$(QT_UIC) $< -o $@

# RCC: Resource Compiler
$(GEN_DIR)/qrc_%.cpp: $(srctree)/%.qrc | $(GEN_DIR)
	@mkdir -p $(dir $@)
	$(Q)$(QT_RCC) $< -o $@ --name $(basename $(notdir $<))

$(GEN_DIR):
	@mkdir -p $@

# ============================================================================
# Library Build Macros
# ============================================================================

# Build a static library
# Usage: $(eval $(call build-static-lib,name))
define build-static-lib
$(1)_OBJS := $$(call src-to-obj,$$($(1)_SRCS))
$(1)_DEPS := $$($(1)_OBJS:.o=.d)

$$($(1)_OBJS): CPPFLAGS += $$($(1)_CPPFLAGS)
$$($(1)_OBJS): CFLAGS += $$($(1)_CFLAGS)
$$($(1)_OBJS): CXXFLAGS += $$($(1)_CXXFLAGS)

$$(LIBDIR)/lib$(1).a: $$($(1)_OBJS) $$($(1)_EXTRA_DEPS)
	@mkdir -p $$(dir $$@)
	$$(Q)$$(AR) rcs $$@ $$($(1)_OBJS)
	$$(Q)$$(RANLIB) $$@ 2>/dev/null || true

-include $$($(1)_DEPS)
endef

# Build a shared library
# Usage: $(eval $(call build-shared-lib,name,version))
define build-shared-lib
$(1)_OBJS := $$(call src-to-obj,$$($(1)_SRCS))
$(1)_DEPS := $$($(1)_OBJS:.o=.d)

$$($(1)_OBJS): CPPFLAGS += $$($(1)_CPPFLAGS)
$$($(1)_OBJS): CFLAGS += $$($(1)_CFLAGS) -fPIC
$$($(1)_OBJS): CXXFLAGS += $$($(1)_CXXFLAGS) -fPIC

$$(LIBDIR)/$(DLL_PREFIX)$(1)$(DLL_SUFFIX): $$($(1)_OBJS) $$($(1)_EXTRA_DEPS)
	@mkdir -p $$(dir $$@)
	$$(Q)$$(CXX) -shared $$(SONAME_FLAG) $$(LDFLAGS) $$($(1)_LDFLAGS) -o $$@ $$($(1)_OBJS) $$($(1)_LDLIBS)

-include $$($(1)_DEPS)
endef

# ============================================================================
# Executable Build Macros
# ============================================================================

# Build an executable
# Usage: $(eval $(call build-exe,name))
define build-exe
$(1)_OBJS := $$(call src-to-obj,$$($(1)_SRCS))
$(1)_DEPS := $$($(1)_OBJS:.o=.d)

$$($(1)_OBJS): CPPFLAGS += $$($(1)_CPPFLAGS)
$$($(1)_OBJS): CFLAGS += $$($(1)_CFLAGS)
$$($(1)_OBJS): CXXFLAGS += $$($(1)_CXXFLAGS)

$$(BINDIR)/$(1)$(EXE_SUFFIX): $$($(1)_OBJS) $$($(1)_LIBS) $$($(1)_EXTRA_DEPS)
	@mkdir -p $$(dir $$@)
	$$(Q)$$(if $$(TOOLCHAIN_ID:msvc=),\
		$$(CXX) $$($(1)_OBJS) $$(LDFLAGS) $$($(1)_LDFLAGS) $$($(1)_LDLIBS) -o $$@,\
		$$(LD) $$(MSVC_LDFLAGS) /OUT:$$@ $$($(1)_OBJS) $$($(1)_LDLIBS))

-include $$($(1)_DEPS)
endef

# Build a GUI executable (Windows subsystem on Windows)
# Usage: $(eval $(call build-gui-exe,name))
define build-gui-exe
$(1)_OBJS := $$(call src-to-obj,$$($(1)_SRCS))
$(1)_DEPS := $$($(1)_OBJS:.o=.d)

$$($(1)_OBJS): CPPFLAGS += $$($(1)_CPPFLAGS)
$$($(1)_OBJS): CFLAGS += $$($(1)_CFLAGS)
$$($(1)_OBJS): CXXFLAGS += $$($(1)_CXXFLAGS)

$$(BINDIR)/$(1)$(EXE_SUFFIX): $$($(1)_OBJS) $$($(1)_LIBS) $$($(1)_EXTRA_DEPS)
	@mkdir -p $$(dir $$@)
ifeq ($(TARGET_OS),windows)
	$$(Q)$$(if $$(TOOLCHAIN_ID:msvc=),\
		$$(CXX) $$($(1)_OBJS) -mwindows $$(LDFLAGS) $$($(1)_LDFLAGS) $$($(1)_LDLIBS) -o $$@,\
		$$(LD) $$(MSVC_LDFLAGS) /SUBSYSTEM:WINDOWS /OUT:$$@ $$($(1)_OBJS) $$($(1)_LDLIBS))
else ifeq ($(TARGET_OS),macos)
	$$(Q)$$(CXX) $$($(1)_OBJS) $$(LDFLAGS) $$($(1)_LDFLAGS) $$($(1)_LDLIBS) -o $$@
else
	$$(Q)$$(CXX) $$($(1)_OBJS) $$(LDFLAGS) $$($(1)_LDFLAGS) $$($(1)_LDLIBS) -o $$@
endif

-include $$($(1)_DEPS)
endef

# ============================================================================
# macOS App Bundle Macro
# ============================================================================

define build-macos-bundle
$(1).app: $$(BINDIR)/$(1)$(EXE_SUFFIX)
	@mkdir -p $(1).app/Contents/MacOS
	@mkdir -p $(1).app/Contents/Resources
	@cp $$(BINDIR)/$(1)$(EXE_SUFFIX) $(1).app/Contents/MacOS/$(1)
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > $(1).app/Contents/Info.plist
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(1).app/Contents/Info.plist
	@echo '<plist version="1.0"><dict>' >> $(1).app/Contents/Info.plist
	@echo '<key>CFBundleExecutable</key><string>$(1)</string>' >> $(1).app/Contents/Info.plist
	@echo '<key>CFBundleIdentifier</key><string>$($(1)_BUNDLE_ID)</string>' >> $(1).app/Contents/Info.plist
	@echo '<key>CFBundleName</key><string>$($(1)_BUNDLE_NAME)</string>' >> $(1).app/Contents/Info.plist
	@echo '<key>CFBundleVersion</key><string>$($(1)_VERSION)</string>' >> $(1).app/Contents/Info.plist
	@echo '</dict></plist>' >> $(1).app/Contents/Info.plist
endef

# ============================================================================
# Source Finding Utilities
# ============================================================================

# Find all C/C++ sources in a directory (excluding common non-source dirs)
find-src = $(shell find $(1) -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' \) \
	-not -path '*/tests/*' -not -path '*/test/*' -not -path '*/bench*/*' \
	-not -path '*/examples/*' -not -path '*/docs/*' -not -path '*/man/*' \
	-not -path '*/cmake/*' -not -path '*/CMakeFiles/*' -not -path '*/.git/*' \
	2>/dev/null)

# Find all headers with Q_OBJECT for moc
find-moc-hdrs = $(shell grep -l Q_OBJECT $(1)/*.h 2>/dev/null)

# Find all .ui files
find-ui = $(shell find $(1) -name '*.ui' 2>/dev/null)

# Find all .qrc files
find-qrc = $(shell find $(1) -name '*.qrc' 2>/dev/null)

# ============================================================================
# Dependency File Handling
# ============================================================================

# Include all generated dependency files
-include $(shell find $(DEPDIR) -name '*.d' 2>/dev/null)

# ============================================================================
# Export
# ============================================================================

export OBJDIR LIBDIR BINDIR JARDIR GEN_DIR DEPDIR
