# SPDX-License-Identifier: GPL-3.0
# launcher/minecraft - Minecraft Core Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

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
MC_MOC_SRCS := $(patsubst $(srctree)/launcher/minecraft/%.h,$(obj)/moc_%.cpp,$(filter %.h,$(MC_MOC_HEADERS)))
MC_MOC_SRCS += $(patsubst $(srctree)/launcher/minecraft/%.hpp,$(obj)/moc_%.cpp,$(filter %.hpp,$(MC_MOC_HEADERS)))
MC_MOC_OBJS := $(MC_MOC_SRCS:.cpp=.o)

MC_CXXFLAGS := \
	-I$(srctree)/launcher/minecraft \
	-I$(srctree)/launcher \
	-I$(srctree)/libnbtplusplus/include \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: minecraft minecraft-clean

minecraft: $(MC_LIB)

$(MC_LIB): $(MC_ALL_OBJS) $(MC_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Pattern rules for different subdirectories
$(obj)/%.o: $(srctree)/launcher/minecraft/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(MC_CXXFLAGS) -c -o $@ $<

# MOC rules
$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/minecraft -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/minecraft/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/minecraft -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(MC_CXXFLAGS) -c -o $@ $<

minecraft-clean:
	$(Q)rm -rf $(obj)

-include $(MC_ALL_OBJS:.o=.d)
