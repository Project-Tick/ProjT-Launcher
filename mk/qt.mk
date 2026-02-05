# ============================================================================
# mk/qt.mk - Qt Framework build integration
# ============================================================================
#
# This file handles Qt from the bundled qt/ subtree.
# Subtrees: qt/qtbase, qt/qtnetworkauth, qt/qtimageformats,
#           qt/qtpositioning, qt/qtserialport, qt/qtwebchannel,
#           qt/qtwebengine
#
# IMPORTANT: Do not modify subtree sources - build out of tree.
#
# ============================================================================

ifndef _MK_QT_INCLUDED
_MK_QT_INCLUDED := 1

include $(srctree)/mk/config.mk

# ============================================================================
# Qt Directories
# ============================================================================

QT_SRCDIR := $(srctree)/qt
QT_BUILDDIR := $(OBJDIR)/qt
QT_PREFIX := $(QT_BUILDDIR)/install
QT_HOSTDIR := $(QT_BUILDDIR)/host

# Qt module source directories (subtrees)
QTBASE_SRC := $(QT_SRCDIR)/qtbase
QTSHADERTOOLS_SRC := $(QT_SRCDIR)/qtshadertools
QTDECLARATIVE_SRC := $(QT_SRCDIR)/qtdeclarative
QTNETWORKAUTH_SRC := $(QT_SRCDIR)/qtnetworkauth
QTIMAGEFORMATS_SRC := $(QT_SRCDIR)/qtimageformats
QTPOSITIONING_SRC := $(QT_SRCDIR)/qtpositioning
QTSERIALPORT_SRC := $(QT_SRCDIR)/qtserialport
QTWEBCHANNEL_SRC := $(QT_SRCDIR)/qtwebchannel
QTWEBENGINE_SRC := $(QT_SRCDIR)/qtwebengine

# Qt tools (moc, uic, rcc, etc.)
QT_MOC := $(QT_PREFIX)/libexec/moc
QT_UIC := $(QT_PREFIX)/libexec/uic
QT_RCC := $(QT_PREFIX)/libexec/rcc
QT_LRELEASE := $(QT_PREFIX)/libexec/lrelease
QT_LUPDATE := $(QT_PREFIX)/libexec/lupdate

# Qt pkg-config
QT_PKG_CONFIG_PATH := $(QT_PREFIX)/lib/pkgconfig

# ============================================================================
# Qt Version (auto-detected from subtree)
# ============================================================================

# Read version from qtbase/.cmake.conf or CMakeLists.txt
QT_VERSION := $(shell grep -oP 'QT_REPO_MODULE_VERSION\s*=\s*"\K[^"]+' $(QTBASE_SRC)/.cmake.conf 2>/dev/null || echo "6.8.0")
QT_VERSION_MAJOR := $(word 1,$(subst ., ,$(QT_VERSION)))
QT_VERSION_MINOR := $(word 2,$(subst ., ,$(QT_VERSION)))
QT_VERSION_PATCH := $(word 3,$(subst ., ,$(QT_VERSION)))

# ============================================================================
# Qt Configure Options
# ============================================================================

QT_CONFIGURE_BASE := \
	-prefix $(QT_PREFIX) \
	-opensource \
	-confirm-license \
	-release \
	-nomake examples \
	-nomake tests

# Static vs Shared
ifeq ($(CONFIG_QT_STATIC),y)
QT_CONFIGURE_BASE += -static
else
QT_CONFIGURE_BASE += -shared
endif

# Minimal build
ifeq ($(CONFIG_QT_MINIMAL),y)
QT_CONFIGURE_BASE += \
	-no-feature-sql \
	-no-feature-testlib \
	-no-feature-printsupport
endif

# Platform-specific options
ifeq ($(CONFIG_TARGET_LINUX),y)
QT_CONFIGURE_PLATFORM := -platform linux-g++

# XCB (X11) support from Kconfig
ifeq ($(CONFIG_QT_PLATFORM_XCB),y)
QT_CONFIGURE_PLATFORM += -xcb -xcb-xlib -bundled-xcb-xinput
else
QT_CONFIGURE_PLATFORM += -no-xcb
endif

# Wayland support from Kconfig
ifeq ($(CONFIG_QT_PLATFORM_WAYLAND),y)
QT_CONFIGURE_PLATFORM += -feature-wayland-client
endif

# EGLFS support from Kconfig
ifeq ($(CONFIG_QT_PLATFORM_EGLFS),y)
QT_CONFIGURE_PLATFORM += -eglfs
else
QT_CONFIGURE_PLATFORM += -no-eglfs
endif

# Linux Framebuffer from Kconfig
ifeq ($(CONFIG_QT_PLATFORM_LINUXFB),y)
QT_CONFIGURE_PLATFORM += -linuxfb
else
QT_CONFIGURE_PLATFORM += -no-linuxfb
endif

# VNC support from Kconfig
ifeq ($(CONFIG_QT_PLATFORM_VNC),y)
QT_CONFIGURE_PLATFORM += -vnc
endif

# DBus
ifeq ($(CONFIG_QT_FEATURE_DBUS),y)
QT_CONFIGURE_PLATFORM += -dbus-linked
else
QT_CONFIGURE_PLATFORM += -no-dbus
endif
endif

ifeq ($(CONFIG_TARGET_WINDOWS),y)
QT_CONFIGURE_PLATFORM := -platform win32-g++
endif

ifeq ($(CONFIG_TARGET_MACOS),y)
QT_CONFIGURE_PLATFORM := \
	-platform macx-clang \
	-sdk macosx
ifeq ($(CONFIG_MACOS_UNIVERSAL),y)
QT_CONFIGURE_PLATFORM += \
	-- -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"
endif
endif

# Feature flags
ifeq ($(CONFIG_QT_FEATURE_OPENGL),y)
QT_CONFIGURE_FEATURES += -opengl desktop
else
QT_CONFIGURE_FEATURES += -no-opengl
endif

ifeq ($(CONFIG_QT_FEATURE_OPENSSL),y)
QT_CONFIGURE_FEATURES += -openssl-linked
else
QT_CONFIGURE_FEATURES += -no-openssl
endif

ifeq ($(CONFIG_QT_FEATURE_ICU),y)
QT_CONFIGURE_FEATURES += -icu
else
QT_CONFIGURE_FEATURES += -no-icu
endif

ifeq ($(CONFIG_QT_FEATURE_ZSTD),y)
QT_CONFIGURE_FEATURES += -zstd
else
QT_CONFIGURE_FEATURES += -no-zstd
endif

# Use bundled zlib
ifeq ($(CONFIG_LIB_ZLIB),y)
QT_CONFIGURE_FEATURES += \
	-system-zlib \
	-- -DZLIB_ROOT=$(LIBDIR)
endif

# Use bundled libpng
ifeq ($(CONFIG_LIB_LIBPNG),y)
QT_CONFIGURE_FEATURES += \
	-system-libpng \
	-- -DPNG_ROOT=$(LIBDIR)
endif

QT_CONFIGURE_FLAGS := \
	$(QT_CONFIGURE_BASE) \
	$(QT_CONFIGURE_PLATFORM) \
	$(QT_CONFIGURE_FEATURES)

# ============================================================================
# Qt Build Rules
# ============================================================================

# QtBase build directory
QTBASE_BUILDDIR := $(QT_BUILDDIR)/qtbase

$(QTBASE_BUILDDIR)/.configured: $(QTBASE_SRC)/configure
	@mkdir -p $(QTBASE_BUILDDIR)
	@echo "  CONFIGURE qtbase"
	$(Q)cd $(QTBASE_BUILDDIR) && \
		$(QTBASE_SRC)/configure $(QT_CONFIGURE_FLAGS)
	$(Q)touch $@

$(QTBASE_BUILDDIR)/.built: $(QTBASE_BUILDDIR)/.configured
	@echo "  BUILD   qtbase"
	$(Q)cmake --build $(QTBASE_BUILDDIR) --parallel $(PARALLEL_JOBS)
	$(Q)touch $@

$(QT_PREFIX)/.qtbase-installed: $(QTBASE_BUILDDIR)/.built
	@echo "  INSTALL qtbase"
	$(Q)cmake --install $(QTBASE_BUILDDIR)
	$(Q)touch $@

.PHONY: qt-base
qt-base: $(QT_PREFIX)/.qtbase-installed

# ============================================================================
# Qt Module Build Template
# ============================================================================

# Template for building Qt modules (after qtbase)
# $(1) = module name (e.g., qtnetworkauth)
# $(2) = source directory
define QT_MODULE_template

$(QT_BUILDDIR)/$(1)/.configured: $(QT_PREFIX)/.qtbase-installed $(2)/CMakeLists.txt
	@mkdir -p $(QT_BUILDDIR)/$(1)
	@echo "  CONFIGURE $(1)"
	$(Q)cd $(QT_BUILDDIR)/$(1) && \
		$(QT_PREFIX)/bin/qt-configure-module $(2)
	$(Q)touch $$@

$(QT_BUILDDIR)/$(1)/.built: $(QT_BUILDDIR)/$(1)/.configured
	@echo "  BUILD   $(1)"
	$(Q)cmake --build $(QT_BUILDDIR)/$(1) --parallel $(PARALLEL_JOBS)
	$(Q)touch $$@

$(QT_PREFIX)/.$(1)-installed: $(QT_BUILDDIR)/$(1)/.built
	@echo "  INSTALL $(1)"
	$(Q)cmake --install $(QT_BUILDDIR)/$(1)
	$(Q)touch $$@

.PHONY: qt-$(1)
qt-$(1): $(QT_PREFIX)/.$(1)-installed

endef

# Generate rules for each Qt module
ifeq ($(CONFIG_QT_MODULE_QTNETWORKAUTH),y)
$(eval $(call QT_MODULE_template,qtnetworkauth,$(QTNETWORKAUTH_SRC)))
QT_MODULES += qt-qtnetworkauth
endif

ifeq ($(CONFIG_QT_MODULE_QTIMAGEFORMATS),y)
$(eval $(call QT_MODULE_template,qtimageformats,$(QTIMAGEFORMATS_SRC)))
QT_MODULES += qt-qtimageformats
endif

ifeq ($(CONFIG_QT_MODULE_QTPOSITIONING),y)
$(eval $(call QT_MODULE_template,qtpositioning,$(QTPOSITIONING_SRC)))
QT_MODULES += qt-qtpositioning
endif

ifeq ($(CONFIG_QT_MODULE_QTSERIALPORT),y)
$(eval $(call QT_MODULE_template,qtserialport,$(QTSERIALPORT_SRC)))
QT_MODULES += qt-qtserialport
endif

ifeq ($(CONFIG_QT_MODULE_QTWEBCHANNEL),y)
$(eval $(call QT_MODULE_template,qtwebchannel,$(QTWEBCHANNEL_SRC)))
QT_MODULES += qt-qtwebchannel
endif

# QtShaderTools - required by Qt Quick (provides qsb tool)
ifeq ($(CONFIG_QT_MODULE_QTWEBENGINE),y)
$(eval $(call QT_MODULE_template,qtshadertools,$(QTSHADERTOOLS_SRC)))
QT_MODULES += qt-qtshadertools
endif

# QtDeclarative (Qt Quick/QML) - required by QtWebEngine, depends on qtshadertools
ifeq ($(CONFIG_QT_MODULE_QTWEBENGINE),y)

# Override qtdeclarative to depend on qtshadertools
$(QT_BUILDDIR)/qtdeclarative/.configured: $(QT_PREFIX)/.qtbase-installed $(QT_PREFIX)/.qtshadertools-installed $(QTDECLARATIVE_SRC)/CMakeLists.txt
	@mkdir -p $(QT_BUILDDIR)/qtdeclarative
	@echo "  CONFIGURE qtdeclarative"
	$(Q)cd $(QT_BUILDDIR)/qtdeclarative && \
		$(QT_PREFIX)/bin/qt-configure-module $(QTDECLARATIVE_SRC)
	$(Q)touch $@

$(QT_BUILDDIR)/qtdeclarative/.built: $(QT_BUILDDIR)/qtdeclarative/.configured
	@echo "  BUILD   qtdeclarative"
	$(Q)cmake --build $(QT_BUILDDIR)/qtdeclarative --parallel $(PARALLEL_JOBS)
	$(Q)touch $@

$(QT_PREFIX)/.qtdeclarative-installed: $(QT_BUILDDIR)/qtdeclarative/.built
	@echo "  INSTALL qtdeclarative"
	$(Q)cmake --install $(QT_BUILDDIR)/qtdeclarative
	$(Q)touch $@

.PHONY: qt-qtdeclarative
qt-qtdeclarative: $(QT_PREFIX)/.qtdeclarative-installed

QT_MODULES += qt-qtdeclarative
endif

# QtWebEngine is special - needs WebChannel and Declarative first
ifeq ($(CONFIG_QT_MODULE_QTWEBENGINE),y)

$(QT_BUILDDIR)/qtwebengine/.configured: $(QT_PREFIX)/.qtbase-installed $(QT_PREFIX)/.qtwebchannel-installed $(QT_PREFIX)/.qtdeclarative-installed $(QTWEBENGINE_SRC)/CMakeLists.txt
	@mkdir -p $(QT_BUILDDIR)/qtwebengine
	@echo "  CONFIGURE qtwebengine"
	$(Q)cd $(QT_BUILDDIR)/qtwebengine && \
		$(QT_PREFIX)/bin/qt-configure-module $(QTWEBENGINE_SRC) \
		$(if $(CONFIG_QT_WEBENGINE_PROPRIETARY_CODECS),-webengine-proprietary-codecs,)
	$(Q)touch $@

$(QT_BUILDDIR)/qtwebengine/.built: $(QT_BUILDDIR)/qtwebengine/.configured
	@echo "  BUILD   qtwebengine (this will take a long time)"
	$(Q)cmake --build $(QT_BUILDDIR)/qtwebengine --parallel $(PARALLEL_JOBS)
	$(Q)touch $@

$(QT_PREFIX)/.qtwebengine-installed: $(QT_BUILDDIR)/qtwebengine/.built
	@echo "  INSTALL qtwebengine"
	$(Q)cmake --install $(QT_BUILDDIR)/qtwebengine
	$(Q)touch $@

.PHONY: qt-qtwebengine
qt-qtwebengine: $(QT_PREFIX)/.qtwebengine-installed

QT_MODULES += qt-qtwebengine
endif

# ============================================================================
# Main Qt Targets
# ============================================================================

.PHONY: qt
qt: qt-base $(QT_MODULES)
	@echo "Qt built and installed to $(QT_PREFIX)"

.PHONY: qt-clean
qt-clean:
	$(Q)rm -rf $(QT_BUILDDIR)
	@echo "Qt build directory cleaned."

.PHONY: qt-tools
qt-tools: qt-base
	@echo "Qt tools available at $(QT_PREFIX)/libexec/"

# ============================================================================
# Qt Integration Variables
# ============================================================================

# Export for other makefiles
export QT_PREFIX
export QT_MOC
export QT_UIC
export QT_RCC

# Qt include/lib flags for dependent modules
QT_CFLAGS := $(shell PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --cflags Qt6Core Qt6Gui Qt6Widgets Qt6Network 2>/dev/null || echo "-I$(QT_PREFIX)/include -I$(QT_PREFIX)/include/QtCore -I$(QT_PREFIX)/include/QtGui -I$(QT_PREFIX)/include/QtWidgets -I$(QT_PREFIX)/include/QtNetwork")
QT_LIBS := $(shell PKG_CONFIG_PATH=$(QT_PKG_CONFIG_PATH) pkg-config --libs Qt6Core Qt6Gui Qt6Widgets Qt6Network 2>/dev/null || echo "-L$(QT_PREFIX)/lib -lQt6Core -lQt6Gui -lQt6Widgets -lQt6Network")

# If system Qt is selected
ifeq ($(CONFIG_QT_SYSTEM),y)
ifneq ($(CONFIG_QT_PREFIX),)
QT_PREFIX := $(CONFIG_QT_PREFIX)
endif
QT_CFLAGS := $(shell pkg-config --cflags Qt6Core Qt6Gui Qt6Widgets Qt6Network 2>/dev/null)
QT_LIBS := $(shell pkg-config --libs Qt6Core Qt6Gui Qt6Widgets Qt6Network 2>/dev/null)
QT_MOC := $(shell pkg-config --variable=libexecdir Qt6Core)/moc
QT_UIC := $(shell pkg-config --variable=libexecdir Qt6Core)/uic
QT_RCC := $(shell pkg-config --variable=libexecdir Qt6Core)/rcc
endif

export QT_CFLAGS
export QT_LIBS

# ============================================================================
# Qt Meta-Object Compiler (moc) Rules
# ============================================================================

# Rule for generating moc files
# Usage: $(call moc_rule,source.h,moc_source.cpp)
define moc_rule
$(2): $(1) $(QT_MOC)
	@mkdir -p $$(dir $$@)
	@echo "  MOC     $$<"
	$(Q)$(QT_MOC) $$< -o $$@
endef

# Rule for generating rcc files
# Usage: $(call rcc_rule,resources.qrc,qrc_resources.cpp)
define rcc_rule
$(2): $(1) $(QT_RCC)
	@mkdir -p $$(dir $$@)
	@echo "  RCC     $$<"
	$(Q)$(QT_RCC) $$< -o $$@
endef

# Rule for generating ui files
# Usage: $(call uic_rule,widget.ui,ui_widget.h)
define uic_rule
$(2): $(1) $(QT_UIC)
	@mkdir -p $$(dir $$@)
	@echo "  UIC     $$<"
	$(Q)$(QT_UIC) $$< -o $$@
endef

# ============================================================================
# Qt Dependencies Check
# ============================================================================

.PHONY: qt-check
qt-check:
ifeq ($(CONFIG_QT_SYSTEM),y)
	@echo "Checking for system Qt..."
	@pkg-config --exists Qt6Core || (echo "ERROR: Qt6 not found. Install Qt6 or use bundled Qt." && exit 1)
	@echo "System Qt found: $$(pkg-config --modversion Qt6Core)"
else ifeq ($(CONFIG_QT_BUNDLED),y)
	@echo "Using bundled Qt from qt/ subtree"
	@test -d $(QTBASE_SRC) || (echo "ERROR: Qt subtree not found at $(QTBASE_SRC)" && exit 1)
	@echo "Qt source found at $(QTBASE_SRC)"
endif

# ============================================================================
# Help
# ============================================================================

.PHONY: qt-help
qt-help:
	@echo "Qt Build Targets:"
	@echo "  qt           - Build all Qt modules"
	@echo "  qt-base      - Build QtBase only"
	@echo "  qt-qtnetworkauth  - Build QtNetworkAuth"
	@echo "  qt-qtimageformats - Build QtImageFormats"
	@echo "  qt-qtwebchannel   - Build QtWebChannel"
	@echo "  qt-qtwebengine    - Build QtWebEngine (slow!)"
	@echo "  qt-clean     - Clean Qt build directory"
	@echo "  qt-check     - Verify Qt availability"
	@echo ""
	@echo "Qt is configured via menuconfig under 'Qt Framework'"
	@echo "Current mode: $(if $(CONFIG_QT_SYSTEM),System Qt,$(if $(CONFIG_QT_BUNDLED),Bundled Qt,Not configured))"

endif # _MK_QT_INCLUDED
