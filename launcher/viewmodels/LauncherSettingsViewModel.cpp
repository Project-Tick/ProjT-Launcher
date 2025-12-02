// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  Global launcher settings ViewModel implementation
 */

#include "LauncherSettingsViewModel.h"

#include <QFileDialog>
#include <QProcess>
#include <QStandardPaths>

#include "Application.h"
#include "settings/SettingsObject.h"

LauncherSettingsViewModel::LauncherSettingsViewModel(QObject* parent)
    : QObject(parent)
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
    m_renamingBehavior = s->get("InstanceRenamingMode").toString();
    if (m_renamingBehavior.isEmpty()) m_renamingBehavior = "ask";
    
    QString launchBehavior = s->get("LaunchMaximized").toString();
    if (launchBehavior == "hide") m_launchAction = "hideWindow";
    else if (launchBehavior == "close") m_launchAction = "closeWindow";
    else m_launchAction = "doNothing";
    
    m_showConsole = s->get("ShowConsole").toBool();
    m_autoCloseConsole = s->get("AutoCloseConsole").toBool();
    m_showConsoleOnCrash = s->get("ShowConsoleOnError").toBool();
    
    m_instancesFolder = s->get("InstanceDir").toString();
    m_modsFolder = s->get("CentralModsDir").toString();
    m_iconsFolder = s->get("IconsDir").toString();
    
    m_concurrentDownloads = s->get("NumberOfConcurrentDownloads").toInt();
    if (m_concurrentDownloads <= 0) m_concurrentDownloads = 6;
    m_validateDownloads = s->get("ValidateDownloads").toBool();
    
    // Minecraft Page
    m_showGameTime = s->get("ShowGameTime").toBool();
    m_showGlobalGameTime = s->get("ShowGlobalGameTime").toBool();
    m_enableManageModsButton = s->get("ModsButtonVisible").toBool();
    m_enableFeralGamemode = s->get("EnableFeralGamemode").toBool();
    m_enableDiscreteGpu = s->get("UseDiscreteGpu").toBool();
    m_enableMangoHud = s->get("UseMangoHud").toBool();
    
    // Java Page
    m_defaultJavaPath = s->get("JavaPath").toString();
    m_defaultMinMemory = s->get("MinMemAlloc").toInt();
    m_defaultMaxMemory = s->get("MaxMemAlloc").toInt();
    m_defaultJvmArgs = s->get("JvmArgs").toString();
    
    // Appearance Page
    m_theme = s->get("ApplicationTheme").toString();
    if (m_theme.isEmpty()) m_theme = "Dark";
    m_iconTheme = s->get("IconTheme").toString();
    if (m_iconTheme.isEmpty()) m_iconTheme = "Default";
    m_showToolbarText = s->get("ToolbarsLocked").toBool();
    m_instanceListIcons = s->get("ShowInstanceListIcons").toBool();
    m_showInstanceStatusLight = s->get("ShowInstanceStatusLight").toBool();
    m_enableCat = s->get("TheCat").toBool();
    
    // Proxy Page
    int proxyTypeInt = s->get("ProxyType").toInt();
    switch (proxyTypeInt) {
        case 1: m_proxyType = "system"; break;
        case 2: m_proxyType = "http"; break;
        case 3: m_proxyType = "socks5"; break;
        default: m_proxyType = "none"; break;
    }
    m_proxyHost = s->get("ProxyAddr").toString();
    m_proxyPort = s->get("ProxyPort").toInt();
    m_proxyUsername = s->get("ProxyUser").toString();
    m_proxyPassword = s->get("ProxyPass").toString();
    
    // Language
    m_currentLanguage = s->get("Language").toString();
    if (m_currentLanguage.isEmpty()) m_currentLanguage = "en_US";
    
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
}

void LauncherSettingsViewModel::saveToApplication()
{
    auto s = APPLICATION->settings();
    
    s->set("InstSortMode", m_sortByName ? "Name" : "LastLaunch");
    s->set("InstanceRenamingMode", m_renamingBehavior);
    
    if (m_launchAction == "hideWindow") s->set("LaunchMaximized", "hide");
    else if (m_launchAction == "closeWindow") s->set("LaunchMaximized", "close");
    else s->set("LaunchMaximized", "none");
    
    s->set("ShowConsole", m_showConsole);
    s->set("AutoCloseConsole", m_autoCloseConsole);
    s->set("ShowConsoleOnError", m_showConsoleOnCrash);
    
    s->set("InstanceDir", m_instancesFolder);
    s->set("CentralModsDir", m_modsFolder);
    s->set("IconsDir", m_iconsFolder);
    
    s->set("NumberOfConcurrentDownloads", m_concurrentDownloads);
    s->set("ValidateDownloads", m_validateDownloads);
    
    s->set("ShowGameTime", m_showGameTime);
    s->set("ShowGlobalGameTime", m_showGlobalGameTime);
    s->set("ModsButtonVisible", m_enableManageModsButton);
    s->set("EnableFeralGamemode", m_enableFeralGamemode);
    s->set("UseDiscreteGpu", m_enableDiscreteGpu);
    s->set("UseMangoHud", m_enableMangoHud);
    
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
    if (m_proxyType == "system") proxyTypeInt = 1;
    else if (m_proxyType == "http") proxyTypeInt = 2;
    else if (m_proxyType == "socks5") proxyTypeInt = 3;
    s->set("ProxyType", proxyTypeInt);
    s->set("ProxyAddr", m_proxyHost);
    s->set("ProxyPort", m_proxyPort);
    s->set("ProxyUser", m_proxyUsername);
    s->set("ProxyPass", m_proxyPassword);
    
    s->set("Language", m_currentLanguage);
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

// === Launcher Page Getters ===

bool LauncherSettingsViewModel::sortByName() const { return m_sortByName; }
QString LauncherSettingsViewModel::renamingBehavior() const { return m_renamingBehavior; }
QString LauncherSettingsViewModel::launchAction() const { return m_launchAction; }
bool LauncherSettingsViewModel::showConsole() const { return m_showConsole; }
bool LauncherSettingsViewModel::autoCloseConsole() const { return m_autoCloseConsole; }
bool LauncherSettingsViewModel::showConsoleOnCrash() const { return m_showConsoleOnCrash; }
QString LauncherSettingsViewModel::instancesFolder() const { return m_instancesFolder; }
QString LauncherSettingsViewModel::modsFolder() const { return m_modsFolder; }
QString LauncherSettingsViewModel::iconsFolder() const { return m_iconsFolder; }
int LauncherSettingsViewModel::concurrentDownloads() const { return m_concurrentDownloads; }
bool LauncherSettingsViewModel::validateDownloads() const { return m_validateDownloads; }

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
        if (value == "hideWindow") settingValue = "hide";
        else if (value == "closeWindow") settingValue = "close";
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

// === Minecraft Page Getters ===

bool LauncherSettingsViewModel::showGameTime() const { return m_showGameTime; }
bool LauncherSettingsViewModel::showGlobalGameTime() const { return m_showGlobalGameTime; }
bool LauncherSettingsViewModel::enableManageModsButton() const { return m_enableManageModsButton; }
bool LauncherSettingsViewModel::enableFeralGamemode() const { return m_enableFeralGamemode; }
bool LauncherSettingsViewModel::enableDiscreteGpu() const { return m_enableDiscreteGpu; }
bool LauncherSettingsViewModel::enableMangoHud() const { return m_enableMangoHud; }

// === Minecraft Page Setters ===

void LauncherSettingsViewModel::setShowGameTime(bool value)
{
    if (m_showGameTime != value) {
        m_showGameTime = value;
        APPLICATION->settings()->set("ShowGameTime", value);
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
        APPLICATION->settings()->set("UseMangoHud", value);
        emit enableMangoHudChanged();
    }
}

// === Java Page Getters ===

QString LauncherSettingsViewModel::defaultJavaPath() const { return m_defaultJavaPath; }
int LauncherSettingsViewModel::defaultMinMemory() const { return m_defaultMinMemory; }
int LauncherSettingsViewModel::defaultMaxMemory() const { return m_defaultMaxMemory; }
QString LauncherSettingsViewModel::defaultJvmArgs() const { return m_defaultJvmArgs; }

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

QString LauncherSettingsViewModel::theme() const { return m_theme; }
QString LauncherSettingsViewModel::iconTheme() const { return m_iconTheme; }
bool LauncherSettingsViewModel::showToolbarText() const { return m_showToolbarText; }
bool LauncherSettingsViewModel::instanceListIcons() const { return m_instanceListIcons; }
bool LauncherSettingsViewModel::showInstanceStatusLight() const { return m_showInstanceStatusLight; }
bool LauncherSettingsViewModel::enableCat() const { return m_enableCat; }

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

// === Proxy Page Getters ===

QString LauncherSettingsViewModel::proxyType() const { return m_proxyType; }
QString LauncherSettingsViewModel::proxyHost() const { return m_proxyHost; }
int LauncherSettingsViewModel::proxyPort() const { return m_proxyPort; }
QString LauncherSettingsViewModel::proxyUsername() const { return m_proxyUsername; }
QString LauncherSettingsViewModel::proxyPassword() const { return m_proxyPassword; }

// === Proxy Page Setters ===

void LauncherSettingsViewModel::setProxyType(const QString& type)
{
    if (m_proxyType != type) {
        m_proxyType = type;
        int proxyTypeInt = 0;
        if (type == "system") proxyTypeInt = 1;
        else if (type == "http") proxyTypeInt = 2;
        else if (type == "socks5") proxyTypeInt = 3;
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

QString LauncherSettingsViewModel::currentLanguage() const { return m_currentLanguage; }

QStringList LauncherSettingsViewModel::availableLanguages() const
{
    // Return available language codes
    return QStringList() << "en_US" << "tr_TR" << "de_DE" << "fr_FR" << "es_ES"
                         << "pt_BR" << "ru_RU" << "zh_CN" << "zh_TW" << "ja_JP"
                         << "ko_KR" << "it_IT" << "pl_PL" << "nl_NL" << "uk_UA";
}

void LauncherSettingsViewModel::setCurrentLanguage(const QString& lang)
{
    if (m_currentLanguage != lang) {
        m_currentLanguage = lang;
        APPLICATION->settings()->set("Language", lang);
        emit currentLanguageChanged();
    }
}
