# SPDX-License-Identifier: GPL-3.0
# launcher/translations - Translations Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/translations

TRANS_SRCS := \
	POTranslator.cpp \
	TranslationsModel.cpp

TRANS_OBJS := $(addprefix $(obj)/, $(TRANS_SRCS:.cpp=.o))
TRANS_LIB := $(obj)/libtranslations.a

# MOC
TRANS_MOC_HEADERS := $(shell find $(srctree)/launcher/translations -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
TRANS_MOC_SRCS := $(patsubst $(srctree)/launcher/translations/%.h,$(obj)/moc_%.cpp,$(TRANS_MOC_HEADERS))
TRANS_MOC_OBJS := $(TRANS_MOC_SRCS:.cpp=.o)

TRANS_CXXFLAGS := \
	-I$(srctree)/launcher/translations \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: translations translations-clean

translations: $(TRANS_LIB)

$(TRANS_LIB): $(TRANS_OBJS) $(TRANS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/translations/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TRANS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/translations/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(TRANS_CXXFLAGS) -c -o $@ $<

translations-clean:
	$(Q)rm -rf $(obj)

-include $(TRANS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
