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

class LauncherViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString displayName READ displayName NOTIFY displayNameChanged)
    Q_PROPERTY(QString versionString READ versionString NOTIFY versionStringChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)

   public:
    explicit LauncherViewModel(QObject* parent = nullptr);

    QString displayName() const;
    QString versionString() const;
    bool isBusy() const;

    void setDisplayName(const QString& name);
    void setVersionString(const QString& version);
    void setBusy(bool busy);

   signals:
    void displayNameChanged();
    void versionStringChanged();
    void busyChanged();

   private:
    QString m_displayName;
    QString m_versionString;
    bool m_busy = false;
};
