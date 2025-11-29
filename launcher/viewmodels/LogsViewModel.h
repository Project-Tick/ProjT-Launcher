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

class LogsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString logText READ logText NOTIFY logTextChanged)
    Q_PROPERTY(QString category READ category NOTIFY categoryChanged)
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy NOTIFY busyChanged)

   public:
    explicit LogsViewModel(QObject* parent = nullptr);

    QString logText() const;
    QString category() const;
    bool isBusy() const;

    void setLogText(const QString& text);
    void setCategory(const QString& category);
    void setBusy(bool busy);

    void requestRefresh();

   signals:
    void logTextChanged();
    void categoryChanged();
    void busyChanged();
    void logsUpdated();
    void refreshRequested(const QString& category);

   private:
    QString m_logText;
    QString m_category;
    bool m_busy = false;
};
