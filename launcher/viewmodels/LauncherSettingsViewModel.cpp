// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "LauncherSettingsViewModel.h"

#include <QFileDialog>
#include <QProcess>
#include <QStandardPaths>

#include "Application.h"
#include "settings/SettingsObject.h"

LauncherSettingsViewModel::LauncherSettingsViewModel(QObject* parent) : QObject(parent)
{
    loadSettings();
}

void LauncherSettingsViewModel::loadSettings()
{
    loadFromApplication();
}

void LauncherSettingsViewModel::loadFromApplication()
{
    auto s = APPLICATION->settings();

    // Launcher Page
    m_sortByName = s->get("InstSortMode").toString() == "Name";
    m_renamingBehavior = s->get("InstRenamingMode").toString();
    // Map old values to new
    if (m_renamingBehavior.isEmpty() || m_renamingBehavior == "AskEverytime")
        m_renamingBehavior = "ask";
    else if (m_renamingBehavior == "AlwaysRename")
        m_renamingBehavior = "always";
    else if (m_renamingBehavior == "NeverRename")
        m_renamingBehavior = "never";

    // Launch action based on CloseAfterLaunch and QuitAfterGameStop settings
    bool closeAfterLaunch = s->get("CloseAfterLaunch").toBool();
    bool quitAfterGameStop = s->get("QuitAfterGameStop").toBool();
    if (quitAfterGameStop)
        m_launchAction = "closeWindow";
    else if (closeAfterLaunch)
        m_launchAction = "hideWindow";
    else
        m_launchAction = "doNothing";

    m_showConsole = s->get("ShowConsole").toBool();
    m_autoCloseConsole = s->get("AutoCloseConsole").toBool();
    m_showConsoleOnCrash = s->get("ShowConsoleOnError").toBool();

    m_instancesFolder = s->get("InstanceDir").toString();
    m_modsFolder = s->get("CentralModsDir").toString();
    m_iconsFolder = s->get("IconsDir").toString();

    m_concurrentDownloads = s->get("NumberOfConcurrentDownloads").toInt();
    if (m_concurrentDownloads <= 0)
        m_concurrentDownloads = 6;
    m_validateDownloads = s->get("ValidateDownloads").toBool();

    // Minecraft Page
    m_showGameTime = s->get("RecordGameTime").toBool();
    m_showGlobalGameTime = s->get("ShowGlobalGameTime").toBool();
    m_enableManageModsButton = true;  // ModsButtonVisible may not exist, default true
    m_enableFeralGamemode = s->get("EnableFeralGamemode").toBool();
    m_enableDiscreteGpu = s->get("UseDiscreteGpu").toBool();
    m_enableMangoHud = s->get("EnableMangoHud").toBool();
    m_startMaximized = s->get("LaunchMaximized").toBool();
    m_windowWidth = s->get("MinecraftWinWidth").toInt();
    if (m_windowWidth <= 0)
        m_windowWidth = 854;
    m_windowHeight = s->get("MinecraftWinHeight").toInt();
    if (m_windowHeight <= 0)
        m_windowHeight = 480;
    m_showGameLog = true;  // ShowGameLog setting may not exist, default true
    m_useNativeOpenAL = s->get("UseNativeOpenAL").toBool();
    m_useNativeGLFW = s->get("UseNativeGLFW").toBool();
    m_skipMigrationCheck = false;  // Default false

    // Java Page
    m_defaultJavaPath = s->get("JavaPath").toString();
    m_defaultMinMemory = s->get("MinMemAlloc").toInt();
    m_defaultMaxMemory = s->get("MaxMemAlloc").toInt();
    m_defaultJvmArgs = s->get("JvmArgs").toString();

    // Appearance Page
    m_theme = s->get("ApplicationTheme").toString();
    if (m_theme.isEmpty())
        m_theme = "system";
    m_iconTheme = s->get("IconTheme").toString();
    if (m_iconTheme.isEmpty())
        m_iconTheme = "pe_colored";
    m_showToolbarText = !s->get("ToolbarsLocked").toBool();  // inverted: ToolbarsLocked means toolbar text not shown
    m_buttonStyle = s->get("ToolbarButtonStyle").toInt();
    if (m_buttonStyle < 0 || m_buttonStyle > 3)
        m_buttonStyle = 3;             // Default: TextUnderIcon
    m_instanceListIcons = true;        // Not stored in settings, default true
    m_showInstanceStatusLight = true;  // Not stored in settings, default true
    m_enableCat = s->get("TheCat").toBool();
    m_checkForUpdates = s->get("AutoUpdate").toBool();

    // Proxy Page
    int proxyTypeInt = s->get("ProxyType").toInt();
    switch (proxyTypeInt) {
    case 1:
        m_proxyType = "system";
        break;
    case 2:
        m_proxyType = "http";
        break;
    case 3:
        m_proxyType = "socks5";
        break;
    default:
        m_proxyType = "none";
        break;
    }
    m_proxyHost = s->get("ProxyAddr").toString();
    m_proxyPort = s->get("ProxyPort").toInt();
    m_proxyUsername = s->get("ProxyUser").toString();
    m_proxyPassword = s->get("ProxyPass").toString();

    // Language
    m_currentLanguage = s->get("Language").toString();
    if (m_currentLanguage.isEmpty())
        m_currentLanguage = "en_US";

    // API Page
    m_pastebinType = s->get("PastebinType").toInt();
    m_pastebinCustomUrl = s->get("PastebinCustomAPIBase").toString();
    m_msaClientId = s->get("MSAClientIDOverride").toString();
    m_curseforgeApiKey = s->get("FlameKeyOverride").toString();
    m_modrinthToken = s->get("ModrinthToken").toString();
    m_metaUrl = s->get("MetaURLOverride").toString();
    m_userAgentOverride = s->get("UserAgentOverride").toString();

    // External Tools Page
    m_jprofilerPath = s->get("JProfilerPath").toString();
    m_jvisualvmPath = s->get("JVisualVMPath").toString();
    m_mceditPath = s->get("MCEditPath").toString();
    m_jsonEditorPath = s->get("JsonEditor").toString();

    // Emit all signals
    emit sortByNameChanged();
    emit renamingBehaviorChanged();
    emit launchActionChanged();
    emit showConsoleChanged();
    emit autoCloseConsoleChanged();
    emit showConsoleOnCrashChanged();
    emit instancesFolderChanged();
    emit modsFolderChanged();
    emit iconsFolderChanged();
    emit concurrentDownloadsChanged();
    emit validateDownloadsChanged();
    emit showGameTimeChanged();
    emit showGlobalGameTimeChanged();
    emit enableManageModsButtonChanged();
    emit enableFeralGamemodeChanged();
    emit enableDiscreteGpuChanged();
    emit enableMangoHudChanged();
    emit startMaximizedChanged();
    emit windowWidthChanged();
    emit windowHeightChanged();
    emit showGameLogChanged();
    emit useNativeOpenALChanged();
    emit useNativeGLFWChanged();
    emit skipMigrationCheckChanged();
    emit defaultJavaPathChanged();
    emit defaultMinMemoryChanged();
    emit defaultMaxMemoryChanged();
    emit defaultJvmArgsChanged();
    emit themeChanged();
    emit iconThemeChanged();
    emit showToolbarTextChanged();
    emit instanceListIconsChanged();
    emit showInstanceStatusLightChanged();
    emit enableCatChanged();
    emit proxyTypeChanged();
    emit proxyHostChanged();
    emit proxyPortChanged();
    emit proxyUsernameChanged();
    emit proxyPasswordChanged();
    emit currentLanguageChanged();
    emit pastebinTypeChanged();
    emit pastebinCustomUrlChanged();
    emit msaClientIdChanged();
    emit curseforgeApiKeyChanged();
    emit modrinthTokenChanged();
    emit metaUrlChanged();
    emit userAgentOverrideChanged();
    emit jprofilerPathChanged();
    emit jvisualvmPathChanged();
    emit mceditPathChanged();
    emit jsonEditorPathChanged();
}

void LauncherSettingsViewModel::saveToApplication()
{
    auto s = APPLICATION->settings();

    s->set("InstSortMode", m_sortByName ? "Name" : "LastLaunch");

    // Map internal values back to setting values
    QString instRenamingMode = "AskEverytime";
    if (m_renamingBehavior == "always")
        instRenamingMode = "AlwaysRename";
    else if (m_renamingBehavior == "never")
        instRenamingMode = "NeverRename";
    s->set("InstRenamingMode", instRenamingMode);

    if (m_launchAction == "hideWindow")
        s->set("CloseAfterLaunch", true);
    else if (m_launchAction == "closeWindow")
        s->set("QuitAfterGameStop", true);
    else {
        s->set("CloseAfterLaunch", false);
        s->set("QuitAfterGameStop", false);
    }

    s->set("ShowConsole", m_showConsole);
    s->set("AutoCloseConsole", m_autoCloseConsole);
    s->set("ShowConsoleOnError", m_showConsoleOnCrash);

    s->set("InstanceDir", m_instancesFolder);
    s->set("CentralModsDir", m_modsFolder);
    s->set("IconsDir", m_iconsFolder);

    s->set("NumberOfConcurrentDownloads", m_concurrentDownloads);
    s->set("ValidateDownloads", m_validateDownloads);

    s->set("RecordGameTime", m_showGameTime);
    s->set("ShowGlobalGameTime", m_showGlobalGameTime);
    // ModsButtonVisible not in settings
    s->set("EnableFeralGamemode", m_enableFeralGamemode);
    s->set("UseDiscreteGpu", m_enableDiscreteGpu);
    s->set("EnableMangoHud", m_enableMangoHud);
    s->set("LaunchMaximized", m_startMaximized);
    s->set("MinecraftWinWidth", m_windowWidth);
    s->set("MinecraftWinHeight", m_windowHeight);
    // ShowGameLog not in settings
    s->set("UseNativeOpenAL", m_useNativeOpenAL);
    s->set("UseNativeGLFW", m_useNativeGLFW);
    // SkipMigrationCheck not in settings

    s->set("JavaPath", m_defaultJavaPath);
    s->set("MinMemAlloc", m_defaultMinMemory);
    s->set("MaxMemAlloc", m_defaultMaxMemory);
    s->set("JvmArgs", m_defaultJvmArgs);

    s->set("ApplicationTheme", m_theme);
    s->set("IconTheme", m_iconTheme);
    s->set("ToolbarsLocked", m_showToolbarText);
    s->set("ShowInstanceListIcons", m_instanceListIcons);
    s->set("ShowInstanceStatusLight", m_showInstanceStatusLight);
    s->set("TheCat", m_enableCat);

    int proxyTypeInt = 0;
    if (m_proxyType == "system")
        proxyTypeInt = 1;
    else if (m_proxyType == "http")
        proxyTypeInt = 2;
    else if (m_proxyType == "socks5")
        proxyTypeInt = 3;
    s->set("ProxyType", proxyTypeInt);
    s->set("ProxyAddr", m_proxyHost);
    s->set("ProxyPort", m_proxyPort);
    s->set("ProxyUser", m_proxyUsername);
    s->set("ProxyPass", m_proxyPassword);

    s->set("Language", m_currentLanguage);

    // API Page
    s->set("PastebinType", m_pastebinType);
    s->set("PastebinCustomAPIBase", m_pastebinCustomUrl);
    s->set("MSAClientIDOverride", m_msaClientId);
    s->set("FlameKeyOverride", m_curseforgeApiKey);
    s->set("ModrinthToken", m_modrinthToken);
    s->set("MetaURLOverride", m_metaUrl);
    s->set("UserAgentOverride", m_userAgentOverride);

    // External Tools Page
    s->set("JProfilerPath", m_jprofilerPath);
    s->set("JVisualVMPath", m_jvisualvmPath);
    s->set("MCEditPath", m_mceditPath);
    s->set("JsonEditor", m_jsonEditorPath);
}

void LauncherSettingsViewModel::applySettings()
{
    saveToApplication();
    emit settingsApplied();
}

void LauncherSettingsViewModel::resetToDefaults()
{
    m_sortByName = true;
    m_renamingBehavior = "ask";
    m_launchAction = "doNothing";
    m_showConsole = false;
    m_autoCloseConsole = true;
    m_showConsoleOnCrash = true;
    m_concurrentDownloads = 6;
    m_validateDownloads = true;

    m_showGameTime = true;
    m_showGlobalGameTime = true;
    m_enableManageModsButton = true;
    m_enableFeralGamemode = false;
    m_enableDiscreteGpu = false;
    m_enableMangoHud = false;

    m_defaultJavaPath.clear();
    m_defaultMinMemory = 512;
    m_defaultMaxMemory = 4096;
    m_defaultJvmArgs.clear();

    m_theme = "Dark";
    m_iconTheme = "Default";
    m_showToolbarText = true;
    m_instanceListIcons = true;
    m_showInstanceStatusLight = true;
    m_enableCat = false;

    m_proxyType = "none";
    m_proxyHost.clear();
    m_proxyPort = 8080;
    m_proxyUsername.clear();
    m_proxyPassword.clear();

    m_currentLanguage = "en_US";

    // Emit all changed signals
    loadFromApplication();
}

void LauncherSettingsViewModel::testJavaPath(const QString& path)
{
    if (path.isEmpty()) {
        emit javaTestResult(false, tr("No Java path specified"));
        return;
    }

    QProcess process;
    process.start(path, QStringList() << "-version");

    if (!process.waitForStarted(5000)) {
        emit javaTestResult(false, tr("Failed to start Java"));
        return;
    }

    if (!process.waitForFinished(10000)) {
        emit javaTestResult(false, tr("Java process timed out"));
        return;
    }

    QString output = QString::fromUtf8(process.readAllStandardError());
    if (output.contains("version")) {
        emit javaTestResult(true, tr("Java found: %1").arg(output.split('\n').first()));
    } else {
        emit javaTestResult(false, tr("Invalid Java installation"));
    }
}

void LauncherSettingsViewModel::autoDetectJava()
{
    // Search for Java installations in common paths
    QStringList javaPaths;

#ifdef Q_OS_WIN
    // Windows paths
    javaPaths << "C:/Program Files/Java"
              << "C:/Program Files (x86)/Java"
              << "C:/Program Files/Eclipse Adoptium"
              << "C:/Program Files/AdoptOpenJDK" << QDir::homePath() + "/.jdks";
#elif defined(Q_OS_MAC)
    // macOS paths
    javaPaths << "/Library/Java/JavaVirtualMachines"
              << "/System/Library/Frameworks/JavaVM.framework/Versions" << QDir::homePath() + "/Library/Java/JavaVirtualMachines";
#else
    // Linux paths
    javaPaths << "/usr/lib/jvm"
              << "/usr/lib64/jvm"
              << "/usr/local/lib/jvm" << QDir::homePath() + "/.jdks" << QDir::homePath() + "/.sdkman/candidates/java";
#endif

    QStringList foundJavas;
    for (const QString& basePath : javaPaths) {
        QDir dir(basePath);
        if (!dir.exists())
            continue;

        QStringList jdkDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QString& jdkDir : jdkDirs) {
            QString javaExe = basePath + "/" + jdkDir + "/bin/java";
#ifdef Q_OS_WIN
            javaExe += ".exe";
#endif
            if (QFile::exists(javaExe)) {
                foundJavas << javaExe;
            }
        }
    }

    // Also check PATH
    QString pathEnv = qgetenv("PATH");
#ifdef Q_OS_WIN
    QStringList pathDirs = pathEnv.split(';');
#else
    QStringList pathDirs = pathEnv.split(':');
#endif
    for (const QString& pathDir : pathDirs) {
        QString javaExe = pathDir + "/java";
#ifdef Q_OS_WIN
        javaExe += ".exe";
#endif
        if (QFile::exists(javaExe) && !foundJavas.contains(javaExe)) {
            foundJavas.prepend(javaExe);  // PATH java first
        }
    }

    emit javaAutoDetected(foundJavas);
}

// === Launcher Page Getters ===

bool LauncherSettingsViewModel::sortByName() const
{
    return m_sortByName;
}
QString LauncherSettingsViewModel::renamingBehavior() const
{
    return m_renamingBehavior;
}
QString LauncherSettingsViewModel::launchAction() const
{
    return m_launchAction;
}
bool LauncherSettingsViewModel::showConsole() const
{
    return m_showConsole;
}
bool LauncherSettingsViewModel::autoCloseConsole() const
{
    return m_autoCloseConsole;
}
bool LauncherSettingsViewModel::showConsoleOnCrash() const
{
    return m_showConsoleOnCrash;
}
QString LauncherSettingsViewModel::instancesFolder() const
{
    return m_instancesFolder;
}
QString LauncherSettingsViewModel::modsFolder() const
{
    return m_modsFolder;
}
QString LauncherSettingsViewModel::iconsFolder() const
{
    return m_iconsFolder;
}
int LauncherSettingsViewModel::concurrentDownloads() const
{
    return m_concurrentDownloads;
}
bool LauncherSettingsViewModel::validateDownloads() const
{
    return m_validateDownloads;
}
bool LauncherSettingsViewModel::preferMenuBar() const
{
    return m_preferMenuBar;
}
bool LauncherSettingsViewModel::autoUpdateCheck() const
{
    return m_autoUpdateCheck;
}
int LauncherSettingsViewModel::updateInterval() const
{
    return m_updateInterval;
}
QString LauncherSettingsViewModel::javaFolder() const
{
    return m_javaFolder;
}
QString LauncherSettingsViewModel::skinsFolder() const
{
    return m_skinsFolder;
}
QString LauncherSettingsViewModel::downloadsFolder() const
{
    return m_downloadsFolder;
}
bool LauncherSettingsViewModel::downloadsDirWatchRecursive() const
{
    return m_downloadsDirWatchRecursive;
}
bool LauncherSettingsViewModel::downloadsDirMove() const
{
    return m_downloadsDirMove;
}
bool LauncherSettingsViewModel::metadataEnabled() const
{
    return m_metadataEnabled;
}
bool LauncherSettingsViewModel::dependenciesEnabled() const
{
    return m_dependenciesEnabled;
}
bool LauncherSettingsViewModel::modpackUpdatePrompt() const
{
    return m_modpackUpdatePrompt;
}
int LauncherSettingsViewModel::logHistoryLimit() const
{
    return m_logHistoryLimit;
}
bool LauncherSettingsViewModel::stopLoggingOnOverflow() const
{
    return m_stopLoggingOnOverflow;
}
int LauncherSettingsViewModel::concurrentTasks() const
{
    return m_concurrentTasks;
}
int LauncherSettingsViewModel::retryLimit() const
{
    return m_retryLimit;
}
int LauncherSettingsViewModel::httpTimeout() const
{
    return m_httpTimeout;
}
bool LauncherSettingsViewModel::autoBackupBeforeLaunch() const
{
    return m_autoBackupBeforeLaunch;
}

// === Launcher Page Setters ===

void LauncherSettingsViewModel::setSortByName(bool value)
{
    if (m_sortByName != value) {
        m_sortByName = value;
        APPLICATION->settings()->set("InstSortMode", value ? "Name" : "LastLaunch");
        emit sortByNameChanged();
    }
}

void LauncherSettingsViewModel::setRenamingBehavior(const QString& value)
{
    if (m_renamingBehavior != value) {
        m_renamingBehavior = value;
        APPLICATION->settings()->set("InstanceRenamingMode", value);
        emit renamingBehaviorChanged();
    }
}

void LauncherSettingsViewModel::setLaunchAction(const QString& value)
{
    if (m_launchAction != value) {
        m_launchAction = value;
        QString settingValue = "none";
        if (value == "hideWindow")
            settingValue = "hide";
        else if (value == "closeWindow")
            settingValue = "close";
        APPLICATION->settings()->set("LaunchMaximized", settingValue);
        emit launchActionChanged();
    }
}

void LauncherSettingsViewModel::setShowConsole(bool value)
{
    if (m_showConsole != value) {
        m_showConsole = value;
        APPLICATION->settings()->set("ShowConsole", value);
        emit showConsoleChanged();
    }
}

void LauncherSettingsViewModel::setAutoCloseConsole(bool value)
{
    if (m_autoCloseConsole != value) {
        m_autoCloseConsole = value;
        APPLICATION->settings()->set("AutoCloseConsole", value);
        emit autoCloseConsoleChanged();
    }
}

void LauncherSettingsViewModel::setShowConsoleOnCrash(bool value)
{
    if (m_showConsoleOnCrash != value) {
        m_showConsoleOnCrash = value;
        APPLICATION->settings()->set("ShowConsoleOnError", value);
        emit showConsoleOnCrashChanged();
    }
}

void LauncherSettingsViewModel::setInstancesFolder(const QString& path)
{
    if (m_instancesFolder != path) {
        m_instancesFolder = path;
        APPLICATION->settings()->set("InstanceDir", path);
        emit instancesFolderChanged();
    }
}

void LauncherSettingsViewModel::setModsFolder(const QString& path)
{
    if (m_modsFolder != path) {
        m_modsFolder = path;
        APPLICATION->settings()->set("CentralModsDir", path);
        emit modsFolderChanged();
    }
}

void LauncherSettingsViewModel::setIconsFolder(const QString& path)
{
    if (m_iconsFolder != path) {
        m_iconsFolder = path;
        APPLICATION->settings()->set("IconsDir", path);
        emit iconsFolderChanged();
    }
}

void LauncherSettingsViewModel::setConcurrentDownloads(int value)
{
    if (m_concurrentDownloads != value) {
        m_concurrentDownloads = value;
        APPLICATION->settings()->set("NumberOfConcurrentDownloads", value);
        emit concurrentDownloadsChanged();
    }
}

void LauncherSettingsViewModel::setValidateDownloads(bool value)
{
    if (m_validateDownloads != value) {
        m_validateDownloads = value;
        APPLICATION->settings()->set("ValidateDownloads", value);
        emit validateDownloadsChanged();
    }
}

void LauncherSettingsViewModel::setPreferMenuBar(bool value)
{
    if (m_preferMenuBar != value) {
        m_preferMenuBar = value;
        APPLICATION->settings()->set("PreferMenuBar", value);
        emit preferMenuBarChanged();
    }
}

void LauncherSettingsViewModel::setAutoUpdateCheck(bool value)
{
    if (m_autoUpdateCheck != value) {
        m_autoUpdateCheck = value;
        APPLICATION->settings()->set("AutoUpdateCheck", value);
        emit autoUpdateCheckChanged();
    }
}

void LauncherSettingsViewModel::setUpdateInterval(int value)
{
    if (m_updateInterval != value) {
        m_updateInterval = value;
        APPLICATION->settings()->set("UpdateInterval", value);
        emit updateIntervalChanged();
    }
}

void LauncherSettingsViewModel::setJavaFolder(const QString& path)
{
    if (m_javaFolder != path) {
        m_javaFolder = path;
        APPLICATION->settings()->set("JavaDir", path);
        emit javaFolderChanged();
    }
}

void LauncherSettingsViewModel::setSkinsFolder(const QString& path)
{
    if (m_skinsFolder != path) {
        m_skinsFolder = path;
        APPLICATION->settings()->set("SkinsDir", path);
        emit skinsFolderChanged();
    }
}

void LauncherSettingsViewModel::setDownloadsFolder(const QString& path)
{
    if (m_downloadsFolder != path) {
        m_downloadsFolder = path;
        APPLICATION->settings()->set("DownloadsDir", path);
        emit downloadsFolderChanged();
    }
}

void LauncherSettingsViewModel::setDownloadsDirWatchRecursive(bool value)
{
    if (m_downloadsDirWatchRecursive != value) {
        m_downloadsDirWatchRecursive = value;
        APPLICATION->settings()->set("DownloadsDirWatchRecursive", value);
        emit downloadsDirWatchRecursiveChanged();
    }
}

void LauncherSettingsViewModel::setDownloadsDirMove(bool value)
{
    if (m_downloadsDirMove != value) {
        m_downloadsDirMove = value;
        APPLICATION->settings()->set("DownloadsDirMove", value);
        emit downloadsDirMoveChanged();
    }
}

void LauncherSettingsViewModel::setMetadataEnabled(bool value)
{
    if (m_metadataEnabled != value) {
        m_metadataEnabled = value;
        APPLICATION->settings()->set("MetadataEnabled", value);
        emit metadataEnabledChanged();
    }
}

void LauncherSettingsViewModel::setDependenciesEnabled(bool value)
{
    if (m_dependenciesEnabled != value) {
        m_dependenciesEnabled = value;
        APPLICATION->settings()->set("DependenciesEnabled", value);
        emit dependenciesEnabledChanged();
    }
}

void LauncherSettingsViewModel::setModpackUpdatePrompt(bool value)
{
    if (m_modpackUpdatePrompt != value) {
        m_modpackUpdatePrompt = value;
        APPLICATION->settings()->set("ModpackUpdatePrompt", value);
        emit modpackUpdatePromptChanged();
    }
}

void LauncherSettingsViewModel::setLogHistoryLimit(int value)
{
    if (m_logHistoryLimit != value) {
        m_logHistoryLimit = value;
        APPLICATION->settings()->set("LogHistoryLimit", value);
        emit logHistoryLimitChanged();
    }
}

void LauncherSettingsViewModel::setStopLoggingOnOverflow(bool value)
{
    if (m_stopLoggingOnOverflow != value) {
        m_stopLoggingOnOverflow = value;
        APPLICATION->settings()->set("StopLoggingOnOverflow", value);
        emit stopLoggingOnOverflowChanged();
    }
}

void LauncherSettingsViewModel::setConcurrentTasks(int value)
{
    if (m_concurrentTasks != value) {
        m_concurrentTasks = value;
        APPLICATION->settings()->set("ConcurrentTasks", value);
        emit concurrentTasksChanged();
    }
}

void LauncherSettingsViewModel::setRetryLimit(int value)
{
    if (m_retryLimit != value) {
        m_retryLimit = value;
        APPLICATION->settings()->set("RetryLimit", value);
        emit retryLimitChanged();
    }
}

void LauncherSettingsViewModel::setHttpTimeout(int value)
{
    if (m_httpTimeout != value) {
        m_httpTimeout = value;
        APPLICATION->settings()->set("HttpTimeout", value);
        emit httpTimeoutChanged();
    }
}

void LauncherSettingsViewModel::setAutoBackupBeforeLaunch(bool value)
{
    if (m_autoBackupBeforeLaunch != value) {
        m_autoBackupBeforeLaunch = value;
        APPLICATION->settings()->set("AutoBackupBeforeLaunch", value);
        emit autoBackupBeforeLaunchChanged();
    }
}

// === Minecraft Page Getters ===

bool LauncherSettingsViewModel::showGameTime() const
{
    return m_showGameTime;
}
bool LauncherSettingsViewModel::showGlobalGameTime() const
{
    return m_showGlobalGameTime;
}
bool LauncherSettingsViewModel::enableManageModsButton() const
{
    return m_enableManageModsButton;
}
bool LauncherSettingsViewModel::enableFeralGamemode() const
{
    return m_enableFeralGamemode;
}
bool LauncherSettingsViewModel::enableDiscreteGpu() const
{
    return m_enableDiscreteGpu;
}
bool LauncherSettingsViewModel::enableMangoHud() const
{
    return m_enableMangoHud;
}
bool LauncherSettingsViewModel::startMaximized() const
{
    return m_startMaximized;
}
int LauncherSettingsViewModel::windowWidth() const
{
    return m_windowWidth;
}
int LauncherSettingsViewModel::windowHeight() const
{
    return m_windowHeight;
}
bool LauncherSettingsViewModel::showGameLog() const
{
    return m_showGameLog;
}
bool LauncherSettingsViewModel::useNativeOpenAL() const
{
    return m_useNativeOpenAL;
}
bool LauncherSettingsViewModel::useNativeGLFW() const
{
    return m_useNativeGLFW;
}
bool LauncherSettingsViewModel::skipMigrationCheck() const
{
    return m_skipMigrationCheck;
}

// === Minecraft Page Setters ===

void LauncherSettingsViewModel::setShowGameTime(bool value)
{
    if (m_showGameTime != value) {
        m_showGameTime = value;
        APPLICATION->settings()->set("RecordGameTime", value);
        emit showGameTimeChanged();
    }
}

void LauncherSettingsViewModel::setShowGlobalGameTime(bool value)
{
    if (m_showGlobalGameTime != value) {
        m_showGlobalGameTime = value;
        APPLICATION->settings()->set("ShowGlobalGameTime", value);
        emit showGlobalGameTimeChanged();
    }
}

void LauncherSettingsViewModel::setEnableManageModsButton(bool value)
{
    if (m_enableManageModsButton != value) {
        m_enableManageModsButton = value;
        APPLICATION->settings()->set("ModsButtonVisible", value);
        emit enableManageModsButtonChanged();
    }
}

void LauncherSettingsViewModel::setEnableFeralGamemode(bool value)
{
    if (m_enableFeralGamemode != value) {
        m_enableFeralGamemode = value;
        APPLICATION->settings()->set("EnableFeralGamemode", value);
        emit enableFeralGamemodeChanged();
    }
}

void LauncherSettingsViewModel::setEnableDiscreteGpu(bool value)
{
    if (m_enableDiscreteGpu != value) {
        m_enableDiscreteGpu = value;
        APPLICATION->settings()->set("UseDiscreteGpu", value);
        emit enableDiscreteGpuChanged();
    }
}

void LauncherSettingsViewModel::setEnableMangoHud(bool value)
{
    if (m_enableMangoHud != value) {
        m_enableMangoHud = value;
        APPLICATION->settings()->set("EnableMangoHud", value);
        emit enableMangoHudChanged();
    }
}

void LauncherSettingsViewModel::setStartMaximized(bool value)
{
    if (m_startMaximized != value) {
        m_startMaximized = value;
        APPLICATION->settings()->set("LaunchMaximized", value);
        emit startMaximizedChanged();
    }
}

void LauncherSettingsViewModel::setWindowWidth(int value)
{
    if (m_windowWidth != value) {
        m_windowWidth = value;
        APPLICATION->settings()->set("MinecraftWinWidth", value);
        emit windowWidthChanged();
    }
}

void LauncherSettingsViewModel::setWindowHeight(int value)
{
    if (m_windowHeight != value) {
        m_windowHeight = value;
        APPLICATION->settings()->set("MinecraftWinHeight", value);
        emit windowHeightChanged();
    }
}

void LauncherSettingsViewModel::setShowGameLog(bool value)
{
    if (m_showGameLog != value) {
        m_showGameLog = value;
        APPLICATION->settings()->set("ShowGameLog", value);
        emit showGameLogChanged();
    }
}

void LauncherSettingsViewModel::setUseNativeOpenAL(bool value)
{
    if (m_useNativeOpenAL != value) {
        m_useNativeOpenAL = value;
        APPLICATION->settings()->set("UseNativeOpenAL", value);
        emit useNativeOpenALChanged();
    }
}

void LauncherSettingsViewModel::setUseNativeGLFW(bool value)
{
    if (m_useNativeGLFW != value) {
        m_useNativeGLFW = value;
        APPLICATION->settings()->set("UseNativeGLFW", value);
        emit useNativeGLFWChanged();
    }
}

void LauncherSettingsViewModel::setSkipMigrationCheck(bool value)
{
    if (m_skipMigrationCheck != value) {
        m_skipMigrationCheck = value;
        APPLICATION->settings()->set("SkipMigrationCheck", value);
        emit skipMigrationCheckChanged();
    }
}

// === Java Page Getters ===

QString LauncherSettingsViewModel::defaultJavaPath() const
{
    return m_defaultJavaPath;
}
int LauncherSettingsViewModel::defaultMinMemory() const
{
    return m_defaultMinMemory;
}
int LauncherSettingsViewModel::defaultMaxMemory() const
{
    return m_defaultMaxMemory;
}
QString LauncherSettingsViewModel::defaultJvmArgs() const
{
    return m_defaultJvmArgs;
}

// === Java Page Setters ===

void LauncherSettingsViewModel::setDefaultJavaPath(const QString& path)
{
    if (m_defaultJavaPath != path) {
        m_defaultJavaPath = path;
        APPLICATION->settings()->set("JavaPath", path);
        emit defaultJavaPathChanged();
    }
}

void LauncherSettingsViewModel::setDefaultMinMemory(int value)
{
    if (m_defaultMinMemory != value) {
        m_defaultMinMemory = value;
        APPLICATION->settings()->set("MinMemAlloc", value);
        emit defaultMinMemoryChanged();
    }
}

void LauncherSettingsViewModel::setDefaultMaxMemory(int value)
{
    if (m_defaultMaxMemory != value) {
        m_defaultMaxMemory = value;
        APPLICATION->settings()->set("MaxMemAlloc", value);
        emit defaultMaxMemoryChanged();
    }
}

void LauncherSettingsViewModel::setDefaultJvmArgs(const QString& args)
{
    if (m_defaultJvmArgs != args) {
        m_defaultJvmArgs = args;
        APPLICATION->settings()->set("JvmArgs", args);
        emit defaultJvmArgsChanged();
    }
}

// === Appearance Page Getters ===

QString LauncherSettingsViewModel::theme() const
{
    return m_theme;
}
QString LauncherSettingsViewModel::iconTheme() const
{
    return m_iconTheme;
}
bool LauncherSettingsViewModel::showToolbarText() const
{
    return m_showToolbarText;
}
bool LauncherSettingsViewModel::instanceListIcons() const
{
    return m_instanceListIcons;
}
bool LauncherSettingsViewModel::showInstanceStatusLight() const
{
    return m_showInstanceStatusLight;
}
bool LauncherSettingsViewModel::enableCat() const
{
    return m_enableCat;
}

// === Appearance Page Setters ===

void LauncherSettingsViewModel::setTheme(const QString& theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        APPLICATION->settings()->set("ApplicationTheme", theme);
        emit themeChanged();
    }
}

void LauncherSettingsViewModel::setIconTheme(const QString& theme)
{
    if (m_iconTheme != theme) {
        m_iconTheme = theme;
        APPLICATION->settings()->set("IconTheme", theme);
        emit iconThemeChanged();
    }
}

void LauncherSettingsViewModel::setShowToolbarText(bool value)
{
    if (m_showToolbarText != value) {
        m_showToolbarText = value;
        APPLICATION->settings()->set("ToolbarsLocked", value);
        emit showToolbarTextChanged();
    }
}

void LauncherSettingsViewModel::setInstanceListIcons(bool value)
{
    if (m_instanceListIcons != value) {
        m_instanceListIcons = value;
        APPLICATION->settings()->set("ShowInstanceListIcons", value);
        emit instanceListIconsChanged();
    }
}

void LauncherSettingsViewModel::setShowInstanceStatusLight(bool value)
{
    if (m_showInstanceStatusLight != value) {
        m_showInstanceStatusLight = value;
        APPLICATION->settings()->set("ShowInstanceStatusLight", value);
        emit showInstanceStatusLightChanged();
    }
}

void LauncherSettingsViewModel::setEnableCat(bool value)
{
    if (m_enableCat != value) {
        m_enableCat = value;
        APPLICATION->settings()->set("TheCat", value);
        emit enableCatChanged();
    }
}

// === Button Style and Updates ===

int LauncherSettingsViewModel::buttonStyle() const
{
    return m_buttonStyle;
}

void LauncherSettingsViewModel::setButtonStyle(int value)
{
    if (m_buttonStyle != value) {
        m_buttonStyle = value;
        APPLICATION->settings()->set("ToolbarButtonStyle", value);
        emit buttonStyleChanged();
    }
}

bool LauncherSettingsViewModel::checkForUpdates() const
{
    return m_checkForUpdates;
}

void LauncherSettingsViewModel::setCheckForUpdates(bool value)
{
    if (m_checkForUpdates != value) {
        m_checkForUpdates = value;
        APPLICATION->settings()->set("AutoUpdate", value);
        emit checkForUpdatesChanged();
    }
}

// === Proxy Page Getters ===

QString LauncherSettingsViewModel::proxyType() const
{
    return m_proxyType;
}
QString LauncherSettingsViewModel::proxyHost() const
{
    return m_proxyHost;
}
int LauncherSettingsViewModel::proxyPort() const
{
    return m_proxyPort;
}
QString LauncherSettingsViewModel::proxyUsername() const
{
    return m_proxyUsername;
}
QString LauncherSettingsViewModel::proxyPassword() const
{
    return m_proxyPassword;
}

// === Proxy Page Setters ===

void LauncherSettingsViewModel::setProxyType(const QString& type)
{
    if (m_proxyType != type) {
        m_proxyType = type;
        int proxyTypeInt = 0;
        if (type == "system")
            proxyTypeInt = 1;
        else if (type == "http")
            proxyTypeInt = 2;
        else if (type == "socks5")
            proxyTypeInt = 3;
        APPLICATION->settings()->set("ProxyType", proxyTypeInt);
        emit proxyTypeChanged();
    }
}

void LauncherSettingsViewModel::setProxyHost(const QString& host)
{
    if (m_proxyHost != host) {
        m_proxyHost = host;
        APPLICATION->settings()->set("ProxyAddr", host);
        emit proxyHostChanged();
    }
}

void LauncherSettingsViewModel::setProxyPort(int port)
{
    if (m_proxyPort != port) {
        m_proxyPort = port;
        APPLICATION->settings()->set("ProxyPort", port);
        emit proxyPortChanged();
    }
}

void LauncherSettingsViewModel::setProxyUsername(const QString& user)
{
    if (m_proxyUsername != user) {
        m_proxyUsername = user;
        APPLICATION->settings()->set("ProxyUser", user);
        emit proxyUsernameChanged();
    }
}

void LauncherSettingsViewModel::setProxyPassword(const QString& password)
{
    if (m_proxyPassword != password) {
        m_proxyPassword = password;
        APPLICATION->settings()->set("ProxyPass", password);
        emit proxyPasswordChanged();
    }
}

// === Language ===

QString LauncherSettingsViewModel::currentLanguage() const
{
    return m_currentLanguage;
}

QStringList LauncherSettingsViewModel::availableLanguages() const
{
    // Return available language codes
    return QStringList() << "en_US"
                         << "tr_TR"
                         << "de_DE"
                         << "fr_FR"
                         << "es_ES"
                         << "pt_BR"
                         << "ru_RU"
                         << "zh_CN"
                         << "zh_TW"
                         << "ja_JP"
                         << "ko_KR"
                         << "it_IT"
                         << "pl_PL"
                         << "nl_NL"
                         << "uk_UA";
}

void LauncherSettingsViewModel::setCurrentLanguage(const QString& lang)
{
    if (m_currentLanguage != lang) {
        m_currentLanguage = lang;
        APPLICATION->settings()->set("Language", lang);
        emit currentLanguageChanged();
    }
}

// === API Page Getters ===

int LauncherSettingsViewModel::pastebinType() const
{
    return m_pastebinType;
}
QString LauncherSettingsViewModel::pastebinCustomUrl() const
{
    return m_pastebinCustomUrl;
}
QString LauncherSettingsViewModel::msaClientId() const
{
    return m_msaClientId;
}
QString LauncherSettingsViewModel::curseforgeApiKey() const
{
    return m_curseforgeApiKey;
}
QString LauncherSettingsViewModel::modrinthToken() const
{
    return m_modrinthToken;
}
QString LauncherSettingsViewModel::metaUrl() const
{
    return m_metaUrl;
}
QString LauncherSettingsViewModel::userAgentOverride() const
{
    return m_userAgentOverride;
}
QStringList LauncherSettingsViewModel::pasteServiceTypes() const
{
    return QStringList() << tr("0xd9 (mclo.gs)") << tr("paste.ee") << tr("hastebin") << tr("Custom");
}
int LauncherSettingsViewModel::pasteServiceType() const
{
    return m_pasteServiceType;
}
QString LauncherSettingsViewModel::pasteBaseUrl() const
{
    return m_pasteBaseUrl;
}
QString LauncherSettingsViewModel::resourceUrl() const
{
    return m_resourceUrl;
}
QString LauncherSettingsViewModel::userAgent() const
{
    return m_userAgent;
}
QString LauncherSettingsViewModel::technicClientId() const
{
    return m_technicClientId;
}

// === API Page Setters ===

void LauncherSettingsViewModel::setPastebinType(int type)
{
    if (m_pastebinType != type) {
        m_pastebinType = type;
        APPLICATION->settings()->set("PastebinType", type);
        emit pastebinTypeChanged();
    }
}

void LauncherSettingsViewModel::setPastebinCustomUrl(const QString& url)
{
    if (m_pastebinCustomUrl != url) {
        m_pastebinCustomUrl = url;
        APPLICATION->settings()->set("PastebinCustomAPIBase", url);
        emit pastebinCustomUrlChanged();
    }
}

void LauncherSettingsViewModel::setMsaClientId(const QString& id)
{
    if (m_msaClientId != id) {
        m_msaClientId = id;
        APPLICATION->settings()->set("MSAClientIDOverride", id);
        emit msaClientIdChanged();
    }
}

void LauncherSettingsViewModel::setCurseforgeApiKey(const QString& key)
{
    if (m_curseforgeApiKey != key) {
        m_curseforgeApiKey = key;
        APPLICATION->settings()->set("FlameKeyOverride", key);
        emit curseforgeApiKeyChanged();
    }
}

void LauncherSettingsViewModel::setModrinthToken(const QString& token)
{
    if (m_modrinthToken != token) {
        m_modrinthToken = token;
        APPLICATION->settings()->set("ModrinthToken", token);
        emit modrinthTokenChanged();
    }
}

void LauncherSettingsViewModel::setMetaUrl(const QString& url)
{
    if (m_metaUrl != url) {
        m_metaUrl = url;
        APPLICATION->settings()->set("MetaURLOverride", url);
        emit metaUrlChanged();
    }
}

void LauncherSettingsViewModel::setUserAgentOverride(const QString& ua)
{
    if (m_userAgentOverride != ua) {
        m_userAgentOverride = ua;
        APPLICATION->settings()->set("UserAgentOverride", ua);
        emit userAgentOverrideChanged();
    }
}

void LauncherSettingsViewModel::setPasteServiceType(int type)
{
    if (m_pasteServiceType != type) {
        m_pasteServiceType = type;
        APPLICATION->settings()->set("PasteServiceType", type);
        emit pasteServiceTypeChanged();
    }
}

void LauncherSettingsViewModel::setPasteBaseUrl(const QString& url)
{
    if (m_pasteBaseUrl != url) {
        m_pasteBaseUrl = url;
        APPLICATION->settings()->set("PasteBaseUrl", url);
        emit pasteBaseUrlChanged();
    }
}

void LauncherSettingsViewModel::setResourceUrl(const QString& url)
{
    if (m_resourceUrl != url) {
        m_resourceUrl = url;
        APPLICATION->settings()->set("ResourceUrl", url);
        emit resourceUrlChanged();
    }
}

void LauncherSettingsViewModel::setUserAgent(const QString& ua)
{
    if (m_userAgent != ua) {
        m_userAgent = ua;
        APPLICATION->settings()->set("UserAgent", ua);
        emit userAgentChanged();
    }
}

void LauncherSettingsViewModel::setTechnicClientId(const QString& id)
{
    if (m_technicClientId != id) {
        m_technicClientId = id;
        APPLICATION->settings()->set("TechnicClientId", id);
        emit technicClientIdChanged();
    }
}

// === External Tools Page Getters ===

QString LauncherSettingsViewModel::jprofilerPath() const
{
    return m_jprofilerPath;
}
QString LauncherSettingsViewModel::jvisualvmPath() const
{
    return m_jvisualvmPath;
}
QString LauncherSettingsViewModel::mceditPath() const
{
    return m_mceditPath;
}
QString LauncherSettingsViewModel::jsonEditorPath() const
{
    return m_jsonEditorPath;
}

// === External Tools Page Setters ===

void LauncherSettingsViewModel::setJprofilerPath(const QString& path)
{
    if (m_jprofilerPath != path) {
        m_jprofilerPath = path;
        APPLICATION->settings()->set("JProfilerPath", path);
        emit jprofilerPathChanged();
    }
}

void LauncherSettingsViewModel::setJvisualvmPath(const QString& path)
{
    if (m_jvisualvmPath != path) {
        m_jvisualvmPath = path;
        APPLICATION->settings()->set("JVisualVMPath", path);
        emit jvisualvmPathChanged();
    }
}

void LauncherSettingsViewModel::setMceditPath(const QString& path)
{
    if (m_mceditPath != path) {
        m_mceditPath = path;
        APPLICATION->settings()->set("MCEditPath", path);
        emit mceditPathChanged();
    }
}

void LauncherSettingsViewModel::setJsonEditorPath(const QString& path)
{
    if (m_jsonEditorPath != path) {
        m_jsonEditorPath = path;
        APPLICATION->settings()->set("JsonEditor", path);
        emit jsonEditorPathChanged();
    }
}
