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
