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

#include <QList>
#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "minecraft/auth/AccountData.h"
#include "minecraft/auth/steps/Step.hpp"
#include "minecraft/auth/steps/Credentials.hpp"
#include "tasks/Task.h"

/**
 * Authentication flow orchestrator.
 *
 * # Design Rationale
 *
 * This class serves as a bridge between the legacy AccountData-based auth system
 * and the new projt::minecraft::auth namespace. The rewrite was necessary because:
 *
 * 1. **License Isolation**: GPL-2.0-only target requires clean separation from
 *    upstream GPL-3.0 code. The new Step/Credentials API provides this boundary.
 *
 * 2. **Stateless Steps**: Each Step operates on a shared Credentials reference,
 *    eliminating inter-step coupling and enabling future parallelization.
 *
 * 3. **Testability**: Pure Credentials-based flow can be unit tested without
 *    Qt event loop or AccountData persistence concerns.
 *
 * The legacy sync in succeed() maintains backward compatibility with existing
 * consumers (MinecraftAccount, AccountList, UI dialogs) while the internal
 * implementation uses the new architecture.
 *
 * # Ownership Model
 *
 * Steps are managed via shared_qobject_ptr (Step::Ptr) which combines Qt's
 * parent-child model with reference counting. This is intentional:
 * - QObject is required for signal/slot connections
 * - Reference counting prevents double-delete when steps are queued
 * - No explicit parent is set; the Ptr handles cleanup automatically
 */
class AuthFlow : public Task
{
	Q_OBJECT

public:
	/**
	 * Authentication action to perform.
	 */
	enum class Action
	{
		Refresh,   ///< Silent token refresh
		Login,     ///< Interactive browser login
		DeviceCode ///< Device code flow
	};

	explicit AuthFlow(AccountData* data, Action action = Action::Refresh);
	~AuthFlow() override = default;

	void executeTask() override;

	[[nodiscard]] AccountTaskState taskState() const noexcept { return m_taskState; }

public slots:
	bool abort() override;

signals:
	/**
	 * Emitted when browser authorization is required.
	 */
	void authorizeWithBrowser(const QUrl& url);

	/**
	 * Emitted when device code authorization is required.
	 */
	void authorizeWithBrowserWithExtra(QString url, QString code, int expiresIn);

private slots:
	void onStepCompleted(projt::minecraft::auth::StepResult result, QString message);

private:
	/**
	 * Build the authentication pipeline based on account type and action.
	 * @return true if pipeline was successfully built, false on configuration error
	 */
	[[nodiscard]] bool buildPipeline(Action action);

	void executeNextStep();
	void succeed();
	void failWithState(AccountTaskState state, const QString& reason);
	bool updateState(AccountTaskState newState, const QString& reason = QString());

	// Convert new StepResult to intermediate flow state (not final AccountTaskState)
	[[nodiscard]] static AccountTaskState stepResultToFlowState(projt::minecraft::auth::StepResult result) noexcept;

	// Convert new TokenValidity to legacy Validity
	[[nodiscard]] static Validity toValidity(projt::minecraft::auth::TokenValidity validity) noexcept;

	AccountTaskState m_taskState = AccountTaskState::STATE_CREATED;
	QList<projt::minecraft::auth::Step::Ptr> m_steps;
	projt::minecraft::auth::Step::Ptr m_currentStep;

	// Flow control
	bool m_aborted = false;
	bool m_pipelineValid = false;

	// Legacy AccountData for compatibility with existing consumers
	AccountData* m_legacyData = nullptr;

	// New credentials structure (populated during auth, synced to legacy at end)
	projt::minecraft::auth::Credentials m_credentials;
};
