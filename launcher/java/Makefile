# SPDX-License-Identifier: GPL-3.0
# launcher/java - Java Detection Module Makefile

# Get absolute path before any includes modify MAKEFILE_LIST
THIS_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_DIR := $(dir $(THIS_MAKEFILE))

# Default paths if not provided by parent make
srctree ?= $(abspath $(THIS_DIR)/../..)
KBUILD_OUTPUT ?= $(srctree)/build

-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

obj := $(KBUILD_OUTPUT)/java

# Core sources
JAVA_CORE_SRCS := \
	core/RuntimeInstall.cpp \
	core/RuntimePackage.cpp \
	core/RuntimeVersion.cpp

# Download sources
JAVA_DOWNLOAD_SRCS := \
	download/RuntimeArchiveTask.cpp \
	download/RuntimeLinkTask.cpp \
	download/RuntimeManifestTask.cpp

# Services sources
JAVA_SERVICES_SRCS := \
	services/RuntimeCatalog.cpp \
	services/RuntimeEnvironment.cpp \
	services/RuntimeProbeTask.cpp \
	services/RuntimeScanner.cpp

JAVA_SRCS := $(JAVA_CORE_SRCS) $(JAVA_DOWNLOAD_SRCS) $(JAVA_SERVICES_SRCS)
JAVA_OBJS := $(addprefix $(obj)/, $(JAVA_SRCS:.cpp=.o))
JAVA_LIB := $(obj)/libjava.a

# MOC headers (Qt Q_OBJECT classes)
JAVA_MOC_HEADERS := \
	services/RuntimeCatalog.hpp \
	services/RuntimeProbeTask.hpp \
	download/RuntimeArchiveTask.hpp \
	download/RuntimeLinkTask.hpp \
	download/RuntimeManifestTask.hpp

JAVA_MOC_SRCS := $(patsubst %.hpp,$(obj)/moc_%.cpp,$(notdir $(JAVA_MOC_HEADERS)))
JAVA_MOC_OBJS := $(JAVA_MOC_SRCS:.cpp=.o)

JAVA_CXXFLAGS := \
	-DLAUNCHER_APPLICATION \
	-I$(srctree)/launcher/java \
	-I$(srctree)/launcher \
	-I$(srctree)/quazip \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/include \
	-I$(obj) \
	-I$(QT_PREFIX)/include/QtXml \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

.PHONY: all java java-clean

all: java

java: $(JAVA_LIB)

$(JAVA_LIB): $(JAVA_OBJS) $(JAVA_MOC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Pattern rule for subdirectories (core/, download/, services/)
$(obj)/core/%.o: $(srctree)/launcher/java/core/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

$(obj)/download/%.o: $(srctree)/launcher/java/download/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

$(obj)/services/%.o: $(srctree)/launcher/java/services/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

$(obj)/%.o: $(srctree)/launcher/java/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

# MOC rules for services/*.hpp
$(obj)/moc_RuntimeCatalog.cpp: $(srctree)/launcher/java/services/RuntimeCatalog.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/launcher -o $@ $<

$(obj)/moc_RuntimeProbeTask.cpp: $(srctree)/launcher/java/services/RuntimeProbeTask.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/launcher -o $@ $<

# MOC rules for download/*.hpp
$(obj)/moc_RuntimeArchiveTask.cpp: $(srctree)/launcher/java/download/RuntimeArchiveTask.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/launcher -o $@ $<

$(obj)/moc_RuntimeLinkTask.cpp: $(srctree)/launcher/java/download/RuntimeLinkTask.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/launcher -o $@ $<

$(obj)/moc_RuntimeManifestTask.cpp: $(srctree)/launcher/java/download/RuntimeManifestTask.hpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_CFLAGS) -I$(srctree)/launcher -o $@ $<

# Compile MOC outputs
$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@echo "  CXX     $<"
	$(Q)$(CXX) $(JAVA_CXXFLAGS) -c -o $@ $<

java-clean:
	$(Q)rm -rf $(obj)

-include $(JAVA_OBJS:.o=.d)

# Note: This module uses custom rules above instead of module.mk
# because it has complex subdirectory structure (core/, download/, services/)
