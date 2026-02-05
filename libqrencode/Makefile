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
# Define inline macros directly (config.h is problematic)
CFLAGS += -DSTATIC_IN_RELEASE=static -DMAJOR_VERSION=4 -DMINOR_VERSION=1 -DMICRO_VERSION=1 -DHAVE_STRDUP=1 -DVERSION='"4.1.1"'
INCLUDES := -I$(CURDIR)

all: $(LIBDIR)/libqrencode.a

$(LIBDIR)/libqrencode.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $^
	@echo "  Built "

$(OBJDIR)/%.o: $(CURDIR)/%.c | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $@

clean:
	$(Q)rm -rf $(OBJDIR) $(LIBDIR)/libqrencode.a

.PHONY: all clean
