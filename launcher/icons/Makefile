# SPDX-License-Identifier: GPL-3.0
# launcher/icons - Icon Management Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Qt tools
QT_PREFIX ?= $(KBUILD_OUTPUT)/obj/qt/install
MOC := $(QT_PREFIX)/libexec/moc

# Object output directory
obj := $(KBUILD_OUTPUT)/icons

# Sources
ICONS_SRCS := \
	IconEntry.cpp \
	IconList.cpp \
	IconUtils.cpp

# Objects
ICONS_OBJS := $(addprefix $(obj)/, $(ICONS_SRCS:.cpp=.o))

# Library output
ICONS_LIB := $(obj)/libicons.a

# MOC files
ICONS_MOC_HEADERS := $(wildcard $(srctree)/launcher/icons/*.hpp)
ICONS_MOC_SRCS := $(patsubst $(srctree)/launcher/icons/%.hpp,$(obj)/moc_%.cpp,$(ICONS_MOC_HEADERS))
ICONS_MOC_OBJS := $(ICONS_MOC_SRCS:.cpp=.o)

# Flags
ICONS_CXXFLAGS := \
	-I$(srctree)/launcher/icons \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

# Targets
.PHONY: all icons icons-clean

all: icons

icons: $(ICONS_LIB)

$(ICONS_LIB): $(ICONS_OBJS) $(ICONS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/icons/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(ICONS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/icons/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -I$(srctree)/launcher/icons -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(ICONS_CXXFLAGS) -c -o $@ $<

icons-clean:
	@echo "  CLEAN   icons"
	$(Q)rm -rf $(obj)

-include $(ICONS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
