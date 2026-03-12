# SPDX-License-Identifier: GPL-2.0

include mk/config.mk
include mk/host.mk

CROSS_PREFIX := $(call cfg-unquote,$(CONFIG_CROSS_COMPILE))
SYSROOT := $(call cfg-unquote,$(CONFIG_SYSROOT))

CC_CFG := $(call cfg-unquote,$(CONFIG_CC))
CXX_CFG := $(call cfg-unquote,$(CONFIG_CXX))
AR_CFG := $(call cfg-unquote,$(CONFIG_AR))
STRIP_CFG := $(call cfg-unquote,$(CONFIG_STRIP))

ifneq ($(CROSS_PREFIX),)
CC ?= $(CROSS_PREFIX)gcc
CXX ?= $(CROSS_PREFIX)g++
AR ?= $(CROSS_PREFIX)ar
STRIP ?= $(CROSS_PREFIX)strip
RANLIB ?= $(CROSS_PREFIX)ranlib
endif

ifneq ($(CC_CFG),)
CC := $(CC_CFG)
endif
ifneq ($(CXX_CFG),)
CXX := $(CXX_CFG)
endif
ifneq ($(AR_CFG),)
AR := $(AR_CFG)
endif
ifneq ($(STRIP_CFG),)
STRIP := $(STRIP_CFG)
endif

CFLAGS += $(call cfg-unquote,$(CONFIG_CFLAGS_EXTRA))
CXXFLAGS += $(call cfg-unquote,$(CONFIG_CXXFLAGS_EXTRA))
LDFLAGS += $(call cfg-unquote,$(CONFIG_LDFLAGS_EXTRA))

ifneq ($(SYSROOT),)
CFLAGS += --sysroot=$(SYSROOT)
CXXFLAGS += --sysroot=$(SYSROOT)
LDFLAGS += --sysroot=$(SYSROOT)
endif

export CC CXX AR STRIP RANLIB CFLAGS CXXFLAGS LDFLAGS
