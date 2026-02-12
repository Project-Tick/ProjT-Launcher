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
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include <QNetworkReply>
#include <QObject>
#include <QString>

#include "QObjectPtr.h"

namespace projt::minecraft::auth
{

	/**
	 * Result state for authentication pipeline steps.
	 * Each step emits one of these states upon completion.
	 */
	enum class StepResult
	{
		Continue,	 ///< Step succeeded, proceed to next step
		Succeeded,	 ///< Final success - authentication complete
		Offline,	 ///< Network unavailable - soft failure, can retry
		SoftFailure, ///< Recoverable error - partial auth, can retry
		HardFailure, ///< Unrecoverable error - tokens invalid
		Disabled,	 ///< Account disabled (e.g., client ID changed)
		Gone		 ///< Account no longer exists
	};

	// Forward declaration
	struct Credentials;

	/**
	 * Abstract base class for authentication pipeline steps.
	 *
	 * Each step performs a discrete authentication action (e.g., OAuth exchange,
	 * token validation, profile fetch) and emits `completed` when done.
	 *
	 * Steps are designed to be stateless between runs - all persistent data
	 * is stored in the Credentials object passed at construction.
	 */
	class Step : public QObject
	{
		Q_OBJECT

	  public:
		using Ptr = shared_qobject_ptr<Step>;

		/**
		 * Construct a step with a reference to the credential store.
		 * @param credentials Mutable reference to authentication data.
		 */
		explicit Step(Credentials& credentials) noexcept : QObject(nullptr), m_credentials(credentials)
		{}

		~Step() noexcept override = default;

		// Rule of Zero - no copy/move (QObject constraint)
		Step(const Step&)			 = delete;
		Step& operator=(const Step&) = delete;
		Step(Step&&)				 = delete;
		Step& operator=(Step&&)		 = delete;

		/**
		 * Human-readable description of what this step does.
		 * Used for progress display and logging.
		 */
		[[nodiscard]] virtual QString description() const = 0;

	  public slots:
		/**
		 * Execute this authentication step.
		 * Implementations must emit `completed` when done (success or failure).
		 */
		virtual void execute() = 0;

		/**
		 * Request cancellation of an in-progress step.
		 * Default implementation does nothing. Override for cancellable steps.
		 */
		virtual void cancel() noexcept
		{}

	  signals:
		/**
		 * Emitted when the step completes (successfully or with error).
		 * @param result The outcome of this step.
		 * @param message Human-readable status message.
		 */
		void completed(StepResult result, QString message);

		/**
		 * Emitted by OAuth steps when browser authorization is required.
		 * @param url The URL to open in the user's browser.
		 */
		void browserAuthRequired(const QUrl& url);

		/**
		 * Emitted by device code flow steps when user action is required.
		 * @param verificationUrl URL to visit for authentication.
		 * @param userCode        Code to enter at the URL.
		 * @param expiresInSecs   Seconds until the code expires.
		 */
		void deviceCodeReady(QString verificationUrl, QString userCode, int expiresInSecs);

	  protected:
		Credentials& m_credentials;
	};

} // namespace projt::minecraft::auth
