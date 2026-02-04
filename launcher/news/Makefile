# SPDX-License-Identifier: GPL-3.0
# launcher/news - News Module Makefile

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

obj := $(KBUILD_OUTPUT)/news

NEWS_SRCS := \
	NewsChecker.cpp \
	NewsEntry.cpp

NEWS_OBJS := $(addprefix $(obj)/, $(NEWS_SRCS:.cpp=.o))
NEWS_LIB := $(obj)/libnews.a

# MOC
NEWS_MOC_HEADERS := $(shell find $(srctree)/launcher/news -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
NEWS_MOC_SRCS := $(patsubst $(srctree)/launcher/news/%.h,$(obj)/moc_%.cpp,$(NEWS_MOC_HEADERS))
NEWS_MOC_OBJS := $(NEWS_MOC_SRCS:.cpp=.o)

NEWS_CXXFLAGS := \
	-I$(srctree)/launcher/news \
	-I$(srctree)/launcher \
	-I$(QT_PREFIX)/include/QtXml \
	-I$(KBUILD_OUTPUT)/generated/buildconfig \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all news news-clean

all: news

news: $(NEWS_LIB)

$(NEWS_LIB): $(NEWS_OBJS) $(NEWS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/news/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(NEWS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/news/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(NEWS_CXXFLAGS) -c -o $@ $<

news-clean:
	$(Q)rm -rf $(obj)

-include $(NEWS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
