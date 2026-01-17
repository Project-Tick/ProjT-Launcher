# ProjT Launcher - Qt Integration
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick

#============================================================================
# QT MOC (Meta Object Compiler)
#============================================================================

# MOC flags
QT_MOC_FLAGS := $(QT_CFLAGS) -DQT_NO_DEBUG

# MOC rule: Generate moc_*.cpp from *.h files containing Q_OBJECT
$(BUILDDIR)/moc/moc_%.cpp: $(SRCDIR)/launcher/%.h | $(BUILDDIR)/moc
	@echo "[MOC] $(notdir $<)"
	$(Q)$(QT_MOC) $(QT_MOC_FLAGS) -o $@ $<

# MOC rule for subdirectories
$(BUILDDIR)/moc/moc_%.cpp: $(SRCDIR)/launcher/*/%.h | $(BUILDDIR)/moc
	@echo "[MOC] $(notdir $<)"
	$(Q)$(QT_MOC) $(QT_MOC_FLAGS) -o $@ $<

$(BUILDDIR)/moc/moc_%.cpp: $(SRCDIR)/launcher/*/*/%.h | $(BUILDDIR)/moc
	@echo "[MOC] $(notdir $<)"
	$(Q)$(QT_MOC) $(QT_MOC_FLAGS) -o $@ $<

$(BUILDDIR)/moc/moc_%.cpp: $(SRCDIR)/launcher/*/*/*/%.h | $(BUILDDIR)/moc
	@echo "[MOC] $(notdir $<)"
	$(Q)$(QT_MOC) $(QT_MOC_FLAGS) -o $@ $<

#============================================================================
# QT RCC (Resource Compiler)
#============================================================================

# RCC rule: Generate qrc_*.cpp from *.qrc files
$(BUILDDIR)/rcc/qrc_%.cpp: $(SRCDIR)/launcher/resources/%.qrc | $(BUILDDIR)/rcc
	@echo "[RCC] $(notdir $<)"
	$(Q)$(QT_RCC) -o $@ $<

$(BUILDDIR)/rcc/qrc_%.cpp: $(BUILDDIR)/gen/%.qrc | $(BUILDDIR)/rcc
	@echo "[RCC] $(notdir $<)"
	$(Q)$(QT_RCC) -o $@ $<

#============================================================================
# QT UIC (User Interface Compiler)
#============================================================================

# UIC rule: Generate ui_*.h from *.ui files
$(BUILDDIR)/ui/ui_%.h: $(SRCDIR)/launcher/ui/%.ui | $(BUILDDIR)/ui
	@echo "[UIC] $(notdir $<)"
	$(Q)$(QT_UIC) -o $@ $<

# UIC rule for subdirectories
$(BUILDDIR)/ui/ui_%.h: $(SRCDIR)/launcher/ui/*/%.ui | $(BUILDDIR)/ui
	@echo "[UIC] $(notdir $<)"
	$(Q)$(QT_UIC) -o $@ $<

$(BUILDDIR)/ui/ui_%.h: $(SRCDIR)/launcher/ui/*/*/%.ui | $(BUILDDIR)/ui
	@echo "[UIC] $(notdir $<)"
	$(Q)$(QT_UIC) -o $@ $<

#============================================================================
# QT HEADER DISCOVERY
#============================================================================

# Find all headers with Q_OBJECT/Q_GADGET macro
# This is used to automatically determine which headers need MOC processing

define find_qt_headers
$(shell grep -l -r 'Q_OBJECT\|Q_GADGET' $(SRCDIR)/launcher --include='*.h' 2>/dev/null || true)
endef

# Find all .ui files
define find_ui_files
$(shell find $(SRCDIR)/launcher/ui -name '*.ui' 2>/dev/null || true)
endef

# Find all .qrc files
define find_qrc_files
$(shell find $(SRCDIR)/launcher/resources -name '*.qrc' 2>/dev/null || true)
endef

#============================================================================
# GENERATED SOURCE LISTS
#============================================================================

# Headers requiring MOC
QT_HEADERS := $(call find_qt_headers)

# Generate MOC source list
QT_MOC_SRCS := $(patsubst $(SRCDIR)/launcher/%.h,$(BUILDDIR)/moc/moc_%.cpp,$(QT_HEADERS))
QT_MOC_SRCS := $(subst /moc/moc_,/moc/moc_,$(subst /,_,$(QT_MOC_SRCS)))

# UI files
QT_UI_FILES := $(call find_ui_files)
QT_UI_HEADERS := $(patsubst $(SRCDIR)/launcher/ui/%.ui,$(BUILDDIR)/ui/ui_%.h,$(QT_UI_FILES))

# QRC files
QT_QRC_FILES := $(call find_qrc_files)
QT_RCC_SRCS := $(patsubst $(SRCDIR)/launcher/resources/%.qrc,$(BUILDDIR)/rcc/qrc_%.cpp,$(QT_QRC_FILES))

# Debug: Print discovered files
qt-debug:
	@echo "Qt Headers requiring MOC:"
	@for h in $(QT_HEADERS); do echo "  $$h"; done
	@echo ""
	@echo "UI files:"
	@for u in $(QT_UI_FILES); do echo "  $$u"; done
	@echo ""
	@echo "QRC files:"
	@for q in $(QT_QRC_FILES); do echo "  $$q"; done
