# ProjT Launcher - Linux Platform Rules
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick

#============================================================================
# LINUX-SPECIFIC SETTINGS
#============================================================================

# Platform libraries
PLATFORM_LIBS := -lpthread -ldl

# DBus support (optional)
DBUS_CFLAGS :=
DBUS_LIBS :=

# Check for DBus via pkg-config
ifneq ($(shell pkg-config --exists dbus-1 2>/dev/null && echo yes),)
    DBUS_CFLAGS := $(shell pkg-config --cflags dbus-1)
    DBUS_LIBS := $(shell pkg-config --libs dbus-1)
    CXXFLAGS += -DHAVE_DBUS $(DBUS_CFLAGS)
    PLATFORM_LIBS += $(DBUS_LIBS)
endif

# Gamemode support
ifneq ($(shell pkg-config --exists gamemode 2>/dev/null && echo yes),)
    CXXFLAGS += -DHAVE_GAMEMODE
    PLATFORM_LIBS += $(shell pkg-config --libs gamemode)
endif

#============================================================================
# LINUX INSTALLATION PATHS
#============================================================================

# XDG Base Directory Specification
BINDIR := $(PREFIX)/bin
DATADIR := $(PREFIX)/share
ICONDIR := $(DATADIR)/icons/hicolor
APPDIR := $(DATADIR)/applications
METAINFODIR := $(DATADIR)/metainfo
MIMEDIR := $(DATADIR)/mime/packages
MANDIR := $(DATADIR)/man

#============================================================================
# LINUX-SPECIFIC TARGETS
#============================================================================

.PHONY: install-linux install-desktop install-icons

install-linux: install install-desktop install-icons
	@echo "[INSTALL] Linux installation complete"

install-desktop: $(GEN_DIR)/org.projecttick.ProjTLauncher.desktop
	@echo "[INSTALL] Desktop file..."
	$(Q)install -Dm644 $< $(DESTDIR)$(APPDIR)/org.projecttick.ProjTLauncher.desktop

install-icons:
	@echo "[INSTALL] Icons..."
	$(Q)install -Dm644 $(SRCDIR)/program_info/org.projecttick.ProjTLauncher.svg \
	    $(DESTDIR)$(ICONDIR)/scalable/apps/org.projecttick.ProjTLauncher.svg

install-metainfo: $(GEN_DIR)/org.projecttick.ProjTLauncher.metainfo.xml
	@echo "[INSTALL] Metainfo..."
	$(Q)install -Dm644 $< $(DESTDIR)$(METAINFODIR)/org.projecttick.ProjTLauncher.metainfo.xml

install-mime:
	@echo "[INSTALL] MIME types..."
	$(Q)install -Dm644 $(SRCDIR)/program_info/modrinth-mrpack-mime.xml \
	    $(DESTDIR)$(MIMEDIR)/org.projecttick.ProjTLauncher-mrpack.xml
