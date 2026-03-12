# SPDX-License-Identifier: GPL-2.0

include mk/vars.mk

GEN_DIR := $(KBUILD_OUTPUT)/gen

BUILDCONFIG_CPP := $(GEN_DIR)/buildconfig/BuildConfig.cpp

$(BUILDCONFIG_CPP): buildconfig/BuildConfig.cpp.in scripts/gen_buildconfig.py
	@mkdir -p $(dir $@)
	@python3 scripts/gen_buildconfig.py $< $@

# program_info generated files
PROGRAM_INFO_DIR := $(GEN_DIR)/program_info

PROGRAM_INFO_DESKTOP := $(PROGRAM_INFO_DIR)/$(Launcher_AppID).desktop
PROGRAM_INFO_METAINFO := $(PROGRAM_INFO_DIR)/$(Launcher_AppID).metainfo.xml
PROGRAM_INFO_RC := $(PROGRAM_INFO_DIR)/projtlauncher.rc
PROGRAM_INFO_QRC := $(PROGRAM_INFO_DIR)/projtlauncher.qrc
PROGRAM_INFO_MANIFEST := $(PROGRAM_INFO_DIR)/projtlauncher.manifest
PROGRAM_INFO_ICO := $(PROGRAM_INFO_DIR)/projtlauncher.ico
PROGRAM_INFO_SVG := $(PROGRAM_INFO_DIR)/$(Launcher_SVGFileName)
PROGRAM_INFO_LICENSES := $(PROGRAM_INFO_DIR)/CombinedLicenses.txt

$(PROGRAM_INFO_DESKTOP): program_info/$(Launcher_AppID).desktop.in scripts/gen_buildconfig.py
	@mkdir -p $(dir $@)
	@python3 scripts/gen_buildconfig.py $< $@

$(PROGRAM_INFO_METAINFO): program_info/$(Launcher_AppID).metainfo.xml.in scripts/gen_buildconfig.py
	@mkdir -p $(dir $@)
	@python3 scripts/gen_buildconfig.py $< $@

$(PROGRAM_INFO_RC): program_info/projtlauncher.rc.in scripts/gen_buildconfig.py
	@mkdir -p $(dir $@)
	@python3 scripts/gen_buildconfig.py $< $@

$(PROGRAM_INFO_QRC): program_info/projtlauncher.qrc.in scripts/gen_buildconfig.py
	@mkdir -p $(dir $@)
	@python3 scripts/gen_buildconfig.py $< $@

$(PROGRAM_INFO_MANIFEST): program_info/projtlauncher.manifest.in scripts/gen_buildconfig.py
	@mkdir -p $(dir $@)
	@python3 scripts/gen_buildconfig.py $< $@

$(PROGRAM_INFO_ICO): program_info/projtlauncher.ico
	@mkdir -p $(dir $@)
	@cp $< $@

$(PROGRAM_INFO_SVG): program_info/$(Launcher_SVGFileName)
	@mkdir -p $(dir $@)
	@cp $< $@

$(PROGRAM_INFO_LICENSES): LICENSE zlib/LICENSE quazip/COPYING qdcss/LICENSE libnbtplusplus/COPYING libnbtplusplus/COPYING.LESSER launcherjava/LICENSE javacheck/LICENSE
	@mkdir -p $(dir $@)
	@printf "--- zlib ---\\n\\n" > $@
	@cat zlib/LICENSE >> $@
	@printf "\\n\\n--- QuaZip ---\\n\\n" >> $@
	@cat quazip/COPYING >> $@
	@printf "\\n\\n--- qdcss ---\\n\\n" >> $@
	@cat qdcss/LICENSE >> $@
	@printf "\\n\\n--- libnbtplusplus ---\\n\\n" >> $@
	@cat libnbtplusplus/COPYING >> $@
	@printf "\\n\\n" >> $@
	@cat libnbtplusplus/COPYING.LESSER >> $@
	@printf "\\n\\n--- launcherjava ---\\n\\n" >> $@
	@cat launcherjava/LICENSE >> $@
	@printf "\\n\\n--- javacheck ---\\n\\n" >> $@
	@cat javacheck/LICENSE >> $@
