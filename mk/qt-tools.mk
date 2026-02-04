# SPDX-License-Identifier: GPL-2.0

include $(srctree)/mk/flags.mk

QT_PREFIX := $(call cfg-unquote,$(CONFIG_QT_PREFIX))
QT_HOST_BINS := $(call cfg-unquote,$(CONFIG_QT_HOST_BINS))

ifneq ($(call cfg-yes,$(CONFIG_USE_BUNDLED_QT)),)
ifeq ($(QT_PREFIX),)
QT_PREFIX := $(KBUILD_OUTPUT)/qt/install
endif
endif

ifeq ($(QT_HOST_BINS),)
ifneq ($(QT_PREFIX),)
QT_HOST_BINS := $(QT_PREFIX)/bin
endif
endif

QT_PKG_CONFIG ?= pkg-config

QT_COMPONENTS := Qt6Core Qt6Gui Qt6Widgets Qt6Network Qt6Concurrent Qt6Xml Qt6Test Qt6NetworkAuth Qt6OpenGL
ifneq ($(call cfg-yes,$(CONFIG_LAUNCHER_WITH_WEBENGINE)),)
QT_COMPONENTS += Qt6WebEngineWidgets Qt6WebChannel
endif

QT_HAVE_PKG := $(shell $(QT_PKG_CONFIG) --exists Qt6Core >/dev/null 2>&1 && echo yes)

ifeq ($(QT_HAVE_PKG),yes)
QT_CFLAGS := $(shell $(QT_PKG_CONFIG) --cflags $(QT_COMPONENTS))
QT_LIBS := $(shell $(QT_PKG_CONFIG) --libs $(QT_COMPONENTS))
else
ifneq ($(QT_PREFIX),)
QT_CFLAGS := -I$(QT_PREFIX)/include \
	-I$(QT_PREFIX)/include/QtCore \
	-I$(QT_PREFIX)/include/QtGui \
	-I$(QT_PREFIX)/include/QtWidgets \
	-I$(QT_PREFIX)/include/QtNetwork \
	-I$(QT_PREFIX)/include/QtConcurrent \
	-I$(QT_PREFIX)/include/QtXml \
	-I$(QT_PREFIX)/include/QtTest \
	-I$(QT_PREFIX)/include/QtNetworkAuth \
	-I$(QT_PREFIX)/include/QtOpenGL
ifneq ($(call cfg-yes,$(CONFIG_LAUNCHER_WITH_WEBENGINE)),)
QT_CFLAGS += -I$(QT_PREFIX)/include/QtWebEngineWidgets -I$(QT_PREFIX)/include/QtWebChannel
endif
QT_LIBS := -L$(QT_PREFIX)/lib -lQt6Core -lQt6Gui -lQt6Widgets -lQt6Network -lQt6Concurrent -lQt6Xml -lQt6Test -lQt6NetworkAuth -lQt6OpenGL
ifneq ($(call cfg-yes,$(CONFIG_LAUNCHER_WITH_WEBENGINE)),)
QT_LIBS += -lQt6WebEngineWidgets -lQt6WebChannel
endif
endif
endif

ifneq ($(QT_HOST_BINS),)
QT_MOC ?= $(QT_HOST_BINS)/moc
QT_UIC ?= $(QT_HOST_BINS)/uic
QT_RCC ?= $(QT_HOST_BINS)/rcc
QT_LRELEASE ?= $(QT_HOST_BINS)/lrelease
else
QT_MOC ?= moc
QT_UIC ?= uic
QT_RCC ?= rcc
QT_LRELEASE ?= lrelease
endif
