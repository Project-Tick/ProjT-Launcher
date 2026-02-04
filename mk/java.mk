# SPDX-License-Identifier: GPL-2.0

include mk/rules.mk

JAVA ?= java
JAVAC ?= javac
JAR ?= jar

JAVA_TARGET ?= 8

JAVA_OUT := $(KBUILD_OUTPUT)/java

# launcherjava
launcherjava_classes := $(JAVA_OUT)/launcherjava/classes
launcherjava_jar := $(JARDIR)/ProjTLaunch.jar
launcherjava_legacy_jar := $(JARDIR)/ProjTLaunchLegacy.jar
launcherjava_main := org.projecttick.projtlauncher.normal.EntryPoint

$(launcherjava_jar): $(launcherjava_SRC)
	@mkdir -p $(launcherjava_classes) $(JARDIR)
	@$(JAVAC) -source $(JAVA_TARGET) -target $(JAVA_TARGET) -d $(launcherjava_classes) $(launcherjava_SRC)
	@$(JAR) cfe $@ $(launcherjava_main) -C $(launcherjava_classes) .

$(launcherjava_legacy_jar): $(launcherjava_SRC)
	@mkdir -p $(launcherjava_classes) $(JARDIR)
	@$(JAVAC) -source $(JAVA_TARGET) -target $(JAVA_TARGET) -d $(launcherjava_classes) $(launcherjava_SRC)
	@$(JAR) cfe $@ $(launcherjava_main) -C $(launcherjava_classes) .

# javacheck
javacheck_classes := $(JAVA_OUT)/javacheck/classes
javacheck_jar := $(JARDIR)/JavaCheck.jar
javacheck_main := org.projecttick.javacheck.JavaCheck

$(javacheck_jar): $(javacheck_SRC)
	@mkdir -p $(javacheck_classes) $(JARDIR)
	@$(JAVAC) -source $(JAVA_TARGET) -target $(JAVA_TARGET) -Xlint:deprecation -Xlint:unchecked -d $(javacheck_classes) $(javacheck_SRC)
	@$(JAR) cfe $@ $(javacheck_main) -C $(javacheck_classes) .
