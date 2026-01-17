# ProjT Launcher - macOS Platform Rules
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick

#============================================================================
# MACOS-SPECIFIC SETTINGS
#============================================================================

# Platform libraries (frameworks)
PLATFORM_LIBS := -framework Cocoa -framework IOKit -framework CoreFoundation -framework Security

# macOS-specific compiler flags
CXXFLAGS += -mmacosx-version-min=10.15

# Link with Objective-C++ for macOS integration
LDFLAGS += -lobjc

#============================================================================
# APPLICATION BUNDLE
#============================================================================

BUNDLE_NAME := ProjT Launcher.app
BUNDLE_DIR := $(BUILDDIR)/$(BUNDLE_NAME)
BUNDLE_CONTENTS := $(BUNDLE_DIR)/Contents
BUNDLE_MACOS := $(BUNDLE_CONTENTS)/MacOS
BUNDLE_RESOURCES := $(BUNDLE_CONTENTS)/Resources
BUNDLE_FRAMEWORKS := $(BUNDLE_CONTENTS)/Frameworks

#============================================================================
# BUNDLE CREATION
#============================================================================

.PHONY: bundle dmg

bundle: launcher
	@echo "[BUNDLE] Creating app bundle..."
	$(Q)mkdir -p $(BUNDLE_MACOS)
	$(Q)mkdir -p $(BUNDLE_RESOURCES)
	$(Q)mkdir -p $(BUNDLE_FRAMEWORKS)
	
	# Copy binary
	$(Q)cp $(LAUNCHER_BIN) $(BUNDLE_MACOS)/$(LAUNCHER_APP_BINARY_NAME)
	
	# Copy icon
	$(Q)cp $(SRCDIR)/program_info/projtlauncher.icns $(BUNDLE_RESOURCES)/$(LAUNCHER_NAME).icns
	
	# Create Info.plist
	$(Q)cat > $(BUNDLE_CONTENTS)/Info.plist << 'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>$(LAUNCHER_APP_BINARY_NAME)</string>
    <key>CFBundleIconFile</key>
    <string>$(LAUNCHER_NAME).icns</string>
    <key>CFBundleIdentifier</key>
    <string>$(LAUNCHER_APPID)</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>$(LAUNCHER_DISPLAYNAME)</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>$(VERSION_NAME)</string>
    <key>CFBundleVersion</key>
    <string>$(VERSION_NAME)</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>$(LAUNCHER_COPYRIGHT)</string>
</dict>
</plist>
PLIST
	
	@echo "[BUNDLE] App bundle created at $(BUNDLE_DIR)"

#============================================================================
# DMG CREATION
#============================================================================

DMG_NAME := $(LAUNCHER_NAME)-$(VERSION_NAME).dmg
DMG_FILE := $(BUILDDIR)/$(DMG_NAME)

dmg: bundle
	@echo "[DMG] Creating disk image..."
	$(Q)hdiutil create -volname "$(LAUNCHER_DISPLAYNAME)" \
	    -srcfolder $(BUNDLE_DIR) \
	    -ov -format UDZO \
	    $(DMG_FILE)
	@echo "[DMG] Disk image created at $(DMG_FILE)"

#============================================================================
# CODE SIGNING (optional)
#============================================================================

.PHONY: sign notarize

# Sign the app bundle (requires valid Developer ID)
ifdef APPLE_SIGNING_IDENTITY
sign: bundle
	@echo "[SIGN] Signing app bundle..."
	$(Q)codesign --force --deep --sign "$(APPLE_SIGNING_IDENTITY)" \
	    --options runtime \
	    $(BUNDLE_DIR)
	@echo "[SIGN] App bundle signed"
else
sign:
	@echo "[SIGN] APPLE_SIGNING_IDENTITY not set, skipping code signing"
endif

# Notarize the app (requires Apple Developer account)
ifdef APPLE_ID
notarize: sign dmg
	@echo "[NOTARIZE] Submitting for notarization..."
	$(Q)xcrun notarytool submit $(DMG_FILE) \
	    --apple-id "$(APPLE_ID)" \
	    --team-id "$(APPLE_TEAM_ID)" \
	    --password "$(APPLE_PASSWORD)" \
	    --wait
	@echo "[NOTARIZE] Stapling notarization ticket..."
	$(Q)xcrun stapler staple $(DMG_FILE)
	@echo "[NOTARIZE] Done"
else
notarize:
	@echo "[NOTARIZE] APPLE_ID not set, skipping notarization"
endif

#============================================================================
# MACOS INSTALLATION
#============================================================================

install-macos: bundle
	@echo "[INSTALL] Installing to /Applications..."
	$(Q)cp -R $(BUNDLE_DIR) /Applications/
	@echo "[INSTALL] Done"
