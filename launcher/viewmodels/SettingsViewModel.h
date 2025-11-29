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
};
