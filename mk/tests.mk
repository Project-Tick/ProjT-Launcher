# SPDX-License-Identifier: GPL-2.0
#
# mk/tests.mk - Test Infrastructure (Full Tests with All Dependencies)
#
# This module builds and runs ALL tests by linking against the complete
# launcher library and all its dependencies.
#
# Usage:
#   make test      - Build and run all tests
#   make check     - Same as test
#   make tests     - Build tests without running
#   make test-Version_test - Run specific test
#
# Environment Variables:
#   TEST_FILTER    - Filter tests by name pattern
#   TEST_VERBOSE   - Enable verbose test output (set to 1)
#   TEST_TIMEOUT   - Timeout per test in seconds (default: 120)

# ============================================================================
# Configuration
# ============================================================================

-include $(KBUILD_OUTPUT)/.config
-include $(KBUILD_OUTPUT)/include/config/auto.conf

TEST_TIMEOUT ?= 120
TEST_VERBOSE ?= 0
TEST_FILTER ?=

# Toolchain
CXX ?= g++
CC ?= gcc
MOC ?= moc

# Output directories
TEST_OUT := $(KBUILD_OUTPUT)/tests

# Source tree
SRCDIR := $(srctree)
LAUNCHER_SRC := $(SRCDIR)/launcher

# ============================================================================
# Platform Detection
# ============================================================================

TARGET_PLATFORM ?= linux

ifeq ($(TARGET_PLATFORM),windows)
  EXE_EXT := .exe
else
  EXE_EXT :=
endif

# ============================================================================
# Qt Configuration (Full set of modules)
# ============================================================================

QT_MODULES := Qt6Test Qt6Core Qt6Gui Qt6Widgets Qt6Network Qt6Xml Qt6Concurrent Qt6OpenGL Qt6OpenGLWidgets

# Qt6NetworkAuth for OAuth (optional)
QT_NETWORKAUTH_CFLAGS := $(shell pkg-config --cflags Qt6NetworkAuth 2>/dev/null)
QT_NETWORKAUTH_LIBS := $(shell pkg-config --libs Qt6NetworkAuth 2>/dev/null)

ifeq ($(TARGET_PLATFORM),linux)
  QT_MODULES += Qt6DBus
endif

QT_CFLAGS := $(shell pkg-config --cflags $(QT_MODULES) 2>/dev/null) $(QT_NETWORKAUTH_CFLAGS)
QT_LIBS := $(shell pkg-config --libs $(QT_MODULES) 2>/dev/null) $(QT_NETWORKAUTH_LIBS)
OPENSSL_CFLAGS := $(shell pkg-config --cflags openssl 2>/dev/null)
OPENSSL_LIBS := $(shell pkg-config --libs openssl 2>/dev/null || echo "-lssl -lcrypto")

# MOC path
MOC := $(shell pkg-config --variable=libexecdir Qt6Core 2>/dev/null)/moc
ifeq ($(MOC),/moc)
  MOC := moc
endif

# ============================================================================
# All Launcher Libraries (order matters for static linking)
# NOTE: libz.a is excluded - we use libprojtZ.so to avoid symbol conflicts with Qt
# ============================================================================

LAUNCHER_LIBS := \
	$(KBUILD_OUTPUT)/launcher/liblauncher.a \
	$(KBUILD_OUTPUT)/ui/libui.a \
	$(KBUILD_OUTPUT)/updater/libupdater.a \
	$(KBUILD_OUTPUT)/modplatform/libmodplatform.a \
	$(KBUILD_OUTPUT)/minecraft/libminecraft.a \
	$(KBUILD_OUTPUT)/meta/libmeta.a \
	$(KBUILD_OUTPUT)/launch/liblaunch.a \
	$(KBUILD_OUTPUT)/java/libjava.a \
	$(KBUILD_OUTPUT)/icons/libicons.a \
	$(KBUILD_OUTPUT)/tools/libtools.a \
	$(KBUILD_OUTPUT)/translations/libtranslations.a \
	$(KBUILD_OUTPUT)/tasks/libtasks.a \
	$(KBUILD_OUTPUT)/settings/libsettings.a \
	$(KBUILD_OUTPUT)/screenshots/libscreenshots.a \
	$(KBUILD_OUTPUT)/news/libnews.a \
	$(KBUILD_OUTPUT)/net/libnet.a \
	$(KBUILD_OUTPUT)/logs/liblogs.a \
	$(KBUILD_OUTPUT)/console/libconsole.a \
	$(KBUILD_OUTPUT)/lib/libbuildconfig.a \
	$(KBUILD_OUTPUT)/lib/libsysteminfo.a \
	$(KBUILD_OUTPUT)/lib/libquazip.a \
	$(KBUILD_OUTPUT)/lib/liblocalpeer.a \
	$(KBUILD_OUTPUT)/lib/librainbow.a \
	$(KBUILD_OUTPUT)/lib/libqdcss.a \
	$(KBUILD_OUTPUT)/lib/libqrencode.a \
	$(KBUILD_OUTPUT)/lib/libnbt++.a \
	$(KBUILD_OUTPUT)/lib/libpng.a \
	$(KBUILD_OUTPUT)/lib/libbz2.a \
	$(KBUILD_OUTPUT)/lib/libmurmur2.a

# ============================================================================
# Common Compiler Flags
# ============================================================================

# C++ standard from Kconfig (tests match main build standard)
TEST_CPP_STD := $(or $(CONFIG_CPP_STANDARD),-std=c++23)

COMMON_CXXFLAGS := $(TEST_CPP_STD) -fPIC -g -O0 \
	-I$(SRCDIR) \
	-I$(SRCDIR)/launcher \
	-I$(SRCDIR)/launcher/ui \
	-I$(SRCDIR)/launcher/ui/dialogs \
	-I$(SRCDIR)/launcher/ui/pages \
	-I$(SRCDIR)/launcher/ui/pages/global \
	-I$(SRCDIR)/launcher/ui/pages/instance \
	-I$(SRCDIR)/launcher/ui/widgets \
	-I$(SRCDIR)/include \
	-I$(SRCDIR)/zlib \
	-I$(SRCDIR)/libnbtplusplus/include \
	-I$(SRCDIR)/tomlplusplus/include \
	-I$(SRCDIR)/quazip \
	-I$(SRCDIR)/LocalPeer \
	-I$(KBUILD_OUTPUT) \
	-I$(KBUILD_OUTPUT)/include \
	-I$(KBUILD_OUTPUT)/launcher \
	-I$(TEST_OUT) \
	$(QT_CFLAGS) \
	$(OPENSSL_CFLAGS)

# Full link flags with all libraries
# Use --start-group/--end-group to resolve circular dependencies
# cmark for markdown parsing (use local build)
# Use project's shared zlib (libprojtZ.so) to avoid symbol conflicts with system Qt
# Qt links to system zlib, so we use a renamed shared lib to avoid Z_VERSION_ERROR
CMARK_LIBS := -L$(KBUILD_OUTPUT)/lib -lcmark -Wl,-rpath,$(KBUILD_OUTPUT)/lib
PROJT_ZLIB := -L$(KBUILD_OUTPUT)/lib -lprojtZ -Wl,-rpath,$(KBUILD_OUTPUT)/lib
ifeq ($(TARGET_PLATFORM),macos)
  LIB_GROUP_BEGIN :=
  LIB_GROUP_END :=
  PLATFORM_TEST_LIBS := -framework Cocoa
else
  LIB_GROUP_BEGIN := -Wl,--start-group
  LIB_GROUP_END := -Wl,--end-group
  PLATFORM_TEST_LIBS :=
endif
COMMON_LDFLAGS := $(LIB_GROUP_BEGIN) $(LAUNCHER_LIBS) $(LIB_GROUP_END) \
	$(PROJT_ZLIB) $(QT_LIBS) $(CMARK_LIBS) $(OPENSSL_LIBS) -lpthread $(PLATFORM_TEST_LIBS)

# ============================================================================
# All Tests
# ============================================================================

ALL_TESTS := \
	Version_test \
	GradleSpecifier_test \
	RuntimeVersion_test \
	GZip_test \
	ParseUtils_test \
	JavaVersion_test \
	INIFile_test \
	Library_test \
	MojangVersionFormat_test \
	Packwiz_test \
	Index_test \
	MetaComponentParse_test \
	DataPackParse_test \
	ResourcePackParse_test \
	TexturePackParse_test \
	ShaderPackParse_test \
	WorldSaveParse_test \
	ResourceFolderModel_test \
	Task_test \
	CatPack_test \
	XmlLogs_test \
	LogEventParser_test

# ProjTExternalUpdater only on non-macOS
ifneq ($(TARGET_PLATFORM),macos)
  ALL_TESTS += ProjTExternalUpdater_test
endif

ALL_TEST_BINS := $(addprefix $(TEST_OUT)/, $(ALL_TESTS))

# ============================================================================
# Targets
# ============================================================================

.PHONY: test check tests tests-build tests-run tests-clean tests-list tests-help

# Disable parallel execution for test targets to ensure proper ordering
.NOTPARALLEL: test check

# Main targets - tests-run depends on tests-build completion
test: tests-build
	$(Q)$(MAKE) -f $(srctree)/mk/tests.mk srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) tests-run

check: test

tests: tests-build

# Create directories
$(TEST_OUT)/.dir:
	$(Q)mkdir -p $(TEST_OUT)
	$(Q)touch $@

# Generate MOC files for test sources
$(TEST_OUT)/%_test.moc: $(SRCDIR)/tests/%_test.cpp | $(TEST_OUT)/.dir
	@echo "  MOC     $(@F)"
	$(Q)$(MOC) $(QT_CFLAGS) -i $< -o $@

# ============================================================================
# Generic Test Build Rule
# Each test is linked against all launcher libraries
# ============================================================================

$(TEST_OUT)/%_test: $(SRCDIR)/tests/%_test.cpp $(TEST_OUT)/%_test.moc $(LAUNCHER_LIBS) | $(TEST_OUT)/.dir
	@echo "  TEST    $(@F)"
	$(Q)$(CXX) $(COMMON_CXXFLAGS) -o $@ $< $(COMMON_LDFLAGS)

# ============================================================================
# Build all tests
# ============================================================================

# Build the shared zlib (libprojtZ.so) needed for test linking
zlib-shared:
	$(Q)$(MAKE) -f $(srctree)/mk/subtrees.mk srctree=$(srctree) KBUILD_OUTPUT=$(KBUILD_OUTPUT) zlib-shared

tests-build: zlib-shared $(ALL_TEST_BINS)
	@echo ""
	@echo "  BUILD   $(words $(ALL_TEST_BINS)) tests built"

# ============================================================================
# Run Tests
# ============================================================================

tests-run:
	@echo ""
	@echo "=================================================================="
	@echo "  Running ProjT-Launcher Tests"
	@echo "=================================================================="
	@echo ""
	@PASSED=0; FAILED=0; SKIPPED=0; \
	for test in $(ALL_TEST_BINS); do \
		testname=$$(basename $$test); \
		if [ -n "$(TEST_FILTER)" ] && ! echo "$$testname" | grep -q "$(TEST_FILTER)"; then \
			SKIPPED=$$((SKIPPED + 1)); \
			continue; \
		fi; \
		if [ ! -x "$$test" ]; then \
			printf "  SKIP    %-30s (not built)\n" "$$testname"; \
			SKIPPED=$$((SKIPPED + 1)); \
			continue; \
		fi; \
		printf "  RUN     %-30s" "$$testname"; \
		if [ "$(TEST_VERBOSE)" = "1" ]; then \
			if timeout $(TEST_TIMEOUT) $$test -v1 2>&1; then \
				echo "  PASS"; \
				PASSED=$$((PASSED + 1)); \
			else \
				echo "  FAIL"; \
				FAILED=$$((FAILED + 1)); \
			fi; \
		else \
			if timeout $(TEST_TIMEOUT) $$test 2>&1 >/dev/null; then \
				echo "PASS"; \
				PASSED=$$((PASSED + 1)); \
			else \
				echo "FAIL"; \
				timeout $(TEST_TIMEOUT) $$test 2>&1 | tail -20; \
				FAILED=$$((FAILED + 1)); \
			fi; \
		fi; \
	done; \
	echo ""; \
	echo "=================================================================="; \
	echo "  Results: $$PASSED passed, $$FAILED failed, $$SKIPPED skipped"; \
	echo "=================================================================="; \
	if [ $$FAILED -gt 0 ]; then exit 1; fi

# Run specific test
test-%: $(TEST_OUT)/%
	@echo "  RUN     $*"
	$(Q)$< -v1

# ============================================================================
# Utilities
# ============================================================================

tests-list:
	@echo "Available tests:"
	@for t in $(ALL_TESTS); do echo "  - $$t"; done
	@echo ""
	@echo "Run specific test: make test-<name>"
	@echo "Example: make test-Version_test"

tests-clean:
	@echo "  CLEAN   tests"
	$(Q)rm -rf $(TEST_OUT)

tests-help:
	@echo "Test targets:"
	@echo "  make test              - Build and run all tests"
	@echo "  make check             - Same as 'make test'"
	@echo "  make tests             - Build tests without running"
	@echo "  make tests-list        - List available tests"
	@echo "  make test-<name>       - Run specific test"
	@echo "  make tests-clean       - Clean test outputs"
	@echo ""
	@echo "Environment variables:"
	@echo "  TEST_VERBOSE=1         - Verbose test output"
	@echo "  TEST_FILTER=<pattern>  - Filter tests by pattern"
	@echo "  TEST_TIMEOUT=<seconds> - Test timeout (default: 120)"
