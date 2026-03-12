# SPDX-License-Identifier: GPL-3.0
# launcher/logs - Log Parsing Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/logs

LOGS_SRCS := \
	LogEventParser.cpp \
	LogRedactor.cpp

LOGS_OBJS := $(addprefix $(obj)/, $(LOGS_SRCS:.cpp=.o))
LOGS_LIB := $(obj)/liblogs.a

# MOC
LOGS_MOC_HEADERS := $(shell find $(srctree)/launcher/logs -name '*.hpp' -o -name '*.h' | xargs grep -l Q_OBJECT 2>/dev/null)
LOGS_MOC_SRCS := $(patsubst $(srctree)/launcher/logs/%.hpp,$(obj)/moc_%.cpp,$(filter %.hpp,$(LOGS_MOC_HEADERS)))
LOGS_MOC_OBJS := $(LOGS_MOC_SRCS:.cpp=.o)

LOGS_CXXFLAGS := \
	-I$(srctree)/launcher/logs \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all logs logs-clean

all: logs

logs: $(LOGS_LIB)

$(LOGS_LIB): $(LOGS_OBJS) $(LOGS_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/logs/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LOGS_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/logs/%.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LOGS_CXXFLAGS) -c -o $@ $<

logs-clean:
	$(Q)rm -rf $(obj)

-include $(LOGS_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
