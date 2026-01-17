# ProjT Launcher - Windows Platform Rules
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick

#============================================================================
# WINDOWS-SPECIFIC SETTINGS
#============================================================================

# Platform libraries
PLATFORM_LIBS := -lws2_32 -lshlwapi -lole32 -luuid -lversion -ladvapi32 -lshell32 -luser32 -lgdi32 -lcomdlg32

# Windows-specific compiler flags
CXXFLAGS += -DUNICODE -D_UNICODE -DWIN32_LEAN_AND_MEAN -DNOMINMAX

# Static linking for C++ runtime (optional, for portable builds)
ifdef STATIC_RUNTIME
    LDFLAGS += -static-libgcc -static-libstdc++
endif

#============================================================================
# WINDOWS RESOURCE COMPILATION
#============================================================================

WINDRES := windres

# Windows resource file
RC_FILE := $(GEN_DIR)/projtlauncher.rc
RES_FILE := $(BUILDDIR)/obj/projtlauncher.res.o

# Resource compilation rule
$(RES_FILE): $(RC_FILE) | $(BUILDDIR)/obj
	@echo "[RC] $(notdir $<)"
	$(Q)$(WINDRES) -I$(SRCDIR)/program_info -I$(GEN_DIR) -i $< -o $@

# Add resource to launcher objects
LAUNCHER_OBJS += $(RES_FILE)

#============================================================================
# WINDOWS MANIFEST
#============================================================================

MANIFEST_FILE := $(GEN_DIR)/projtlauncher.manifest

# Embed manifest using mt.exe (if available) or link with windres
ifdef MT
win-manifest: $(LAUNCHER_BIN)
	@echo "[MT] Embedding manifest..."
	$(Q)$(MT) -manifest $(MANIFEST_FILE) -outputresource:$(LAUNCHER_BIN);1
endif

#============================================================================
# CONSOLE SUBSYSTEM HANDLING
#============================================================================

# By default, we build as a GUI application (no console window)
# Use -mconsole for debug builds to see console output
ifeq ($(DEBUG),1)
    LDFLAGS += -mconsole
else
    LDFLAGS += -mwindows
endif

#============================================================================
# WINDOWS-SPECIFIC TARGETS
#============================================================================

.PHONY: installer portable

# Create NSIS installer (requires NSIS installed and in PATH)
installer: launcher $(GEN_DIR)/win_install.nsi
	@echo "[NSIS] Creating installer..."
	$(Q)makensis $(GEN_DIR)/win_install.nsi

# Create portable package
portable: launcher
	@echo "[PKG] Creating portable package..."
	$(Q)mkdir -p $(BUILDDIR)/portable
	$(Q)cp $(LAUNCHER_BIN) $(BUILDDIR)/portable/
	$(Q)cp $(SRCDIR)/program_info/portable.txt $(BUILDDIR)/portable/
	$(Q)cp -r $(BUILDDIR)/deps/lib/*.dll $(BUILDDIR)/portable/ 2>/dev/null || true
	@echo "[PKG] Portable package created at $(BUILDDIR)/portable/"

#============================================================================
# DLL COPYING
#============================================================================

# Copy required Qt DLLs for distribution
copy-qt-dlls:
	@echo "[DLL] Copying Qt DLLs..."
	$(Q)mkdir -p $(BUILDDIR)/bin
	$(Q)cp $(QT_BINDIR)/Qt6Core.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6Gui.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6Widgets.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6Network.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6Concurrent.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6Xml.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6Core5Compat.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6NetworkAuth.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	$(Q)cp $(QT_BINDIR)/Qt6OpenGL.dll $(BUILDDIR)/bin/ 2>/dev/null || true
	@echo "[DLL] Qt DLLs copied"

# Copy Qt plugins
copy-qt-plugins:
	@echo "[DLL] Copying Qt plugins..."
	$(Q)mkdir -p $(BUILDDIR)/bin/platforms
	$(Q)mkdir -p $(BUILDDIR)/bin/styles
	$(Q)mkdir -p $(BUILDDIR)/bin/imageformats
	$(Q)cp $(QT_PREFIX)/plugins/platforms/qwindows.dll $(BUILDDIR)/bin/platforms/ 2>/dev/null || true
	$(Q)cp $(QT_PREFIX)/plugins/styles/*.dll $(BUILDDIR)/bin/styles/ 2>/dev/null || true
	$(Q)cp $(QT_PREFIX)/plugins/imageformats/*.dll $(BUILDDIR)/bin/imageformats/ 2>/dev/null || true
	@echo "[DLL] Qt plugins copied"

# Full Windows distribution
dist-windows: launcher copy-qt-dlls copy-qt-plugins
	@echo "[DIST] Windows distribution ready at $(BUILDDIR)/bin/"
