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

class SettingsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString instanceId READ instanceId NOTIFY instanceIdChanged)
    Q_PROPERTY(QString currentCategory READ currentCategory NOTIFY currentCategoryChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)

   public:
    explicit SettingsViewModel(QObject* parent = nullptr);

    QString instanceId() const;
    QString currentCategory() const;
    bool isBusy() const;

    void setInstanceId(const QString& id);
    void setCurrentCategory(const QString& category);
    void setBusy(bool busy);

    void notifySettingsLoaded();
    void notifySettingsChanged();
    void notifySaveRequested();

   signals:
    void instanceIdChanged();
    void currentCategoryChanged();
    void busyChanged();
    void settingsLoaded();
    void settingsChanged();
    void saveRequested();

   private:
    QString m_instanceId;
    QString m_currentCategory;
    bool m_busy = false;
};
