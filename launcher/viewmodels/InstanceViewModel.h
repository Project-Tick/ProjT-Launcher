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

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantMap>

#include "BaseInstance.h"

class InstanceViewModel : public QObject {
    Q_OBJECT

    // Instance identification
    Q_PROPERTY(QString instanceId READ instanceId WRITE setInstanceId NOTIFY instanceIdChanged)
    Q_PROPERTY(bool hasInstance READ hasInstance NOTIFY hasInstanceChanged)

    // Basic info
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString iconKey READ iconKey WRITE setIconKey NOTIFY iconKeyChanged)
    Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
    Q_PROPERTY(QString instanceType READ instanceType NOTIFY instanceTypeChanged)
    Q_PROPERTY(QString instanceRoot READ instanceRoot NOTIFY instanceRootChanged)
    Q_PROPERTY(QString gameRoot READ gameRoot NOTIFY gameRootChanged)

    // Status
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool hasBrokenVersion READ hasBrokenVersion NOTIFY hasBrokenVersionChanged)
    Q_PROPERTY(bool hasUpdateAvailable READ hasUpdateAvailable NOTIFY hasUpdateAvailableChanged)
    Q_PROPERTY(bool hasCrashed READ hasCrashed NOTIFY hasCrashedChanged)
    Q_PROPERTY(bool canLaunch READ canLaunch NOTIFY canLaunchChanged)
    Q_PROPERTY(bool canEdit READ canEdit NOTIFY canEditChanged)

    // Playtime
    Q_PROPERTY(qint64 totalTimePlayed READ totalTimePlayed NOTIFY totalTimePlayedChanged)
    Q_PROPERTY(qint64 lastTimePlayed READ lastTimePlayed NOTIFY lastTimePlayedChanged)
    Q_PROPERTY(QString totalTimePlayedFormatted READ totalTimePlayedFormatted NOTIFY totalTimePlayedChanged)
    Q_PROPERTY(QString lastTimePlayedFormatted READ lastTimePlayedFormatted NOTIFY lastTimePlayedChanged)
    Q_PROPERTY(qint64 lastLaunch READ lastLaunch NOTIFY lastLaunchChanged)

    // Managed pack info
    Q_PROPERTY(bool isManagedPack READ isManagedPack NOTIFY isManagedPackChanged)
    Q_PROPERTY(QString managedPackType READ managedPackType NOTIFY managedPackTypeChanged)
    Q_PROPERTY(QString managedPackName READ managedPackName NOTIFY managedPackNameChanged)
    Q_PROPERTY(QString managedPackVersionName READ managedPackVersionName NOTIFY managedPackVersionNameChanged)

    // Java settings (override)
    Q_PROPERTY(bool overrideJava READ overrideJava WRITE setOverrideJava NOTIFY overrideJavaChanged)
    Q_PROPERTY(QString javaPath READ javaPath WRITE setJavaPath NOTIFY javaPathChanged)
    Q_PROPERTY(QString jvmArgs READ jvmArgs WRITE setJvmArgs NOTIFY jvmArgsChanged)

    // Memory settings (override)
    Q_PROPERTY(bool overrideMemory READ overrideMemory WRITE setOverrideMemory NOTIFY overrideMemoryChanged)
    Q_PROPERTY(int minMemory READ minMemory WRITE setMinMemory NOTIFY minMemoryChanged)
    Q_PROPERTY(int maxMemory READ maxMemory WRITE setMaxMemory NOTIFY maxMemoryChanged)

    // Window settings (override)
    Q_PROPERTY(bool overrideWindow READ overrideWindow WRITE setOverrideWindow NOTIFY overrideWindowChanged)
    Q_PROPERTY(int windowWidth READ windowWidth WRITE setWindowWidth NOTIFY windowWidthChanged)
    Q_PROPERTY(int windowHeight READ windowHeight WRITE setWindowHeight NOTIFY windowHeightChanged)
    Q_PROPERTY(bool maximizeWindow READ maximizeWindow WRITE setMaximizeWindow NOTIFY maximizeWindowChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)

    // Game options
    Q_PROPERTY(bool showConsole READ showConsole WRITE setShowConsole NOTIFY showConsoleChanged)
    Q_PROPERTY(bool closeOnLaunch READ closeOnLaunch WRITE setCloseOnLaunch NOTIFY closeOnLaunchChanged)
    Q_PROPERTY(bool quitAfterGame READ quitAfterGame WRITE setQuitAfterGame NOTIFY quitAfterGameChanged)

    // Commands
    Q_PROPERTY(QString preLaunchCommand READ preLaunchCommand WRITE setPreLaunchCommand NOTIFY preLaunchCommandChanged)
    Q_PROPERTY(QString postExitCommand READ postExitCommand WRITE setPostExitCommand NOTIFY postExitCommandChanged)
    Q_PROPERTY(QString wrapperCommand READ wrapperCommand WRITE setWrapperCommand NOTIFY wrapperCommandChanged)

    // Screenshots list
    Q_PROPERTY(QStringList screenshotPaths READ screenshotPaths NOTIFY screenshotPathsChanged)
    Q_PROPERTY(QStringList screenshotNames READ screenshotNames NOTIFY screenshotPathsChanged)

    // Worlds list
    Q_PROPERTY(QStringList worldPaths READ worldPaths NOTIFY worldPathsChanged)
    Q_PROPERTY(QStringList worldNames READ worldNames NOTIFY worldPathsChanged)

    // Servers list
    Q_PROPERTY(QStringList serverNames READ serverNames NOTIFY serversChanged)
    Q_PROPERTY(QStringList serverAddresses READ serverAddresses NOTIFY serversChanged)

    // Version info
    Q_PROPERTY(QString minecraftVersion READ minecraftVersion NOTIFY versionChanged)
    Q_PROPERTY(QString modLoaderName READ modLoaderName NOTIFY versionChanged)
    Q_PROPERTY(QString modLoaderVersion READ modLoaderVersion NOTIFY versionChanged)
    Q_PROPERTY(QVariantList componentsModel READ componentsModel NOTIFY componentsModelChanged)

    // Mods
    Q_PROPERTY(QVariantList modsModel READ modsModel NOTIFY modsModelChanged)
    Q_PROPERTY(int modsCount READ modsCount NOTIFY modsModelChanged)

    // Resource packs
    Q_PROPERTY(QVariantList resourcePacksModel READ resourcePacksModel NOTIFY resourcePacksModelChanged)
    Q_PROPERTY(int resourcePacksCount READ resourcePacksCount NOTIFY resourcePacksModelChanged)

    // Shader packs
    Q_PROPERTY(QVariantList shaderPacksModel READ shaderPacksModel NOTIFY shaderPacksModelChanged)
    Q_PROPERTY(int shaderPacksCount READ shaderPacksCount NOTIFY shaderPacksModelChanged)

    // Texture packs
    Q_PROPERTY(QVariantList texturePacksModel READ texturePacksModel NOTIFY texturePacksModelChanged)
    Q_PROPERTY(int texturePacksCount READ texturePacksCount NOTIFY texturePacksModelChanged)

   public:
    explicit InstanceViewModel(QObject* parent = nullptr);

    // Instance identification
    QString instanceId() const;
    void setInstanceId(const QString& id);
    bool hasInstance() const;

    // Basic info
    QString name() const;
    void setName(const QString& name);
    QString iconKey() const;
    void setIconKey(const QString& key);
    QString notes() const;
    void setNotes(const QString& notes);
    QString instanceType() const;
    QString instanceRoot() const;
    QString gameRoot() const;

    // Status
    bool isRunning() const;
    bool hasBrokenVersion() const;
    bool hasUpdateAvailable() const;
    bool hasCrashed() const;
    bool canLaunch() const;
    bool canEdit() const;

    // Playtime
    qint64 totalTimePlayed() const;
    qint64 lastTimePlayed() const;
    QString totalTimePlayedFormatted() const;
    QString lastTimePlayedFormatted() const;
    qint64 lastLaunch() const;

    // Managed pack
    bool isManagedPack() const;
    QString managedPackType() const;
    QString managedPackName() const;
    QString managedPackVersionName() const;
    Q_PROPERTY(QString managedPackUrl READ managedPackUrl NOTIFY managedPackUrlChanged)
    QString managedPackUrl() const;

    // Managed pack actions
    Q_INVOKABLE void checkForPackUpdates();
    Q_INVOKABLE void updatePack();
    Q_INVOKABLE void exportPack();

    // Screenshots actions
    Q_INVOKABLE void refreshScreenshots();
    Q_INVOKABLE void copyScreenshotToClipboard(int index);
    Q_INVOKABLE void deleteScreenshot(int index);
    Q_INVOKABLE void openScreenshot(int index);

    // Servers actions
    Q_INVOKABLE void refreshServers();
    Q_INVOKABLE void addServer(const QString& name, const QString& address);
    Q_INVOKABLE void editServer(int index, const QString& name, const QString& address);
    Q_INVOKABLE void deleteServer(int index);
    Q_INVOKABLE void moveServerUp(int index);
    Q_INVOKABLE void moveServerDown(int index);

    // Worlds actions
    Q_INVOKABLE void refreshWorlds();
    Q_INVOKABLE void importWorld();
    Q_INVOKABLE void importWorldFromPath(const QString& path);
    Q_INVOKABLE void copyWorld(int index);
    Q_INVOKABLE void backupWorld(int index);
    Q_INVOKABLE void deleteWorld(int index);
    Q_INVOKABLE void openWorldFolder(int index);

    // Screenshots/Worlds lists
    QStringList screenshotPaths() const;
    QStringList screenshotNames() const;
    QStringList worldPaths() const;
    QStringList worldNames() const;

    // Servers list
    QStringList serverNames() const;
    QStringList serverAddresses() const;

    // Version info
    QString minecraftVersion() const;
    QString modLoaderName() const;
    QString modLoaderVersion() const;
    QVariantList componentsModel() const;

    // Version actions
    Q_INVOKABLE void refreshVersionComponents();
    Q_INVOKABLE void setComponentEnabled(int index, bool enabled);
    Q_INVOKABLE void moveComponentUp(int index);
    Q_INVOKABLE void moveComponentDown(int index);
    Q_INVOKABLE void removeComponent(int index);
    Q_INVOKABLE void changeMinecraftVersion(const QString& version);
    Q_INVOKABLE void installModLoader(const QString& loaderType, const QString& version);

    // Mods
    QVariantList modsModel() const;
    int modsCount() const;
    Q_INVOKABLE void refreshMods();
    Q_INVOKABLE void addMod(const QString& filePath);
    Q_INVOKABLE void removeMod(int index);
    Q_INVOKABLE void enableMod(int index, bool enabled);
    Q_INVOKABLE void openModsFolder();

    // Resource packs
    QVariantList resourcePacksModel() const;
    int resourcePacksCount() const;
    Q_INVOKABLE void refreshResourcePacks();
    Q_INVOKABLE void addResourcePack(const QString& filePath);
    Q_INVOKABLE void removeResourcePack(int index);
    Q_INVOKABLE void enableResourcePack(int index, bool enabled);

    // Shader packs
    QVariantList shaderPacksModel() const;
    int shaderPacksCount() const;
    Q_INVOKABLE void refreshShaderPacks();
    Q_INVOKABLE void addShaderPack(const QString& filePath);
    Q_INVOKABLE void removeShaderPack(int index);
    Q_INVOKABLE void enableShaderPack(int index, bool enabled);

    // Texture packs
    QVariantList texturePacksModel() const;
    int texturePacksCount() const;
    Q_INVOKABLE void refreshTexturePacks();
    Q_INVOKABLE void addTexturePack(const QString& filePath);
    Q_INVOKABLE void removeTexturePack(int index);
    Q_INVOKABLE void enableTexturePack(int index, bool enabled);

    // Java settings
    bool overrideJava() const;
    void setOverrideJava(bool override);
    QString javaPath() const;
    void setJavaPath(const QString& path);
    QString jvmArgs() const;
    void setJvmArgs(const QString& args);
    Q_INVOKABLE void autoDetectJava(const QString& instanceId);

    // Memory settings
    bool overrideMemory() const;
    void setOverrideMemory(bool override);
    int minMemory() const;
    void setMinMemory(int mb);
    int maxMemory() const;
    void setMaxMemory(int mb);

    // Window settings
    bool overrideWindow() const;
    void setOverrideWindow(bool override);
    int windowWidth() const;
    void setWindowWidth(int width);
    int windowHeight() const;
    void setWindowHeight(int height);
    bool maximizeWindow() const;
    void setMaximizeWindow(bool maximize);
    bool fullscreen() const;
    void setFullscreen(bool fs);

    // Game options
    bool showConsole() const;
    void setShowConsole(bool show);
    bool closeOnLaunch() const;
    void setCloseOnLaunch(bool close);
    bool quitAfterGame() const;
    void setQuitAfterGame(bool quit);

    // Commands
    QString preLaunchCommand() const;
    void setPreLaunchCommand(const QString& cmd);
    QString postExitCommand() const;
    void setPostExitCommand(const QString& cmd);
    QString wrapperCommand() const;
    void setWrapperCommand(const QString& cmd);

    // Actions
    Q_INVOKABLE void launch();
    Q_INVOKABLE void launchOffline();
    Q_INVOKABLE void kill();
    Q_INVOKABLE void openFolder();
    Q_INVOKABLE void openGameFolder();
    Q_INVOKABLE void openResourcePacksFolder();
    Q_INVOKABLE void openShaderPacksFolder();
    Q_INVOKABLE void openScreenshotsFolder();
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void reloadSettings();
    Q_INVOKABLE void resetTimePlayed();

    // Utility
    Q_INVOKABLE QVariantMap getInstanceInfo() const;
    Q_INVOKABLE QString formatPlayTime(qint64 seconds) const;

   signals:
    void instanceIdChanged();
    void hasInstanceChanged();
    void nameChanged();
    void iconKeyChanged();
    void notesChanged();
    void instanceTypeChanged();
    void instanceRootChanged();
    void gameRootChanged();
    void isRunningChanged();
    void hasBrokenVersionChanged();
    void hasUpdateAvailableChanged();
    void hasCrashedChanged();
    void canLaunchChanged();
    void canEditChanged();
    void totalTimePlayedChanged();
    void lastTimePlayedChanged();
    void lastLaunchChanged();
    void isManagedPackChanged();
    void managedPackTypeChanged();
    void managedPackNameChanged();
    void managedPackVersionNameChanged();
    void managedPackUrlChanged();
    void overrideJavaChanged();
    void javaPathChanged();
    void jvmArgsChanged();
    void overrideMemoryChanged();
    void minMemoryChanged();
    void maxMemoryChanged();
    void overrideWindowChanged();
    void windowWidthChanged();
    void windowHeightChanged();
    void maximizeWindowChanged();
    void fullscreenChanged();
    void showConsoleChanged();
    void closeOnLaunchChanged();
    void quitAfterGameChanged();
    void preLaunchCommandChanged();
    void postExitCommandChanged();
    void wrapperCommandChanged();
    void screenshotPathsChanged();
    void worldPathsChanged();
    void serversChanged();
    void javaAutoDetected(const QStringList& javaPaths);

    // Version signals
    void versionChanged();
    void componentsModelChanged();

    // Content model signals
    void modsModelChanged();
    void resourcePacksModelChanged();
    void shaderPacksModelChanged();
    void texturePacksModelChanged();

    void launchRequested(const QString& instanceId);
    void launchOfflineRequested(const QString& instanceId);
    void killRequested(const QString& instanceId);

    // Import/Export signals for QML file dialogs
    void worldImportRequested();
    void packExportRequested();
    void packUpdateCheckResult(bool hasUpdate, const QString& newVersion);
    void worldBackupCompleted(bool success, const QString& backupPath);

   private slots:
    void onInstancePropertiesChanged(BaseInstance* inst);

   private:
    void loadFromInstance();
    void emitAllChanged();
    QString formatTime(qint64 seconds) const;
    void scanScreenshots();
    void scanWorlds();
    void scanServers();
    void saveServers();
    void scanMods();
    void scanResourcePacks();
    void scanShaderPacks();
    void scanTexturePacks();

    QString m_instanceId;
    InstancePtr m_instance;
    QStringList m_screenshotPaths;
    QStringList m_screenshotNames;
    QStringList m_worldPaths;
    QStringList m_worldNames;
    QStringList m_serverNames;
    QStringList m_serverAddresses;
    QVariantList m_modsModel;
    QVariantList m_resourcePacksModel;
    QVariantList m_shaderPacksModel;
    QVariantList m_texturePacksModel;
};
