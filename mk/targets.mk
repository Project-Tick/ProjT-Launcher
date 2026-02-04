# SPDX-License-Identifier: GPL-2.0
#
# ProjT Launcher - Recursive Build System
#
# Each module manages its own Makefile. This file only
# calls modules in the correct dependency order.
# 
# SUBTREES (from .github/subtrees.txt) - DO NOT MODIFY:
#   zlib, tomlplusplus, json, qt/*
# These are built via mk/subtrees.mk wrappers.
#
# Build phases:
#   1. configure  - Generate headers from .in files
#   2. subtrees   - Build subtree wrappers
#   3. qt         - Build Qt (if bundled)
#   4. libs       - Build local libraries
#   5. launcher   - Build main application

-include $(KBUILD_OUTPUT)/.config

# Quiet/Verbose
ifeq ($(V),1)
Q :=
else
Q := @
endif

export srctree KBUILD_OUTPUT Q V

# Include configure system
include $(srctree)/mk/configure.mk

# Include Qt build system
include $(srctree)/mk/qt.mk

# ============================================================================
# SUBTREES (wrapper-only, do not modify source)
# ============================================================================

# These come from .github/subtrees.txt
SUBTREE_LIBS := zlib tomlplusplus json
SUBTREE_QT := qt/qtbase qt/qtnetworkauth qt/qtimageformats \
              qt/qtpositioning qt/qtserialport qt/qtwebchannel qt/qtwebengine

# ============================================================================
# NON-SUBTREE MODULE DEFINITIONS
# ============================================================================

# Base libraries (no dependencies) - NOT subtrees
LIBS_TIER0_LOCAL := bzip2 murmur2

# Tier 1: zlib-dependent - NOT subtrees
LIBS_TIER1 := libpng quazip

# Tier 2: Other base libraries - cmark is local, toml/json are subtrees
LIBS_TIER2_LOCAL := cmark libnbtplusplus libqrencode

# Tier 3: Qt-dependent modules - NOT subtrees
LIBS_TIER3 := rainbow qdcss LocalPeer

# Platform-specific
ifeq ($(CONFIG_TARGET_LINUX),y)
LIBS_PLATFORM := gamemode
endif

# Project modules
PROJT_MODULES := buildconfig systeminfo program_info

# Java modules
JAVA_MODULES := javacheck launcherjava

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

# ============================================================================
# BUILD FUNCTIONS
# ============================================================================

# Build a local module (has its own Makefile we control)
define build_local
	@if [ -f "$(srctree)/$(1)/Makefile" ]; then \
		echo "  BUILD   $(1)"; \
		$(MAKE) -C $(srctree)/$(1) \
			srctree=$(srctree) \
			KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
			Q=$(Q) V=$(V) \
			all 2>&1 | sed 's/^/    /' || true; \
	else \
		echo "  SKIP    $(1) (no Makefile)"; \
	fi
endef

# Clean a local module
define clean_local
	@if [ -f "$(srctree)/$(1)/Makefile" ]; then \
		echo "  CLEAN   $(1)"; \
		$(MAKE) -C $(srctree)/$(1) \
			srctree=$(srctree) \
			KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
			clean 2>/dev/null || true; \
	fi
endef

# ============================================================================
# BUILD TARGETS
# ============================================================================

.PHONY: all build libs launcher java-modules tests clean help
.PHONY: libs-tier0 libs-tier1 libs-tier2 libs-tier3 libs-platform projt-modules
.PHONY: launcher-submodules launcher-main subtrees configure qt-build

# Main target
all: build

# Full build with all phases
build: configure subtrees qt-build libs java-modules launcher-all
	@echo ""
	@echo "Build complete!"
	@echo "Output: $(KBUILD_OUTPUT)"

# ============================================================================
# PHASE 1: CONFIGURE (generate headers from .in files)
# ============================================================================

configure: $(GENERATED_FILES)
	@echo "=== Configure Complete (generated files ready) ==="

# ============================================================================
# PHASE 2: SUBTREE TARGETS (via mk/subtrees.mk wrappers)
# ============================================================================

subtrees: configure
	@echo "=== Building Subtree Libraries (via wrappers) ==="
	$(Q)$(MAKE) -f $(srctree)/mk/subtrees.mk \
		srctree=$(srctree) \
		KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
		subtrees

# ============================================================================
# PHASE 3: QT BUILD (if using bundled Qt)
# ============================================================================

ifeq ($(CONFIG_QT_BUNDLED),y)
qt-build: subtrees
	@echo "=== Building Bundled Qt ==="
	$(Q)$(MAKE) -f $(srctree)/mk/qt.mk \
		srctree=$(srctree) \
		KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
		qt
else
qt-build:
	@echo "=== Using System Qt (skipping build) ==="
endif

# ============================================================================
# PHASE 4: LIBRARY TARGETS
# ============================================================================

libs: qt-build libs-tier0 libs-tier1 libs-tier2 libs-tier3 libs-platform projt-modules

libs-tier0: configure subtrees
	@echo "=== Building Tier 0 Libraries (local) ==="
	$(call build_local,bzip2)
	$(call build_local,murmur2)

libs-tier1: libs-tier0
	@echo "=== Building Tier 1 Libraries (zlib deps) ==="
	$(call build_local,libpng)
	$(call build_local,quazip)

libs-tier2: libs-tier0
	@echo "=== Building Tier 2 Libraries ==="
	$(call build_local,cmark)
	$(call build_local,libnbtplusplus)
	$(call build_local,libqrencode)

libs-tier3: libs-tier0 libs-tier1 libs-tier2 qt-build
	@echo "=== Building Tier 3 Libraries (Qt deps) ==="
	$(call build_local,rainbow)
	$(call build_local,qdcss)
	$(call build_local,LocalPeer)

libs-platform:
ifdef LIBS_PLATFORM
	@echo "=== Building Platform Libraries ==="
	$(call build_local,gamemode)
endif

projt-modules: libs-tier0 libs-tier1 libs-tier2 libs-tier3
	@echo "=== Building ProjT Modules ==="
	$(call build_local,buildconfig)
	$(call build_local,systeminfo)
	$(call build_local,program_info)

# ============================================================================
# PHASE 5: JAVA TARGETS
# ============================================================================

java-modules:
	@echo "=== Building Java Modules ==="
	$(call build_local,javacheck)
	$(call build_local,launcherjava)

# ============================================================================
# PHASE 6: LAUNCHER TARGETS
# ============================================================================

launcher-all: libs projt-modules launcher-submodules launcher-main

launcher-submodules:
	@echo "=== Building Launcher Submodules ==="
	$(call build_local,launcher/console)
	$(call build_local,launcher/filelink)
	$(call build_local,launcher/icons)
	$(call build_local,launcher/java)
	$(call build_local,launcher/launch)
	$(call build_local,launcher/logs)
	$(call build_local,launcher/meta)
	$(call build_local,launcher/minecraft)
	$(call build_local,launcher/modplatform)
	$(call build_local,launcher/net)
	$(call build_local,launcher/news)
	$(call build_local,launcher/resources)
	$(call build_local,launcher/screenshots)
	$(call build_local,launcher/settings)
	$(call build_local,launcher/tasks)
	$(call build_local,launcher/tools)
	$(call build_local,launcher/translations)
	$(call build_local,launcher/ui)
	$(call build_local,launcher/updater)

launcher-main: launcher-submodules
	@echo "=== Building Launcher Main ==="
	$(call build_local,launcher)

# ============================================================================
# TEST TARGETS
# ============================================================================

tests: build
	@echo "=== Building Tests ==="
	$(call build_local,tests)
	$(call build_local,fuzz)

# ============================================================================
# INDIVIDUAL MODULE TARGETS
# ============================================================================

.PHONY: zlib bzip2 murmur2 libpng quazip cmark libnbtplusplus
.PHONY: libqrencode tomlplusplus json rainbow qdcss LocalPeer
.PHONY: gamemode buildconfig systeminfo program_info
.PHONY: javacheck launcherjava

# Subtrees (via wrappers - DO NOT call their Makefiles directly)
zlib tomlplusplus json:
	$(Q)$(MAKE) -f $(srctree)/mk/subtrees.mk \
		srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) $@

# Local libraries
bzip2:
	$(call build_local,bzip2)
murmur2:
	$(call build_local,murmur2)

# Tier 1 (local, depends on zlib subtree)
libpng: zlib
	$(call build_local,libpng)
quazip: zlib
	$(call build_local,quazip)

# Tier 2 (local only)
cmark:
	$(call build_local,cmark)
libnbtplusplus: zlib
	$(call build_local,libnbtplusplus)
libqrencode:
	$(call build_local,libqrencode)

# Tier 3 (local, Qt-dependent)
rainbow:
	$(call build_local,rainbow)
qdcss:
	$(call build_local,qdcss)
LocalPeer:
	$(call build_local,LocalPeer)

# Platform
gamemode:
	$(call build_local,gamemode)

# ProjT modules
buildconfig:
	$(call build_local,buildconfig)
systeminfo:
	$(call build_local,systeminfo)
program_info:
	$(call build_local,program_info)

# Java
javacheck:
	$(call build_local,javacheck)
launcherjava:
	$(call build_local,launcherjava)

# ============================================================================
# CLEAN TARGETS
# ============================================================================

clean:
	@echo "=== Cleaning All Modules ==="
	@echo "  Cleaning generated files..."
	$(Q)rm -rf $(GENERATED_DIR)
	@echo "  Cleaning subtrees via wrapper..."
	$(Q)$(MAKE) -f $(srctree)/mk/subtrees.mk \
		srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) subtrees-clean
ifeq ($(CONFIG_QT_BUNDLED),y)
	@echo "  Cleaning Qt build..."
	$(Q)$(MAKE) -f $(srctree)/mk/qt.mk \
		srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) qt-clean
endif
	@echo "  Cleaning local modules..."
	$(call clean_local,bzip2)
	$(call clean_local,murmur2)
	$(call clean_local,libpng)
	$(call clean_local,quazip)
	$(call clean_local,cmark)
	$(call clean_local,libnbtplusplus)
	$(call clean_local,libqrencode)
	$(call clean_local,rainbow)
	$(call clean_local,qdcss)
	$(call clean_local,LocalPeer)
	$(call clean_local,gamemode)
	$(call clean_local,buildconfig)
	$(call clean_local,systeminfo)
	$(call clean_local,program_info)
	$(call clean_local,javacheck)
	$(call clean_local,launcherjava)
	$(call clean_local,launcher/console)
	$(call clean_local,launcher/filelink)
	$(call clean_local,launcher/icons)
	$(call clean_local,launcher/java)
	$(call clean_local,launcher/launch)
	$(call clean_local,launcher/logs)
	$(call clean_local,launcher/meta)
	$(call clean_local,launcher/minecraft)
	$(call clean_local,launcher/modplatform)
	$(call clean_local,launcher/net)
	$(call clean_local,launcher/news)
	$(call clean_local,launcher/resources)
	$(call clean_local,launcher/screenshots)
	$(call clean_local,launcher/settings)
	$(call clean_local,launcher/tasks)
	$(call clean_local,launcher/tools)
	$(call clean_local,launcher/translations)
	$(call clean_local,launcher/ui)
	$(call clean_local,launcher/updater)
	$(call clean_local,launcher)
	$(call clean_local,tests)
	$(call clean_local,fuzz)
	$(Q)rm -rf $(KBUILD_OUTPUT)/bin
	$(Q)rm -rf $(KBUILD_OUTPUT)/lib
	$(Q)rm -rf $(KBUILD_OUTPUT)/obj
	$(Q)rm -rf $(KBUILD_OUTPUT)/jars
	@echo "Clean complete"

# ============================================================================
# UTILITY TARGETS
# ============================================================================

list-modules:
	@echo "Available modules:"
	@echo ""
	@echo "=== SUBTREES (do not modify) ==="
	@echo "  zlib tomlplusplus json"
	@echo "  Qt: $(SUBTREE_QT)"
	@echo ""
	@echo "=== LOCAL MODULES ==="
	@echo "  Tier 0:   $(LIBS_TIER0_LOCAL)"
	@echo "  Tier 1:   $(LIBS_TIER1)"
	@echo "  Tier 2:   $(LIBS_TIER2_LOCAL)"
	@echo "  Tier 3:   $(LIBS_TIER3)"
	@echo "  Platform: $(LIBS_PLATFORM)"
	@echo "  ProjT:    $(PROJT_MODULES)"
	@echo "  Java:     $(JAVA_MODULES)"
	@echo "  Launcher: $(LAUNCHER_SUBMODULES)"
	@echo "  Tests:    $(TEST_MODULES)"

deps-graph:
	@echo "digraph deps {"
	@echo "  rankdir=LR;"
	@echo "  # Subtrees (external)"
	@echo "  subgraph cluster_subtrees {"
	@echo "    label=\"Subtrees (do not modify)\";"
	@echo "    \"zlib\" [style=filled,fillcolor=lightblue];"
	@echo "    \"tomlplusplus\" [style=filled,fillcolor=lightblue];"
	@echo "    \"json\" [style=filled,fillcolor=lightblue];"
	@echo "  }"
	@echo "  # Local Tier 0"
	@echo "  \"bzip2\"; \"murmur2\";"
	@echo "  # Tier 1 deps"
	@echo "  \"zlib\" -> \"libpng\";"
	@echo "  \"zlib\" -> \"quazip\";"
	@echo "  # Tier 2"
	@echo "  \"cmark\"; \"libqrencode\";"
	@echo "  \"zlib\" -> \"libnbtplusplus\";"
	@echo "  # Tier 3"
	@echo "  \"qt\" -> \"rainbow\";"
	@echo "  \"qt\" -> \"qdcss\";"
	@echo "  \"qt\" -> \"LocalPeer\";"
	@echo "  # Launcher"
	@echo "  \"libs\" -> \"launcher\";"
	@echo "}"

help:
	@echo "ProjT Launcher Recursive Build System"
	@echo ""
	@echo "Build phases:"
	@echo "  configure      - Generate headers from .in templates"
	@echo "  subtrees       - Build subtree library wrappers"
	@echo "  qt-build       - Build Qt (if bundled, otherwise skipped)"
	@echo "  libs           - Build all local libraries"
	@echo "  launcher-all   - Build launcher with submodules"
	@echo ""
	@echo "Main targets:"
	@echo "  all / build    - Run all phases in order"
	@echo "  tests          - Build tests after main build"
	@echo "  clean          - Clean all build outputs"
	@echo ""
	@echo "Subtree modules (wrapper only, source untouched):"
	@echo "  zlib, tomlplusplus, json"
	@echo "  Qt: qt/qtbase, qt/qtnetworkauth, etc."
	@echo ""
	@echo "Local modules:"
	@echo "  bzip2, murmur2, libpng, quazip, cmark, etc."
	@echo ""
	@echo "Configuration:"
	@echo "  make menuconfig - Configure build options"
	@echo "  Generated files go to: $(GENERATED_DIR)"
	@echo ""
	@echo "Utility:"
	@echo "  list-modules   - Show all modules"
	@echo "  deps-graph     - Generate DOT dependency graph"

.PHONY: list-modules deps-graph help
