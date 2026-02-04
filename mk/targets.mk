# SPDX-License-Identifier: GPL-2.0
#
# ProjT Launcher - Recursive Build System
#
# Each module manages its own Makefile. This file only
# calls modules in the correct dependency order.

-include $(KBUILD_OUTPUT)/.config

# Quiet/Verbose
ifeq ($(V),1)
Q :=
else
Q := @
endif

export srctree KBUILD_OUTPUT Q V

# ============================================================================
# MODULE DEFINITIONS
# ============================================================================

# Base libraries (no dependencies)
LIBS_TIER0 := \
	zlib \
	bzip2 \
	murmur2

# Tier 1: zlib-dependent
LIBS_TIER1 := \
	libpng \
	quazip

# Tier 2: Other base libraries
LIBS_TIER2 := \
	cmark \
	libnbtplusplus \
	libqrencode \
	tomlplusplus \
	json

# Tier 3: Qt-dependent modules
LIBS_TIER3 := \
	rainbow \
	qdcss \
	LocalPeer

# Platform-specific
ifeq ($(CONFIG_TARGET_LINUX),y)
LIBS_PLATFORM := gamemode
endif

# Project modules
PROJT_MODULES := \
	buildconfig \
	systeminfo \
	program_info

# Java modules
JAVA_MODULES := \
	javacheck \
	launcherjava

# Main launcher and submodules
LAUNCHER_SUBMODULES := \
	launcher/console \
	launcher/filelink \
	launcher/icons \
	launcher/java \
	launcher/launch \
	launcher/logs \
	launcher/meta \
	launcher/minecraft \
	launcher/modplatform \
	launcher/net \
	launcher/news \
	launcher/resources \
	launcher/screenshots \
	launcher/settings \
	launcher/tasks \
	launcher/tools \
	launcher/translations \
	launcher/ui \
	launcher/updater

# Test and fuzz
TEST_MODULES := tests fuzz

# All modules
ALL_MODULES := \
	$(LIBS_TIER0) \
	$(LIBS_TIER1) \
	$(LIBS_TIER2) \
	$(LIBS_TIER3) \
	$(LIBS_PLATFORM) \
	$(PROJT_MODULES) \
	$(JAVA_MODULES) \
	$(LAUNCHER_SUBMODULES) \
	launcher

# ============================================================================
# RECURSIVE BUILD INFRASTRUCTURE
# ============================================================================

# Module build command
define make-module
	@echo "  BUILD   $(1)"
	$(Q)if [ -f "$(srctree)/$(1)/Makefile" ]; then \
		$(MAKE) -C $(srctree)/$(1) \
			srctree=$(srctree) \
			KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
			Q=$(Q) V=$(V) \
			all 2>&1 | sed 's/^/    /' || exit 1; \
	else \
		echo "    SKIP  $(1) (no Makefile)"; \
	fi
endef

# Module clean command
define clean-module
	@echo "  CLEAN   $(1)"
	$(Q)if [ -f "$(srctree)/$(1)/Makefile" ]; then \
		$(MAKE) -C $(srctree)/$(1) \
			srctree=$(srctree) \
			KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
			clean 2>/dev/null || true; \
	fi
endef

# ============================================================================
# BUILD TARGETS
# ============================================================================

.PHONY: all build libs launcher java tests clean help

# Main target
all: build

# Full build
build: libs java launcher
	@echo ""
	@echo "Build complete!"
	@echo "Output: $(KBUILD_OUTPUT)"

# Libraries (ordered by tier)
libs: libs-tier0 libs-tier1 libs-tier2 libs-tier3 libs-platform projt-modules

libs-tier0:
	@echo "=== Building Tier 0 Libraries (no deps) ==="
	$(foreach mod,$(LIBS_TIER0),$(call make-module,$(mod)))

libs-tier1: libs-tier0
	@echo "=== Building Tier 1 Libraries (zlib deps) ==="
	$(foreach mod,$(LIBS_TIER1),$(call make-module,$(mod)))

libs-tier2: libs-tier0
	@echo "=== Building Tier 2 Libraries ==="
	$(foreach mod,$(LIBS_TIER2),$(call make-module,$(mod)))

libs-tier3: libs-tier0 libs-tier1 libs-tier2
	@echo "=== Building Tier 3 Libraries (Qt deps) ==="
	$(foreach mod,$(LIBS_TIER3),$(call make-module,$(mod)))

libs-platform:
ifdef LIBS_PLATFORM
	@echo "=== Building Platform Libraries ==="
	$(foreach mod,$(LIBS_PLATFORM),$(call make-module,$(mod)))
endif

projt-modules: libs-tier0 libs-tier1 libs-tier2 libs-tier3
	@echo "=== Building ProjT Modules ==="
	$(foreach mod,$(PROJT_MODULES),$(call make-module,$(mod)))

# Java
java:
	@echo "=== Building Java Modules ==="
	$(foreach mod,$(JAVA_MODULES),$(call make-module,$(mod)))

# Launcher (including all submodules)
launcher: libs projt-modules launcher-submodules launcher-main

launcher-submodules:
	@echo "=== Building Launcher Submodules ==="
	$(foreach mod,$(LAUNCHER_SUBMODULES),$(call make-module,$(mod)))

launcher-main: launcher-submodules
	@echo "=== Building Launcher Main ==="
	$(call make-module,launcher)

# Tests
tests: build
	@echo "=== Building Tests ==="
	$(foreach mod,$(TEST_MODULES),$(call make-module,$(mod)))

# ============================================================================
# INDIVIDUAL MODULE TARGETS
# ============================================================================

# Create separate target for each module
define module-target
.PHONY: $(notdir $(1)) $(1)
$(notdir $(1)) $(1):
	$$(call make-module,$(1))

$(notdir $(1))-clean $(1)-clean:
	$$(call clean-module,$(1))
endef

$(foreach mod,$(ALL_MODULES),$(eval $(call module-target,$(mod))))

# ============================================================================
# CLEAN TARGETS
# ============================================================================

clean:
	@echo "=== Cleaning All Modules ==="
	$(foreach mod,$(ALL_MODULES),$(call clean-module,$(mod)))
	$(foreach mod,$(TEST_MODULES),$(call clean-module,$(mod)))
	$(Q)rm -rf $(KBUILD_OUTPUT)/bin
	$(Q)rm -rf $(KBUILD_OUTPUT)/lib
	$(Q)rm -rf $(KBUILD_OUTPUT)/obj
	$(Q)rm -rf $(KBUILD_OUTPUT)/jars
	@echo "Clean complete"

# ============================================================================
# UTILITY TARGETS
# ============================================================================

# Module list
list-modules:
	@echo "Available modules:"
	@echo ""
	@echo "Libraries Tier 0: $(LIBS_TIER0)"
	@echo "Libraries Tier 1: $(LIBS_TIER1)"
	@echo "Libraries Tier 2: $(LIBS_TIER2)"
	@echo "Libraries Tier 3: $(LIBS_TIER3)"
	@echo "Platform:         $(LIBS_PLATFORM)"
	@echo "ProjT:            $(PROJT_MODULES)"
	@echo "Java:             $(JAVA_MODULES)"
	@echo "Launcher:         $(LAUNCHER_SUBMODULES)"
	@echo "Tests:            $(TEST_MODULES)"

# Dependency graph (DOT format)
deps-graph:
	@echo "digraph deps {"
	@echo "  rankdir=LR;"
	@echo "  # Tier 0 - no deps"
	@for m in $(LIBS_TIER0); do echo "  \"$$m\";"; done
	@echo "  # Tier 1 - zlib deps"
	@for m in $(LIBS_TIER1); do echo "  \"zlib\" -> \"$$m\";"; done
	@echo "  # Tier 2"
	@for m in $(LIBS_TIER2); do echo "  \"$$m\";"; done
	@echo "  # Tier 3 - Qt deps"
	@for m in $(LIBS_TIER3); do echo "  \"qt\" -> \"$$m\";"; done
	@echo "  # Launcher submodules"
	@for m in $(LAUNCHER_SUBMODULES); do echo "  \"libs\" -> \"$$m\";"; done
	@echo "  # Main launcher"
	@for m in $(LAUNCHER_SUBMODULES); do echo "  \"$$m\" -> \"launcher\";"; done
	@echo "}"

# Help
help:
	@echo "ProjT Launcher Recursive Build System"
	@echo ""
	@echo "Main targets:"
	@echo "  all / build    - Build everything"
	@echo "  libs           - Build all libraries"
	@echo "  launcher       - Build launcher with submodules"
	@echo "  java           - Build Java modules"
	@echo "  tests          - Build tests"
	@echo "  clean          - Clean all modules"
	@echo ""
	@echo "Individual modules:"
	@echo "  make <module>       - Build specific module"
	@echo "  make <module>-clean - Clean specific module"
	@echo ""
	@echo "Utility:"
	@echo "  list-modules   - Show all modules"
	@echo "  deps-graph     - Generate DOT dependency graph"

.PHONY: libs-tier0 libs-tier1 libs-tier2 libs-tier3 libs-platform projt-modules
.PHONY: launcher-submodules launcher-main list-modules deps-graph help
