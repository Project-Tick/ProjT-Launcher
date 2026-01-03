// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
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

#include <QImage>
#include <QList>
#include <QNetworkReply>
#include <QObject>
#include <QSet>

#include "minecraft/auth/AccountData.h"
#include "minecraft/auth/AuthStep.h"
#include "tasks/Task.h"

class AuthFlow : public Task {
    Q_OBJECT

   public:
    enum class Action { Refresh, Login, DeviceCode };

    explicit AuthFlow(AccountData* data, Action action = Action::Refresh);
    virtual ~AuthFlow() = default;

    void executeTask() override;

    AccountTaskState taskState() { return m_taskState; }

   public slots:
    bool abort() override;

   signals:
    void authorizeWithBrowser(const QUrl& url);
    void authorizeWithBrowserWithExtra(QString url, QString code, int expiresIn);

   protected:
    void succeed();
    void nextStep();

   private slots:
    // NOTE: true -> non-terminal state, false -> terminal state
    bool changeState(AccountTaskState newState, QString reason = QString());
    void stepFinished(AccountTaskState resultingState, QString message);

   private:
    AccountTaskState m_taskState = AccountTaskState::STATE_CREATED;
    QList<AuthStep::Ptr> m_steps;
    AuthStep::Ptr m_currentStep;
    AccountData* m_data = nullptr;
};
