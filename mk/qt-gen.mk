# SPDX-License-Identifier: GPL-2.0

include mk/qt-tools.mk

MOC_DIR := $(KBUILD_OUTPUT)/gen/moc
UIC_DIR := $(KBUILD_OUTPUT)/gen/uic
RCC_DIR := $(KBUILD_OUTPUT)/gen/rcc

CPPFLAGS += -I$(UIC_DIR)

src-rel-qt = $(patsubst $(srctree)/%,%,$(1))
qt-moc-out = $(MOC_DIR)/$(call src-rel-qt,$(1)).moc.cpp
qt-uic-out = $(UIC_DIR)/ui_$(notdir $(basename $(1))).h
qt-rcc-out = $(RCC_DIR)/qrc_$(notdir $(basename $(1))).cpp

# Generate moc source

define qt-moc-rule
$(call qt-moc-out,$(1)): $(1)
	@mkdir -p $(dir $$@)
	$(QT_MOC) $< -o $$@
endef

# Generate uic header

define qt-uic-rule
$(call qt-uic-out,$(1)): $(1)
	@mkdir -p $(UIC_DIR)
	$(QT_UIC) $< -o $$@
endef

# Generate rcc source

define qt-rcc-rule
$(call qt-rcc-out,$(1)): $(1)
	@mkdir -p $(RCC_DIR)
	$(QT_RCC) $< -o $$@
endef
