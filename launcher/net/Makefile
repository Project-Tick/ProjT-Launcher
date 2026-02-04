# SPDX-License-Identifier: GPL-3.0
# launcher/net - Network Module Makefile

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

obj := $(KBUILD_OUTPUT)/net

NET_SRCS := \
	ApiDownload.cpp \
	ApiUpload.cpp \
	Download.cpp \
	FileSink.cpp \
	HttpMetaCache.cpp \
	Logging.cpp \
	MetaCacheSink.cpp \
	NetJob.cpp \
	NetRequest.cpp \
	PasteUpload.cpp \
	Upload.cpp

NET_OBJS := $(addprefix $(obj)/, $(NET_SRCS:.cpp=.o))
NET_LIB := $(obj)/libnet.a

# MOC
NET_MOC_HEADERS := $(shell find $(srctree)/launcher/net -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
NET_MOC_SRCS := $(patsubst $(srctree)/launcher/net/%.h,$(obj)/moc_%.cpp,$(NET_MOC_HEADERS))
NET_MOC_OBJS := $(NET_MOC_SRCS:.cpp=.o)

NET_CXXFLAGS := \
	-I$(srctree)/launcher/net \
	-I$(srctree)/launcher \
	-I$(KBUILD_OUTPUT)/generated/buildconfig \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all net net-clean

all: net

net: $(NET_LIB)

$(NET_LIB): $(NET_OBJS) $(NET_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/net/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(NET_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/net/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(NET_CXXFLAGS) -c -o $@ $<

net-clean:
	$(Q)rm -rf $(obj)

-include $(NET_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
