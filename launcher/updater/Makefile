# SPDX-License-Identifier: GPL-3.0
# launcher/updater - Updater Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/updater

# Core updater sources
UPDATER_SRCS := ProjTExternalUpdater.cpp

# ProjT updater submodule
UPDATER_SRCS += $(wildcard $(srctree)/launcher/updater/projtupdater/*.cpp)

# macOS Sparkle (Objective-C++)
ifeq ($(CONFIG_TARGET_MACOS),y)
UPDATER_OBJCXX_SRCS := MacSparkleUpdater.mm
UPDATER_OBJCXX_OBJS := $(addprefix $(obj)/, $(UPDATER_OBJCXX_SRCS:.mm=.o))
endif

UPDATER_OBJS := $(addprefix $(obj)/, $(notdir $(UPDATER_SRCS:.cpp=.o)))
UPDATER_LIB := $(obj)/libupdater.a

# MOC
UPDATER_MOC_HEADERS := $(shell find $(srctree)/launcher/updater -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
UPDATER_MOC_SRCS := $(patsubst $(srctree)/launcher/updater/%.h,$(obj)/moc_%.cpp,$(UPDATER_MOC_HEADERS))
UPDATER_MOC_OBJS := $(UPDATER_MOC_SRCS:.cpp=.o)

UPDATER_CXXFLAGS := \
	-I$(srctree)/launcher/updater \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

# macOS frameworks
ifeq ($(CONFIG_TARGET_MACOS),y)
UPDATER_CXXFLAGS += -fobjc-arc
UPDATER_LDFLAGS += -framework Sparkle -framework AppKit
endif

.PHONY: updater updater-clean

updater: $(UPDATER_LIB)

$(UPDATER_LIB): $(UPDATER_OBJS) $(UPDATER_OBJCXX_OBJS) $(UPDATER_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/updater/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UPDATER_CXXFLAGS) -c -o $@ $<

$(obj)/%.o: $(srctree)/launcher/updater/projtupdater/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UPDATER_CXXFLAGS) -c -o $@ $<

$(obj)/%.o: $(srctree)/launcher/updater/%.mm
	@mkdir -p $(dir $@)
	@echo "  OBJCXX  $<"
	$(Q)$(CXX) $(UPDATER_CXXFLAGS) -ObjC++ -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/updater/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UPDATER_CXXFLAGS) -c -o $@ $<

updater-clean:
	$(Q)rm -rf $(obj)

-include $(UPDATER_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
