# SPDX-License-Identifier: LGPL-2.1+
# libqrencode wrapper Makefile

srctree ?= $(shell dirname $(CURDIR))
KBUILD_OUTPUT ?= $(srctree)/build

OBJDIR := $(KBUILD_OUTPUT)/libqrencode
LIBDIR := $(KBUILD_OUTPUT)/lib

ifeq ($(V),1)
Q :=
else
Q := @
endif

# Sources
SOURCES := \
    qrencode.c \
    qrinput.c \
    bitstream.c \
    qrspec.c \
    rsecc.c \
    split.c \
    mask.c \
    mqrspec.c \
    mmask.c

OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))

# Compiler flags
CFLAGS ?= -O2 -g -fPIC -Wall -pipe
# Force config.h usage so version/static macros are always defined.
override CFLAGS += -DHAVE_CONFIG_H=1
INCLUDES := -I$(CURDIR)

ifeq ($(WINDOWS_TOOLCHAIN),msvc)
OBJ_EXT := obj
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.$(OBJ_EXT)))
INCLUDES := /I$(CURDIR)
QRENCODE_COMPILE = $(CC) $(CFLAGS) $(INCLUDES) /c /Fo$@ $<
QRENCODE_AR = $(AR) /nologo /OUT:$@ $^
else ifneq ($(filter lib lib.exe,$(notdir $(firstword $(AR)))),)
OBJ_EXT := obj
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.$(OBJ_EXT)))
INCLUDES := /I$(CURDIR)
QRENCODE_COMPILE = $(CC) $(CFLAGS) $(INCLUDES) /c /Fo$@ $<
QRENCODE_AR = $(AR) /nologo /OUT:$@ $^
else
OBJ_EXT := o
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.$(OBJ_EXT)))
QRENCODE_COMPILE = $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
QRENCODE_AR = $(AR) rcs $@ $^
endif

all: $(LIBDIR)/libqrencode.a

$(LIBDIR)/libqrencode.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(Q)$(QRENCODE_AR)
	@echo "  Built "

$(OBJDIR)/%.$(OBJ_EXT): %.c | $(OBJDIR)
	@echo "  CC      $<"
	@mkdir -p $(dir $@)
	$(Q)$(QRENCODE_COMPILE)

$(OBJDIR):
	@mkdir -p $@

clean:
	$(Q)rm -rf $(OBJDIR) $(LIBDIR)/libqrencode.a

.PHONY: all clean
