# SPDX-License-Identifier: GPL-3.0
# launcher/screenshots - Screenshots Module Makefile

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

obj := $(KBUILD_OUTPUT)/screenshots

SCREENSHOTS_SRCS := \
	ImgurAlbumCreation.cpp \
	ImgurUpload.cpp

SCREENSHOTS_OBJS := $(addprefix $(obj)/, $(SCREENSHOTS_SRCS:.cpp=.o))
SCREENSHOTS_LIB := $(obj)/libscreenshots.a

# MOC
SCREENSHOTS_MOC_HEADERS := $(shell find $(srctree)/launcher/screenshots -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
SCREENSHOTS_MOC_SRCS := $(patsubst $(srctree)/launcher/screenshots/%.h,$(obj)/moc_%.cpp,$(SCREENSHOTS_MOC_HEADERS))
SCREENSHOTS_MOC_OBJS := $(SCREENSHOTS_MOC_SRCS:.cpp=.o)

SCREENSHOTS_CXXFLAGS := \
	-I$(srctree)/launcher/screenshots \
	-I$(srctree)/launcher \
	-I$(KBUILD_OUTPUT)/generated/buildconfig \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all screenshots screenshots-clean

all: screenshots

screenshots: $(SCREENSHOTS_LIB)

$(SCREENSHOTS_LIB): $(SCREENSHOTS_OBJS) $(SCREENSHOTS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/screenshots/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(SCREENSHOTS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/screenshots/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(SCREENSHOTS_CXXFLAGS) -c -o $@ $<

screenshots-clean:
	$(Q)rm -rf $(obj)

-include $(SCREENSHOTS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
