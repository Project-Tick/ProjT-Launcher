# SPDX-License-Identifier: GPL-3.0
# launcher/ui - UI Module Makefile
#
# This is the main UI module containing all Qt-based user interface components.

# Get absolute path before any includes modify MAKEFILE_LIST
THIS_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_DIR := $(dir $(THIS_MAKEFILE))

# Default paths if not provided by parent make
srctree ?= $(abspath $(THIS_DIR)/../..)
KBUILD_OUTPUT ?= $(srctree)/build

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Launcher Hub feature - controlled by Kconfig
# If CONFIG_FEATURE_LAUNCHER_HUB is not set (not 'y'), disable launcher hub
ifneq ($(CONFIG_FEATURE_LAUNCHER_HUB),y)
  LAUNCHER_HUB_DEFINES := -DPROJT_DISABLE_LAUNCHER_HUB
else
  LAUNCHER_HUB_DEFINES :=
endif

# Use Qt tools if not set
MOC ?= $(QT_MOC)
UIC ?= $(QT_UIC)
ifeq ($(TARGET_PLATFORM),macos)
QT_DEFINES += -DQ_OS_MACOS
endif

obj := $(KBUILD_OUTPUT)/ui

# ============================================================================
# SOURCE FILES
# ============================================================================

# Core UI sources
UI_CORE_SRCS := \
	GuiUtil.cpp \
	InstanceWindow.cpp \
	LaunchMenu.cpp \
	MainWindow.cpp \
	ViewLogWindow.cpp

# Dialogs
UI_DIALOGS_SRCS := $(wildcard $(srctree)/launcher/ui/dialogs/*.cpp)
UI_DIALOGS_SRCS += $(wildcard $(srctree)/launcher/ui/dialogs/skins/*.cpp)
UI_DIALOGS_SRCS += $(wildcard $(srctree)/launcher/ui/dialogs/skins/draw/*.cpp)
UI_DIALOGS_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_DIALOGS_SRCS))

# Instance View
UI_INSTANCEVIEW_SRCS := $(wildcard $(srctree)/launcher/ui/instanceview/*.cpp)
UI_INSTANCEVIEW_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_INSTANCEVIEW_SRCS))

# Page Dialog
UI_PAGEDIALOG_SRCS := $(wildcard $(srctree)/launcher/ui/pagedialog/*.cpp)
UI_PAGEDIALOG_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_PAGEDIALOG_SRCS))

# Global Pages
UI_PAGES_GLOBAL_SRCS := $(wildcard $(srctree)/launcher/ui/pages/global/*.cpp)
UI_PAGES_GLOBAL_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_PAGES_GLOBAL_SRCS))

# Instance Pages
UI_PAGES_INSTANCE_SRCS := $(wildcard $(srctree)/launcher/ui/pages/instance/*.cpp)
UI_PAGES_INSTANCE_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_PAGES_INSTANCE_SRCS))

# Modplatform Pages
UI_PAGES_MP_SRCS := $(wildcard $(srctree)/launcher/ui/pages/modplatform/*.cpp)
UI_PAGES_MP_SRCS += $(wildcard $(srctree)/launcher/ui/pages/modplatform/*/*.cpp)
UI_PAGES_MP_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_PAGES_MP_SRCS))

# Setup Wizard
UI_SETUPWIZARD_SRCS := $(wildcard $(srctree)/launcher/ui/setupwizard/*.cpp)
UI_SETUPWIZARD_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_SETUPWIZARD_SRCS))

# Java UI
UI_JAVA_SRCS := $(wildcard $(srctree)/launcher/ui/java/*.cpp)
UI_JAVA_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_JAVA_SRCS))

# Tasks UI
UI_TASKS_SRCS := $(wildcard $(srctree)/launcher/ui/tasks/*.cpp)
UI_TASKS_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_TASKS_SRCS))

# Themes
UI_THEMES_SRCS := $(wildcard $(srctree)/launcher/ui/themes/*.cpp)
UI_THEMES_SRCS += $(wildcard $(srctree)/launcher/ui/themes/*.mm)
UI_THEMES_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(filter %.cpp,$(UI_THEMES_SRCS)))
UI_THEMES_OBJS += $(patsubst $(srctree)/launcher/ui/%.mm,$(obj)/%.mm.o,$(filter %.mm,$(UI_THEMES_SRCS)))

# Widgets
UI_WIDGETS_SRCS := $(wildcard $(srctree)/launcher/ui/widgets/*.cpp)
UI_WIDGETS_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_WIDGETS_SRCS))

# All UI objects
UI_CORE_OBJS := $(addprefix $(obj)/, $(UI_CORE_SRCS:.cpp=.o))
UI_ALL_OBJS := $(UI_CORE_OBJS) $(UI_DIALOGS_OBJS) $(UI_INSTANCEVIEW_OBJS) \
	$(UI_PAGEDIALOG_OBJS) $(UI_PAGES_GLOBAL_OBJS) $(UI_PAGES_INSTANCE_OBJS) \
	$(UI_PAGES_MP_OBJS) $(UI_SETUPWIZARD_OBJS) $(UI_JAVA_OBJS) \
	$(UI_TASKS_OBJS) $(UI_THEMES_OBJS) $(UI_WIDGETS_OBJS)

UI_LIB := $(obj)/libui.a

# ============================================================================
# UI FILES (UIC Processing)
# ============================================================================
UI_FILES := $(shell find $(srctree)/launcher/ui -name '*.ui')
# Flatten paths - all ui_*.h go directly into $(obj)/
UIC_HEADERS := $(addprefix $(obj)/ui_,$(addsuffix .h,$(basename $(notdir $(UI_FILES)))))

# ============================================================================
# MOC PROCESSING
# ============================================================================
UI_MOC_HEADERS := $(shell find $(srctree)/launcher/ui -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
UI_MOC_SRCS := $(patsubst $(srctree)/launcher/ui/%.h,$(obj)/moc_%.cpp,$(UI_MOC_HEADERS))
UI_MOC_OBJS := $(UI_MOC_SRCS:.cpp=.o)

# ============================================================================
# COMPILER FLAGS
# ============================================================================
UI_CXXFLAGS := \
	-I$(srctree)/launcher/ui \
	-I$(srctree)/launcher \
	-I$(srctree)/buildconfig -I$(KBUILD_OUTPUT)/obj/generated \
	-I$(srctree)/libnbtplusplus/include \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/quazip \
	-I$(srctree)/qdcss/include \
	-I$(srctree)/tomlplusplus/include \
	-I$(srctree)/LocalPeer \
	-I$(srctree)/rainbow/include \
	-I$(srctree)/cmark/src \
	-I$(srctree)/murmur2/src \
	-I$(srctree)/gamemode/lib \
	-I$(srctree)/systeminfo/include \
	-I$(srctree)/libqrencode \
	-I$(QT_PREFIX)/include/QtConcurrent \
	-I$(QT_PREFIX)/include/QtNetworkAuth \
	-I$(QT_PREFIX)/include/QtXml \
	-I$(QT_PREFIX)/include/QtOpenGL \
	-I$(QT_PREFIX)/include/QtOpenGLWidgets \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(LAUNCHER_HUB_DEFINES) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

# MOC needs Qt includes to resolve Q_INTERFACES macros
# Use QT_CFLAGS which already includes proper Qt paths for both system and bundled Qt
MOC_INCLUDES := \
	$(QT_CFLAGS) \
	-I$(srctree)/launcher/ui \
	-I$(srctree)/launcher \
	-I$(obj)

# ============================================================================
# BUILD TARGETS
# ============================================================================
.PHONY: all ui ui-clean

all: ui

ui: $(UIC_HEADERS) $(UI_LIB)

$(UI_LIB): $(UI_ALL_OBJS) $(UI_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Pattern rules for compilation
# Note: -I$(dir $@) ensures inline moc files are found (e.g., dialogs/InstallLoaderDialog.moc)
$(obj)/%.o: $(srctree)/launcher/ui/%.cpp | $(UIC_HEADERS)
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UI_CXXFLAGS) -I$(dir $@) -c -o $@ $<

$(obj)/%.mm.o: $(srctree)/launcher/ui/%.mm | $(UIC_HEADERS)
	@mkdir -p $(dir $@)
	@echo "  OBJCXX  $<"
	$(Q)$(CXX) $(UI_CXXFLAGS) -I$(dir $@) -ObjC++ -c -o $@ $<

# UIC processing - different paths
$(obj)/ui_%.h: $(srctree)/launcher/ui/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

# Flatten nested UI files
$(obj)/ui_%.h: $(srctree)/launcher/ui/dialogs/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/global/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/instance/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

# Modplatform pages
$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/atlauncher/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/flame/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/import_ftb/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/legacy_ftb/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/modrinth/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/modplatform/technic/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

# Dialogs/skins
$(obj)/ui_%.h: $(srctree)/launcher/ui/dialogs/skins/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/widgets/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/setupwizard/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

# MOC processing - flatten all paths
$(obj)/moc_%.cpp: $(srctree)/launcher/ui/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/dialogs/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/instanceview/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pagedialog/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pages/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pages/global/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pages/instance/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pages/modplatform/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/dialogs/skins/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/java/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/tasks/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/setupwizard/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/themes/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/widgets/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UI_CXXFLAGS) -c -o $@ $<

# ============================================================================
# INLINE MOC PROCESSING (for files that #include "X.moc")
# ============================================================================

# Inline MOC rules - generate .moc from .cpp
$(obj)/dialogs/%.moc: $(srctree)/launcher/ui/dialogs/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/instanceview/%.moc: $(srctree)/launcher/ui/instanceview/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/java/%.moc: $(srctree)/launcher/ui/java/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/pages/instance/%.moc: $(srctree)/launcher/ui/pages/instance/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

$(obj)/widgets/%.moc: $(srctree)/launcher/ui/widgets/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) $(MOC_INCLUDES) -o $@ $<

# Inline MOC dependencies
$(obj)/dialogs/InstallLoaderDialog.o: $(obj)/dialogs/InstallLoaderDialog.moc
$(obj)/instanceview/InstanceDelegate.o: $(obj)/instanceview/InstanceDelegate.moc
$(obj)/java/InstallJavaDialog.o: $(obj)/java/InstallJavaDialog.moc
$(obj)/pages/instance/ManagedPackPage.o: $(obj)/pages/instance/ManagedPackPage.moc
$(obj)/pages/instance/ScreenshotsPage.o: $(obj)/pages/instance/ScreenshotsPage.moc
$(obj)/pages/instance/ServersPage.o: $(obj)/pages/instance/ServersPage.moc
$(obj)/pages/instance/VersionPage.o: $(obj)/pages/instance/VersionPage.moc
$(obj)/pages/instance/WorldListPage.o: $(obj)/pages/instance/WorldListPage.moc
$(obj)/widgets/CheckComboBox.o: $(obj)/widgets/CheckComboBox.moc
$(obj)/widgets/LauncherHubWidget.o: $(obj)/widgets/LauncherHubWidget.moc
$(obj)/widgets/ModFilterWidget.o: $(obj)/widgets/ModFilterWidget.moc
$(obj)/widgets/WideBar.o: $(obj)/widgets/WideBar.moc

ui-clean:
	@echo "  CLEAN   ui"
	$(Q)rm -rf $(obj)

-include $(UI_ALL_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
