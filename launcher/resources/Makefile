# SPDX-License-Identifier: GPL-3.0
# launcher/resources - Resources Makefile
#
# This generates Qt resource files (QRC -> CPP)

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/resources

# Find all QRC files
QRC_FILES := $(shell find $(srctree)/launcher/resources -name '*.qrc' 2>/dev/null)

# Generate CPP files from QRC
RCC_SRCS := $(patsubst $(srctree)/launcher/resources/%.qrc,$(obj)/qrc_%.cpp,$(QRC_FILES))
RCC_OBJS := $(RCC_SRCS:.cpp=.o)

RESOURCES_LIB := $(obj)/libresources.a

RESOURCES_CXXFLAGS := \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: resources resources-clean

resources: $(RESOURCES_LIB)

$(RESOURCES_LIB): $(RCC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# RCC processing
$(obj)/qrc_%.cpp: $(srctree)/launcher/resources/%.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name $(basename $(notdir $<)) -o $@ $<

# Flatten nested QRCs
$(obj)/qrc_%.cpp: $(srctree)/launcher/resources/backgrounds/%.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name $(basename $(notdir $<)) -o $@ $<

$(obj)/qrc_%.cpp: $(srctree)/launcher/resources/documents/%.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name $(basename $(notdir $<)) -o $@ $<

$(obj)/qrc_%.cpp: $(srctree)/launcher/resources/flat/%.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name $(basename $(notdir $<)) -o $@ $<

$(obj)/%.o: $(obj)/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(RESOURCES_CXXFLAGS) -c -o $@ $<

resources-clean:
	@echo "  CLEAN   resources"
	$(Q)rm -rf $(obj)
