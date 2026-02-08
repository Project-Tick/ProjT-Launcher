# SPDX-License-Identifier: GPL-3.0
# launcher/ - Main Application Makefile
#
# This is the primary module of ProjT-Launcher containing
# the core application logic, UI, networking, and Minecraft integration.
#
# This Makefile can work in two modes:
# 1. Standalone: builds everything from source lists below
# 2. Recursive: calls subdirectory Makefiles (kernel-style)

# Include configuration
-include $(srctree)/.config
-include $(srctree)/scripts/Kbuild.include

# Qt tools
MOC ?= $(QT_MOC)
UIC ?= $(QT_UIC)
RCC ?= $(QT_RCC)

# Qt MOC include paths (for resolving dependencies in headers)
MOC_INCLUDES := \
	-I$(QT_PREFIX)/include \
	-I$(QT_PREFIX)/include/QtCore \
	-I$(QT_PREFIX)/include/QtWidgets \
	-I$(QT_PREFIX)/include/QtGui \
	-I$(QT_PREFIX)/include/QtNetwork \
	-I$(srctree)/launcher/tasks \
	-I$(srctree)/quazip \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/include

# Local include paths (bundled libs)
includes-y := libpng zlib

# Object output directory
obj := $(KBUILD_OUTPUT)/launcher

# ============================================================================
# SUBDIRECTORY MAKEFILES (Recursive Build)
# ============================================================================
# These subdirectories have their own Makefiles that will be called
SUBDIRS := \
	console \
	filelink \
	icons \
	java \
	launch \
	logs \
	meta \
	minecraft \
	modplatform \
	net \
	news \
	resources \
	screenshots \
	settings \
	tasks \
	tools \
	translations \
	ui \
	updater

# Recursive targets
.PHONY: subdirs $(SUBDIRS) subdirs-clean

subdirs: $(SUBDIRS)

$(SUBDIRS):
	@echo "  BUILD   launcher/$@"
	$(Q)$(MAKE) -C $(srctree)/launcher/$@ \
		srctree=$(srctree) \
		KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
		Q=$(Q)

subdirs-clean:
	@for dir in $(SUBDIRS); do \
		echo "  CLEAN   launcher/$$dir"; \
		$(MAKE) -C $(srctree)/launcher/$$dir \
			srctree=$(srctree) \
			KBUILD_OUTPUT=$(KBUILD_OUTPUT) \
			$${dir}-clean 2>/dev/null || true; \
	done

# Dependencies between subdirs
ui: settings tasks icons
minecraft: net meta settings tasks
modplatform: net minecraft
updater: net settings
launch: minecraft settings tasks

# ============================================================================
# CORE SOURCES - Main application files
# ============================================================================
core-y := \
	Application.cpp \
	ApplicationMessage.cpp \
	BaseInstaller.cpp \
	BaseInstance.cpp \
	BaseVersionList.cpp \
	Commandline.cpp \
	DataMigrationTask.cpp \
	DesktopServices.cpp \
	FileSystem.cpp \
	GZip.cpp \
	InstanceCopyPrefs.cpp \
	InstanceCopyTask.cpp \
	InstanceCreationTask.cpp \
	InstanceDirUpdate.cpp \
	InstanceImportTask.cpp \
	InstanceList.cpp \
	InstanceTask.cpp \
	JavaCommon.cpp \
	Json.cpp \
	KonamiCode.cpp \
	LaunchController.cpp \
	LoggedProcess.cpp \
	MMCTime.cpp \
	MMCZip.cpp \
	MangoHud.cpp \
	Markdown.cpp \
	MessageLevel.cpp \
	PngReader.cpp \
	RecursiveFileSystemWatcher.cpp \
	ResourceDownloadTask.cpp \
	StringUtils.cpp \
	SysInfo.cpp \
	Untar.cpp \
	Version.cpp \
	VersionProxyModel.cpp \
	main.cpp

# ============================================================================
# SUBDIRECTORY SOURCES
# ============================================================================

# Console module
console-y := \
	console/BufferedLogView.cpp \
	console/DelegateProgressItem.cpp

# Filelink module
filelink-y := \
	filelink/FileLink.cpp

# Icons module
icons-y := \
	icons/IconList.cpp \
	icons/MMCIcon.cpp

# Java detection and management
java-y := \
	java/core/JavaProbeTask.cpp \
	java/core/JavaVersionFormatter.cpp \
	java/core/JavaVersionParser.cpp \
	java/core/JavaVersionPath.cpp \
	java/core/JavaVersionResult.cpp \
	java/download/ArchiveDownloadTask.cpp \
	java/download/ManifestDownloadTask.cpp \
	java/services/AdoptiumJavaDownloader.cpp \
	java/services/AzulJavaDownloader.cpp \
	java/services/MojangJavaDownloader.cpp

# Launch pipeline
launch-y := \
	launch/LaunchLineRouter.cpp \
	launch/LaunchLogModel.cpp \
	launch/LaunchPipeline.cpp \
	launch/LaunchStage.cpp \
	launch/LaunchVariableExpander.cpp \
	launch/TaskBridgeStage.cpp \
	launch/steps/CheckJava.cpp \
	launch/steps/ClaimAccount.cpp \
	launch/steps/CreateGameFolders.cpp \
	launch/steps/DirectJavaLaunch.cpp \
	launch/steps/PreLaunchCommand.cpp \
	launch/steps/PostLaunchCommand.cpp \
	launch/steps/PrintInstanceInfo.cpp \
	launch/steps/QuitAfterGameStop.cpp \
	launch/steps/ScanModFolders.cpp \
	launch/steps/TextPrint.cpp \
	launch/steps/Update.cpp \
	launch/steps/VerifyJavaInstall.cpp

# Logs module
logs-y := \
	logs/LogOutput.cpp \
	logs/LogParser.cpp

# Meta module
meta-y := \
	meta/BaseEntity.cpp \
	meta/Index.cpp \
	meta/JsonFormat.cpp \
	meta/Version.cpp \
	meta/VersionList.cpp

# Minecraft core
minecraft-y := \
	minecraft/AssetsUtils.cpp \
	minecraft/BackupManager.cpp \
	minecraft/Component.cpp \
	minecraft/ComponentUpdateTask.cpp \
	minecraft/LaunchProfile.cpp \
	minecraft/Library.cpp \
	minecraft/Logging.cpp \
	minecraft/MinecraftInstance.cpp \
	minecraft/MinecraftInstanceLaunchMenu.cpp \
	minecraft/MinecraftLoadAndCheck.cpp \
	minecraft/MojangVersionFormat.cpp \
	minecraft/OneSixVersionFormat.cpp \
	minecraft/PackProfile.cpp \
	minecraft/ParseUtils.cpp \
	minecraft/ProfileUtils.cpp \
	minecraft/Rule.cpp \
	minecraft/ShortcutUtils.cpp \
	minecraft/VanillaInstanceCreationTask.cpp \
	minecraft/VersionFile.cpp \
	minecraft/VersionFilterData.cpp \
	minecraft/World.cpp \
	minecraft/WorldList.cpp

# Minecraft auth
minecraft-auth-y := \
	minecraft/auth/AccountData.cpp \
	minecraft/auth/AccountList.cpp \
	minecraft/auth/AccountTask.cpp \
	minecraft/auth/AuthRequest.cpp \
	minecraft/auth/AuthSession.cpp \
	minecraft/auth/MinecraftAccount.cpp \
	minecraft/auth/Parsers.cpp \
	minecraft/auth/flows/AuthContext.cpp \
	minecraft/auth/flows/AuthFlow.cpp \
	minecraft/auth/flows/MSA.cpp \
	minecraft/auth/flows/Offline.cpp

# Minecraft launch
minecraft-launch-y := \
	minecraft/launch/CreateGameFolders.cpp \
	minecraft/launch/ExtractNatives.cpp \
	minecraft/launch/LauncherPartLaunch.cpp \
	minecraft/launch/MinecraftServerTarget.cpp \
	minecraft/launch/PrintInstanceInfo.cpp \
	minecraft/launch/ReconstructAssets.cpp \
	minecraft/launch/ScanModFolders.cpp \
	minecraft/launch/VerifyJavaInstall.cpp \
	minecraft/launch/ClaimAccount.cpp \
	minecraft/launch/InjectAuthlib.cpp \
	minecraft/launch/DirectJavaLaunch.cpp

# Minecraft mods
minecraft-mod-y := \
	minecraft/mod/DataPack.cpp \
	minecraft/mod/DataPackFolderModel.cpp \
	minecraft/mod/Mod.cpp \
	minecraft/mod/ModFolderModel.cpp \
	minecraft/mod/Resource.cpp \
	minecraft/mod/ResourceFolderModel.cpp \
	minecraft/mod/ResourcePack.cpp \
	minecraft/mod/ResourcePackFolderModel.cpp \
	minecraft/mod/ShaderPack.cpp \
	minecraft/mod/TexturePack.cpp \
	minecraft/mod/TexturePackFolderModel.cpp \
	minecraft/mod/WorldSave.cpp \
	minecraft/mod/tasks/GetModDependenciesTask.cpp \
	minecraft/mod/tasks/LocalDataPackParseTask.cpp \
	minecraft/mod/tasks/LocalModParseTask.cpp \
	minecraft/mod/tasks/LocalResourcePackParseTask.cpp \
	minecraft/mod/tasks/LocalResourceParse.cpp \
	minecraft/mod/tasks/LocalShaderPackParseTask.cpp \
	minecraft/mod/tasks/LocalTexturePackParseTask.cpp \
	minecraft/mod/tasks/LocalWorldSaveParseTask.cpp \
	minecraft/mod/tasks/ModFolderLoadTask.cpp \
	minecraft/mod/tasks/ResourceFolderLoadTask.cpp

# Minecraft skins
minecraft-skins-y := \
	minecraft/skins/CapeChange.cpp \
	minecraft/skins/SkinDelete.cpp \
	minecraft/skins/SkinList.cpp \
	minecraft/skins/SkinModel.cpp \
	minecraft/skins/SkinUpload.cpp

# Minecraft update
minecraft-update-y := \
	minecraft/update/AssetUpdateTask.cpp \
	minecraft/update/FMLLibrariesTask.cpp \
	minecraft/update/LibrariesTask.cpp

# Modplatform core
modplatform-y := \
	modplatform/EnsureMetadataTask.cpp \
	modplatform/ModIndex.cpp \
	modplatform/ResourceAPI.cpp \
	modplatform/ResourceType.cpp \
	modplatform/helpers/ExportModsToStringTask.cpp \
	modplatform/helpers/GetModDependenciesTask.cpp \
	modplatform/helpers/HashUtils.cpp \
	modplatform/helpers/NetworkResourceAPI.cpp \
	modplatform/helpers/OverrideUtils.cpp

# Modplatform - ATLauncher
modplatform-atlauncher-y := \
	modplatform/atlauncher/ATLPackInstallTask.cpp \
	modplatform/atlauncher/ATLPackManifest.cpp

# Modplatform - CurseForge/Flame
modplatform-flame-y := \
	modplatform/flame/FileResolvingTask.cpp \
	modplatform/flame/FlameAPI.cpp \
	modplatform/flame/FlameCheckUpdate.cpp \
	modplatform/flame/FlameInstanceCreationTask.cpp \
	modplatform/flame/FlameModIndex.cpp \
	modplatform/flame/FlamePackExportTask.cpp

# Modplatform - Modrinth
modplatform-modrinth-y := \
	modplatform/modrinth/ModrinthAPI.cpp \
	modplatform/modrinth/ModrinthCheckUpdate.cpp \
	modplatform/modrinth/ModrinthInstanceCreationTask.cpp \
	modplatform/modrinth/ModrinthPackExportTask.cpp \
	modplatform/modrinth/ModrinthPackIndex.cpp \
	modplatform/modrinth/ModrinthPackManifest.cpp

# Modplatform - Technic
modplatform-technic-y := \
	modplatform/technic/SingleZipPackInstallTask.cpp \
	modplatform/technic/SolderPackInstallTask.cpp \
	modplatform/technic/SolderPackManifest.cpp \
	modplatform/technic/TechnicPackProcessor.cpp

# Modplatform - FTB
modplatform-ftb-y := \
	modplatform/legacy_ftb/PackFetchTask.cpp \
	modplatform/legacy_ftb/PackInstallTask.cpp \
	modplatform/legacy_ftb/PrivatePackManager.cpp \
	modplatform/import_ftb/PackHelpers.cpp

# Modplatform - Packwiz
modplatform-packwiz-y := \
	modplatform/packwiz/Packwiz.cpp

# Net module
net-y := \
	net/ApiDownload.cpp \
	net/ApiUpload.cpp \
	net/Download.cpp \
	net/FileSink.cpp \
	net/HttpMetaCache.cpp \
	net/Logging.cpp \
	net/MetaCacheSink.cpp \
	net/NetJob.cpp \
	net/NetRequest.cpp \
	net/PasteUpload.cpp \
	net/Upload.cpp

# News module
news-y := \
	news/NewsChecker.cpp \
	news/NewsEntry.cpp

# Screenshots module
screenshots-y := \
	screenshots/ImgurAlbumCreation.cpp \
	screenshots/ImgurUpload.cpp \
	screenshots/Screenshot.cpp \
	screenshots/ScreenshotList.cpp

# Settings module
settings-y := \
	settings/INIFile.cpp \
	settings/INISettingsObject.cpp \
	settings/OverrideSetting.cpp \
	settings/PassthroughSetting.cpp \
	settings/Setting.cpp \
	settings/SettingsObject.cpp

# Tasks module
tasks-y := \
	tasks/ConcurrentTask.cpp \
	tasks/MultipleOptionsTask.cpp \
	tasks/SequentialTask.cpp \
	tasks/Task.cpp

# Tools module
tools-y := \
	tools/BaseExternalTool.cpp \
	tools/BaseProfiler.cpp \
	tools/GenericProfiler.cpp \
	tools/JProfiler.cpp \
	tools/JVisualVM.cpp \
	tools/MCEditTool.cpp

# Translations module
translations-y := \
	translations/POTranslator.cpp \
	translations/TranslationsModel.cpp

# Updater module
updater-y := \
	updater/ProjTExternalUpdater.cpp \
	updater/projtupdater/UpdateChecker.cpp \
	updater/projtupdater/UpdateController.cpp

# macOS specific
updater-$(CONFIG_TARGET_MACOS) += \
	updater/MacSparkleUpdater.mm

# ============================================================================
# UI SOURCES
# ============================================================================

# UI Core
ui-y := \
	ui/GuiUtil.cpp \
	ui/InstanceWindow.cpp \
	ui/LaunchMenu.cpp \
	ui/MainWindow.cpp \
	ui/ViewLogWindow.cpp

# UI Dialogs
ui-dialogs-y := \
	ui/dialogs/AboutDialog.cpp \
	ui/dialogs/BackupDialog.cpp \
	ui/dialogs/BlockedModsDialog.cpp \
	ui/dialogs/ChooseProviderDialog.cpp \
	ui/dialogs/CopyInstanceDialog.cpp \
	ui/dialogs/CreateShortcutDialog.cpp \
	ui/dialogs/CustomMessageBox.cpp \
	ui/dialogs/ExportInstanceDialog.cpp \
	ui/dialogs/ExportPackDialog.cpp \
	ui/dialogs/ExportToModListDialog.cpp \
	ui/dialogs/IconPickerDialog.cpp \
	ui/dialogs/ImportResourceDialog.cpp \
	ui/dialogs/InstallLoaderDialog.cpp \
	ui/dialogs/LauncherHubDialog.cpp \
	ui/dialogs/MSALoginDialog.cpp \
	ui/dialogs/NewComponentDialog.cpp \
	ui/dialogs/NewInstanceDialog.cpp \
	ui/dialogs/NewsDialog.cpp \
	ui/dialogs/OfflineLoginDialog.cpp \
	ui/dialogs/ProfileSelectDialog.cpp \
	ui/dialogs/ProfileSetupDialog.cpp \
	ui/dialogs/ProgressDialog.cpp \
	ui/dialogs/ResourceDownloadDialog.cpp \
	ui/dialogs/ResourceUpdateDialog.cpp \
	ui/dialogs/ReviewMessageBox.cpp \
	ui/dialogs/ScrollMessageBox.cpp \
	ui/dialogs/UpdateAvailableDialog.cpp \
	ui/dialogs/VersionSelectDialog.cpp \
	ui/dialogs/skins/SkinManageDialog.cpp \
	ui/dialogs/skins/SkinUploadDialog.cpp

# UI Instance View
ui-instanceview-y := \
	ui/instanceview/InstanceDelegate.cpp \
	ui/instanceview/InstanceProxyModel.cpp \
	ui/instanceview/InstanceTableProxyModel.cpp \
	ui/instanceview/InstanceView.cpp \
	ui/instanceview/VisualGroup.cpp

# UI Page Dialog
ui-pagedialog-y := \
	ui/pagedialog/PageContainer.cpp \
	ui/pagedialog/PageDialog.cpp \
	ui/pagedialog/PageMultiContainer.cpp \
	ui/pagedialog/PageSidebar.cpp

# UI Global Pages
ui-pages-global-y := \
	ui/pages/global/AccountListPage.cpp \
	ui/pages/global/APIPage.cpp \
	ui/pages/global/CustomCommandsPage.cpp \
	ui/pages/global/EnvironmentVariablesPage.cpp \
	ui/pages/global/ExternalToolsPage.cpp \
	ui/pages/global/JavaPage.cpp \
	ui/pages/global/LauncherPage.cpp \
	ui/pages/global/MinecraftPage.cpp \
	ui/pages/global/ProxyPage.cpp

# UI Instance Pages
ui-pages-instance-y := \
	ui/pages/instance/DataPackPage.cpp \
	ui/pages/instance/ExternalResourcesPage.cpp \
	ui/pages/instance/InstanceSettingsPage.cpp \
	ui/pages/instance/LogPage.cpp \
	ui/pages/instance/McRecoverPage.cpp \
	ui/pages/instance/ModFolderPage.cpp \
	ui/pages/instance/NotesPage.cpp \
	ui/pages/instance/OtherLogsPage.cpp \
	ui/pages/instance/OverridePage.cpp \
	ui/pages/instance/ResourcePackPage.cpp \
	ui/pages/instance/ScreenshotsPage.cpp \
	ui/pages/instance/ServersPage.cpp \
	ui/pages/instance/ShaderPackPage.cpp \
	ui/pages/instance/TexturePackPage.cpp \
	ui/pages/instance/VersionPage.cpp \
	ui/pages/instance/WorldListPage.cpp

# UI Modplatform Pages
ui-pages-modplatform-y := \
	ui/pages/modplatform/ModModel.cpp \
	ui/pages/modplatform/ModPage.cpp \
	ui/pages/modplatform/ResourceModel.cpp \
	ui/pages/modplatform/ResourcePage.cpp \
	ui/pages/modplatform/ResourcePackModel.cpp \
	ui/pages/modplatform/ResourcePackPage.cpp \
	ui/pages/modplatform/ShaderPackModel.cpp \
	ui/pages/modplatform/ShaderPackPage.cpp \
	ui/pages/modplatform/TexturePackModel.cpp \
	ui/pages/modplatform/TexturePackPage.cpp \
	ui/pages/modplatform/atlauncher/AtlFilterModel.cpp \
	ui/pages/modplatform/atlauncher/AtlListModel.cpp \
	ui/pages/modplatform/atlauncher/AtlOptionalModDialog.cpp \
	ui/pages/modplatform/atlauncher/AtlPage.cpp \
	ui/pages/modplatform/atlauncher/AtlUserInteractionSupportImpl.cpp \
	ui/pages/modplatform/flame/FlameModel.cpp \
	ui/pages/modplatform/flame/FlamePage.cpp \
	ui/pages/modplatform/flame/FlameResourceModels.cpp \
	ui/pages/modplatform/flame/FlameResourcePages.cpp \
	ui/pages/modplatform/ftb/FtbFilterModel.cpp \
	ui/pages/modplatform/ftb/FtbListModel.cpp \
	ui/pages/modplatform/ftb/FtbPage.cpp \
	ui/pages/modplatform/legacy_ftb/ListModel.cpp \
	ui/pages/modplatform/legacy_ftb/Page.cpp \
	ui/pages/modplatform/modrinth/ModrinthModel.cpp \
	ui/pages/modplatform/modrinth/ModrinthPage.cpp \
	ui/pages/modplatform/modrinth/ModrinthResourceModels.cpp \
	ui/pages/modplatform/modrinth/ModrinthResourcePages.cpp \
	ui/pages/modplatform/technic/TechnicModel.cpp \
	ui/pages/modplatform/technic/TechnicPage.cpp \
	ui/pages/modplatform/ImportPage.cpp

# UI Setup Wizard
ui-setupwizard-y := \
	ui/setupwizard/AutoJavaWizardPage.cpp \
	ui/setupwizard/BaseWizardPage.cpp \
	ui/setupwizard/JavaWizardPage.cpp \
	ui/setupwizard/LanguageWizardPage.cpp \
	ui/setupwizard/PasteWizardPage.cpp \
	ui/setupwizard/SetupWizard.cpp \
	ui/setupwizard/ThemeWizardPage.cpp

# UI Java
ui-java-y := \
	ui/java/JavaDownloadTask.cpp \
	ui/java/JavaInstaller.cpp

# UI Tasks
ui-tasks-y := \
	ui/tasks/BackgroundTask.cpp \
	ui/tasks/BackgroundTaskHandler.cpp

# UI Themes
ui-themes-y := \
	ui/themes/CatPack.cpp \
	ui/themes/CustomTheme.cpp \
	ui/themes/DarkTheme.cpp \
	ui/themes/ITheme.cpp \
	ui/themes/SystemTheme.cpp \
	ui/themes/ThemeManager.cpp

# UI Widgets
ui-widgets-y := \
	ui/widgets/Common.cpp \
	ui/widgets/CustomFocusFrame.cpp \
	ui/widgets/DropLabel.cpp \
	ui/widgets/EnvironmentVariables.cpp \
	ui/widgets/ErrorFrame.cpp \
	ui/widgets/FocusLineEdit.cpp \
	ui/widgets/IconLabel.cpp \
	ui/widgets/InfoFrame.cpp \
	ui/widgets/JavaDownloader.cpp \
	ui/widgets/JavaSettingsWidget.cpp \
	ui/widgets/LabeledToolButton.cpp \
	ui/widgets/LineSeparator.cpp \
	ui/widgets/LogView.cpp \
	ui/widgets/ModFilterWidget.cpp \
	ui/widgets/ModListView.cpp \
	ui/widgets/ModResourceWidget.cpp \
	ui/widgets/PageContainer.cpp \
	ui/widgets/PageSidebar.cpp \
	ui/widgets/ProjectItem.cpp \
	ui/widgets/QRWidget.cpp \
	ui/widgets/StatusLabel.cpp \
	ui/widgets/SubTaskProgressBar.cpp \
	ui/widgets/ThemeCustomizationWidget.cpp \
	ui/widgets/VariableSizedImageObject.cpp \
	ui/widgets/VersionListView.cpp \
	ui/widgets/VersionSelectWidget.cpp

# ============================================================================
# QT UI FILES (need uic processing)
# ============================================================================
UI_FILES := \
	ui/MainWindow.ui \
	ui/dialogs/AboutDialog.ui \
	ui/dialogs/BackupDialog.ui \
	ui/dialogs/BlockedModsDialog.ui \
	ui/dialogs/ChooseProviderDialog.ui \
	ui/dialogs/CopyInstanceDialog.ui \
	ui/dialogs/CreateShortcutDialog.ui \
	ui/dialogs/ExportInstanceDialog.ui \
	ui/dialogs/ExportPackDialog.ui \
	ui/dialogs/ExportToModListDialog.ui \
	ui/dialogs/IconPickerDialog.ui \
	ui/dialogs/ImportResourceDialog.ui \
	ui/dialogs/MSALoginDialog.ui \
	ui/dialogs/NewComponentDialog.ui \
	ui/dialogs/NewInstanceDialog.ui \
	ui/dialogs/NewsDialog.ui \
	ui/dialogs/OfflineLoginDialog.ui \
	ui/dialogs/ProfileSelectDialog.ui \
	ui/dialogs/ProfileSetupDialog.ui \
	ui/dialogs/ProgressDialog.ui \
	ui/dialogs/ReviewMessageBox.ui \
	ui/dialogs/ScrollMessageBox.ui \
	ui/dialogs/UpdateAvailableDialog.ui \
	ui/pages/global/AccountListPage.ui \
	ui/pages/global/APIPage.ui \
	ui/pages/global/CustomCommandsPage.ui \
	ui/pages/global/EnvironmentVariablesPage.ui \
	ui/pages/global/ExternalToolsPage.ui \
	ui/pages/global/JavaPage.ui \
	ui/pages/global/LauncherPage.ui \
	ui/pages/global/MinecraftPage.ui \
	ui/pages/global/ProxyPage.ui \
	ui/pages/instance/InstanceSettingsPage.ui \
	ui/pages/instance/LogPage.ui \
	ui/pages/instance/NotesPage.ui \
	ui/pages/instance/OtherLogsPage.ui \
	ui/pages/instance/VersionPage.ui \
	ui/widgets/CustomCommands.ui \
	ui/widgets/EnvironmentVariables.ui \
	ui/widgets/JavaSettingsWidget.ui \
	ui/widgets/ModFilterWidget.ui \
	ui/widgets/ThemeCustomizationWidget.ui

# ============================================================================
# RESOURCE FILES
# ============================================================================
QRC_FILES := \
	resources/launcher.qrc \
	resources/backgrounds/backgrounds.qrc \
	resources/documents/documents.qrc \
	resources/flat/flat.qrc \
	resources/flat_white/flat_white.qrc \
	resources/ios/ios.qrc \
	resources/legacy/legacy.qrc \
	resources/mac/mac.qrc \
	resources/macos/macos.qrc \
	resources/pe_blue/pe_blue.qrc \
	resources/pe_dark/pe_dark.qrc \
	resources/pe_light/pe_light.qrc \
	resources/pe_colored/pe_colored.qrc \
	resources/skins/skins.qrc

# ============================================================================
# COLLECT ALL SOURCES
# ============================================================================
LAUNCHER_SRCS := \
	$(core-y) \
	$(console-y) \
	$(filelink-y) \
	$(icons-y) \
	$(java-y) \
	$(launch-y) \
	$(logs-y) \
	$(meta-y) \
	$(minecraft-y) \
	$(minecraft-auth-y) \
	$(minecraft-launch-y) \
	$(minecraft-mod-y) \
	$(minecraft-skins-y) \
	$(minecraft-update-y) \
	$(modplatform-y) \
	$(modplatform-atlauncher-y) \
	$(modplatform-flame-y) \
	$(modplatform-modrinth-y) \
	$(modplatform-technic-y) \
	$(modplatform-ftb-y) \
	$(modplatform-packwiz-y) \
	$(net-y) \
	$(news-y) \
	$(screenshots-y) \
	$(settings-y) \
	$(tasks-y) \
	$(tools-y) \
	$(translations-y) \
	$(updater-y) \
	$(ui-y) \
	$(ui-dialogs-y) \
	$(ui-instanceview-y) \
	$(ui-pagedialog-y) \
	$(ui-pages-global-y) \
	$(ui-pages-instance-y) \
	$(ui-pages-modplatform-y) \
	$(ui-setupwizard-y) \
	$(ui-java-y) \
	$(ui-tasks-y) \
	$(ui-themes-y) \
	$(ui-widgets-y)

# Generate object file list (only core files - subdirs have their own Makefiles)
LAUNCHER_OBJS := $(addprefix $(obj)/, $(core-y:.cpp=.o))

# MOC headers (only root launcher/ headers)
MOC_HEADERS := $(shell grep -l Q_OBJECT $(srctree)/launcher/*.h 2>/dev/null)
MOC_SRCS := $(patsubst $(srctree)/launcher/%.h,$(obj)/moc_%.cpp,$(MOC_HEADERS))
MOC_OBJS := $(MOC_SRCS:.cpp=.o)

# Qt Resource files
QRC_SRCS := \
	$(obj)/qrc_projtlauncher.cpp \
	$(obj)/qrc_multimc.cpp \
	$(obj)/qrc_backgrounds.cpp \
	$(obj)/qrc_documents.cpp \
	$(obj)/qrc_pe_dark.cpp \
	$(obj)/qrc_pe_light.cpp \
	$(obj)/qrc_pe_blue.cpp \
	$(obj)/qrc_pe_colored.cpp \
	$(obj)/qrc_breeze_dark.cpp \
	$(obj)/qrc_breeze_light.cpp \
	$(obj)/qrc_OSX.cpp \
	$(obj)/qrc_iOS.cpp \
	$(obj)/qrc_flat.cpp \
	$(obj)/qrc_flat_white.cpp \
	$(obj)/qrc_shaders.cpp

QRC_OBJS := $(QRC_SRCS:.cpp=.o)

# UIC and RCC are handled by subdirectory Makefiles (launcher/ui, launcher/resources)
# Core launcher files don't need them directly

# ============================================================================
# COMPILER FLAGS
# ============================================================================
# Get Qt include dir from pkg-config for system Qt
QT_INCLUDE_DIR := $(shell pkg-config --variable=includedir Qt6Core 2>/dev/null || echo $(QT_PREFIX)/include)

LAUNCHER_CXXFLAGS := \
	-I$(srctree)/launcher \
	-I$(srctree)/include \
	-I$(srctree)/buildconfig \
        -I$(srctree)/zlib \
	-I$(srctree)/quazip \
	-I$(srctree)/quazip/quazip \
	-I$(srctree)/qdcss/include \
	-I$(srctree)/tomlplusplus/include \
	-I$(srctree)/LocalPeer/include \
	-I$(srctree)/rainbow/include \
	-I$(srctree)/cmark/src \
	-I$(srctree)/cmark/build/src \
	-I$(srctree)/murmur2/src \
	-I$(srctree)/gamemode/lib \
	-I$(srctree)/systeminfo/include \
	-I$(srctree)/libqrencode \
	-I$(KBUILD_OUTPUT)/ui \
	-I$(QT_INCLUDE_DIR)/QtConcurrent \
	-I$(QT_INCLUDE_DIR)/QtNetworkAuth \
	-I$(QT_INCLUDE_DIR)/QtXml \
	-I$(QT_INCLUDE_DIR)/QtDBus \
	-I$(obj) \
	-DLAUNCHER_APPLICATION \
	$(QT_CFLAGS) \
	$(CXXFLAGS)

LAUNCHER_LDFLAGS := \
	$(QT_LIBS) \
	$(LDFLAGS)

# ============================================================================
# BUILD TARGETS
# ============================================================================
.PHONY: all launcher launcher-clean launcher-install

all: launcher

# Main target
launcher: $(obj)/liblauncher.a

# Static library for linking (core files + MOC + RCC resources)
$(obj)/liblauncher.a: $(LAUNCHER_OBJS) $(MOC_OBJS) $(QRC_OBJS)
	@echo "  AR      $@"
	$(Q)$(AR) rcs $@ $^

# Pattern rule for C++ compilation
# -I$(obj) ensures inline moc files can be found
$(obj)/%.o: $(srctree)/launcher/%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LAUNCHER_CXXFLAGS) -I$(obj) -c -o $@ $<

# Pattern rule for Objective-C++ (macOS)
$(obj)/%.o: $(srctree)/launcher/%.mm
	@mkdir -p $(dir $@)
	@echo "  OBJCXX  $<"
	$(Q)$(CXX) $(LAUNCHER_CXXFLAGS) -ObjC++ -c -o $@ $<

# MOC processing for headers
$(obj)/moc_%.cpp: $(srctree)/launcher/%.h
	@mkdir -p $(dir $@)
	@echo "  MOC     $<"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher -I$(srctree) $(MOC_INCLUDES) -o $@ $<

$(obj)/moc_%.o: $(obj)/moc_%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LAUNCHER_CXXFLAGS) -c -o $@ $<

# Inline MOC processing for cpp files (generates .moc from .cpp)
$(obj)/%.moc: $(srctree)/launcher/%.cpp
	@mkdir -p $(dir $@)
	@echo "  MOC     $< (inline)"
	$(Q)$(MOC) $(QT_DEFINES) -DLAUNCHER_APPLICATION -I$(srctree)/launcher -I$(srctree) $(MOC_INCLUDES) -o $@ $<

# Inline MOC dependencies
$(obj)/InstanceList.o: $(obj)/InstanceList.moc
$(obj)/VersionProxyModel.o: $(obj)/VersionProxyModel.moc

# UIC processing
$(obj)/ui_%.h: $(srctree)/launcher/ui/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/dialogs/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/global/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/pages/instance/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

$(obj)/ui_%.h: $(srctree)/launcher/ui/widgets/%.ui
	@mkdir -p $(dir $@)
	@echo "  UIC     $<"
	$(Q)$(UIC) -o $@ $<

# RCC processing - resources directory
$(obj)/qrc_%.cpp: $(srctree)/launcher/resources/%.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name $(basename $(notdir $<)) -o $@ $<

# RCC processing - resources subdirectories
$(obj)/qrc_multimc.cpp: $(srctree)/launcher/resources/multimc/multimc.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name multimc -o $@ $<

$(obj)/qrc_backgrounds.cpp: $(srctree)/launcher/resources/backgrounds/backgrounds.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name backgrounds -o $@ $<

$(obj)/qrc_documents.cpp: $(srctree)/launcher/resources/documents/documents.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name documents -o $@ $<

$(obj)/qrc_pe_dark.cpp: $(srctree)/launcher/resources/pe_dark/pe_dark.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name pe_dark -o $@ $<

$(obj)/qrc_pe_light.cpp: $(srctree)/launcher/resources/pe_light/pe_light.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name pe_light -o $@ $<

$(obj)/qrc_pe_blue.cpp: $(srctree)/launcher/resources/pe_blue/pe_blue.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name pe_blue -o $@ $<

$(obj)/qrc_pe_colored.cpp: $(srctree)/launcher/resources/pe_colored/pe_colored.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name pe_colored -o $@ $<

$(obj)/qrc_breeze_dark.cpp: $(srctree)/launcher/resources/breeze_dark/breeze_dark.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name breeze_dark -o $@ $<

$(obj)/qrc_breeze_light.cpp: $(srctree)/launcher/resources/breeze_light/breeze_light.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name breeze_light -o $@ $<

$(obj)/qrc_OSX.cpp: $(srctree)/launcher/resources/OSX/OSX.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name OSX -o $@ $<

$(obj)/qrc_iOS.cpp: $(srctree)/launcher/resources/iOS/iOS.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name iOS -o $@ $<

$(obj)/qrc_flat.cpp: $(srctree)/launcher/resources/flat/flat.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name flat -o $@ $<

$(obj)/qrc_flat_white.cpp: $(srctree)/launcher/resources/flat_white/flat_white.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name flat_white -o $@ $<

$(obj)/qrc_shaders.cpp: $(srctree)/launcher/resources/shaders/shaders.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name shaders -o $@ $<

# Generated QRC files (from program_info)
$(obj)/qrc_projtlauncher.cpp: $(KBUILD_OUTPUT)/generated/program_info/projtlauncher.qrc
	@mkdir -p $(dir $@)
	@echo "  RCC     $<"
	$(Q)$(RCC) --name projtlauncher -o $@ $<

$(obj)/qrc_%.o: $(obj)/qrc_%.cpp
	@mkdir -p $(dir $@)
	@echo "  CXX     $<"
	$(Q)$(CXX) $(LAUNCHER_CXXFLAGS) -c -o $@ $<

# Clean target
launcher-clean: subdirs-clean
	@echo "  CLEAN   launcher"
	$(Q)rm -rf $(obj)

# Install target
launcher-install: launcher
	@echo "  INSTALL launcher"
	$(Q)install -d $(DESTDIR)$(PREFIX)/lib
	$(Q)install -m 644 $(obj)/liblauncher.a $(DESTDIR)$(PREFIX)/lib/

# ============================================================================
# RECURSIVE BUILD MODE
# ============================================================================
# Use 'make launcher-recursive' to build using subdirectory Makefiles
# Use 'make launcher' for monolithic build (all sources in this file)

# Collect all subdir libraries
SUBDIR_LIBS := \
	$(KBUILD_OUTPUT)/ui/libui.a \
	$(KBUILD_OUTPUT)/minecraft/libminecraft.a \
	$(KBUILD_OUTPUT)/modplatform/libmodplatform.a \
	$(KBUILD_OUTPUT)/net/libnet.a \
	$(KBUILD_OUTPUT)/meta/libmeta.a \
	$(KBUILD_OUTPUT)/java/libjava.a \
	$(KBUILD_OUTPUT)/launch/liblaunch.a \
	$(KBUILD_OUTPUT)/settings/libsettings.a \
	$(KBUILD_OUTPUT)/tasks/libtasks.a \
	$(KBUILD_OUTPUT)/tools/libtools.a \
	$(KBUILD_OUTPUT)/translations/libtranslations.a \
	$(KBUILD_OUTPUT)/updater/libupdater.a \
	$(KBUILD_OUTPUT)/logs/liblogs.a \
	$(KBUILD_OUTPUT)/news/libnews.a \
	$(KBUILD_OUTPUT)/screenshots/libscreenshots.a \
	$(KBUILD_OUTPUT)/icons/libicons.a \
	$(KBUILD_OUTPUT)/console/libconsole.a \
	$(KBUILD_OUTPUT)/resources/libresources.a

# Core-only objects (files in launcher/ root, not subdirs)
CORE_OBJS := $(addprefix $(obj)/, $(core-y:.cpp=.o))

launcher-recursive: subdirs $(CORE_OBJS)
	@echo "  AR      $(obj)/liblauncher.a (recursive)"
	$(Q)mkdir -p $(obj)
	$(Q)$(AR) rcs $(obj)/liblauncher.a $(CORE_OBJS)
	@echo "  MERGE   Merging subdir libraries..."
	$(Q)for lib in $(SUBDIR_LIBS); do \
		if [ -f "$$lib" ]; then \
			$(AR) x $$lib && $(AR) rcs $(obj)/liblauncher.a *.o && rm -f *.o; \
		fi; \
	done

.PHONY: launcher-recursive

# Header dependencies
-include $(LAUNCHER_OBJS:.o=.d)
-include $(MOC_OBJS:.o=.d)
-include $(RCC_OBJS:.o=.d)
-include $(CORE_OBJS:.o=.d)

# Include common build rules
include $(srctree)/mk/module.mk
