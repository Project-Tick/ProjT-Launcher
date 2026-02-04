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

# Google Test
GTEST_DIR ?= /usr/src/googletest/googletest
GTEST_CFLAGS := -I$(GTEST_DIR)/include
GTEST_LIBS := -lgtest -lgtest_main -lpthread

# Test flags
TEST_CXXFLAGS := \
	-I$(srctree)/launcher \
	-I$(srctree)/include \
	-I$(srctree)/tests \
	$(GTEST_CFLAGS) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

TEST_LDFLAGS := \
	-L$(KBUILD_OUTPUT)/launcher \
	-llauncher \
	$(GTEST_LIBS) \
	$(QT_LIBS) \
	$(LDFLAGS)

# Build all tests
.PHONY: tests tests-clean tests-run

tests: $(TEST_BINS)

# Pattern rule for test binaries
$(obj)/%: $(obj)/%.o $(KBUILD_OUTPUT)/launcher/liblauncher.a
	@mkdir -p $(dir $@)
	@echo "  LD      $@"
	$(Q)$(CXX) -o $@ $< $(TEST_LDFLAGS)

# Pattern rule for test object files
$(obj)/%.o: $(srctree)/tests/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TEST_CXXFLAGS) -c -o $@ $<

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
