# SPDX-License-Identifier: GPL-3.0
# launcher/launch - Launch Pipeline Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/launch

# Core launch sources
LAUNCH_SRCS := \
	LaunchLineRouter.cpp \
	LaunchLogModel.cpp \
	LaunchPipeline.cpp \
	LaunchStage.cpp \
	LaunchVariableExpander.cpp \
	TaskBridgeStage.cpp

# Steps sources
LAUNCH_STEPS_SRCS := $(wildcard $(srctree)/launcher/launch/steps/*.cpp)
LAUNCH_STEPS_OBJS := $(patsubst $(srctree)/launcher/launch/steps/%.cpp,$(obj)/steps/%.o,$(LAUNCH_STEPS_SRCS))

LAUNCH_OBJS := $(addprefix $(obj)/, $(LAUNCH_SRCS:.cpp=.o))
LAUNCH_LIB := $(obj)/liblaunch.a

# MOC
LAUNCH_MOC_HEADERS := $(shell find $(srctree)/launcher/launch -name '*.hpp' -o -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
LAUNCH_MOC_SRCS := $(patsubst $(srctree)/launcher/launch/%.hpp,$(obj)/moc_%.cpp,$(LAUNCH_MOC_HEADERS))
LAUNCH_MOC_OBJS := $(LAUNCH_MOC_SRCS:.cpp=.o)

LAUNCH_CXXFLAGS := \
	-I$(srctree)/launcher/launch \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: launch launch-clean

launch: $(LAUNCH_LIB)

$(LAUNCH_LIB): $(LAUNCH_OBJS) $(LAUNCH_STEPS_OBJS) $(LAUNCH_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/launch/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LAUNCH_CXXFLAGS) -c -o $@ $<

$(obj)/steps/%.o: $(srctree)/launcher/launch/steps/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LAUNCH_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/launch/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LAUNCH_CXXFLAGS) -c -o $@ $<

launch-clean:
	$(Q)rm -rf $(obj)

-include $(LAUNCH_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
