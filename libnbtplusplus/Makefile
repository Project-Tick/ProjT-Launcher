# SPDX-License-Identifier: LGPL-3.0
# libnbtplusplus - Makefile
#
# Build rules for NBT format library.

# Library name
lib := nbt++

# Source files
lib-y := \
	src/endian_str.o \
	src/tag.o \
	src/tag_compound.o \
	src/tag_list.o \
	src/tag_string.o \
	src/value.o \
	src/value_initializer.o \
	src/io/stream_reader.o \
	src/io/stream_writer.o \
	src/io/ozlibstream.o \
	src/io/izlibstream.o \
	src/text/json_formatter.o

# Include paths
includes-y := libnbtplusplus/include zlib

# Compiler flags
cxxflags-y := -std=c++17

# Dependencies
deps-y := zlib

# Include common build rules
include $(srctree)/mk/module.mk
