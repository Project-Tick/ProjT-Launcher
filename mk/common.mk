# ProjT Launcher - Common Make Rules
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick

#============================================================================
# DIRECTORY CREATION
#============================================================================

$(BUILDDIR)/obj:
	@mkdir -p $@

$(BUILDDIR)/obj/%:
	@mkdir -p $@

$(BUILDDIR)/moc:
	@mkdir -p $@

$(BUILDDIR)/ui:
	@mkdir -p $@

$(BUILDDIR)/rcc:
	@mkdir -p $@

$(BUILDDIR)/deps/lib:
	@mkdir -p $@

$(BUILDDIR)/deps/include:
	@mkdir -p $@

#============================================================================
# DEPENDENCY TRACKING
#============================================================================

DEPDIR := $(BUILDDIR)/deps.d
$(shell mkdir -p $(DEPDIR) > /dev/null 2>&1)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$(notdir $*).d

#============================================================================
# C++ COMPILATION RULES
#============================================================================

# All includes
ALL_INCLUDES := \
    -I$(SRCDIR) \
    -I$(SRCDIR)/launcher \
    -I$(SRCDIR)/buildconfig \
    -I$(BUILDDIR)/gen \
    -I$(BUILDDIR)/moc \
    -I$(BUILDDIR)/ui \
    -I$(BUILDDIR)/deps/include \
    $(QT_CFLAGS) \
    $(OPENSSL_CFLAGS)

# Pattern rule for launcher C++ files
$(BUILDDIR)/obj/%.o: $(SRCDIR)/launcher/%.cpp | $(BUILDDIR)/obj
	@mkdir -p $(dir $@)
	@echo "[CXX] $(notdir $<)"
	$(Q)$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(ALL_INCLUDES) -c -o $@ $<

# Pattern rule for buildconfig C++ files
$(BUILDDIR)/obj/buildconfig/%.o: $(BUILDDIR)/gen/%.cpp | $(BUILDDIR)/obj/buildconfig
	@mkdir -p $(dir $@)
	@echo "[CXX] $(notdir $<)"
	$(Q)$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(ALL_INCLUDES) -c -o $@ $<

# Pattern rule for MOC-generated C++ files
$(BUILDDIR)/obj/moc_%.o: $(BUILDDIR)/moc/moc_%.cpp | $(BUILDDIR)/obj
	@echo "[CXX] $(notdir $<)"
	$(Q)$(CXX) $(CXXFLAGS) $(ALL_INCLUDES) -c -o $@ $<

# Pattern rule for RCC-generated C++ files
$(BUILDDIR)/obj/qrc_%.o: $(BUILDDIR)/rcc/qrc_%.cpp | $(BUILDDIR)/obj
	@echo "[CXX] $(notdir $<)"
	$(Q)$(CXX) $(CXXFLAGS) $(ALL_INCLUDES) -c -o $@ $<

#============================================================================
# C COMPILATION RULES
#============================================================================

$(BUILDDIR)/obj/%.o: $(SRCDIR)/%.c | $(BUILDDIR)/obj
	@mkdir -p $(dir $@)
	@echo "[CC] $(notdir $<)"
	$(Q)$(CC) $(DEPFLAGS) $(CFLAGS) -I$(BUILDDIR)/deps/include -c -o $@ $<

#============================================================================
# STATIC LIBRARY RULES
#============================================================================

# Generic static library creation
define make_static_lib
	@echo "[AR] $@"
	$(Q)$(AR) rcs $@ $^
endef

#============================================================================
# INCLUDE DEPENDENCIES
#============================================================================

-include $(wildcard $(DEPDIR)/*.d)
