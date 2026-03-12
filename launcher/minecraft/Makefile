# SPDX-License-Identifier: GPL-3.0
# launcher/minecraft - Minecraft Core Module Makefile

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

obj := $(KBUILD_OUTPUT)/minecraft

# Core Minecraft sources
MC_CORE_SRCS := \
	AssetsUtils.cpp \
	BackupManager.cpp \
	Component.cpp \
	ComponentUpdateTask.cpp \
	LaunchProfile.cpp \
	Library.cpp \
	Logging.cpp \
	MinecraftInstance.cpp \
	MinecraftInstanceLaunchMenu.cpp \
	MinecraftLoadAndCheck.cpp \
	MojangVersionFormat.cpp \
	OneSixVersionFormat.cpp \
	PackProfile.cpp \
	ParseUtils.cpp \
	ProfileUtils.cpp \
	Rule.cpp \
	ShortcutUtils.cpp \
	VanillaInstanceCreationTask.cpp \
	VersionFile.cpp \
	VersionFilterData.cpp \
	World.cpp \
	WorldList.cpp

# Auth submodule
MC_AUTH_SRCS := $(wildcard $(srctree)/launcher/minecraft/auth/*.cpp)
MC_AUTH_SRCS += $(wildcard $(srctree)/launcher/minecraft/auth/flows/*.cpp)
MC_AUTH_SRCS += $(wildcard $(srctree)/launcher/minecraft/auth/steps/*.cpp)
MC_AUTH_OBJS := $(patsubst $(srctree)/launcher/minecraft/%.cpp,$(obj)/%.o,$(MC_AUTH_SRCS))

# Launch submodule
MC_LAUNCH_SRCS := $(wildcard $(srctree)/launcher/minecraft/launch/*.cpp)
MC_LAUNCH_OBJS := $(patsubst $(srctree)/launcher/minecraft/%.cpp,$(obj)/%.o,$(MC_LAUNCH_SRCS))

# Mod submodule
MC_MOD_SRCS := $(wildcard $(srctree)/launcher/minecraft/mod/*.cpp)
MC_MOD_SRCS += $(wildcard $(srctree)/launcher/minecraft/mod/tasks/*.cpp)
MC_MOD_OBJS := $(patsubst $(srctree)/launcher/minecraft/%.cpp,$(obj)/%.o,$(MC_MOD_SRCS))

# Skins submodule
MC_SKINS_SRCS := $(wildcard $(srctree)/launcher/minecraft/skins/*.cpp)
MC_SKINS_OBJS := $(patsubst $(srctree)/launcher/minecraft/%.cpp,$(obj)/%.o,$(MC_SKINS_SRCS))

# Update submodule
MC_UPDATE_SRCS := $(wildcard $(srctree)/launcher/minecraft/update/*.cpp)
MC_UPDATE_OBJS := $(patsubst $(srctree)/launcher/minecraft/%.cpp,$(obj)/%.o,$(MC_UPDATE_SRCS))

# All objects
MC_CORE_OBJS := $(addprefix $(obj)/, $(MC_CORE_SRCS:.cpp=.o))
MC_ALL_OBJS := $(MC_CORE_OBJS) $(MC_AUTH_OBJS) $(MC_LAUNCH_OBJS) $(MC_MOD_OBJS) $(MC_SKINS_OBJS) $(MC_UPDATE_OBJS)

MC_LIB := $(obj)/libminecraft.a

# MOC processing
MC_MOC_HEADERS := $(shell find $(srctree)/launcher/minecraft -name '*.h' -o -name '*.hpp' | xargs grep -l Q_OBJECT 2>/dev/null)

# Generate MOC source file names - extract just the base filename
MC_MOC_SRCS := $(foreach h,$(MC_MOC_HEADERS),$(obj)/moc_$(notdir $(basename $(h))).cpp)
MC_MOC_OBJS := $(MC_MOC_SRCS:.cpp=.o)

# Inline MOC files (included directly in cpp files)
MC_INLINE_MOC := $(obj)/MinecraftInstance.moc

MC_CXXFLAGS := \
	-I$(srctree)/launcher/minecraft \
	-I$(srctree)/launcher \
	-I$(srctree)/libnbtplusplus/include \
	-I$(srctree)/buildconfig -I$(KBUILD_OUTPUT)/obj/generated \
	-I$(QT_PREFIX)/include/QtConcurrent \
	-I$(QT_PREFIX)/include/QtNetworkAuth \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/quazip \
	-I$(srctree)/zlib \
	-I$(srctree)/gamemode/lib \
	-I$(srctree)/qdcss/include \
	-I$(srctree)/tomlplusplus/include \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all minecraft minecraft-clean

all: minecraft

minecraft: $(MC_LIB)

$(MC_LIB): $(MC_ALL_OBJS) $(MC_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Generate inline moc files (for #include "X.moc" in cpp files)
$(obj)/%.moc: $(srctree)/launcher/minecraft/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# Inline moc for auth/steps/*.cpp
$(obj)/auth/steps/%.moc: $(srctree)/launcher/minecraft/auth/steps/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MinecraftInstance.o depends on the inline moc file
$(obj)/MinecraftInstance.o: $(obj)/MinecraftInstance.moc

# MicrosoftOAuthStep.o depends on inline moc file
$(obj)/auth/steps/MicrosoftOAuthStep.o: $(obj)/auth/steps/MicrosoftOAuthStep.moc

# Pattern rules for different subdirectories
$(obj)/%.o: $(srctree)/launcher/minecraft/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(MC_CXXFLAGS) -I$(dir $@) -c -o $@ $<

# MOC rules - root level .h files
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for auth/*.hpp
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/auth/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for auth/steps/*.hpp
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/auth/steps/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for launch/*.hpp
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/launch/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for mod/*.hpp
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/mod/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for mod/tasks/*.hpp
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/mod/tasks/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for skins/*.h
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/skins/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

# MOC rules for update/*.h
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/update/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher/minecraft -I$(srctree)/launcher $(QT_CFLAGS) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(MC_CXXFLAGS) -c -o $@ $<

minecraft-clean:
	$(Q)rm -rf $(obj)

-include $(MC_ALL_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
