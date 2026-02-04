# SPDX-License-Identifier: GPL-3.0
# launcher/ui - UI Module Makefile
#
# This is the main UI module containing all Qt-based user interface components.

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

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
UI_THEMES_OBJS := $(patsubst $(srctree)/launcher/ui/%.cpp,$(obj)/%.o,$(UI_THEMES_SRCS))

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
UIC_HEADERS := $(patsubst $(srctree)/launcher/ui/%.ui,$(obj)/ui_%.h,$(UI_FILES))

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
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

# ============================================================================
# BUILD TARGETS
# ============================================================================
.PHONY: ui ui-clean

ui: $(UIC_HEADERS) $(UI_LIB)

$(UI_LIB): $(UI_ALL_OBJS) $(UI_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Pattern rules for compilation
$(obj)/%.o: $(srctree)/launcher/ui/%.cpp | $(UIC_HEADERS)
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UI_CXXFLAGS) -c -o $@ $<

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
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/dialogs/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/instanceview/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pagedialog/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/pages/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/setupwizard/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/themes/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/ui/widgets/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/ui -I$(obj) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(UI_CXXFLAGS) -c -o $@ $<

ui-clean:
	@echo "  CLEAN   ui"
	$(Q)rm -rf $(obj)

-include $(UI_ALL_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
