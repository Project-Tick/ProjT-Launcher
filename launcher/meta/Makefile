# SPDX-License-Identifier: GPL-3.0
# launcher/meta - Meta Version Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/meta

META_SRCS := \
	BaseEntity.cpp \
	Index.cpp \
	JsonFormat.cpp \
	Version.cpp \
	VersionList.cpp

META_OBJS := $(addprefix $(obj)/, $(META_SRCS:.cpp=.o))
META_LIB := $(obj)/libmeta.a

# MOC
META_MOC_HEADERS := $(shell find $(srctree)/launcher/meta -name '*.hpp' | xargs grep -l Q_OBJECT 2>/dev/null)
META_MOC_SRCS := $(patsubst $(srctree)/launcher/meta/%.hpp,$(obj)/moc_%.cpp,$(META_MOC_HEADERS))
META_MOC_OBJS := $(META_MOC_SRCS:.cpp=.o)

META_CXXFLAGS := \
	-I$(srctree)/launcher/meta \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: meta meta-clean

meta: $(META_LIB)

$(META_LIB): $(META_OBJS) $(META_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/meta/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(META_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/meta/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(META_CXXFLAGS) -c -o $@ $<

meta-clean:
	$(Q)rm -rf $(obj)

-include $(META_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
