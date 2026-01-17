# ProjT Launcher - Launcher Source Files
# SPDX-License-Identifier: GPL-3.0-only
# Copyright (C) 2026 Project Tick
#
# This file defines all source files for the launcher application
# Included by Makefile.in

#============================================================================
# CORE SOURCES
#============================================================================

LAUNCHER_CORE_SRCS = \
    BaseInstaller.cpp \
    BaseVersionList.cpp \
    InstanceList.cpp \
    InstanceTask.cpp \
    LoggedProcess.cpp \
    MessageLevel.cpp \
    BaseInstance.cpp \
    InstanceDirUpdate.cpp \
    MMCZip.cpp \
    Untar.cpp \
    StringUtils.cpp \
    InstanceCreationTask.cpp \
    InstanceCopyPrefs.cpp \
    InstanceCopyTask.cpp \
    InstanceImportTask.cpp \
    ResourceDownloadTask.cpp \
    Json.cpp \
    FileSystem.cpp \
    GZip.cpp \
    Commandline.cpp \
    Version.cpp \
    RecursiveFileSystemWatcher.cpp \
    MMCTime.cpp

#============================================================================
# NETWORK SOURCES
#============================================================================

LAUNCHER_NET_SRCS = \
    net/Download.cpp \
    net/FileSink.cpp \
    net/HttpMetaCache.cpp \
    net/MetaCacheSink.cpp \
    net/Logging.cpp \
    net/NetJob.cpp \
    net/PasteUpload.cpp \
    net/Upload.cpp \
    net/ApiDownload.cpp \
    net/ApiUpload.cpp \
    net/NetRequest.cpp

#============================================================================
# LAUNCH SOURCES
#============================================================================

LAUNCHER_LAUNCH_SRCS = \
    launch/steps/RuntimeProbeStep.cpp \
    launch/steps/ServerJoinResolveStep.cpp \
    launch/steps/LaunchCommandStep.cpp \
    launch/steps/LogMessageStep.cpp \
    launch/steps/QuitAfterGameStep.cpp \
    launch/steps/HostLookupReportStep.cpp \
    launch/LaunchStage.cpp \
    launch/LaunchPipeline.cpp \
    launch/LaunchLineRouter.cpp \
    launch/LaunchLogModel.cpp \
    launch/TaskBridgeStage.cpp \
    launch/LaunchVariableExpander.cpp \
    logs/LogEventParser.cpp

#============================================================================
# MINECRAFT SOURCES
#============================================================================

LAUNCHER_MC_SRCS = \
    minecraft/Logging.cpp \
    minecraft/BackupManager.cpp \
    minecraft/auth/AccountData.cpp \
    minecraft/auth/AccountList.cpp \
    minecraft/auth/AuthSession.cpp \
    minecraft/auth/MinecraftAccount.cpp \
    minecraft/auth/Parsers.cpp \
    minecraft/auth/AuthFlow.cpp \
    minecraft/auth/steps/MicrosoftOAuthStep.cpp \
    minecraft/auth/steps/DeviceCodeAuthStep.cpp \
    minecraft/auth/steps/XboxLiveUserStep.cpp \
    minecraft/auth/steps/XboxSecurityTokenStep.cpp \
    minecraft/auth/steps/XboxProfileFetchStep.cpp \
    minecraft/auth/steps/MinecraftServicesLoginStep.cpp \
    minecraft/auth/steps/MinecraftProfileFetchStep.cpp \
    minecraft/auth/steps/GameEntitlementsStep.cpp \
    minecraft/auth/steps/SkinDownloadStep.cpp \
    minecraft/update/AssetUpdateTask.cpp \
    minecraft/update/FMLLibrariesTask.cpp \
    minecraft/update/FoldersTask.cpp \
    minecraft/update/LibrariesTask.cpp \
    minecraft/launch/ClaimAccount.cpp \
    minecraft/launch/CreateGameFolders.cpp \
    minecraft/launch/ModMinecraftJar.cpp \
    minecraft/launch/ExtractNatives.cpp \
    minecraft/launch/LauncherPartLaunch.cpp \
    minecraft/launch/MinecraftTarget.cpp \
    minecraft/launch/PrintInstanceInfo.cpp \
    minecraft/launch/ReconstructAssets.cpp \
    minecraft/launch/ScanModFolders.cpp \
    minecraft/launch/VerifyJavaInstall.cpp \
    minecraft/launch/AutoInstallJava.cpp \
    minecraft/MinecraftInstance.cpp \
    minecraft/MinecraftInstanceLaunchMenu.cpp \
    minecraft/LaunchProfile.cpp \
    minecraft/Component.cpp \
    minecraft/PackProfile.cpp \
    minecraft/ComponentUpdateTask.cpp \
    minecraft/MinecraftLoadAndCheck.cpp \
    minecraft/MojangVersionFormat.cpp \
    minecraft/Rule.cpp \
    minecraft/OneSixVersionFormat.cpp \
    minecraft/ParseUtils.cpp \
    minecraft/ProfileUtils.cpp \
    minecraft/ShortcutUtils.cpp \
    minecraft/Library.cpp \
    minecraft/VanillaInstanceCreationTask.cpp \
    minecraft/VersionFile.cpp \
    minecraft/VersionFilterData.cpp \
    minecraft/World.cpp \
    minecraft/WorldList.cpp \
    minecraft/mod/Mod.cpp \
    minecraft/mod/ModFolderModel.cpp \
    minecraft/mod/Resource.cpp \
    minecraft/mod/ResourceFolderModel.cpp \
    minecraft/mod/DataPack.cpp \
    minecraft/mod/DataPackFolderModel.cpp \
    minecraft/mod/ResourcePack.cpp \
    minecraft/mod/ResourcePackFolderModel.cpp \
    minecraft/mod/TexturePack.cpp \
    minecraft/mod/ShaderPack.cpp \
    minecraft/mod/WorldSave.cpp \
    minecraft/mod/TexturePackFolderModel.cpp \
    minecraft/mod/tasks/ResourceFolderLoadTask.cpp \
    minecraft/mod/tasks/LocalModParseTask.cpp \
    minecraft/mod/tasks/LocalResourceUpdateTask.cpp \
    minecraft/mod/tasks/LocalDataPackParseTask.cpp \
    minecraft/mod/tasks/LocalTexturePackParseTask.cpp \
    minecraft/mod/tasks/LocalShaderPackParseTask.cpp \
    minecraft/mod/tasks/LocalWorldSaveParseTask.cpp \
    minecraft/mod/tasks/LocalResourceParse.cpp \
    minecraft/mod/tasks/GetModDependenciesTask.cpp \
    minecraft/AssetsUtils.cpp \
    minecraft/skins/CapeChange.cpp \
    minecraft/skins/SkinUpload.cpp \
    minecraft/skins/SkinDelete.cpp \
    minecraft/skins/SkinModel.cpp \
    minecraft/skins/SkinList.cpp

#============================================================================
# UI SOURCES
#============================================================================

LAUNCHER_UI_SRCS = \
    DesktopServices.cpp \
    KonamiCode.cpp \
    Markdown.cpp

#============================================================================
# APPLICATION SOURCES
#============================================================================

LAUNCHER_APP_SRCS = \
    Application.cpp \
    DataMigrationTask.cpp \
    ApplicationMessage.cpp \
    SysInfo.cpp \
    LaunchController.cpp \
    JavaCommon.cpp \
    main.cpp

#============================================================================
# JAVA SOURCES
#============================================================================

LAUNCHER_JAVA_SRCS = \
    java/core/RuntimeVersion.cpp \
    java/core/RuntimeInstall.cpp \
    java/core/RuntimePackage.cpp \
    java/services/RuntimeEnvironment.cpp \
    java/services/RuntimeScanner.cpp \
    java/services/RuntimeProbeTask.cpp \
    java/services/RuntimeCatalog.cpp \
    java/download/RuntimeArchiveTask.cpp \
    java/download/RuntimeManifestTask.cpp \
    java/download/RuntimeLinkTask.cpp

#============================================================================
# OTHER SOURCES
#============================================================================

LAUNCHER_OTHER_SRCS = \
    news/NewsChecker.cpp \
    news/NewsEntry.cpp \
    icons/IconUtils.cpp \
    screenshots/ImgurUpload.cpp \
    screenshots/ImgurAlbumCreation.cpp \
    tasks/Task.cpp \
    tasks/ConcurrentTask.cpp \
    tasks/SequentialTask.cpp \
    tasks/MultipleOptionsTask.cpp \
    settings/INIFile.cpp \
    settings/INISettingsObject.cpp \
    settings/OverrideSetting.cpp \
    settings/PassthroughSetting.cpp \
    settings/Setting.cpp \
    settings/SettingsObject.cpp \
    translations/TranslationsModel.cpp \
    translations/POTranslator.cpp \
    tools/BaseExternalTool.cpp \
    tools/BaseProfiler.cpp \
    tools/JProfiler.cpp \
    tools/JVisualVM.cpp \
    tools/MCEditTool.cpp \
    tools/GenericProfiler.cpp \
    meta/BaseEntity.cpp \
    meta/JsonFormat.cpp \
    meta/Version.cpp \
    meta/VersionList.cpp \
    meta/Index.cpp \
    updater/ProjTExternalUpdater.cpp

#============================================================================
# MODPLATFORM SOURCES
#============================================================================

LAUNCHER_MODPLATFORM_SRCS = \
    modplatform/ModIndex.cpp \
    modplatform/ResourceType.cpp \
    modplatform/ResourceAPI.cpp \
    modplatform/EnsureMetadataTask.cpp \
    modplatform/flame/FlameAPI.cpp \
    modplatform/flame/FlameModIndex.cpp \
    modplatform/flame/PackManifest.cpp \
    modplatform/flame/FileResolvingTask.cpp \
    modplatform/flame/FlameCheckUpdate.cpp \
    modplatform/flame/FlameInstanceCreationTask.cpp \
    modplatform/flame/FlamePackExportTask.cpp \
    modplatform/modrinth/ModrinthAPI.cpp \
    modplatform/modrinth/ModrinthPackIndex.cpp \
    modplatform/modrinth/ModrinthCheckUpdate.cpp \
    modplatform/modrinth/ModrinthInstanceCreationTask.cpp \
    modplatform/modrinth/ModrinthPackExportTask.cpp \
    modplatform/packwiz/Packwiz.cpp \
    modplatform/legacy_ftb/PackFetchTask.cpp \
    modplatform/legacy_ftb/PackInstallTask.cpp \
    modplatform/legacy_ftb/PrivatePackManager.cpp \
    modplatform/import_ftb/PackInstallTask.cpp \
    modplatform/import_ftb/PackHelpers.cpp \
    modplatform/technic/SingleZipPackInstallTask.cpp \
    modplatform/technic/SolderPackInstallTask.cpp \
    modplatform/technic/SolderPackManifest.cpp \
    modplatform/technic/TechnicPackProcessor.cpp \
    modplatform/atlauncher/ATLPackIndex.cpp \
    modplatform/atlauncher/ATLPackInstallTask.cpp \
    modplatform/atlauncher/ATLPackManifest.cpp \
    modplatform/atlauncher/ATLShareCode.cpp \
    modplatform/helpers/HashUtils.cpp \
    modplatform/helpers/OverrideUtils.cpp \
    modplatform/helpers/ExportToModList.cpp

#============================================================================
# COMBINE ALL SOURCES
#============================================================================

LAUNCHER_ALL_SRCS = \
    $(LAUNCHER_CORE_SRCS) \
    $(LAUNCHER_NET_SRCS) \
    $(LAUNCHER_LAUNCH_SRCS) \
    $(LAUNCHER_MC_SRCS) \
    $(LAUNCHER_UI_SRCS) \
    $(LAUNCHER_APP_SRCS) \
    $(LAUNCHER_JAVA_SRCS) \
    $(LAUNCHER_OTHER_SRCS) \
    $(LAUNCHER_MODPLATFORM_SRCS)

#============================================================================
# OBJECT FILES
#============================================================================

LAUNCHER_OBJS = $(patsubst %.cpp,$(LAUNCHER_OBJ)/%$(OBJ_SUFFIX),$(LAUNCHER_ALL_SRCS))

# Add BuildConfig object
LAUNCHER_OBJS += $(LAUNCHER_OBJ)/BuildConfig$(OBJ_SUFFIX)
