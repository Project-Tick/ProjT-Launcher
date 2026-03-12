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

ifeq ($(WINDOWS_TOOLCHAIN),msvc)
MOC_OBJ_EXT := obj
MOC_QT_CFLAGS := $(patsubst /I%,-I%,$(QT_CFLAGS))
else
MOC_OBJ_EXT := o
MOC_QT_CFLAGS := $(QT_CFLAGS)
endif

# Add moc objects to library
EXTRA_OBJS := \
	$(OBJDIR)/moc_quagzipfile.$(MOC_OBJ_EXT) \
	$(OBJDIR)/moc_quaziodevice.$(MOC_OBJ_EXT) \
	$(OBJDIR)/moc_quazipfile.$(MOC_OBJ_EXT)

# MOC rules
$(OBJDIR)/moc_quagzipfile.cpp: $(CURDIR)/quazip/quagzipfile.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(MOC_QT_CFLAGS) -I$(srctree)/quazip $< -o $@

$(OBJDIR)/moc_quaziodevice.cpp: $(CURDIR)/quazip/quaziodevice.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(MOC_QT_CFLAGS) -I$(srctree)/quazip $< -o $@

$(OBJDIR)/moc_quazipfile.cpp: $(CURDIR)/quazip/quazipfile.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(MOC_QT_CFLAGS) -I$(srctree)/quazip $< -o $@

$(OBJDIR)/moc_%.$(MOC_OBJ_EXT): $(OBJDIR)/moc_%.cpp
	@echo "  CXX     $<"
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
	$(Q)$(CXX) $(BASE_CXXFLAGS) $(QT_CFLAGS) -DQUAZIP_STATIC /I$(srctree)/quazip /I$(srctree)/zlib /c /Fo$@ $<
else
	$(Q)$(CXX) $(BASE_CXXFLAGS) $(QT_CFLAGS) -DQUAZIP_STATIC -I$(srctree)/quazip -I$(srctree)/zlib -c -o $@ $<
endif

# Mark that we have custom STATIC_LIB rule
CUSTOM_STATIC_LIB_RULE := 1

# Include common build rules
include $(srctree)/mk/module.mk

# Custom library rule to include MOC objects
$(STATIC_LIB): $(OBJS) $(EXTRA_OBJS) | $(LIBDIR)
ifeq ($(WINDOWS_TOOLCHAIN),msvc)
	@echo "  LIB     $@"
	$(Q)$(AR) /nologo /OUT:$@ $^
else
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^
ifneq ($(strip $(RANLIB)),)
	$(Q)$(RANLIB) $@
endif
endif
