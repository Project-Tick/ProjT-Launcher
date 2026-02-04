# SPDX-License-Identifier: GPL-3.0
# launcher/tools - External Tools Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

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
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: tools tools-clean

tools: $(TOOLS_LIB)

$(TOOLS_LIB): $(TOOLS_OBJS) $(TOOLS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

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
