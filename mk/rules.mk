# SPDX-License-Identifier: GPL-2.0

include mk/qt-tools.mk

OBJDIR := $(KBUILD_OUTPUT)/obj
LIBDIR := $(KBUILD_OUTPUT)/lib
BINDIR := $(KBUILD_OUTPUT)/bin
JARDIR := $(KBUILD_OUTPUT)/jars

CPPFLAGS += -I$(KBUILD_OUTPUT)/include -I$(KBUILD_OUTPUT)/include/generated

src-rel = $(patsubst $(srctree)/%,%,$(1))

# Source -> object mapping
src-to-obj = $(foreach s,$(1),\
	$(if $(filter $(KBUILD_OUTPUT)/%,$(s)),\
		$(OBJDIR)/$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.cxx,%.o,$(patsubst %.mm,%.o,$(patsubst $(KBUILD_OUTPUT)/%,%,$(s)))))))),\
		$(OBJDIR)/$(patsubst %.c,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cc,%.o,$(patsubst %.cxx,%.o,$(patsubst %.mm,%.o,$(call src-rel,$(s))))))))\
	))

# Pattern rules
$(OBJDIR)/%.o: $(srctree)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR)/%.o: $(srctree)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR)/%.o: $(srctree)/%.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR)/%.o: $(srctree)/%.cxx
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR)/%.o: $(srctree)/%.mm
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(OBJDIR)/%.o: $(KBUILD_OUTPUT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Library/executable helpers

define build-static-lib
$$(LIBDIR)/lib$(1).a: $$($(1)_OBJS)
	@mkdir -p $$(dir $$@)
	$$(AR) rcs $$@ $$^
endef

define build-exe
$$(BINDIR)/$(1): $$($(1)_OBJS) | $$($(1)_LIBS)
	@mkdir -p $$(dir $$@)
	$$(CXX) $$($(1)_OBJS) $$(LDFLAGS) $$($(1)_LDLIBS) -o $$@
endef

# Auto include deps
DEPS := $(shell find $(OBJDIR) -name '*.d' 2>/dev/null)
-include $(DEPS)
