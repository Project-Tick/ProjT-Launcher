# SPDX-License-Identifier: GPL-3.0
# launcher/updater - Updater Module Makefile

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
UIC ?= $(QT_UIC)

obj := $(KBUILD_OUTPUT)/updater

# UI files
UI_FILES := $(wildcard $(srctree)/launcher/updater/projtupdater/*.ui)
UIC_HEADERS := $(patsubst $(srctree)/launcher/updater/projtupdater/%.ui,$(obj)/ui_%.h,$(UI_FILES))

# Core updater sources - only ProjTExternalUpdater for the main launcher
# The projtupdater/ directory contains the standalone updater application
UPDATER_SRCS := ProjTExternalUpdater.cpp

# macOS Sparkle (Objective-C++)
ifeq ($(CONFIG_TARGET_MACOS),y)
UPDATER_OBJCXX_SRCS := MacSparkleUpdater.mm
UPDATER_OBJCXX_OBJS := $(addprefix $(obj)/, $(UPDATER_OBJCXX_SRCS:.mm=.o))
endif

UPDATER_OBJS := $(addprefix $(obj)/, $(notdir $(UPDATER_SRCS:.cpp=.o)))
UPDATER_LIB := $(obj)/libupdater.a

# MOC - only ExternalUpdater.h and ProjTExternalUpdater.h for the library
# MacSparkleUpdater.h is macOS only
ifeq ($(CONFIG_TARGET_MACOS),y)
UPDATER_MOC_HEADERS := $(srctree)/launcher/updater/ExternalUpdater.h \
                       $(srctree)/launcher/updater/ProjTExternalUpdater.h \
                       $(srctree)/launcher/updater/MacSparkleUpdater.h
else
UPDATER_MOC_HEADERS := $(srctree)/launcher/updater/ExternalUpdater.h \
                       $(srctree)/launcher/updater/ProjTExternalUpdater.h
endif
UPDATER_MOC_SRCS := $(patsubst $(srctree)/launcher/updater/%.h,$(obj)/moc_%.cpp,$(UPDATER_MOC_HEADERS))
UPDATER_MOC_OBJS := $(UPDATER_MOC_SRCS:.cpp=.o)

UPDATER_CXXFLAGS := \
	-I$(srctree)/launcher/updater \
	-I$(srctree)/launcher \
	-I$(srctree)/buildconfig -I$(KBUILD_OUTPUT)/obj/generated \
	-I$(srctree)/systeminfo/include \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/quazip \
	-I$(srctree)/cmark/src \
	-I$(srctree)/cmark/build/src \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

# macOS frameworks
ifeq ($(CONFIG_TARGET_MACOS),y)
UPDATER_CXXFLAGS += -fobjc-arc
UPDATER_LDFLAGS += -framework Sparkle -framework AppKit
endif

.PHONY: all updater updater-clean

all: updater

updater: $(UIC_HEADERS) $(UPDATER_LIB)

$(UPDATER_LIB): $(UPDATER_OBJS) $(UPDATER_OBJCXX_OBJS) $(UPDATER_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# UIC rules
$(obj)/ui_%.h: $(srctree)/launcher/updater/projtupdater/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/%.o: $(srctree)/launcher/updater/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UPDATER_CXXFLAGS) -c -o $@ $<

$(obj)/%.o: $(srctree)/launcher/updater/projtupdater/%.cpp | $(UIC_HEADERS)
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
