# SPDX-License-Identifier: zlib-acknowledgement
# libpng wrapper Makefile

srctree ?= $(shell dirname $(CURDIR))
KBUILD_OUTPUT ?= $(srctree)/build

OBJDIR := $(KBUILD_OUTPUT)/libpng
LIBDIR := $(KBUILD_OUTPUT)/lib

# Zlib is required
ZLIB_DIR := $(srctree)/zlib

ifeq ($(V),1)
Q :=
else
Q := @
endif

# Sources (core libpng files only)
SOURCES := \
    png.c \
    pngerror.c \
    pngget.c \
    pngmem.c \
    pngpread.c \
    pngread.c \
    pngrio.c \
    pngrtran.c \
    pngrutil.c \
    pngset.c \
    pngtrans.c \
    pngwio.c \
    pngwrite.c \
    pngwtran.c \
    pngwutil.c

OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.o))

# Compiler flags
CFLAGS ?= -O2 -g -fPIC -Wall -pipe
CFLAGS += -DPNG_ARM_NEON_OPT=0
INCLUDES := -I$(CURDIR) -I$(ZLIB_DIR) -I$(OBJDIR)

ifeq ($(WINDOWS_TOOLCHAIN),msvc)
OBJ_EXT := obj
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.$(OBJ_EXT)))
INCLUDES := /I$(CURDIR) /I$(ZLIB_DIR) /I$(OBJDIR)
LIBPNG_COMPILE = $(CC) $(CFLAGS) $(INCLUDES) /c /Fo$@ $<
LIBPNG_AR = $(AR) /nologo /OUT:$@ $^
else
OBJ_EXT := o
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.c=.$(OBJ_EXT)))
LIBPNG_COMPILE = $(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<
LIBPNG_AR = $(AR) rcs $@ $^
endif

all: $(LIBDIR)/libpng.a

$(LIBDIR)/libpng.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(Q)$(LIBPNG_AR)
	@echo "  Built "

# Generate pnglibconf.h from template if not exists
$(OBJDIR)/pnglibconf.h: $(CURDIR)/scripts/pnglibconf.h.prebuilt | $(OBJDIR)
	@echo "  GEN     pnglibconf.h"
	$(Q)cp $< $@

$(OBJDIR)/%.$(OBJ_EXT): $(CURDIR)/%.c $(OBJDIR)/pnglibconf.h | $(OBJDIR)
	@echo "  CC      $<"
	$(Q)$(LIBPNG_COMPILE)

$(OBJDIR):
	@mkdir -p $@

clean:
	$(Q)rm -rf $(OBJDIR) $(LIBDIR)/libpng.a

.PHONY: all clean
