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
	quazip/zip.o \
	moc_quagzipfile.o \
	moc_quaziodevice.o \
	moc_quazipfile.o

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

# Qt tools
QT_PREFIX ?= $(KBUILD_OUTPUT)/obj/qt/install
MOC := $(QT_PREFIX)/libexec/moc

# MOC rules
$(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quagzipfile.cpp: quazip/quagzipfile.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) $< -o $@

$(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quaziodevice.cpp: quazip/quaziodevice.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) $< -o $@

$(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quazipfile.cpp: quazip/quazipfile.h
	@echo "  MOC     $<"
	$(Q)mkdir -p $(dir $@)
	$(Q)$(MOC) $(QT_CFLAGS) $< -o $@

$(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quagzipfile.o: $(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quagzipfile.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c -o $@ $<

$(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quaziodevice.o: $(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quaziodevice.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c -o $@ $<

$(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quazipfile.o: $(KBUILD_OUTPUT)/obj/$(MODULE_PATH)/moc_quazipfile.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(CXXFLAGS) $(QT_CFLAGS) -c -o $@ $<

# Include common build rules
include $(srctree)/mk/module.mk
