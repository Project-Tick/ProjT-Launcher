# SPDX-License-Identifier: GPL-2.0

include mk/toolchain.mk

MSVC := $(filter cl cl.exe,$(notdir $(CXX)))

# C++ standard from Kconfig (default: -std=c++23)
CPP_STD := $(or $(CONFIG_CPP_STANDARD),-std=c++23)
C_STD := $(or $(CONFIG_C_STANDARD),-std=c23)

# Base flags
ifneq ($(MSVC),)
CFLAGS += /W4
CXXFLAGS += /std:c++20 /W4 /permissive-
else
CFLAGS += $(C_STD) -Wall -Wextra -fstack-protector-strong
CXXFLAGS += $(CPP_STD) -Wall -Wextra -fstack-protector-strong
endif

# Project-wide defines
CXXFLAGS += -DQT_WARN_DEPRECATED_UP_TO=0x060200 -DQT_DISABLE_DEPRECATED_UP_TO=0x060000
CXXFLAGS += -DTOML_ENABLE_FLOAT16=0
