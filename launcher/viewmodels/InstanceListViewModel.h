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

class InstanceListViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)
    Q_PROPERTY(QString selectedInstanceId READ selectedInstanceId NOTIFY selectedInstanceIdChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)

   public:
    explicit InstanceListViewModel(QObject* parent = nullptr);

    int totalCount() const;
    QString selectedInstanceId() const;
    bool isBusy() const;

    void setTotalCount(int count);
    void setSelectedInstanceId(const QString& id);
    void setBusy(bool busy);

   signals:
    void totalCountChanged();
    void selectedInstanceIdChanged();
    void busyChanged();

   private:
    int m_totalCount = 0;
    QString m_selectedInstanceId;
    bool m_busy = false;
};
