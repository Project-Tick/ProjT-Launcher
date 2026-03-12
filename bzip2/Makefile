# PT2ziplib - Makefile
#
# Build rules for PT2ziplib compression library.

# Library name
lib := bz2

# Source files (library only, no executables)
lib-y := \
	blocksort.o \
	huffman.o \
	crctable.o \
	randtable.o \
	compress.o \
	decompress.o \
	bzlib.o

# Include paths
includes-y := bzip2

# Compiler flags
ccflags-y := -DBZ_NO_STDIO

# For large file support
ccflags-y += -D_FILE_OFFSET_BITS=64

# Include common build rules
include $(srctree)/mk/module.mk
