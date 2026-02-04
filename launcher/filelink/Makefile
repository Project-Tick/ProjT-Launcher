# SPDX-License-Identifier: GPL-3.0
# launcher/filelink - FileLink Module Makefile
#
# FileLink is a helper tool for creating symbolic links on Windows.
# It's built as a separate executable that runs with elevated privileges.

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Only build on Windows
ifndef CONFIG_TARGET_WINDOWS
$(warning filelink is only built for Windows targets)
endif

# Object output directory
obj := $(KBUILD_OUTPUT)/filelink

# Sources
FILELINK_SRCS := \
	FileLink.cpp \
	filelink_main.cpp

# Objects
FILELINK_OBJS := $(addprefix $(obj)/, $(FILELINK_SRCS:.cpp=.o))

# Output binary
FILELINK_BIN := $(BINDIR)/filelink$(EXEEXT)

# Flags
FILELINK_CXXFLAGS := \
	-I$(srctree)/launcher/filelink \
	-I$(srctree)/launcher \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

FILELINK_LDFLAGS := \
	$(QT_LIBS) \
	$(LDFLAGS)

# Windows manifest for UAC elevation
ifeq ($(CONFIG_TARGET_WINDOWS),y)
FILELINK_LDFLAGS += -Wl,--manifest,$(srctree)/launcher/filelink/filelink.exe.manifest
endif

# Targets
.PHONY: filelink filelink-clean

filelink: $(FILELINK_BIN)

$(FILELINK_BIN): $(FILELINK_OBJS) | $(BINDIR)
	@echo "  LD      $@"
	$(Q)$(CXX) -o $@ $^ $(FILELINK_LDFLAGS)

$(BINDIR):
	@mkdir -p $@

$(obj)/%.o: $(srctree)/launcher/filelink/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(FILELINK_CXXFLAGS) -c -o $@ $<

filelink-clean:
	@echo "  CLEAN   filelink"
	$(Q)rm -rf $(obj) $(FILELINK_BIN)

-include $(FILELINK_OBJS:.o=.d)
