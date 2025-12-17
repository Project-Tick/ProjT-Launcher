// SPDX-License-Identifier: GPL-3.0-only
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
#include <QObject>
#include <memory>

#include "minecraft/auth/AuthStep.h"
#include "net/NetJob.h"
#include "net/Upload.h"

class XboxAuthorizationStep : public AuthStep {
    Q_OBJECT

   public:
    explicit XboxAuthorizationStep(AccountData* data, Token* token, QString relyingParty, QString authorizationKind);
    virtual ~XboxAuthorizationStep() noexcept = default;

    void perform() override;

    QString describe() override;

   private:
    bool processSTSError();

   private slots:
    void onRequestDone();

   private:
    Token* m_token;
    QString m_relyingParty;
    QString m_authorizationKind;

    std::shared_ptr<QByteArray> m_response;
    Net::Upload::Ptr m_request;
    NetJob::Ptr m_task;
};
