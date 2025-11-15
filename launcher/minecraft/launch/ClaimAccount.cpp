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
#include "ClaimAccount.h"
#include <launch/LaunchTask.h>

#include "Application.h"
#include "minecraft/auth/AccountList.h"

ClaimAccount::ClaimAccount(LaunchTask* parent, AuthSessionPtr session) : LaunchStep(parent)
{
    if (session->status == AuthSession::Status::PlayableOnline && !session->demo) {
        auto accounts = APPLICATION->accounts();
        m_account = accounts->getAccountByProfileName(session->player_name);
    }
}

void ClaimAccount::executeTask()
{
    if (m_account) {
        lock.reset(new UseLock(m_account));
        emitSucceeded();
    }
}

void ClaimAccount::finalize()
{
    lock.reset();
}
