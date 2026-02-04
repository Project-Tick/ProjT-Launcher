# SPDX-License-Identifier: GPL-2.0

# Shared config helpers for Make-based build front-end

srctree ?= $(CURDIR)
O ?= build
KBUILD_OUTPUT ?= $(abspath $(O))

KCONFIG_CONFIG ?= $(KBUILD_OUTPUT)/.config
KCONFIG_AUTOCONFIG ?= $(KBUILD_OUTPUT)/include/config/auto.conf

-include $(KCONFIG_AUTOCONFIG)
-include $(KCONFIG_CONFIG)

cfg-yes = $(filter y,$(strip $(1)))
cfg-on = $(if $(call cfg-yes,$(1)),ON,OFF)

cfg-unquote = $(strip $(subst ",,$(1)))

# Default build type if config is missing
CONFIG_BUILD_TYPE ?= "Debug"
CONFIG_QT_VERSION_MAJOR ?= "6"
CONFIG_QT_PREFIX ?= ""
CONFIG_QT_HOST_BINS ?= ""
CONFIG_USE_BUNDLED_QT ?= n
