# SPDX-License-Identifier: GPL-3.0
# include/ - Generated Headers Makefile
#
# This directory contains generated header files that are
# created during the build process.

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/include

# Generated headers destination
GENERATED_DIR := $(obj)/generated

# Version header
VERSION_HEADER := $(GENERATED_DIR)/Version.h

# Build configuration header
CONFIG_HEADER := $(GENERATED_DIR)/BuildConfig.h

.PHONY: generated-headers generated-clean

generated-headers: $(VERSION_HEADER) $(CONFIG_HEADER)

$(VERSION_HEADER): | $(GENERATED_DIR)
	@echo "  GEN     $@"
	@echo "#pragma once" > $@
	@echo "#define PROJT_VERSION_MAJOR $(VERSION_MAJOR)" >> $@
	@echo "#define PROJT_VERSION_MINOR $(VERSION_MINOR)" >> $@
	@echo "#define PROJT_VERSION_PATCH $(VERSION_PATCH)" >> $@
	@echo "#define PROJT_VERSION_STRING \"$(VERSION)\"" >> $@
	@echo "#define PROJT_GIT_COMMIT \"$(GIT_COMMIT)\"" >> $@
	@echo "#define PROJT_BUILD_DATE \"$(shell date -u +%Y-%m-%d)\"" >> $@

$(CONFIG_HEADER): $(srctree)/.config | $(GENERATED_DIR)
	@echo "  GEN     $@"
	@echo "#pragma once" > $@
	@echo "" >> $@
	@echo "// Auto-generated from Kconfig" >> $@
	@grep "^CONFIG_" $(srctree)/.config 2>/dev/null | \
		sed 's/CONFIG_\([^=]*\)=y/#define CONFIG_\1 1/' | \
		sed 's/CONFIG_\([^=]*\)=\([0-9]*\)$$/#define CONFIG_\1 \2/' | \
		sed 's/CONFIG_\([^=]*\)="\(.*\)"/#define CONFIG_\1 "\2"/' >> $@ || true

$(GENERATED_DIR):
	@mkdir -p $@

generated-clean:
	@echo "  CLEAN   generated"
	$(Q)rm -rf $(obj)
