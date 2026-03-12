# SPDX-License-Identifier: GPL-3.0
# launcher/modplatform - Mod Platform Module Makefile

# Get absolute path before any includes modify MAKEFILE_LIST
THIS_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_DIR := $(dir $(THIS_MAKEFILE))

# Default paths if not provided by parent make
srctree ?= $(abspath $(THIS_DIR)/../..)
KBUILD_OUTPUT ?= $(srctree)/build

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Use QT_MOC if MOC is not set (parent exports QT_MOC)
MOC ?= $(QT_MOC)

obj := $(KBUILD_OUTPUT)/modplatform

# Core sources
MP_CORE_SRCS := \
	EnsureMetadataTask.cpp \
	ModIndex.cpp \
	ResourceAPI.cpp \
	ResourceType.cpp

# Helpers
MP_HELPERS_SRCS := $(wildcard $(srctree)/launcher/modplatform/helpers/*.cpp)
MP_HELPERS_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_HELPERS_SRCS))

# ATLauncher
MP_ATL_SRCS := $(wildcard $(srctree)/launcher/modplatform/atlauncher/*.cpp)
MP_ATL_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_ATL_SRCS))

# CurseForge/Flame
MP_FLAME_SRCS := $(wildcard $(srctree)/launcher/modplatform/flame/*.cpp)
MP_FLAME_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_FLAME_SRCS))

# Modrinth
MP_MODRINTH_SRCS := $(wildcard $(srctree)/launcher/modplatform/modrinth/*.cpp)
MP_MODRINTH_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_MODRINTH_SRCS))

# Technic
MP_TECHNIC_SRCS := $(wildcard $(srctree)/launcher/modplatform/technic/*.cpp)
MP_TECHNIC_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_TECHNIC_SRCS))

# Legacy FTB
MP_LFTB_SRCS := $(wildcard $(srctree)/launcher/modplatform/legacy_ftb/*.cpp)
MP_LFTB_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_LFTB_SRCS))

# Import FTB
MP_IFTB_SRCS := $(wildcard $(srctree)/launcher/modplatform/import_ftb/*.cpp)
MP_IFTB_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_IFTB_SRCS))

# Packwiz
MP_PACKWIZ_SRCS := $(wildcard $(srctree)/launcher/modplatform/packwiz/*.cpp)
MP_PACKWIZ_OBJS := $(patsubst $(srctree)/launcher/modplatform/%.cpp,$(obj)/%.o,$(MP_PACKWIZ_SRCS))

# All objects
MP_CORE_OBJS := $(addprefix $(obj)/, $(MP_CORE_SRCS:.cpp=.o))
MP_ALL_OBJS := $(MP_CORE_OBJS) $(MP_HELPERS_OBJS) $(MP_ATL_OBJS) $(MP_FLAME_OBJS) \
	$(MP_MODRINTH_OBJS) $(MP_TECHNIC_OBJS) $(MP_LFTB_OBJS) $(MP_IFTB_OBJS) $(MP_PACKWIZ_OBJS)

MP_LIB := $(obj)/libmodplatform.a

# MOC
MP_MOC_HEADERS := $(shell find $(srctree)/launcher/modplatform -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
MP_MOC_SRCS := $(patsubst $(srctree)/launcher/modplatform/%.h,$(obj)/moc_%.cpp,$(MP_MOC_HEADERS))
MP_MOC_OBJS := $(MP_MOC_SRCS:.cpp=.o)

MP_CXXFLAGS := \
	-I$(srctree)/launcher/modplatform \
	-I$(srctree)/launcher \
	-I$(srctree)/murmur2/src \
	-I$(srctree)/buildconfig -I$(KBUILD_OUTPUT)/obj/generated \
	-I$(srctree)/tomlplusplus/include \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/quazip \
	-I$(srctree)/zlib \
	-I$(QT_PREFIX)/include/QtConcurrent \
	-I$(QT_PREFIX)/include/QtXml \
	-I$(srctree)/systeminfo/include \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all modplatform modplatform-clean

all: modplatform

modplatform: $(MP_LIB)

$(MP_LIB): $(MP_ALL_OBJS) $(MP_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/modplatform/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(MP_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/modplatform/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(MP_CXXFLAGS) -c -o $@ $<

modplatform-clean:
	$(Q)rm -rf $(obj)

-include $(MP_ALL_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
