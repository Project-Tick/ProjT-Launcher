# SPDX-License-Identifier: GPL-2.0

include mk/toolchain.mk

MSVC := $(filter cl cl.exe,$(notdir $(CXX)))

# Base flags
ifneq ($(MSVC),)
CFLAGS += /W4
CXXFLAGS += /std:c++20 /W4 /permissive-
else
CFLAGS += -Wall -Wextra -fstack-protector-strong
CXXFLAGS += -std=gnu++23 -Wall -Wextra -fstack-protector-strong
endif

# Project-wide defines
CXXFLAGS += -DQT_WARN_DEPRECATED_UP_TO=0x060200 -DQT_DISABLE_DEPRECATED_UP_TO=0x060000
CXXFLAGS += -DTOML_ENABLE_FLOAT16=0
