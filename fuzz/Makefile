# SPDX-License-Identifier: GPL-3.0
# fuzz/ - Fuzzing Tests Makefile
#
# Build rules for fuzzing tests using AFL, libFuzzer, or honggfuzz.

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Object output directory
obj := $(KBUILD_OUTPUT)/fuzz

# Fuzz sources
FUZZ_SRCS := \
	fuzz_gzip.cpp \
	fuzz_nbt_reader.cpp \
	fuzz_qjson_parse.cpp

# Fuzz targets
FUZZ_TARGETS := $(addprefix $(obj)/, $(FUZZ_SRCS:.cpp=))

# Fuzzer selection
FUZZER ?= libfuzzer

# Compiler selection based on fuzzer
ifeq ($(FUZZER),afl)
    FUZZ_CC := afl-clang++
    FUZZ_CXXFLAGS := -fsanitize=address,undefined
    FUZZ_LDFLAGS := -fsanitize=address,undefined
else ifeq ($(FUZZER),libfuzzer)
    FUZZ_CC := clang++
    FUZZ_CXXFLAGS := -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer
    FUZZ_LDFLAGS := -fsanitize=fuzzer,address,undefined
else ifeq ($(FUZZER),honggfuzz)
    FUZZ_CC := hfuzz-clang++
    FUZZ_CXXFLAGS := -fsanitize=address,undefined
    FUZZ_LDFLAGS := -fsanitize=address,undefined
else
    FUZZ_CC := $(CXX)
    FUZZ_CXXFLAGS :=
    FUZZ_LDFLAGS :=
endif

# Common flags
FUZZ_CXXFLAGS += \
	-I$(srctree)/launcher \
	-I$(srctree)/include \
	-I$(srctree)/libnbtplusplus/include \
	$(QT_CFLAGS) \
	$(CXXFLAGS) \
	-g -O1

FUZZ_LDFLAGS += \
	-L$(KBUILD_OUTPUT)/launcher \
	-L$(KBUILD_OUTPUT)/libnbtplusplus \
	-llauncher \
	-lnbt++ \
	$(QT_LIBS) \
	$(LDFLAGS)

# Corpus directories
CORPUS_DIR := $(obj)/corpus
CRASH_DIR := $(obj)/crashes

# Build all fuzzers
.PHONY: fuzz fuzz-clean fuzz-run

fuzz: $(FUZZ_TARGETS) | $(CORPUS_DIR) $(CRASH_DIR)

# Pattern rule for fuzz targets
$(obj)/fuzz_%: $(srctree)/fuzz/fuzz_%.cpp
	@mkdir -p $(dir $@)
	@echo "  FUZZ    $@"
	$(Q)$(FUZZ_CC) $(FUZZ_CXXFLAGS) -o $@ $< $(FUZZ_LDFLAGS)

# Create directories
$(CORPUS_DIR) $(CRASH_DIR):
	@mkdir -p $@

# Run fuzzer
fuzz-run-%: $(obj)/fuzz_% | $(CORPUS_DIR)/% $(CRASH_DIR)
	@echo "  FUZZ    Running $* fuzzer"
	$(Q)$< $(CORPUS_DIR)/$* -artifact_prefix=$(CRASH_DIR)/ -max_total_time=60

$(CORPUS_DIR)/%:
	@mkdir -p $@

# Run all fuzzers (quick test)
fuzz-quick: $(FUZZ_TARGETS)
	@echo "Running quick fuzz tests..."
	@for fuzz in $(FUZZ_TARGETS); do \
		name=$$(basename $$fuzz | sed 's/fuzz_//'); \
		mkdir -p $(CORPUS_DIR)/$$name; \
		echo "  FUZZ    $$name (10s)"; \
		$$fuzz $(CORPUS_DIR)/$$name -max_total_time=10 || true; \
	done

# Clean
fuzz-clean:
	@echo "  CLEAN   fuzz"
	$(Q)rm -rf $(obj)

# AFL specific targets
ifeq ($(FUZZER),afl)
fuzz-afl-%: $(obj)/fuzz_%
	@mkdir -p $(CORPUS_DIR)/$* $(obj)/afl-out
	afl-fuzz -i $(CORPUS_DIR)/$* -o $(obj)/afl-out -- $<
endif
