# SPDX-License-Identifier: Apache-2.0
# launcherjava - Makefile
#
# Build rules for Java launcher components.
# Builds two JAR files: ProjTLaunch.jar and ProjTLaunchLegacy.jar

# Output directories
JARDIR := $(KBUILD_OUTPUT)/jars

# Java compiler settings
JAVAC ?= javac
JAR ?= jar
JAVA_TARGET ?= 8
JAVA_SOURCE ?= 8
JAVAC_FLAGS := -source $(JAVA_SOURCE) -target $(JAVA_TARGET) -encoding UTF-8 -Xlint:-options

# Build directories
JAVA_BUILD := $(KBUILD_OUTPUT)/java-classes/launcherjava
JAVA_BUILD_LEGACY := $(KBUILD_OUTPUT)/java-classes/launcherjava-legacy

# Source directory
SRCDIR := $(srctree)/launcherjava

# ============================================================================
# ProjTLaunch.jar - Normal launcher
# ============================================================================

LAUNCH_JAR := $(JARDIR)/ProjTLaunch.jar

LAUNCH_SRCS := \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/EntryPoint.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/launcher/Launcher.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/launcher/impl/AbstractLauncher.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/launcher/impl/StandardLauncher.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/exception/ParameterNotFoundException.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/exception/ParseException.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/utils/Parameters.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/utils/ReflectionUtils.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/utils/logging/Level.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/utils/logging/Log.java \
	$(SRCDIR)/org/projecttick/projtlauncher/normal/legacy/LegacyProxy.java

$(LAUNCH_JAR): $(LAUNCH_SRCS) | $(JARDIR)
	@mkdir -p $(JAVA_BUILD)
	@echo "  JAVAC   ProjTLaunch"
	$(Q)$(JAVAC) $(JAVAC_FLAGS) -d $(JAVA_BUILD) $(LAUNCH_SRCS)
	@echo "  JAR     $@"
	$(Q)$(JAR) cf $@ -C $(JAVA_BUILD) .

# ============================================================================
# ProjTLaunchLegacy.jar - Legacy launcher (depends on ProjTLaunch.jar)
# ============================================================================

LEGACY_JAR := $(JARDIR)/ProjTLaunchLegacy.jar

LEGACY_SRCS := \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/LegacyFrame.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/LegacyLauncher.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/fix/online/Handler.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/fix/online/OnlineFixes.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/fix/online/OnlineModeFix.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/fix/online/SkinFix.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/Base64.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/api/MojangApi.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/api/Texture.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/json/JsonParseException.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/json/JsonParser.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/url/ByteArrayUrlConnection.java \
	$(SRCDIR)/org/projecttick/projtlauncher/legacy/utils/url/UrlUtils.java \
	$(SRCDIR)/net/minecraft/Launcher.java \
	$(SRCDIR)/legacyshim/LegacyProxy.java

$(LEGACY_JAR): $(LEGACY_SRCS) $(LAUNCH_JAR) | $(JARDIR)
	@mkdir -p $(JAVA_BUILD_LEGACY)
	@echo "  JAVAC   ProjTLaunchLegacy"
	$(Q)$(JAVAC) $(JAVAC_FLAGS) -cp $(LAUNCH_JAR) -d $(JAVA_BUILD_LEGACY) $(LEGACY_SRCS)
	@echo "  JAR     $@"
	$(Q)$(JAR) cf $@ -C $(JAVA_BUILD_LEGACY) .

# ============================================================================
# Targets
# ============================================================================

$(JARDIR):
	@mkdir -p $@

all: $(LAUNCH_JAR) $(LEGACY_JAR)
	@echo "    Built launcherjava"

launcherjava: all

launcherjava-clean:
	$(Q)rm -rf $(JAVA_BUILD) $(JAVA_BUILD_LEGACY) $(LAUNCH_JAR) $(LEGACY_JAR)

clean: launcherjava-clean

.PHONY: all launcherjava launcherjava-clean clean
