# SPDX-License-Identifier: GPL-3.0
# launcher/settings - Settings Module Makefile

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

obj := $(KBUILD_OUTPUT)/settings

SETTINGS_SRCS := \
	INIFile.cpp \
	INISettingsObject.cpp \
	OverrideSetting.cpp \
	PassthroughSetting.cpp \
	Setting.cpp \
	SettingsObject.cpp

SETTINGS_OBJS := $(addprefix $(obj)/, $(SETTINGS_SRCS:.cpp=.o))
SETTINGS_LIB := $(obj)/libsettings.a

# MOC
SETTINGS_MOC_HEADERS := $(shell find $(srctree)/launcher/settings -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
SETTINGS_MOC_SRCS := $(patsubst $(srctree)/launcher/settings/%.h,$(obj)/moc_%.cpp,$(SETTINGS_MOC_HEADERS))
SETTINGS_MOC_OBJS := $(SETTINGS_MOC_SRCS:.cpp=.o)

SETTINGS_CXXFLAGS := \
	-I$(srctree)/launcher/settings \
	-I$(srctree)/launcher \
	-I$(KBUILD_OUTPUT)/generated/buildconfig \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all settings settings-clean

all: settings

settings: $(SETTINGS_LIB)

$(SETTINGS_LIB): $(SETTINGS_OBJS) $(SETTINGS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/settings/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(SETTINGS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/settings/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(SETTINGS_CXXFLAGS) -c -o $@ $<

settings-clean:
	$(Q)rm -rf $(obj)

-include $(SETTINGS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
