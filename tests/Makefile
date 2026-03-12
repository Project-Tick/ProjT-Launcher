# SPDX-License-Identifier: GPL-3.0
# tests/ - Unit Tests Makefile
#
# Build rules for ProjT-Launcher unit tests.
# Uses Google Test framework.

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Object output directory
obj := $(KBUILD_OUTPUT)/tests

# Test sources
TEST_SRCS := \
	CatPack_test.cpp \
	DataPackParse_test.cpp \
	FileSystem_test.cpp \
	GZip_test.cpp \
	GradleSpecifier_test.cpp \
	INIFile_test.cpp \
	Index_test.cpp \
	JavaVersion_test.cpp \
	Library_test.cpp \
	LogEventParser_test.cpp \
	MetaComponentParse_test.cpp \
	MojangVersionFormat_test.cpp \
	Packwiz_test.cpp \
	ParseUtils_test.cpp \
	ProjTExternalUpdater_test.cpp \
	ResourceFolderModel_test.cpp \
	ResourcePackParse_test.cpp \
	RuntimeVersion_test.cpp \
	ShaderPackParse_test.cpp \
	Task_test.cpp \
	TexturePackParse_test.cpp \
	Version_test.cpp \
	WorldSaveParse_test.cpp \
	XmlLogs_test.cpp

# Test binaries (one per test file)
TEST_BINS := $(addprefix $(obj)/, $(TEST_SRCS:.cpp=))

# Objects
TEST_OBJS := $(addprefix $(obj)/, $(TEST_SRCS:.cpp=.o))

# Google Test (disabled - using Qt Test instead)
# GTEST_DIR ?= /usr/src/googletest/googletest
# GTEST_CFLAGS := -I$(GTEST_DIR)/include
# GTEST_LIBS := -lgtest -lgtest_main -lpthread

# Qt Test include path
QT_TEST_CFLAGS := $(shell pkg-config --cflags Qt6Test Qt6Core Qt6Gui Qt6Widgets Qt6Network 2>/dev/null)
QT_TEST_LIBS := $(shell pkg-config --libs Qt6Test Qt6Core Qt6Gui Qt6Widgets Qt6Network 2>/dev/null)

# Test flags
TEST_CXXFLAGS := \
	-std=c++17 -fPIC \
	-I$(srctree)/launcher \
	-I$(srctree)/include \
	-I$(srctree)/tests \
	$(QT_TEST_CFLAGS) \
	$(CXXFLAGS)

TEST_LDFLAGS := \
	-L$(KBUILD_OUTPUT)/launcher \
	-llauncher \
	$(QT_TEST_LIBS) \
	$(LDFLAGS)

# Build all tests
.PHONY: all tests tests-clean tests-run

all:
	@echo "  Tests require full launcher build and Qt6Test"
	@echo "  TODO: Integrate tests with launcher libraries"

tests-full: $(TEST_BINS)

# Qt MOC path
MOC := $(shell pkg-config --variable=libexecdir Qt6Core 2>/dev/null)/moc
ifeq ($(MOC),/moc)
MOC := moc
endif

# Generate .moc files from test cpp files
$(obj)/%.moc: $(srctree)/tests/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_TEST_CFLAGS) -i $< -o $@

# Pattern rule for test binaries
$(obj)/%: $(obj)/%.o $(KBUILD_OUTPUT)/launcher/liblauncher.a
	@mkdir -p $(dir $@)
	@echo "  LD      $@"
	$(Q)$(CXX) -o $@ $< $(TEST_LDFLAGS)

# Pattern rule for test object files - depends on moc
$(obj)/%.o: $(srctree)/tests/%.cpp $(obj)/%.moc
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TEST_CXXFLAGS) -I$(obj) -c -o $@ $<

# Run all tests
tests-run: tests
	@echo "Running tests..."
	@for test in $(TEST_BINS); do \
		echo "  TEST    $$(basename $$test)"; \
		$$test --gtest_color=yes || exit 1; \
	done
	@echo "All tests passed!"

# Run specific test
test-%: $(obj)/%
	@echo "  TEST    $*"
	$(Q)$< --gtest_color=yes

# Clean
tests-clean:
	@echo "  CLEAN   tests"
	$(Q)rm -rf $(obj)

# Dependencies
-include $(TEST_OBJS:.o=.d)
