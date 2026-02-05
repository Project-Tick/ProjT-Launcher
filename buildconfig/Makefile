# SPDX-License-Identifier: GPL-2.0
# BuildConfig module - generates build configuration from template

srctree ?= $(shell dirname $(CURDIR))
KBUILD_OUTPUT ?= $(srctree)/build

OBJDIR := $(KBUILD_OUTPUT)/buildconfig
LIBDIR := $(KBUILD_OUTPUT)/lib

# Qt paths - use defaults if .config doesn't exist
-include $(KBUILD_OUTPUT)/.config
QT_INSTALL_PREFIX ?= $(if $(CONFIG_QT_PREFIX),$(CONFIG_QT_PREFIX),/usr)
QT_BIN := $(QT_INSTALL_PREFIX)/libexec

# Version defaults (from main Makefile)
VERSION ?= 1
PATCHLEVEL ?= 0
SUBLEVEL ?= 5
EXTRAVERSION ?= -1
PROJT_VERSION ?= $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)$(EXTRAVERSION)

# Branding defaults (from main Makefile)
LAUNCHER_DISPLAYNAME ?= ProjT Launcher
LAUNCHER_DOMAIN ?= projecttick.org
LAUNCHER_BUG_TRACKER_URL ?= https://github.com/Project-Tick/ProjT-Launcher/issues
LAUNCHER_NEWS_RSS_URL ?= https://projecttick.org/product/projt-launcher/feed.xml
LAUNCHER_DISCORD_URL ?= https://projecttick.org/projtlauncher/discord

# Source files
SOURCES := BuildConfig.cpp
HEADERS := BuildConfig.h

OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:.cpp=.o))

# Detect Qt include path
QT_INCLUDE := $(shell pkg-config --cflags-only-I Qt6Core 2>/dev/null | sed 's/-I//g')
ifeq ($(QT_INCLUDE),)
QT_INCLUDE := $(QT_INSTALL_PREFIX)/include/qt6
endif

# Compiler flags
CXXFLAGS ?= -O2 -g -fPIC -std=c++17 -Wall -pipe
INCLUDES := -I$(srctree)/include \
            -I$(KBUILD_OUTPUT)/buildconfig \
            -I$(srctree)/buildconfig \
            $(shell pkg-config --cflags Qt6Core Qt6Gui 2>/dev/null || echo "-I$(QT_INCLUDE) -I$(QT_INCLUDE)/QtCore -I$(QT_INCLUDE)/QtGui")

ifeq ($(V),1)
Q :=
else
Q := @
endif

all: $(LIBDIR)/libbuildconfig.a

$(LIBDIR)/libbuildconfig.a: $(OBJECTS)
	@mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $^
	@echo "  Built "

$(OBJDIR)/BuildConfig.cpp: $(srctree)/buildconfig/BuildConfig.cpp.in | $(OBJDIR)
	@echo "  GEN     $@"
	$(Q)sed \
		-e 's|@Launcher_BUG_TRACKER_URL@|$(LAUNCHER_BUG_TRACKER_URL)|g' \
		-e 's|@Launcher_MATRIX_URL@||g' \
		-e 's|@Launcher_DISCORD_URL@|$(LAUNCHER_DISCORD_URL)|g' \
		-e 's|@Launcher_SUBREDDIT_URL@||g' \
		-e 's|@Launcher_BUILD_PLATFORM@|$(shell uname -s)|g' \
		-e 's|@Launcher_IMGUR_CLIENT_ID@||g' \
		-e 's|@Launcher_MSA_CLIENT_ID@||g' \
		-e 's|@Launcher_CURSEFORGE_API_KEY@||g' \
		-e 's|@Launcher_MODRINTH_STAGING_URL@|https://staging-api.modrinth.com/v2|g' \
		-e 's|@Launcher_MODRINTH_PROD_URL@|https://api.modrinth.com/v2|g' \
		-e 's|@Launcher_FLAME_BASE_URL@|https://api.curseforge.com|g' \
		-e 's|@Launcher_APPLICATION_NAME@|$(LAUNCHER_DISPLAYNAME)|g' \
		-e 's|@Launcher_HELP_URL@|https://$(LAUNCHER_DOMAIN)/wiki|g' \
		-e 's|@Launcher_NEWS_RSS_URL@|$(LAUNCHER_NEWS_RSS_URL)|g' \
		-e 's|@Launcher_NEWS_OPEN_URL@|https://$(LAUNCHER_DOMAIN)/news|g' \
		-e 's|@Launcher_PASTE_EE_KEY@||g' \
		-e 's|@Launcher_GIT_COMMIT@|$(shell git -C $(srctree) rev-parse --short HEAD 2>/dev/null || echo unknown)|g' \
		-e 's|@Launcher_GIT_TAG@|$(shell git -C $(srctree) describe --tags --abbrev=0 2>/dev/null || echo dev)|g' \
		-e 's|@Launcher_GIT_REFSPEC@|$(shell git -C $(srctree) rev-parse --abbrev-ref HEAD 2>/dev/null || echo main)|g' \
		-e 's|@Launcher_VERSION_NAME@|$(PROJT_VERSION)|g' \
		-e 's|@Launcher_VERSION_MAJOR@|$(VERSION)|g' \
		-e 's|@Launcher_VERSION_MINOR@|$(PATCHLEVEL)|g' \
		-e 's|@Launcher_VERSION_PATCH@|$(SUBLEVEL)|g' \
		-e 's|@Launcher_VERSION_TWEAK@|0|g' \
		-e 's|@Launcher_UPDATER_ENABLED@|false|g' \
		-e 's|@Launcher_SPARKLE_ENABLED@|false|g' \
		-e 's|@Launcher_CUSTOM_UPDATER_ENABLED@|false|g' \
		-e 's|#cmakedefine01 Launcher_ENABLE_JAVA_DOWNLOADER|#define Launcher_ENABLE_JAVA_DOWNLOADER 1|g' \
		-e 's|@Launcher_[A-Za-z_]*@||g' \
		$< > $@

$(OBJDIR)/%.o: $(OBJDIR)/%.cpp | $(OBJDIR)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $@

clean:
	$(Q)rm -rf $(OBJDIR) $(LIBDIR)/libbuildconfig.a

.PHONY: all clean
