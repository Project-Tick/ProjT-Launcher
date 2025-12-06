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
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 */

#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class LauncherSettingsViewModel : public QObject {
    Q_OBJECT

    // === Launcher Page ===
    Q_PROPERTY(bool sortByName READ sortByName WRITE setSortByName NOTIFY sortByNameChanged)
    Q_PROPERTY(QString renamingBehavior READ renamingBehavior WRITE setRenamingBehavior NOTIFY renamingBehaviorChanged)
    Q_PROPERTY(QString launchAction READ launchAction WRITE setLaunchAction NOTIFY launchActionChanged)
    Q_PROPERTY(bool showConsole READ showConsole WRITE setShowConsole NOTIFY showConsoleChanged)
    Q_PROPERTY(bool autoCloseConsole READ autoCloseConsole WRITE setAutoCloseConsole NOTIFY autoCloseConsoleChanged)
    Q_PROPERTY(bool showConsoleOnCrash READ showConsoleOnCrash WRITE setShowConsoleOnCrash NOTIFY showConsoleOnCrashChanged)
    Q_PROPERTY(QString instancesFolder READ instancesFolder WRITE setInstancesFolder NOTIFY instancesFolderChanged)
    Q_PROPERTY(QString modsFolder READ modsFolder WRITE setModsFolder NOTIFY modsFolderChanged)
    Q_PROPERTY(QString iconsFolder READ iconsFolder WRITE setIconsFolder NOTIFY iconsFolderChanged)
    Q_PROPERTY(int concurrentDownloads READ concurrentDownloads WRITE setConcurrentDownloads NOTIFY concurrentDownloadsChanged)
    Q_PROPERTY(bool validateDownloads READ validateDownloads WRITE setValidateDownloads NOTIFY validateDownloadsChanged)

    // === Minecraft Page ===
    Q_PROPERTY(bool showGameTime READ showGameTime WRITE setShowGameTime NOTIFY showGameTimeChanged)
    Q_PROPERTY(bool showGlobalGameTime READ showGlobalGameTime WRITE setShowGlobalGameTime NOTIFY showGlobalGameTimeChanged)
    Q_PROPERTY(bool enableManageModsButton READ enableManageModsButton WRITE setEnableManageModsButton NOTIFY enableManageModsButtonChanged)
    Q_PROPERTY(bool enableFeralGamemode READ enableFeralGamemode WRITE setEnableFeralGamemode NOTIFY enableFeralGamemodeChanged)
    Q_PROPERTY(bool enableDiscreteGpu READ enableDiscreteGpu WRITE setEnableDiscreteGpu NOTIFY enableDiscreteGpuChanged)
    Q_PROPERTY(bool enableMangoHud READ enableMangoHud WRITE setEnableMangoHud NOTIFY enableMangoHudChanged)
    Q_PROPERTY(bool startMaximized READ startMaximized WRITE setStartMaximized NOTIFY startMaximizedChanged)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)
    Q_PROPERTY(int windowHeight READ windowHeight WRITE setWindowHeight NOTIFY windowHeightChanged)
    Q_PROPERTY(bool showGameLog READ showGameLog WRITE setShowGameLog NOTIFY showGameLogChanged)
    Q_PROPERTY(bool useNativeOpenAL READ useNativeOpenAL WRITE setUseNativeOpenAL NOTIFY useNativeOpenALChanged)
    Q_PROPERTY(bool useNativeGLFW READ useNativeGLFW WRITE setUseNativeGLFW NOTIFY useNativeGLFWChanged)
    Q_PROPERTY(bool skipMigrationCheck READ skipMigrationCheck WRITE setSkipMigrationCheck NOTIFY skipMigrationCheckChanged)

    // === Java Page ===
    Q_PROPERTY(QString defaultJavaPath READ defaultJavaPath WRITE setDefaultJavaPath NOTIFY defaultJavaPathChanged)
    Q_PROPERTY(int defaultMinMemory READ defaultMinMemory WRITE setDefaultMinMemory NOTIFY defaultMinMemoryChanged)
    Q_PROPERTY(int defaultMaxMemory READ defaultMaxMemory WRITE setDefaultMaxMemory NOTIFY defaultMaxMemoryChanged)
    Q_PROPERTY(QString defaultJvmArgs READ defaultJvmArgs WRITE setDefaultJvmArgs NOTIFY defaultJvmArgsChanged)

    // === Appearance Page ===
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(QString iconTheme READ iconTheme WRITE setIconTheme NOTIFY iconThemeChanged)
    Q_PROPERTY(bool showToolbarText READ showToolbarText WRITE setShowToolbarText NOTIFY showToolbarTextChanged)
    Q_PROPERTY(int buttonStyle READ buttonStyle WRITE setButtonStyle NOTIFY buttonStyleChanged)
    Q_PROPERTY(bool instanceListIcons READ instanceListIcons WRITE setInstanceListIcons NOTIFY instanceListIconsChanged)
    Q_PROPERTY(
        bool showInstanceStatusLight READ showInstanceStatusLight WRITE setShowInstanceStatusLight NOTIFY showInstanceStatusLightChanged)
    Q_PROPERTY(bool enableCat READ enableCat WRITE setEnableCat NOTIFY enableCatChanged)
    Q_PROPERTY(bool checkForUpdates READ checkForUpdates WRITE setCheckForUpdates NOTIFY checkForUpdatesChanged)

    // === Proxy Page ===
    Q_PROPERTY(QString proxyType READ proxyType WRITE setProxyType NOTIFY proxyTypeChanged)
    Q_PROPERTY(QString proxyHost READ proxyHost WRITE setProxyHost NOTIFY proxyHostChanged)
    Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged)
    Q_PROPERTY(QString proxyUsername READ proxyUsername WRITE setProxyUsername NOTIFY proxyUsernameChanged)
    Q_PROPERTY(QString proxyPassword READ proxyPassword WRITE setProxyPassword NOTIFY proxyPasswordChanged)

    // === Language ===
    Q_PROPERTY(QString currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged)
    Q_PROPERTY(QStringList availableLanguages READ availableLanguages NOTIFY availableLanguagesChanged)

    // === API Page ===
    Q_PROPERTY(int pastebinType READ pastebinType WRITE setPastebinType NOTIFY pastebinTypeChanged)
    Q_PROPERTY(QString pastebinCustomUrl READ pastebinCustomUrl WRITE setPastebinCustomUrl NOTIFY pastebinCustomUrlChanged)
    Q_PROPERTY(QString msaClientId READ msaClientId WRITE setMsaClientId NOTIFY msaClientIdChanged)
    Q_PROPERTY(QString curseforgeApiKey READ curseforgeApiKey WRITE setCurseforgeApiKey NOTIFY curseforgeApiKeyChanged)
    Q_PROPERTY(QString modrinthToken READ modrinthToken WRITE setModrinthToken NOTIFY modrinthTokenChanged)
    Q_PROPERTY(QString metaUrl READ metaUrl WRITE setMetaUrl NOTIFY metaUrlChanged)
    Q_PROPERTY(QString userAgentOverride READ userAgentOverride WRITE setUserAgentOverride NOTIFY userAgentOverrideChanged)

    // === External Tools Page ===
    Q_PROPERTY(QString jprofilerPath READ jprofilerPath WRITE setJprofilerPath NOTIFY jprofilerPathChanged)
    Q_PROPERTY(QString jvisualvmPath READ jvisualvmPath WRITE setJvisualvmPath NOTIFY jvisualvmPathChanged)
    Q_PROPERTY(QString mceditPath READ mceditPath WRITE setMceditPath NOTIFY mceditPathChanged)
    Q_PROPERTY(QString jsonEditorPath READ jsonEditorPath WRITE setJsonEditorPath NOTIFY jsonEditorPathChanged)

   public:
    explicit LauncherSettingsViewModel(QObject* parent = nullptr);

    // Launcher Page
    bool sortByName() const;
    QString renamingBehavior() const;
    QString launchAction() const;
    bool showConsole() const;
    bool autoCloseConsole() const;
    bool showConsoleOnCrash() const;
    QString instancesFolder() const;
    QString modsFolder() const;
    QString iconsFolder() const;
    int concurrentDownloads() const;
    bool validateDownloads() const;

    void setSortByName(bool value);
    void setRenamingBehavior(const QString& value);
    void setLaunchAction(const QString& value);
    void setShowConsole(bool value);
    void setAutoCloseConsole(bool value);
    void setShowConsoleOnCrash(bool value);
    void setInstancesFolder(const QString& path);
    void setModsFolder(const QString& path);
    void setIconsFolder(const QString& path);
    void setConcurrentDownloads(int value);
    void setValidateDownloads(bool value);

    // Minecraft Page
    bool showGameTime() const;
    bool showGlobalGameTime() const;
    bool enableManageModsButton() const;
    bool enableFeralGamemode() const;
    bool enableDiscreteGpu() const;
    bool enableMangoHud() const;
    bool startMaximized() const;
    int windowWidth() const;
    int windowHeight() const;
    bool showGameLog() const;
    bool useNativeOpenAL() const;
    bool useNativeGLFW() const;
    bool skipMigrationCheck() const;

    void setShowGameTime(bool value);
    void setShowGlobalGameTime(bool value);
    void setEnableManageModsButton(bool value);
    void setEnableFeralGamemode(bool value);
    void setEnableDiscreteGpu(bool value);
    void setEnableMangoHud(bool value);
    void setStartMaximized(bool value);
    void setWindowWidth(int value);
    void setWindowHeight(int value);
    void setShowGameLog(bool value);
    void setUseNativeOpenAL(bool value);
    void setUseNativeGLFW(bool value);
    void setSkipMigrationCheck(bool value);

    // Java Page
    QString defaultJavaPath() const;
    int defaultMinMemory() const;
    int defaultMaxMemory() const;
    QString defaultJvmArgs() const;

    void setDefaultJavaPath(const QString& path);
    void setDefaultMinMemory(int value);
    void setDefaultMaxMemory(int value);
    void setDefaultJvmArgs(const QString& args);

    // Appearance Page
    QString theme() const;
    QString iconTheme() const;
    bool showToolbarText() const;
    int buttonStyle() const;
    bool instanceListIcons() const;
    bool showInstanceStatusLight() const;
    bool enableCat() const;
    bool checkForUpdates() const;

    void setTheme(const QString& theme);
    void setIconTheme(const QString& theme);
    void setShowToolbarText(bool value);
    void setButtonStyle(int value);
    void setInstanceListIcons(bool value);
    void setShowInstanceStatusLight(bool value);
    void setEnableCat(bool value);
    void setCheckForUpdates(bool value);

    // Proxy Page
    QString proxyType() const;
    QString proxyHost() const;
    int proxyPort() const;
    QString proxyUsername() const;
    QString proxyPassword() const;

    void setProxyType(const QString& type);
    void setProxyHost(const QString& host);
    void setProxyPort(int port);
    void setProxyUsername(const QString& user);
    void setProxyPassword(const QString& password);

    // Language
    QString currentLanguage() const;
    QStringList availableLanguages() const;
    void setCurrentLanguage(const QString& lang);

    // API Page
    int pastebinType() const;
    QString pastebinCustomUrl() const;
    QString msaClientId() const;
    QString curseforgeApiKey() const;
    QString modrinthToken() const;
    QString metaUrl() const;
    QString userAgentOverride() const;

    void setPastebinType(int type);
    void setPastebinCustomUrl(const QString& url);
    void setMsaClientId(const QString& id);
    void setCurseforgeApiKey(const QString& key);
    void setModrinthToken(const QString& token);
    void setMetaUrl(const QString& url);
    void setUserAgentOverride(const QString& ua);

    // External Tools Page
    QString jprofilerPath() const;
    QString jvisualvmPath() const;
    QString mceditPath() const;
    QString jsonEditorPath() const;

    void setJprofilerPath(const QString& path);
    void setJvisualvmPath(const QString& path);
    void setMceditPath(const QString& path);
    void setJsonEditorPath(const QString& path);

    Q_INVOKABLE void loadSettings();
    Q_INVOKABLE void applySettings();
    Q_INVOKABLE void resetToDefaults();
    Q_INVOKABLE void testJavaPath(const QString& path);
    Q_INVOKABLE void autoDetectJava();

   signals:
    void sortByNameChanged();
    void renamingBehaviorChanged();
    void launchActionChanged();
    void showConsoleChanged();
    void autoCloseConsoleChanged();
    void showConsoleOnCrashChanged();
    void instancesFolderChanged();
    void modsFolderChanged();
    void iconsFolderChanged();
    void concurrentDownloadsChanged();
    void validateDownloadsChanged();

    void showGameTimeChanged();
    void showGlobalGameTimeChanged();
    void enableManageModsButtonChanged();
    void enableFeralGamemodeChanged();
    void enableDiscreteGpuChanged();
    void enableMangoHudChanged();
    void startMaximizedChanged();
    void windowWidthChanged();
    void windowHeightChanged();
    void showGameLogChanged();
    void useNativeOpenALChanged();
    void useNativeGLFWChanged();
    void skipMigrationCheckChanged();

    void defaultJavaPathChanged();
    void defaultMinMemoryChanged();
    void defaultMaxMemoryChanged();
    void defaultJvmArgsChanged();

    void themeChanged();
    void iconThemeChanged();
    void showToolbarTextChanged();
    void buttonStyleChanged();
    void instanceListIconsChanged();
    void showInstanceStatusLightChanged();
    void enableCatChanged();
    void checkForUpdatesChanged();

    void proxyTypeChanged();
    void proxyHostChanged();
    void proxyPortChanged();
    void proxyUsernameChanged();
    void proxyPasswordChanged();

    void currentLanguageChanged();
    void availableLanguagesChanged();

    void pastebinTypeChanged();
    void pastebinCustomUrlChanged();
    void msaClientIdChanged();
    void curseforgeApiKeyChanged();
    void modrinthTokenChanged();
    void metaUrlChanged();
    void userAgentOverrideChanged();

    void jprofilerPathChanged();
    void jvisualvmPathChanged();
    void mceditPathChanged();
    void jsonEditorPathChanged();

    void settingsApplied();
    void javaTestResult(bool success, const QString& message);
    void javaAutoDetected(const QStringList& javaPaths);

   private:
    void loadFromApplication();
    void saveToApplication();

    // Cached values
    bool m_sortByName = true;
    QString m_renamingBehavior = "ask";
    QString m_launchAction = "doNothing";
    bool m_showConsole = false;
    bool m_autoCloseConsole = true;
    bool m_showConsoleOnCrash = true;
    QString m_instancesFolder;
    QString m_modsFolder;
    QString m_iconsFolder;
    int m_concurrentDownloads = 6;
    bool m_validateDownloads = true;

    bool m_showGameTime = true;
    bool m_showGlobalGameTime = true;
    bool m_enableManageModsButton = true;
    bool m_enableFeralGamemode = false;
    bool m_enableDiscreteGpu = false;
    bool m_enableMangoHud = false;
    bool m_startMaximized = false;
    int m_windowWidth = 854;
    int m_windowHeight = 480;
    bool m_showGameLog = true;
    bool m_useNativeOpenAL = false;
    bool m_useNativeGLFW = false;
    bool m_skipMigrationCheck = false;

    QString m_defaultJavaPath;
    int m_defaultMinMemory = 512;
    int m_defaultMaxMemory = 4096;
    QString m_defaultJvmArgs;

    QString m_theme = "Dark";
    QString m_iconTheme = "Default";
    bool m_showToolbarText = true;
    int m_buttonStyle = 3;  // 0=IconOnly, 1=TextOnly, 2=TextBesideIcon, 3=TextUnderIcon
    bool m_instanceListIcons = true;
    bool m_showInstanceStatusLight = true;
    bool m_enableCat = false;
    bool m_checkForUpdates = true;

    QString m_proxyType = "none";
    QString m_proxyHost;
    int m_proxyPort = 8080;
    QString m_proxyUsername;
    QString m_proxyPassword;

    QString m_currentLanguage = "en_US";

    // API Page
    int m_pastebinType = 0;
    QString m_pastebinCustomUrl;
    QString m_msaClientId;
    QString m_curseforgeApiKey;
    QString m_modrinthToken;
    QString m_metaUrl;
    QString m_userAgentOverride;

    // External Tools Page
    QString m_jprofilerPath;
    QString m_jvisualvmPath;
    QString m_mceditPath;
    QString m_jsonEditorPath;
};
