# SPDX-License-Identifier: GPL-3.0
# launcher/console - Console Module Makefile
#
# Console utilities for Windows console handling.

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Object output directory
obj := $(KBUILD_OUTPUT)/console

# Sources (Windows only for now)
ifeq ($(CONFIG_TARGET_WINDOWS),y)
CONSOLE_SRCS := WindowsConsole.cpp
else
CONSOLE_SRCS :=
endif

# Objects
CONSOLE_OBJS := $(addprefix $(obj)/, $(CONSOLE_SRCS:.cpp=.o))

# Library output
CONSOLE_LIB := $(obj)/libconsole.a

# Flags
CONSOLE_CXXFLAGS := \
	-I$(srctree)/launcher/console \
	-I$(srctree)/launcher \
	$(CXXFLAGS)

# Targets
.PHONY: console console-clean

console: $(CONSOLE_LIB)

$(CONSOLE_LIB): $(CONSOLE_OBJS)
ifneq ($(CONSOLE_OBJS),)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
else
	@mkdir -p $(dir $@)
	@echo "  AR      $@ (empty)"
	$(Q)$(AR) rcs $@
endif

$(obj)/%.o: $(srctree)/launcher/console/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(CONSOLE_CXXFLAGS) -c -o $@ $<

console-clean:
	@echo "  CLEAN   console"
	$(Q)rm -rf $(obj)

-include $(CONSOLE_OBJS:.o=.d)
