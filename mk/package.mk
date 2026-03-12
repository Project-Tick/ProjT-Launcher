# SPDX-License-Identifier: GPL-2.0
# ProjT Launcher - Packaging System
#
# This file provides targets for creating distribution packages
# for various platforms: Linux (deb, rpm, AppImage, Flatpak, Snap),
# Windows (NSIS, MSI, ZIP), macOS (DMG, PKG).

include mk/config.mk
include mk/platform.mk

# ============================================================================
# Package Information
# ============================================================================

PKG_NAME := $(call cfg-unquote,$(CONFIG_PACKAGE_NAME))
ifeq ($(PKG_NAME),)
    PKG_NAME := projtlauncher
endif

PKG_VERSION := $(call cfg-unquote,$(CONFIG_PACKAGE_VERSION))
ifeq ($(PKG_VERSION),)
    PKG_VERSION := $(PROJECT_VERSION)
endif

PKG_MAINTAINER := $(call cfg-unquote,$(CONFIG_PACKAGE_MAINTAINER))
ifeq ($(PKG_MAINTAINER),)
    PKG_MAINTAINER := Project Tick <projecttick@projecttick.org>
endif

PKG_DESCRIPTION := A custom Minecraft launcher
PKG_HOMEPAGE := https://projecttick.org/p/projt-launcher
PKG_LICENSE := GPL-3.0-only

# Package output directory
PKG_OUTPUT := $(KBUILD_OUTPUT)/packages
PKG_STAGING := $(KBUILD_OUTPUT)/pkg-staging

# ============================================================================
# Directory Setup
# ============================================================================

$(PKG_OUTPUT) $(PKG_STAGING):
	@mkdir -p $@

# ============================================================================
# Linux DEB Package
# ============================================================================

DEB_ARCH := $(shell dpkg --print-architecture 2>/dev/null || echo amd64)
DEB_FILE := $(PKG_OUTPUT)/$(PKG_NAME)_$(PKG_VERSION)_$(DEB_ARCH).deb

# Application ID for desktop files
APP_ID := org.projecttick.ProjTLauncher
APP_BINARY := projtlauncher
APP_DISPLAY_NAME := ProjT Launcher
APP_COMMON_NAME := ProjT Launcher

# Source directories
PROGRAM_INFO := $(srctree)/program_info

package-deb: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building DEB package..."
	$(Q)rm -rf $(PKG_STAGING)/deb
	$(Q)mkdir -p $(PKG_STAGING)/deb/DEBIAN
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/bin
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/applications
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/scalable/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/16x16/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/24x24/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/32x32/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/48x48/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/64x64/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/128x128/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/icons/hicolor/256x256/apps
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/doc/$(PKG_NAME)
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/metainfo
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/mime/packages
	$(Q)mkdir -p $(PKG_STAGING)/deb/usr/share/man/man6
	
	# Copy binary
	$(Q)cp $(BINDIR)/projtlauncher$(EXE_SUFFIX) $(PKG_STAGING)/deb/usr/bin/
	$(Q)chmod 755 $(PKG_STAGING)/deb/usr/bin/projtlauncher$(EXE_SUFFIX)
	
	# Generate and copy desktop file
	$(Q)sed -e 's|@Launcher_DisplayName@|$(APP_DISPLAY_NAME)|g' \
	        -e 's|@Launcher_APP_BINARY_NAME@|$(APP_BINARY)|g' \
	        -e 's|@Launcher_AppID@|$(APP_ID)|g' \
	        -e 's|@Launcher_CommonName@|$(APP_COMMON_NAME)|g' \
	        $(PROGRAM_INFO)/$(APP_ID).desktop.in > $(PKG_STAGING)/deb/usr/share/applications/$(APP_ID).desktop
	
	# Copy SVG icon
	$(Q)cp $(PROGRAM_INFO)/$(APP_ID).svg $(PKG_STAGING)/deb/usr/share/icons/hicolor/scalable/apps/
	
	# Generate PNG icons if rsvg-convert available
	$(Q)if command -v rsvg-convert >/dev/null 2>&1; then \
	    for size in 16 24 32 48 64 128 256; do \
	        rsvg-convert -w $$size -h $$size $(PROGRAM_INFO)/$(APP_ID).svg \
	            -o $(PKG_STAGING)/deb/usr/share/icons/hicolor/$${size}x$${size}/apps/$(APP_ID).png; \
	    done; \
	else \
	    echo "  NOTE: rsvg-convert not found, skipping PNG icon generation"; \
	fi
	
	# Generate and copy metainfo
	$(Q)sed -e 's|@Launcher_DisplayName@|$(APP_DISPLAY_NAME)|g' \
	        -e 's|@Launcher_APP_BINARY_NAME@|$(APP_BINARY)|g' \
	        -e 's|@Launcher_AppID@|$(APP_ID)|g' \
	        -e 's|@Launcher_CommonName@|$(APP_COMMON_NAME)|g' \
	        $(PROGRAM_INFO)/$(APP_ID).metainfo.xml.in > $(PKG_STAGING)/deb/usr/share/metainfo/$(APP_ID).metainfo.xml 2>/dev/null || true
	
	# Copy MIME type definition
	$(Q)cp $(PROGRAM_INFO)/modrinth-mrpack-mime.xml $(PKG_STAGING)/deb/usr/share/mime/packages/ 2>/dev/null || true
	
	# Generate man page if scdoc available
	$(Q)if command -v scdoc >/dev/null 2>&1; then \
	    scdoc < $(PROGRAM_INFO)/projtlauncher.6.scd > $(PKG_STAGING)/deb/usr/share/man/man6/projtlauncher.6 2>/dev/null || true; \
	    gzip -9 $(PKG_STAGING)/deb/usr/share/man/man6/projtlauncher.6 2>/dev/null || true; \
	fi
	
	# Copy license and docs
	$(Q)cp $(srctree)/COPYING $(PKG_STAGING)/deb/usr/share/doc/$(PKG_NAME)/copyright 2>/dev/null || true
	$(Q)cp $(srctree)/README.md $(PKG_STAGING)/deb/usr/share/doc/$(PKG_NAME)/ 2>/dev/null || true
	
	# Create control file
	@echo "Package: $(PKG_NAME)" > $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Version: $(PKG_VERSION)" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Section: games" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Priority: optional" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Architecture: $(DEB_ARCH)" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Maintainer: $(PKG_MAINTAINER)" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Description: $(PKG_DESCRIPTION)" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Homepage: $(PKG_HOMEPAGE)" >> $(PKG_STAGING)/deb/DEBIAN/control
	@echo "Depends: libqt6widgets6, libqt6network6, libqt6core6, libqt6networkauth6, libcmark0.30.3, zlib1g" >> $(PKG_STAGING)/deb/DEBIAN/control
	
	# Create postinst for MIME database update
	@echo '#!/bin/sh' > $(PKG_STAGING)/deb/DEBIAN/postinst
	@echo 'set -e' >> $(PKG_STAGING)/deb/DEBIAN/postinst
	@echo 'if [ -x /usr/bin/update-mime-database ]; then update-mime-database /usr/share/mime || true; fi' >> $(PKG_STAGING)/deb/DEBIAN/postinst
	@echo 'if [ -x /usr/bin/update-desktop-database ]; then update-desktop-database || true; fi' >> $(PKG_STAGING)/deb/DEBIAN/postinst
	@echo 'if [ -x /usr/bin/gtk-update-icon-cache ]; then gtk-update-icon-cache /usr/share/icons/hicolor || true; fi' >> $(PKG_STAGING)/deb/DEBIAN/postinst
	$(Q)chmod 755 $(PKG_STAGING)/deb/DEBIAN/postinst
	
	# Build package
	$(Q)dpkg-deb --build $(PKG_STAGING)/deb $(DEB_FILE)
	@echo "Created: $(DEB_FILE)"

# ============================================================================
# Linux RPM Package
# ============================================================================

RPM_ARCH := $(shell uname -m)
RPM_FILE := $(PKG_OUTPUT)/$(PKG_NAME)-$(PKG_VERSION)-1.$(RPM_ARCH).rpm

package-rpm: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building RPM package..."
	$(Q)rm -rf $(PKG_STAGING)/rpm
	$(Q)mkdir -p $(PKG_STAGING)/rpm/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/bin
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/lib/projtlauncher
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/applications
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/scalable/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/16x16/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/24x24/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/32x32/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/48x48/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/64x64/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/128x128/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/256x256/apps
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/metainfo
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/mime/packages
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/man/man6
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/usr/share/doc/$(PKG_NAME)
	$(Q)mkdir -p $(PKG_STAGING)/rpm/BUILDROOT/etc/ld.so.conf.d
	
	# Copy binary
	$(Q)cp $(BINDIR)/projtlauncher$(EXE_SUFFIX) $(PKG_STAGING)/rpm/BUILDROOT/usr/bin/
	$(Q)chmod 755 $(PKG_STAGING)/rpm/BUILDROOT/usr/bin/projtlauncher
	
	# Copy shared libraries from build
	$(Q)cp -P $(LIBDIR)/libcmark.so* $(PKG_STAGING)/rpm/BUILDROOT/usr/lib/projtlauncher/ 2>/dev/null || true
	$(Q)for lib in $(LIBDIR)/*.so*; do \
		if [ -f "$$lib" ]; then \
			cp -P "$$lib" $(PKG_STAGING)/rpm/BUILDROOT/usr/lib/projtlauncher/; \
		fi; \
	done 2>/dev/null || true
	
	# Create ldconfig file so system can find our bundled libraries
	@echo '/usr/lib/projtlauncher' > $(PKG_STAGING)/rpm/BUILDROOT/etc/ld.so.conf.d/projtlauncher.conf
	
	# Generate and copy desktop file
	$(Q)sed -e 's|@Launcher_DisplayName@|$(APP_DISPLAY_NAME)|g' \
	        -e 's|@Launcher_APP_BINARY_NAME@|$(APP_BINARY)|g' \
	        -e 's|@Launcher_AppID@|$(APP_ID)|g' \
	        -e 's|@Launcher_CommonName@|$(APP_COMMON_NAME)|g' \
	        $(PROGRAM_INFO)/$(APP_ID).desktop.in > $(PKG_STAGING)/rpm/BUILDROOT/usr/share/applications/$(APP_ID).desktop
	
	# Copy SVG icon
	$(Q)cp $(PROGRAM_INFO)/$(APP_ID).svg $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/scalable/apps/
	
	# Generate PNG icons if rsvg-convert available
	$(Q)if command -v rsvg-convert >/dev/null 2>&1; then \
	    for size in 16 24 32 48 64 128 256; do \
	        rsvg-convert -w $$size -h $$size $(PROGRAM_INFO)/$(APP_ID).svg \
	            -o $(PKG_STAGING)/rpm/BUILDROOT/usr/share/icons/hicolor/$${size}x$${size}/apps/$(APP_ID).png; \
	    done; \
	fi
	
	# Generate and copy metainfo
	$(Q)sed -e 's|@Launcher_DisplayName@|$(APP_DISPLAY_NAME)|g' \
	        -e 's|@Launcher_APP_BINARY_NAME@|$(APP_BINARY)|g' \
	        -e 's|@Launcher_AppID@|$(APP_ID)|g' \
	        -e 's|@Launcher_CommonName@|$(APP_COMMON_NAME)|g' \
	        $(PROGRAM_INFO)/$(APP_ID).metainfo.xml.in > $(PKG_STAGING)/rpm/BUILDROOT/usr/share/metainfo/$(APP_ID).metainfo.xml 2>/dev/null || true
	
	# Copy MIME type definition
	$(Q)cp $(PROGRAM_INFO)/modrinth-mrpack-mime.xml $(PKG_STAGING)/rpm/BUILDROOT/usr/share/mime/packages/ 2>/dev/null || true
	
	# Generate man page if scdoc available
	$(Q)if command -v scdoc >/dev/null 2>&1; then \
	    scdoc < $(PROGRAM_INFO)/projtlauncher.6.scd > $(PKG_STAGING)/rpm/BUILDROOT/usr/share/man/man6/projtlauncher.6 2>/dev/null || true; \
	    gzip -9 $(PKG_STAGING)/rpm/BUILDROOT/usr/share/man/man6/projtlauncher.6 2>/dev/null || true; \
	fi
	
	# Copy license and docs
	$(Q)cp $(srctree)/COPYING $(PKG_STAGING)/rpm/BUILDROOT/usr/share/doc/$(PKG_NAME)/ 2>/dev/null || true
	$(Q)cp $(srctree)/README.md $(PKG_STAGING)/rpm/BUILDROOT/usr/share/doc/$(PKG_NAME)/ 2>/dev/null || true
	
	# Create spec file
	@echo "Name: $(PKG_NAME)" > $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "Version: $(PKG_VERSION)" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "Release: 1" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "Summary: $(PKG_DESCRIPTION)" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "License: $(PKG_LICENSE)" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "URL: $(PKG_HOMEPAGE)" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "Requires: qt6-qtbase qt6-qtnetworkauth" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "AutoReqProv: no" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%description" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "$(PKG_DESCRIPTION)" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "Discover, manage, and play Minecraft instances with mods, modpacks," >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "resource packs, and more. Supports CurseForge, Modrinth, and others." >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%files" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/projtlauncher" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%dir /usr/lib/projtlauncher" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/lib/projtlauncher/*" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%config /etc/ld.so.conf.d/projtlauncher.conf" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/share/applications/$(APP_ID).desktop" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/share/icons/hicolor/scalable/apps/$(APP_ID).svg" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/share/icons/hicolor/*/apps/$(APP_ID).png" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%dir /usr/share/doc/$(PKG_NAME)" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/share/doc/$(PKG_NAME)/*" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%ghost /usr/share/metainfo/$(APP_ID).metainfo.xml" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%ghost /usr/share/mime/packages/modrinth-mrpack-mime.xml" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%ghost /usr/share/man/man6/projtlauncher.6.gz" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%post" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/sbin/ldconfig" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/update-mime-database /usr/share/mime &>/dev/null || :" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/update-desktop-database &>/dev/null || :" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/gtk-update-icon-cache /usr/share/icons/hicolor &>/dev/null || :" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "%postun" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/sbin/ldconfig" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/update-mime-database /usr/share/mime &>/dev/null || :" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/update-desktop-database &>/dev/null || :" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	@echo "/usr/bin/gtk-update-icon-cache /usr/share/icons/hicolor &>/dev/null || :" >> $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	
	# Build package
	$(Q)rpmbuild --define "_topdir $(PKG_STAGING)/rpm" \
		--buildroot $(PKG_STAGING)/rpm/BUILDROOT \
		-bb $(PKG_STAGING)/rpm/SPECS/$(PKG_NAME).spec
	$(Q)cp $(PKG_STAGING)/rpm/RPMS/$(RPM_ARCH)/*.rpm $(PKG_OUTPUT)/
	@echo "Created: $(RPM_FILE)"

# ============================================================================
# Linux AppImage
# ============================================================================

APPIMAGE_FILE := $(PKG_OUTPUT)/$(PKG_NAME)-$(PKG_VERSION)-$(shell uname -m).AppImage
APPIMAGETOOL ?= appimagetool

package-appimage: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building AppImage..."
	$(Q)rm -rf $(PKG_STAGING)/AppDir
	$(Q)mkdir -p $(PKG_STAGING)/AppDir/usr/bin
	$(Q)mkdir -p $(PKG_STAGING)/AppDir/usr/lib
	$(Q)mkdir -p $(PKG_STAGING)/AppDir/usr/share/applications
	$(Q)mkdir -p $(PKG_STAGING)/AppDir/usr/share/icons/hicolor/256x256/apps
	
	# Copy binary
	$(Q)cp $(BINDIR)/projtlauncher$(EXE_SUFFIX) $(PKG_STAGING)/AppDir/usr/bin/
	
	# Create desktop file
	@echo "[Desktop Entry]" > $(PKG_STAGING)/AppDir/usr/share/applications/$(PKG_NAME).desktop
	@echo "Type=Application" >> $(PKG_STAGING)/AppDir/usr/share/applications/$(PKG_NAME).desktop
	@echo "Name=ProjT Launcher" >> $(PKG_STAGING)/AppDir/usr/share/applications/$(PKG_NAME).desktop
	@echo "Exec=projtlauncher" >> $(PKG_STAGING)/AppDir/usr/share/applications/$(PKG_NAME).desktop
	@echo "Icon=projtlauncher" >> $(PKG_STAGING)/AppDir/usr/share/applications/$(PKG_NAME).desktop
	@echo "Categories=Game;" >> $(PKG_STAGING)/AppDir/usr/share/applications/$(PKG_NAME).desktop
	
	# Link desktop and icon to AppDir root
	$(Q)ln -sf usr/share/applications/$(PKG_NAME).desktop $(PKG_STAGING)/AppDir/$(PKG_NAME).desktop
	
	# Create AppRun
	@echo '#!/bin/bash' > $(PKG_STAGING)/AppDir/AppRun
	@echo 'APPDIR="$$(dirname "$$(readlink -f "$$0")")"' >> $(PKG_STAGING)/AppDir/AppRun
	@echo 'export LD_LIBRARY_PATH="$$APPDIR/usr/lib:$$LD_LIBRARY_PATH"' >> $(PKG_STAGING)/AppDir/AppRun
	@echo 'exec "$$APPDIR/usr/bin/projtlauncher" "$$@"' >> $(PKG_STAGING)/AppDir/AppRun
	$(Q)chmod +x $(PKG_STAGING)/AppDir/AppRun
	
	# Build AppImage
	$(Q)ARCH=$(shell uname -m) $(APPIMAGETOOL) $(PKG_STAGING)/AppDir $(APPIMAGE_FILE)
	@echo "Created: $(APPIMAGE_FILE)"

# ============================================================================
# Linux Flatpak
# ============================================================================

FLATPAK_ID := org.projecttick.ProjTLauncher

package-flatpak: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building Flatpak..."
	$(Q)mkdir -p $(PKG_STAGING)/flatpak
	
	# Create manifest
	@echo "app-id: $(FLATPAK_ID)" > $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "runtime: org.kde.Platform" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "runtime-version: '6.6'" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "sdk: org.kde.Sdk" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "command: projtlauncher" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "modules:" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "  - name: projtlauncher" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "    buildsystem: simple" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "    build-commands:" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	@echo "      - install -Dm755 projtlauncher /app/bin/projtlauncher" >> $(PKG_STAGING)/flatpak/$(FLATPAK_ID).yaml
	
	# Build
	$(Q)cd $(PKG_STAGING)/flatpak && \
		flatpak-builder --force-clean build $(FLATPAK_ID).yaml
	$(Q)flatpak build-export $(PKG_OUTPUT)/flatpak-repo $(PKG_STAGING)/flatpak/build
	@echo "Flatpak repository created at: $(PKG_OUTPUT)/flatpak-repo"

# ============================================================================
# Windows NSIS Installer
# ============================================================================

NSIS_FILE := $(PKG_OUTPUT)/$(PKG_NAME)-$(PKG_VERSION)-setup.exe
MAKENSIS ?= makensis

package-nsis: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building NSIS installer..."
	$(Q)rm -rf $(PKG_STAGING)/nsis
	$(Q)mkdir -p $(PKG_STAGING)/nsis
	
	# Copy files
	$(Q)cp $(BINDIR)/projtlauncher.exe $(PKG_STAGING)/nsis/
	
	# Create NSIS script
	@echo '!define PRODUCT_NAME "ProjT Launcher"' > $(PKG_STAGING)/nsis/installer.nsi
	@echo '!define PRODUCT_VERSION "$(PKG_VERSION)"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '!define PRODUCT_PUBLISHER "Project Tick"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '!define PRODUCT_WEB_SITE "$(PKG_HOMEPAGE)"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'Name "$${PRODUCT_NAME} $${PRODUCT_VERSION}"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'OutFile "$(NSIS_FILE)"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'InstallDir "$$PROGRAMFILES64\ProjT Launcher"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'RequestExecutionLevel admin' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'Section "Install"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  SetOutPath $$INSTDIR' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  File "projtlauncher.exe"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  CreateShortcut "$$DESKTOP\ProjT Launcher.lnk" "$$INSTDIR\projtlauncher.exe"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  CreateDirectory "$$SMPROGRAMS\ProjT Launcher"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  CreateShortcut "$$SMPROGRAMS\ProjT Launcher\ProjT Launcher.lnk" "$$INSTDIR\projtlauncher.exe"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  WriteUninstaller "$$INSTDIR\uninstall.exe"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'SectionEnd' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'Section "Uninstall"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  Delete "$$INSTDIR\projtlauncher.exe"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  Delete "$$INSTDIR\uninstall.exe"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  Delete "$$DESKTOP\ProjT Launcher.lnk"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  RMDir /r "$$SMPROGRAMS\ProjT Launcher"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo '  RMDir "$$INSTDIR"' >> $(PKG_STAGING)/nsis/installer.nsi
	@echo 'SectionEnd' >> $(PKG_STAGING)/nsis/installer.nsi
	
	# Build installer
	$(Q)cd $(PKG_STAGING)/nsis && $(MAKENSIS) installer.nsi
	@echo "Created: $(NSIS_FILE)"

# ============================================================================
# Windows ZIP Package
# ============================================================================

ZIP_FILE := $(PKG_OUTPUT)/$(PKG_NAME)-$(PKG_VERSION)-windows.zip

package-zip: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building ZIP package..."
	$(Q)rm -rf $(PKG_STAGING)/zip
	$(Q)mkdir -p $(PKG_STAGING)/zip/$(PKG_NAME)
	$(Q)cp $(BINDIR)/projtlauncher.exe $(PKG_STAGING)/zip/$(PKG_NAME)/
	$(Q)cd $(PKG_STAGING)/zip && zip -r $(ZIP_FILE) $(PKG_NAME)
	@echo "Created: $(ZIP_FILE)"

# ============================================================================
# macOS DMG
# ============================================================================

DMG_FILE := $(PKG_OUTPUT)/$(PKG_NAME)-$(PKG_VERSION).dmg
APP_BUNDLE := $(PKG_STAGING)/$(PKG_NAME).app

package-dmg: build | $(PKG_OUTPUT) $(PKG_STAGING)
	@echo "Building macOS DMG..."
	$(Q)rm -rf $(APP_BUNDLE)
	$(Q)mkdir -p $(APP_BUNDLE)/Contents/MacOS
	$(Q)mkdir -p $(APP_BUNDLE)/Contents/Resources
	$(Q)mkdir -p $(APP_BUNDLE)/Contents/Frameworks
	
	# Copy binary
	$(Q)cp $(BINDIR)/projtlauncher $(APP_BUNDLE)/Contents/MacOS/
	
	# Create Info.plist
	@echo '<?xml version="1.0" encoding="UTF-8"?>' > $(APP_BUNDLE)/Contents/Info.plist
	@echo '<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '<plist version="1.0">' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '<dict>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>CFBundleExecutable</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>projtlauncher</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>CFBundleIdentifier</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>org.projecttick.ProjTLauncher</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>CFBundleName</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>ProjT Launcher</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>CFBundleShortVersionString</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>$(PKG_VERSION)</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>CFBundleVersion</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>$(PKG_VERSION)</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>LSMinimumSystemVersion</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>11.0</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <key>NSHighResolutionCapable</key>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '    <string>true</string>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '</dict>' >> $(APP_BUNDLE)/Contents/Info.plist
	@echo '</plist>' >> $(APP_BUNDLE)/Contents/Info.plist
	
	# Create DMG
	$(Q)hdiutil create -volname "ProjT Launcher" -srcfolder $(APP_BUNDLE) \
		-ov -format UDZO $(DMG_FILE)
	@echo "Created: $(DMG_FILE)"

# ============================================================================
# macOS PKG
# ============================================================================

PKG_FILE := $(PKG_OUTPUT)/$(PKG_NAME)-$(PKG_VERSION).pkg

package-pkg: package-dmg | $(PKG_OUTPUT)
	@echo "Building macOS PKG..."
	$(Q)pkgbuild --root $(PKG_STAGING) \
		--identifier org.projecttick.ProjTLauncher \
		--version $(PKG_VERSION) \
		--install-location /Applications \
		$(PKG_FILE)
	@echo "Created: $(PKG_FILE)"

# ============================================================================
# Meta Targets
# ============================================================================

ifeq ($(TARGET_OS),linux)
    PACKAGE_TARGETS :=
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_DEB)),y)
        PACKAGE_TARGETS += package-deb
    endif
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_RPM)),y)
        PACKAGE_TARGETS += package-rpm
    endif
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_APPIMAGE)),y)
        PACKAGE_TARGETS += package-appimage
    endif
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_FLATPAK)),y)
        PACKAGE_TARGETS += package-flatpak
    endif
else ifeq ($(TARGET_OS),windows)
    PACKAGE_TARGETS :=
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_ZIP)),y)
        PACKAGE_TARGETS += package-zip
    endif
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_NSIS)),y)
        PACKAGE_TARGETS += package-nsis
    endif
else ifeq ($(TARGET_OS),macos)
    PACKAGE_TARGETS :=
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_DMG)),y)
        PACKAGE_TARGETS += package-dmg
    endif
    ifeq ($(call cfg-yes,$(CONFIG_PACKAGE_PKG)),y)
        PACKAGE_TARGETS += package-pkg
    endif
endif

package: $(PACKAGE_TARGETS)
	@echo "All packages built."

package-clean:
	$(Q)rm -rf $(PKG_OUTPUT) $(PKG_STAGING)

.PHONY: package package-deb package-rpm package-appimage package-flatpak
.PHONY: package-nsis package-zip package-dmg package-pkg package-clean
