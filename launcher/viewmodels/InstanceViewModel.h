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
    Q_INVOKABLE void copyWorld(int index);
    Q_INVOKABLE void backupWorld(int index);
    Q_INVOKABLE void deleteWorld(int index);
    Q_INVOKABLE void openWorldFolder(int index);
    
    // Screenshots/Worlds lists
    QStringList screenshotPaths() const;
    QStringList screenshotNames() const;
    QStringList worldPaths() const;
    QStringList worldNames() const;
    
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
    Q_INVOKABLE void openModsFolder();
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
    void javaAutoDetected(const QStringList& javaPaths);
    
    void launchRequested(const QString& instanceId);
    void launchOfflineRequested(const QString& instanceId);
    void killRequested(const QString& instanceId);

private slots:
    void onInstancePropertiesChanged(BaseInstance* inst);

private:
    void loadFromInstance();
    void emitAllChanged();
    QString formatTime(qint64 seconds) const;
    void scanScreenshots();
    void scanWorlds();
    
    QString m_instanceId;
    InstancePtr m_instance;
    QStringList m_screenshotPaths;
    QStringList m_screenshotNames;
    QStringList m_worldPaths;
    QStringList m_worldNames;
};
