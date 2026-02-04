# SPDX-License-Identifier: GPL-2.0

include mk/buildconfig.mk
include mk/qt-gen.mk

# Helper to find sources
find-src = $(shell find $(1) -type f \( -name '*.c' -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' -o -name '*.mm' \) \
	-not -path '*/tests/*' -not -path '*/test/*' -not -path '*/bench*/*' -not -path '*/examples/*' -not -path '*/docs/*' -not -path '*/man/*' -not -path '*/cmake/*' -not -path '*/CMakeFiles/*')

# BuildConfig (Qt Core)
buildconfig_SRCS := $(BUILDCONFIG_CPP)
buildconfig_CPPFLAGS := -Ibuildconfig -I$(GEN_DIR)/buildconfig $(QT_CFLAGS)
buildconfig_CXXFLAGS := 

# systeminfo (Qt Core/Gui/Network)
systeminfo_SRCS := $(filter-out systeminfo/src/sys_test.cpp,$(call find-src,systeminfo/src))
systeminfo_CPPFLAGS := -Isysteminfo/include -Isysteminfo/src $(QT_CFLAGS)

# murmur2
murmur2_SRCS := $(call find-src,murmur2)
murmur2_CPPFLAGS := -Imurmur2

# qdcss (Qt Core)
qdcss_SRCS := $(call find-src,qdcss/src)
qdcss_CPPFLAGS := -Iqdcss/include -Iqdcss/src $(QT_CFLAGS)

# rainbow (Qt Core/Gui)
rainbow_SRCS := $(call find-src,rainbow/src)
rainbow_CPPFLAGS := -Irainbow/include -Irainbow/src $(QT_CFLAGS)

# LocalPeer (Qt Core)
localpeer_SRCS := $(call find-src,LocalPeer)
localpeer_CPPFLAGS := -ILocalPeer $(QT_CFLAGS)

# libnbtplusplus
libnbtplusplus_SRCS := $(call find-src,libnbtplusplus/src)
libnbtplusplus_CPPFLAGS := -Ilibnbtplusplus/include -Ilibnbtplusplus/src -Izlib

# libqrencode
libqrencode_SRCS := $(call find-src,libqrencode/src)
libqrencode_CPPFLAGS := -Ilibqrencode/include -Ilibqrencode/src -Ilibqrencode/lib

# cmark
cmark_SRCS := $(call find-src,cmark/src)
cmark_CPPFLAGS := -Icmark/src -Icmark/src/commonmark

# bzip2 (lib only)
bzip2_SRCS := bzip2/blocksort.c bzip2/bzlib.c bzip2/crctable.c bzip2/huffman.c bzip2/randtable.c
bzip2_CPPFLAGS := -Ibzip2

# zlib (core sources)
zlib_SRCS := zlib/adler32.c zlib/compress.c zlib/crc32.c zlib/deflate.c zlib/infback.c zlib/inffast.c zlib/inflate.c zlib/inftrees.c zlib/trees.c zlib/uncompr.c zlib/zutil.c
zlib_CPPFLAGS := -Izlib

# libpng (core sources, skip contrib/tests)
LIBPNG_CONFIG_H := $(GEN_DIR)/libpng/pnglibconf.h
$(LIBPNG_CONFIG_H): libpng/scripts/pnglibconf.h.prebuilt
	@mkdir -p $(dir $@)
	@cp $< $@

libpng_SRCS := $(shell find libpng -maxdepth 1 -type f -name '*.c' -not -name 'example.c' -not -name 'pngtest.c')
libpng_CPPFLAGS := -I$(GEN_DIR)/libpng -Ilibpng -Ilibpng/include -Izlib

# minizip (from zlib contrib)
minizip_SRCS := $(shell find zlib/contrib/minizip -maxdepth 1 -type f -name '*.c')
minizip_CPPFLAGS := -Izlib -Izlib/contrib/minizip

# quazip (Qt + zlib + bzip2)
quazip_SRCS := $(call find-src,quazip/quazip)
quazip_CPPFLAGS := -Iquazip/quazip -Iquazip/quazip/private $(QT_CFLAGS) -Izlib -Ibzip2

# gamemode (Linux client library)
gamemode_SRCS := gamemode/lib/client_impl.c gamemode/lib/client_loader.c gamemode/common/common-helpers.c gamemode/common/common-pidfds.c
gamemode_CPPFLAGS := -Igamemode/lib -Igamemode/common -D_GNU_SOURCE
gamemode_DBUS_CFLAGS := $(shell pkg-config --cflags dbus-1 2>/dev/null)
gamemode_DBUS_LIBS := $(shell pkg-config --libs dbus-1 2>/dev/null)

# launcherjava (Java)
launcherjava_SRC := $(shell find launcherjava -type f -name '*.java')

# javacheck (Java)
javacheck_SRC := $(shell find javacheck -type f -name '*.java')

launcher_find = $(shell find launcher -type f \\( -name '*.c' -o -name '*.cpp' -o -name '*.cc' -o -name '*.cxx' -o -name '*.mm' \\) -not -path 'launcher/updater/*' -not -path 'launcher/filelink/*' -not -path 'launcher/tests/*' -not -path 'launcher/test/*' -not -path 'launcher/bench*/*')

# launcher (Qt app)
launcher_SRCS := $(launcher_find)
launcher_CPPFLAGS := -Ilauncher -Ilauncher/src -Ilauncher/logic -Ilauncher/logic/api -Ilauncher/logic/base -Ilauncher/logic/instance -Ilauncher/logic/java -Ilauncher/logic/net -Ilauncher/logic/tasks -Ilauncher/logic/minecraft -Ilauncher/logic/settings -Ijson/include -Itomlplusplus/include -Itomlplusplus $(QT_CFLAGS)

launcher_UI := $(shell find launcher -type f -name '*.ui' -not -path 'launcher/updater/*' -not -path 'launcher/filelink/*')
launcher_QRC := $(shell find launcher -type f -name '*.qrc' -not -path 'launcher/updater/*' -not -path 'launcher/filelink/*') $(PROGRAM_INFO_QRC)

# Qt codegen lists
launcher_MOC_HEADERS := $(shell python3 scripts/find_qt_moc.py launcher)
launcher_MOC_SRCS := $(foreach h,$(launcher_MOC_HEADERS),$(call qt-moc-out,$(h)))
launcher_UIC_HDRS := $(foreach u,$(launcher_UI),$(call qt-uic-out,$(u)))
launcher_RCC_SRCS := $(foreach r,$(launcher_QRC),$(call qt-rcc-out,$(r)))

# Generate rules for Qt codegen
$(foreach h,$(launcher_MOC_HEADERS),$(eval $(call qt-moc-rule,$(h))))
$(foreach u,$(launcher_UI),$(eval $(call qt-uic-rule,$(u))))
$(foreach r,$(launcher_QRC),$(eval $(call qt-rcc-rule,$(r))))

launcher_GEN_SRCS := $(launcher_MOC_SRCS) $(launcher_RCC_SRCS)
launcher_ALL_SRCS := $(launcher_SRCS) $(launcher_GEN_SRCS)

# Object lists
buildconfig_OBJS := $(call src-to-obj,$(buildconfig_SRCS))
systeminfo_OBJS := $(call src-to-obj,$(systeminfo_SRCS))
murmur2_OBJS := $(call src-to-obj,$(murmur2_SRCS))
qdcss_OBJS := $(call src-to-obj,$(qdcss_SRCS))
rainbow_OBJS := $(call src-to-obj,$(rainbow_SRCS))
localpeer_OBJS := $(call src-to-obj,$(localpeer_SRCS))
libnbtplusplus_OBJS := $(call src-to-obj,$(libnbtplusplus_SRCS))
libqrencode_OBJS := $(call src-to-obj,$(libqrencode_SRCS))
cmark_OBJS := $(call src-to-obj,$(cmark_SRCS))
bzip2_OBJS := $(call src-to-obj,$(bzip2_SRCS))
zlib_OBJS := $(call src-to-obj,$(zlib_SRCS))
libpng_OBJS := $(call src-to-obj,$(libpng_SRCS))
minizip_OBJS := $(call src-to-obj,$(minizip_SRCS))
quazip_OBJS := $(call src-to-obj,$(quazip_SRCS))
gamemode_OBJS := $(call src-to-obj,$(gamemode_SRCS))
launcher_OBJS := $(call src-to-obj,$(launcher_ALL_SRCS))

# Ensure UI headers are generated before compiling launcher sources
$(launcher_OBJS): | $(launcher_UIC_HDRS)

# Per-module include flags
$(buildconfig_OBJS): CPPFLAGS += $(buildconfig_CPPFLAGS)
$(systeminfo_OBJS): CPPFLAGS += $(systeminfo_CPPFLAGS)
$(murmur2_OBJS): CPPFLAGS += $(murmur2_CPPFLAGS)
$(qdcss_OBJS): CPPFLAGS += $(qdcss_CPPFLAGS)
$(rainbow_OBJS): CPPFLAGS += $(rainbow_CPPFLAGS)
$(localpeer_OBJS): CPPFLAGS += $(localpeer_CPPFLAGS)
$(libnbtplusplus_OBJS): CPPFLAGS += $(libnbtplusplus_CPPFLAGS)
$(libqrencode_OBJS): CPPFLAGS += $(libqrencode_CPPFLAGS)
$(cmark_OBJS): CPPFLAGS += $(cmark_CPPFLAGS)
$(bzip2_OBJS): CPPFLAGS += $(bzip2_CPPFLAGS)
$(zlib_OBJS): CPPFLAGS += $(zlib_CPPFLAGS)
$(libpng_OBJS): CPPFLAGS += $(libpng_CPPFLAGS)
$(libpng_OBJS): | $(LIBPNG_CONFIG_H)
$(minizip_OBJS): CPPFLAGS += $(minizip_CPPFLAGS)
$(quazip_OBJS): CPPFLAGS += $(quazip_CPPFLAGS)
$(gamemode_OBJS): CPPFLAGS += $(gamemode_CPPFLAGS) $(gamemode_DBUS_CFLAGS)
$(launcher_OBJS): CPPFLAGS += $(launcher_CPPFLAGS)
