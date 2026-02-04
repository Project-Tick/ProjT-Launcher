# SPDX-License-Identifier: GPL-3.0
# launcher/java - Java Detection Module Makefile

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/java

# Core sources
JAVA_CORE_SRCS := \
	core/JavaProbeTask.cpp \
	core/JavaVersionFormatter.cpp \
	core/JavaVersionParser.cpp \
	core/JavaVersionPath.cpp \
	core/JavaVersionResult.cpp

# Download sources
JAVA_DOWNLOAD_SRCS := \
	download/ArchiveDownloadTask.cpp \
	download/ManifestDownloadTask.cpp

# Services sources
JAVA_SERVICES_SRCS := \
	services/AdoptiumJavaDownloader.cpp \
	services/AzulJavaDownloader.cpp \
	services/MojangJavaDownloader.cpp

JAVA_SRCS := $(JAVA_CORE_SRCS) $(JAVA_DOWNLOAD_SRCS) $(JAVA_SERVICES_SRCS)
JAVA_OBJS := $(addprefix $(obj)/, $(JAVA_SRCS:.cpp=.o))
JAVA_LIB := $(obj)/libjava.a

# Find MOC headers
JAVA_MOC_HEADERS := $(shell find $(srctree)/launcher/java -name '*.h' -exec grep -l Q_OBJECT {} \; 2>/dev/null)
JAVA_MOC_SRCS := $(patsubst $(srctree)/launcher/java/%.h,$(obj)/moc_%.cpp,$(JAVA_MOC_HEADERS))
JAVA_MOC_OBJS := $(JAVA_MOC_SRCS:.cpp=.o)

JAVA_CXXFLAGS := \
	-I$(srctree)/launcher/java \
	-I$(srctree)/launcher \
	-I$(obj) \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: java java-clean

java: $(JAVA_LIB)

$(JAVA_LIB): $(JAVA_OBJS) $(JAVA_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

$(obj)/%.o: $(srctree)/launcher/java/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

$(obj)/moc_%.cpp: $(srctree)/launcher/java/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

java-clean:
	$(Q)rm -rf $(obj)

-include $(JAVA_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
