# SPDX-License-Identifier: LGPL-2.1
# quazip - Makefile
#
# Build rules for Qt ZIP wrapper library.

# Library name
lib := quazip

# Source files
lib-y := \
	quazip/JlCompress.o \
	quazip/qioapi.o \
	quazip/quaadler32.o \
	quazip/quachecksum32.o \
	quazip/quacrc32.o \
	quazip/quagzipfile.o \
	quazip/quaziodevice.o \
	quazip/quazip.o \
	quazip/quazip_textcodec.o \
	quazip/quazipdir.o \
	quazip/quazipfile.o \
	quazip/quazipfileinfo.o \
	quazip/quazipnewinfo.o \
	quazip/unzip.o \
	quazip/zip.o

# Include paths
includes-y := quazip zlib bzip2

# Compiler flags
ccflags-y := -DQUAZIP_STATIC
cxxflags-y := -DQUAZIP_STATIC

# Qt MOC headers
moc-y := \
	quazip/quagzipfile.h \
	quazip/quaziodevice.h \
	quazip/quazipfile.h

# Qt dependencies
qt-modules-y := Core

# Dependencies
deps-y := zlib bzip2
