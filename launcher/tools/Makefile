# SPDX-License-Identifier: GPL-3.0
# launcher/tools - External Tools Module Makefile

# Get absolute path before any includes modify MAKEFILE_LIST
THIS_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_DIR := $(dir $(THIS_MAKEFILE))

# Default paths if not provided by parent make
srctree ?= $(abspath $(THIS_DIR)/../..)
KBUILD_OUTPUT ?= $(srctree)/build

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Use QT_MOC if MOC is not set
MOC ?= $(QT_MOC)

obj := $(KBUILD_OUTPUT)/tools

TOOLS_SRCS := \
	BaseExternalTool.cpp \
	BaseProfiler.cpp \
	GenericProfiler.cpp \
	JProfiler.cpp \
	JVisualVM.cpp \
	MCEditTool.cpp

TOOLS_OBJS := $(addprefix $(obj)/, $(TOOLS_SRCS:.cpp=.o))
TOOLS_LIB := $(obj)/libtools.a

# MOC
TOOLS_MOC_HEADERS := $(shell find $(srctree)/launcher/tools -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
TOOLS_MOC_SRCS := $(patsubst $(srctree)/launcher/tools/%.h,$(obj)/moc_%.cpp,$(TOOLS_MOC_HEADERS))
TOOLS_MOC_OBJS := $(TOOLS_MOC_SRCS:.cpp=.o)

TOOLS_CXXFLAGS := \
	-I$(srctree)/launcher/tools \
	-I$(srctree)/launcher \
	-I$(KBUILD_OUTPUT)/generated/buildconfig \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all tools tools-clean

all: tools

tools: $(TOOLS_LIB)

$(TOOLS_LIB): $(TOOLS_OBJS) $(TOOLS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Generate inline moc files (for #include "X.moc" in cpp files)
$(obj)/%.moc: $(srctree)/launcher/tools/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/tools -I$(srctree)/launcher -o $@ $<

# Dependencies for inline moc
$(obj)/GenericProfiler.o: $(obj)/GenericProfiler.moc
$(obj)/JProfiler.o: $(obj)/JProfiler.moc
$(obj)/JVisualVM.o: $(obj)/JVisualVM.moc

$(obj)/%.o: $(srctree)/launcher/tools/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TOOLS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/tools/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TOOLS_CXXFLAGS) -c -o $@ $<

tools-clean:
	$(Q)rm -rf $(obj)

-include $(TOOLS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
