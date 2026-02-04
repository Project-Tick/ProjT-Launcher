# SPDX-License-Identifier: GPL-3.0
# program_info - Makefile
#
# Build rules for program info/resources module.

# This module generates platform-specific resources

# Include paths
includes-y := program_info $(GEN_DIR)/program_info

# ============================================================================
# Resource Generation
# ============================================================================

# Version info
VERSION_MAJOR ?= 0
VERSION_MINOR ?= 0  
VERSION_PATCH ?= 5
VERSION_TWEAK ?= 1
VERSION_NAME := $(VERSION_MAJOR).$(VERSION_MINOR).$(VERSION_PATCH).$(VERSION_TWEAK)

# Desktop file (Linux)
$(GEN_DIR)/program_info/org.projecttick.ProjTLauncher.desktop: $(srctree)/program_info/org.projecttick.ProjTLauncher.desktop.in
	@mkdir -p $(@D)
	@echo "  GEN     $@"
	$(Q)sed \
		-e 's|@Launcher_Name@|ProjT Launcher|g' \
		-e 's|@Launcher_APP_BINARY_NAME@|projt-launcher|g' \
		$< > $@

# Metainfo (Linux AppStream)
$(GEN_DIR)/program_info/org.projecttick.ProjTLauncher.metainfo.xml: $(srctree)/program_info/org.projecttick.ProjTLauncher.metainfo.xml.in
	@mkdir -p $(@D)
	@echo "  GEN     $@"
	$(Q)sed \
		-e 's|@Launcher_Name@|ProjT Launcher|g' \
		-e 's|@Launcher_VERSION_NAME@|$(VERSION_NAME)|g' \
		-e 's|@Launcher_BUILD_TIMESTAMP@|$(shell date +%Y-%m-%d)|g' \
		$< > $@

# Windows RC file
$(GEN_DIR)/program_info/projtlauncher.rc: $(srctree)/program_info/projtlauncher.rc.in
	@mkdir -p $(@D)
	@echo "  GEN     $@"
	$(Q)sed \
		-e 's|@Launcher_VERSION_NAME4_COMMA@|$(VERSION_MAJOR),$(VERSION_MINOR),$(VERSION_PATCH),$(VERSION_TWEAK)|g' \
		-e 's|@Launcher_VERSION_NAME4@|$(VERSION_NAME)|g' \
		-e 's|@Launcher_Name@|ProjT Launcher|g' \
		-e 's|@Launcher_Copyright@|© 2024 Project Tick|g' \
		-e 's|@Launcher_Domain@|projecttick.org|g' \
		$< > $@

# Windows manifest
$(GEN_DIR)/program_info/projtlauncher.manifest: $(srctree)/program_info/projtlauncher.manifest.in
	@mkdir -p $(@D)
	@echo "  GEN     $@"
	$(Q)sed \
		-e 's|@Launcher_VERSION_NAME4@|$(VERSION_NAME)|g' \
		-e 's|@Launcher_Name@|ProjT Launcher|g' \
		$< > $@

# Qt resource file
$(GEN_DIR)/program_info/projtlauncher.qrc: $(srctree)/program_info/projtlauncher.qrc.in
	@mkdir -p $(@D)
	@echo "  GEN     $@"
	$(Q)cp $< $@

# Targets based on platform
ifeq ($(TARGET_OS),linux)
    program-info-targets := \
        $(GEN_DIR)/program_info/org.projecttick.ProjTLauncher.desktop \
        $(GEN_DIR)/program_info/org.projecttick.ProjTLauncher.metainfo.xml
else ifeq ($(TARGET_OS),windows)
    program-info-targets := \
        $(GEN_DIR)/program_info/projtlauncher.rc \
        $(GEN_DIR)/program_info/projtlauncher.manifest
endif

program-info-targets += $(GEN_DIR)/program_info/projtlauncher.qrc

program_info: $(program-info-targets)

.PHONY: program_info
