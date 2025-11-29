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

class SettingsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString instanceId READ instanceId NOTIFY instanceIdChanged)
    Q_PROPERTY(QString currentCategory READ currentCategory WRITE setCurrentCategory NOTIFY currentCategoryChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)
    Q_PROPERTY(QString javaPath READ javaPath WRITE setJavaPath NOTIFY javaPathChanged)
    Q_PROPERTY(bool overrideJavaLocation READ overrideJavaLocation WRITE setOverrideJavaLocation NOTIFY overrideJavaLocationChanged)
    Q_PROPERTY(bool saveBusy READ saveBusy NOTIFY saveBusyChanged)
    Q_PROPERTY(QString lastErrorMessage READ lastErrorMessage NOTIFY lastErrorMessageChanged)

   public:
    explicit SettingsViewModel(QObject* parent = nullptr);

    QString instanceId() const;
    QString currentCategory() const;
    bool isBusy() const;
    QString javaPath() const;
    bool overrideJavaLocation() const;
    bool saveBusy() const;
    QString lastErrorMessage() const;

    void setInstanceId(const QString& id);
    void setCurrentCategory(const QString& category);
    void setBusy(bool busy);
    void setJavaPath(const QString& path);
    void setOverrideJavaLocation(bool value);
    void setSaveBusy(bool busy);
    void setLastErrorMessage(const QString& message);

    void notifySettingsLoaded();
    void notifySettingsChanged();
    void notifySaveRequested();

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void saveAll();
    Q_INVOKABLE void resetToDefaultsForCurrentCategory();

   signals:
    void instanceIdChanged();
    void currentCategoryChanged();
    void busyChanged();
    void javaPathChanged();
    void overrideJavaLocationChanged();
    void saveBusyChanged();
    void lastErrorMessageChanged();
    void settingsLoaded();
    void settingsChanged();
    void saveRequested();

   private:
    void loadCurrentSettings();
    void resetJavaCategory();

    QString m_instanceId;
    QString m_currentCategory;
    bool m_busy = false;
    QString m_javaPath;
    bool m_overrideJavaLocation = false;
    bool m_saveBusy = false;
    QString m_lastErrorMessage;
};
