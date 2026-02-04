# SPDX-License-Identifier: GPL-3.0
# javacheck - Makefile
#
# Build rules for Java version checker.
# This builds a small JAR that checks Java compatibility.

# JAR name
JAR_NAME := JavaCheck.jar

# Find all Java sources
JAVA_SRCS := $(shell find $(srctree)/javacheck/src -name '*.java' 2>/dev/null)

# Output
JAR_OUTPUT := $(JARDIR)/$(JAR_NAME)

# Java compiler settings (needs to run on old Java)
JAVAC ?= javac
JAR ?= jar
JAVA_TARGET ?= 8
JAVA_SOURCE ?= 8
JAVAC_FLAGS := -source $(JAVA_SOURCE) -target $(JAVA_TARGET) -encoding UTF-8

# Build directory for class files
JAVA_BUILD := $(KBUILD_OUTPUT)/java-classes/javacheck

# Build JAR
$(JAR_OUTPUT): $(JAVA_SRCS) | $(JARDIR)
	@mkdir -p $(JAVA_BUILD)
	@echo "  JAVAC   javacheck"
	$(Q)$(JAVAC) $(JAVAC_FLAGS) -d $(JAVA_BUILD) $(JAVA_SRCS)
	@echo "  JAR     $@"
	$(Q)$(JAR) cf $@ -C $(JAVA_BUILD) .

$(JARDIR):
	@mkdir -p $@

javacheck: $(JAR_OUTPUT)

javacheck-clean:
	$(Q)rm -rf $(JAVA_BUILD) $(JAR_OUTPUT)

.PHONY: javacheck javacheck-clean
