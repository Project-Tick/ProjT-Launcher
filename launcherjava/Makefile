# SPDX-License-Identifier: Apache-2.0
# launcherjava - Makefile
#
# Build rules for Java launcher components.
# This builds JAR files from Java sources.

# Java source directories
JAVA_SRC_DIRS := \
	net/minecraft \
	org/projt

# JAR name
JAR_NAME := NewLaunch.jar

# Find all Java sources
JAVA_SRCS := $(shell find $(srctree)/launcherjava -name '*.java' 2>/dev/null)

# Output
JAR_OUTPUT := $(JARDIR)/$(JAR_NAME)

# Java compiler settings
JAVAC ?= javac
JAR ?= jar
JAVA_TARGET ?= 8
JAVA_SOURCE ?= 8
JAVAC_FLAGS := -source $(JAVA_SOURCE) -target $(JAVA_TARGET) -encoding UTF-8

# Build directory for class files
JAVA_BUILD := $(KBUILD_OUTPUT)/java-classes/launcherjava

# Build JAR
$(JAR_OUTPUT): $(JAVA_SRCS) | $(JARDIR)
	@mkdir -p $(JAVA_BUILD)
	@echo "  JAVAC   launcherjava"
	$(Q)$(JAVAC) $(JAVAC_FLAGS) -d $(JAVA_BUILD) $(JAVA_SRCS)
	@echo "  JAR     $@"
	$(Q)$(JAR) cf $@ -C $(JAVA_BUILD) .

$(JARDIR):
	@mkdir -p $@

launcherjava: $(JAR_OUTPUT)

launcherjava-clean:
	$(Q)rm -rf $(JAVA_BUILD) $(JAR_OUTPUT)

.PHONY: launcherjava launcherjava-clean
