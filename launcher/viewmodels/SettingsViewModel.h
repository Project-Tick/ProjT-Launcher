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
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <functional>

class QWidget;
class SettingsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString instanceId READ instanceId NOTIFY instanceIdChanged)
    Q_PROPERTY(QString currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY currentCategoryChanged)
    Q_PROPERTY(QStringList categoryList READ categoryList NOTIFY categoryListChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(QString javaPath READ javaPath WRITE setJavaPath NOTIFY javaPathChanged)
    Q_PROPERTY(bool overrideJavaLocation READ overrideJavaLocation WRITE setOverrideJavaLocation NOTIFY overrideJavaLocationChanged)
    Q_PROPERTY(bool overrideMemory READ overrideMemory NOTIFY overrideMemoryChanged)
    Q_PROPERTY(bool overrideLoader READ overrideLoader NOTIFY overrideLoaderChanged)
    Q_PROPERTY(bool overrideEnv READ overrideEnv NOTIFY overrideEnvChanged)
    Q_PROPERTY(bool saveBusy READ saveBusy NOTIFY saveBusyChanged)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY lastErrorMessageChanged)
    Q_PROPERTY(QString preLaunchCommand READ preLaunchCommand NOTIFY customCommandsChanged)
    Q_PROPERTY(QString postExitCommand READ postExitCommand NOTIFY customCommandsChanged)
    Q_PROPERTY(int minMemory READ minMemory NOTIFY memoryChanged)
    Q_PROPERTY(int maxMemory READ maxMemory NOTIFY memoryChanged)
    Q_PROPERTY(QString jvmArgs READ jvmArgs NOTIFY jvmArgsChanged)
    Q_PROPERTY(QString loaderType READ loaderType WRITE setLoaderTypeProperty NOTIFY loaderSettingsChanged)
    Q_PROPERTY(QStringList availableLoaderTypes READ availableLoaderTypes NOTIFY loaderSettingsChanged)
    Q_PROPERTY(QString loaderVersion READ loaderVersion WRITE setLoaderVersionProperty NOTIFY loaderSettingsChanged)
    Q_PROPERTY(QStringList availableLoaderVersions READ availableLoaderVersions NOTIFY loaderSettingsChanged)
    Q_PROPERTY(QString gameArgs READ gameArgs NOTIFY gameSettingsChanged)
    Q_PROPERTY(bool fullscreen READ fullscreen NOTIFY gameSettingsChanged)
    Q_PROPERTY(int resolutionWidth READ resolutionWidth NOTIFY gameSettingsChanged)
    Q_PROPERTY(int resolutionHeight READ resolutionHeight NOTIFY gameSettingsChanged)
    Q_PROPERTY(bool overrideGameDir READ overrideGameDir NOTIFY gameSettingsChanged)
    Q_PROPERTY(QString customGameDir READ customGameDir NOTIFY gameSettingsChanged)
    Q_PROPERTY(QString notes READ notes NOTIFY notesChanged)
    Q_PROPERTY(QString iconKey READ iconKey NOTIFY iconChanged)
    Q_PROPERTY(QStringList availableIcons READ availableIcons NOTIFY iconChanged)
    Q_PROPERTY(QString busyReason READ busyReason NOTIFY busyChanged)

   public:
    explicit SettingsViewModel(QObject* parent = nullptr);

    QString instanceId() const;
    QString currentCategory() const;
    QStringList categoryList() const { return m_categoryList; }
    bool isBusy() const;
    QString javaPath() const;
    bool overrideJavaLocation() const;
    bool saveBusy() const;
    QString lastErrorMessage() const;
    QString preLaunchCommand() const;
    QString postExitCommand() const;
    int minMemory() const { return m_minMemory; }
    int maxMemory() const { return m_maxMemory; }
    QString jvmArgs() const { return m_jvmArgs; }
    bool overrideMemory() const { return m_overrideMemory; }
    bool overrideLoader() const { return m_overrideLoader; }
    bool overrideEnv() const { return m_overrideEnv; }
    QString loaderType() const { return m_loaderType; }
    QStringList availableLoaderTypes() const { return m_availableLoaderTypes; }
    QString loaderVersion() const { return m_loaderVersion; }
    QStringList availableLoaderVersions() const { return m_availableLoaderVersions; }
    QString gameArgs() const { return m_gameArgs; }
    bool fullscreen() const { return m_fullscreen; }
    int resolutionWidth() const { return m_resolutionWidth; }
    int resolutionHeight() const { return m_resolutionHeight; }
    bool overrideGameDir() const { return m_overrideGameDir; }
    QString customGameDir() const { return m_customGameDir; }
    QString notes() const { return m_notes; }
    QString iconKey() const { return m_iconKey; }
    QStringList availableIcons() const { return m_availableIcons; }
    QString busyReason() const { return m_busyReason; }

    void setInstanceId(const QString& id);
    void setCurrentCategory(const QString& category);
    void setCategoryList(const QStringList& categories);
    void setBusy(bool busy);
    void setJavaPath(const QString& path);
    void setOverrideJavaLocation(bool value);
    void setSaveBusy(bool busy);
    void setLastErrorMessage(const QString& message);

    void notifySettingsLoaded();
    void notifySettingsChanged();
    void notifySaveRequested();

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void loadCategory(const QString& category);
    Q_INVOKABLE void applyChanges();
    Q_INVOKABLE void resetChanges();
    Q_INVOKABLE void saveAll();
    Q_INVOKABLE void resetToDefaultsForCurrentCategory();
    Q_INVOKABLE QStringList environmentKeys(const QString& instanceId) const;
    Q_INVOKABLE QString environmentValue(const QString& instanceId, const QString& key) const;
    Q_INVOKABLE void setEnvironmentVar(const QString& instanceId, const QString& key, const QString& value);
    Q_INVOKABLE void removeEnvironmentVar(const QString& instanceId, const QString& key);
    Q_INVOKABLE void clearEnvironmentVars(const QString& instanceId);
    Q_INVOKABLE void setEnvironmentVars(const QString& instanceId, bool overrideEnv, const QMap<QString, QVariant>& vars);
    Q_INVOKABLE void setPreLaunchCommand(const QString& instanceId, const QString& cmd);
    Q_INVOKABLE void setPostExitCommand(const QString& instanceId, const QString& cmd);
    Q_INVOKABLE void setJavaPath(const QString& instanceId, const QString& path);
    Q_INVOKABLE void setOverrideJavaLocation(const QString& instanceId, bool value);
    Q_INVOKABLE void setJavaArgs(const QString& instanceId, const QString& args);
    Q_INVOKABLE void setJavaMemory(const QString& instanceId, int minMem, int maxMem);
    void autoDetectJava(const QString& instanceId, QWidget* parent = nullptr);
    Q_INVOKABLE void setMemorySettings(const QString& instanceId, int minMem, int maxMem);
    Q_INVOKABLE void setJVMArguments(const QString& instanceId, const QString& args);
    Q_INVOKABLE void setLoaderType(const QString& instanceId, const QString& type);
    Q_INVOKABLE void setLoaderVersion(const QString& instanceId, const QString& version);
    Q_INVOKABLE void refreshLoaderVersions(const QString& instanceId);
    void setLoaderPreferences(const QString& instanceId, const QStringList& loaders, bool overrideLoaders);
    Q_INVOKABLE void setOverrideMemory(const QString& instanceId, bool value);
    Q_INVOKABLE void setOverrideLoader(const QString& instanceId, bool value);
    Q_INVOKABLE void setOverrideEnv(const QString& instanceId, bool value);
    void setApplyHook(std::function<bool()> hook);
    void setResetHook(std::function<void()> hook);
    Q_INVOKABLE void setGameArgs(const QString& instanceId, const QString& args);
    Q_INVOKABLE void setFullscreen(const QString& instanceId, bool enabled);
    Q_INVOKABLE void setResolution(const QString& instanceId, int width, int height);
    Q_INVOKABLE void setOverrideGameDir(const QString& instanceId, bool value);
    Q_INVOKABLE void setCustomGameDir(const QString& instanceId, const QString& path);
    Q_INVOKABLE void setNotes(const QString& instanceId, const QString& notes);
    Q_INVOKABLE void setIconKey(const QString& instanceId, const QString& iconKey);
    Q_INVOKABLE void setLoaderTypeProperty(const QString& type);
    Q_INVOKABLE void setLoaderVersionProperty(const QString& version);

   signals:
    void instanceIdChanged();
    void currentCategoryChanged();
    void categoryListChanged();
    void busyChanged();
    void javaPathChanged();
    void overrideJavaLocationChanged();
    void saveBusyChanged();
    void lastErrorMessageChanged();
    void settingsLoaded();
    void settingsChanged();
    void saveRequested();
    void envVarsChanged();
    void customCommandsChanged();
    void memoryChanged();
    void jvmArgsChanged();
    void loaderSettingsChanged();
    void overrideMemoryChanged();
    void overrideLoaderChanged();
    void overrideEnvChanged();
    void started(const QString& reason = {});
    void finished();
    void errorOccurred(const QString& message);
    void gameSettingsChanged();
    void notesChanged();
    void iconChanged();

   private:
    void loadCurrentSettings();
    void resetJavaCategory();
    QMap<QString, QVariant> loadEnv(const QString& instanceId) const;
    void storeEnv(const QString& instanceId, bool overrideEnv, const QMap<QString, QVariant>& vars);
    std::shared_ptr<class SettingsObject> settingsForInstance(const QString& instanceId) const;

    std::function<bool()> m_applyHook;
    std::function<void()> m_resetHook;
    QString m_instanceId;
    QString m_currentCategory;
    QStringList m_categoryList{ QStringLiteral("java") };
    bool m_busy = false;
    QString m_busyReason;
    QString m_javaPath;
    bool m_overrideJavaLocation = false;
    bool m_saveBusy = false;
    QString m_lastErrorMessage;
    QString m_preLaunchCommand;
    QString m_postExitCommand;
    int m_minMemory = 0;
    int m_maxMemory = 0;
    QString m_jvmArgs;
    bool m_overrideMemory = false;
    bool m_overrideLoader = false;
    bool m_overrideEnv = false;
    QString m_loaderType;
    QStringList m_availableLoaderTypes;
    QString m_loaderVersion;
    QStringList m_availableLoaderVersions;
    QString m_gameArgs;
    bool m_fullscreen = false;
    int m_resolutionWidth = 0;
    int m_resolutionHeight = 0;
    bool m_overrideGameDir = false;
    QString m_customGameDir;
    QString m_notes;
    QString m_iconKey;
    QStringList m_availableIcons;
};
