# ProjT Launcher - Dependencies Build Rules
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick
#
# This file contains rules for building fork libraries without CMake

.PHONY: deps-zlib deps-bzip2 deps-quazip deps-cmark deps-libnbt deps-qrencode deps-tomlpp

DEPS_LIB_DIR := $(BUILDDIR)/deps/lib
DEPS_INC_DIR := $(BUILDDIR)/deps/include

# List of all dependency static libraries
DEP_LIBS := \
    $(DEPS_LIB_DIR)/libz.a \
    $(DEPS_LIB_DIR)/libbz2.a \
    $(DEPS_LIB_DIR)/libquazip.a \
    $(DEPS_LIB_DIR)/libcmark.a \
    $(DEPS_LIB_DIR)/libnbt++.a \
    $(DEPS_LIB_DIR)/libqrencode.a

#============================================================================
# ZLIB
#============================================================================

ZLIB_SRCDIR := $(SRCDIR)/zlib
ZLIB_OBJDIR := $(BUILDDIR)/obj/zlib

ZLIB_SRCS := \
    adler32.c compress.c crc32.c deflate.c gzclose.c gzlib.c gzread.c \
    gzwrite.c infback.c inffast.c inflate.c inftrees.c trees.c uncompr.c zutil.c

ZLIB_OBJS := $(patsubst %.c,$(ZLIB_OBJDIR)/%.o,$(ZLIB_SRCS))

deps-zlib: $(DEPS_LIB_DIR)/libz.a
	@echo "[DEPS] zlib built"

$(DEPS_LIB_DIR)/libz.a: $(ZLIB_OBJS) | $(DEPS_LIB_DIR)
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $(ZLIB_OBJS)

$(ZLIB_OBJDIR)/%.o: $(ZLIB_SRCDIR)/%.c | $(ZLIB_OBJDIR)
	@echo "[CC] zlib/$(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -I$(ZLIB_SRCDIR) -c -o $@ $<

$(ZLIB_OBJDIR):
	@mkdir -p $@

# Copy zlib headers
zlib-headers: | $(DEPS_INC_DIR)
	$(Q)cp $(ZLIB_SRCDIR)/zlib.h $(DEPS_INC_DIR)/
	$(Q)cp $(ZLIB_SRCDIR)/zconf.h $(DEPS_INC_DIR)/

#============================================================================
# BZIP2
#============================================================================

BZIP2_SRCDIR := $(SRCDIR)/bzip2
BZIP2_OBJDIR := $(BUILDDIR)/obj/bzip2

BZIP2_SRCS := \
    blocksort.c huffman.c crctable.c randtable.c compress.c decompress.c bzlib.c

BZIP2_OBJS := $(patsubst %.c,$(BZIP2_OBJDIR)/%.o,$(BZIP2_SRCS))

deps-bzip2: $(DEPS_LIB_DIR)/libbz2.a
	@echo "[DEPS] bzip2 built"

$(DEPS_LIB_DIR)/libbz2.a: $(BZIP2_OBJS) | $(DEPS_LIB_DIR)
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $(BZIP2_OBJS)

$(BZIP2_OBJDIR)/%.o: $(BZIP2_SRCDIR)/%.c | $(BZIP2_OBJDIR)
	@echo "[CC] bzip2/$(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -I$(BZIP2_SRCDIR) -c -o $@ $<

$(BZIP2_OBJDIR):
	@mkdir -p $@

# Copy bzip2 headers
bzip2-headers: | $(DEPS_INC_DIR)
	$(Q)cp $(BZIP2_SRCDIR)/bzlib.h $(DEPS_INC_DIR)/

#============================================================================
# QUAZIP
#============================================================================

QUAZIP_SRCDIR := $(SRCDIR)/quazip/quazip
QUAZIP_OBJDIR := $(BUILDDIR)/obj/quazip

QUAZIP_SRCS := \
    JlCompress.cpp \
    qiodevice.cpp \
    quaadler32.cpp \
    quachecksum32.cpp \
    quacrc32.cpp \
    quagzipfile.cpp \
    quaziodevice.cpp \
    quazip.cpp \
    quazipdir.cpp \
    quazipfile.cpp \
    quazipfileinfo.cpp \
    quazipnewinfo.cpp \
    unzip.c \
    zip.c

QUAZIP_OBJS := $(patsubst %.cpp,$(QUAZIP_OBJDIR)/%.o,$(filter %.cpp,$(QUAZIP_SRCS)))
QUAZIP_OBJS += $(patsubst %.c,$(QUAZIP_OBJDIR)/%.o,$(filter %.c,$(QUAZIP_SRCS)))

QUAZIP_CXXFLAGS := $(CXXFLAGS) $(QT_CFLAGS) -I$(QUAZIP_SRCDIR) -I$(ZLIB_SRCDIR) -I$(BZIP2_SRCDIR) -DQUAZIP_STATIC

deps-quazip: deps-zlib deps-bzip2 $(DEPS_LIB_DIR)/libquazip.a
	@echo "[DEPS] quazip built"

$(DEPS_LIB_DIR)/libquazip.a: $(QUAZIP_OBJS) | $(DEPS_LIB_DIR)
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $(QUAZIP_OBJS)

$(QUAZIP_OBJDIR)/%.o: $(QUAZIP_SRCDIR)/%.cpp | $(QUAZIP_OBJDIR)
	@echo "[CXX] quazip/$(notdir $<)"
	$(Q)$(CXX) $(QUAZIP_CXXFLAGS) -c -o $@ $<

$(QUAZIP_OBJDIR)/%.o: $(QUAZIP_SRCDIR)/%.c | $(QUAZIP_OBJDIR)
	@echo "[CC] quazip/$(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -I$(QUAZIP_SRCDIR) -I$(ZLIB_SRCDIR) -c -o $@ $<

$(QUAZIP_OBJDIR):
	@mkdir -p $@

#============================================================================
# CMARK
#============================================================================

CMARK_SRCDIR := $(SRCDIR)/cmark/src
CMARK_OBJDIR := $(BUILDDIR)/obj/cmark

CMARK_SRCS := \
    cmark.c \
    node.c \
    iterator.c \
    blocks.c \
    inlines.c \
    scanners.c \
    utf8.c \
    buffer.c \
    references.c \
    render.c \
    man.c \
    html.c \
    commonmark.c \
    xml.c \
    latex.c \
    houdini_href_e.c \
    houdini_html_e.c \
    houdini_html_u.c

CMARK_OBJS := $(patsubst %.c,$(CMARK_OBJDIR)/%.o,$(CMARK_SRCS))

deps-cmark: $(DEPS_LIB_DIR)/libcmark.a
	@echo "[DEPS] cmark built"

$(DEPS_LIB_DIR)/libcmark.a: $(CMARK_OBJS) | $(DEPS_LIB_DIR)
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $(CMARK_OBJS)

$(CMARK_OBJDIR)/%.o: $(CMARK_SRCDIR)/%.c | $(CMARK_OBJDIR)
	@echo "[CC] cmark/$(notdir $<)"
	$(Q)$(CC) $(CFLAGS) -I$(CMARK_SRCDIR) -I$(SRCDIR)/cmark -c -o $@ $<

$(CMARK_OBJDIR):
	@mkdir -p $@

#============================================================================
# LIBNBTPLUSPLUS
#============================================================================

NBT_SRCDIR := $(SRCDIR)/libnbtplusplus
NBT_OBJDIR := $(BUILDDIR)/obj/libnbt

NBT_SRCS := \
    src/endian_str.cpp \
    src/tag_array.cpp \
    src/tag_compound.cpp \
    src/tag_list.cpp \
    src/tag_primitive.cpp \
    src/tag_string.cpp \
    src/text/json_formatter.cpp \
    src/value.cpp \
    src/value_initializer.cpp \
    src/io/stream_reader.cpp \
    src/io/stream_writer.cpp

NBT_OBJS := $(patsubst src/%.cpp,$(NBT_OBJDIR)/%.o,$(NBT_SRCS))

NBT_CXXFLAGS := $(CXXFLAGS) -I$(NBT_SRCDIR)/include -I$(ZLIB_SRCDIR)

deps-libnbt: deps-zlib $(DEPS_LIB_DIR)/libnbt++.a
	@echo "[DEPS] libnbtplusplus built"

$(DEPS_LIB_DIR)/libnbt++.a: $(NBT_OBJS) | $(DEPS_LIB_DIR)
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $(NBT_OBJS)

$(NBT_OBJDIR)/%.o: $(NBT_SRCDIR)/src/%.cpp | $(NBT_OBJDIR)
	@mkdir -p $(dir $@)
	@echo "[CXX] libnbt/$(notdir $<)"
	$(Q)$(CXX) $(NBT_CXXFLAGS) -c -o $@ $<

$(NBT_OBJDIR):
	@mkdir -p $@
	@mkdir -p $@/text
	@mkdir -p $@/io

#============================================================================
# LIBQRENCODE
#============================================================================

QRENCODE_SRCDIR := $(SRCDIR)/libqrencode
QRENCODE_OBJDIR := $(BUILDDIR)/obj/qrencode

QRENCODE_SRCS := \
    bitstream.c \
    mask.c \
    mmask.c \
    mqrspec.c \
    qrencode.c \
    qrinput.c \
    qrspec.c \
    rsecc.c \
    split.c

QRENCODE_OBJS := $(patsubst %.c,$(QRENCODE_OBJDIR)/%.o,$(QRENCODE_SRCS))

QRENCODE_CFLAGS := $(CFLAGS) -I$(QRENCODE_SRCDIR) -DHAVE_CONFIG_H

deps-qrencode: $(DEPS_LIB_DIR)/libqrencode.a
	@echo "[DEPS] libqrencode built"

$(DEPS_LIB_DIR)/libqrencode.a: $(QRENCODE_OBJS) | $(DEPS_LIB_DIR)
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $(QRENCODE_OBJS)

$(QRENCODE_OBJDIR)/%.o: $(QRENCODE_SRCDIR)/%.c | $(QRENCODE_OBJDIR)
	@echo "[CC] qrencode/$(notdir $<)"
	$(Q)$(CC) $(QRENCODE_CFLAGS) -c -o $@ $<

$(QRENCODE_OBJDIR):
	@mkdir -p $@

#============================================================================
# TOMLPLUSPLUS (Header-only, just copy headers)
#============================================================================

deps-tomlpp: | $(DEPS_INC_DIR)
	@echo "[DEPS] tomlplusplus (header-only)"
	$(Q)mkdir -p $(DEPS_INC_DIR)/toml++
	$(Q)cp -r $(SRCDIR)/tomlplusplus/include/toml++/* $(DEPS_INC_DIR)/toml++/

#============================================================================
# ALL HEADERS
#============================================================================

deps-headers: zlib-headers bzip2-headers deps-tomlpp
	@echo "[DEPS] All headers copied"
