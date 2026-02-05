# SPDX-License-Identifier: LGPL-2.1
# quazip - Makefile
#
# Build rules for Qt ZIP wrapper library.

# Library name
lib := quazip

# Define OBJDIR early for moc rules
OBJDIR := $(KBUILD_OUTPUT)/obj/quazip

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

# Qt tools - use exported QT_MOC from mk/qt.mk
MOC := $(QT_MOC)
ifeq ($(MOC),)
MOC := $(KBUILD_OUTPUT)/obj/qt/install/libexec/moc
endif

# Add moc objects to library
EXTRA_OBJS := \
	$(OBJDIR)/moc_quagzipfile.o \
	$(OBJDIR)/moc_quaziodevice.o \
	$(OBJDIR)/moc_quazipfile.o

# MOC rules
$(OBJDIR)/moc_quagzipfile.cpp: $(CURDIR)/quazip/quagzipfile.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/quazip $< -o $@

$(OBJDIR)/moc_quaziodevice.cpp: $(CURDIR)/quazip/quaziodevice.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/quazip $< -o $@

$(OBJDIR)/moc_quazipfile.cpp: $(CURDIR)/quazip/quazipfile.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/quazip $< -o $@

$(OBJDIR)/moc_%.o: $(OBJDIR)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(BASE_CXXFLAGS) $(QT_CFLAGS) -DQUAZIP_STATIC -I$(srctree)/quazip -I$(srctree)/zlib -c -o $@ $<

# Include common build rules
include $(srctree)/mk/module.mk

# Override library to include MOC objects
$(STATIC_LIB): $(OBJS) $(EXTRA_OBJS) | $(LIBDIR)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
	$(Q)$(RANLIB) $@
