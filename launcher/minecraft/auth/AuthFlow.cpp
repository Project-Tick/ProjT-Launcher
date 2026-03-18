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

#include "AuthFlow.hpp"

#include <QDebug>

#include "Application.h"
#include "minecraft/auth/steps/Steps.hpp"

AuthFlow::AuthFlow(AccountData* data, Action action) : Task(), m_legacyData(data)
{
	Q_ASSERT(data != nullptr);

	// Initialize credentials from legacy data if refreshing
	if (action == Action::Refresh && data)
	{
		m_credentials.msaClientId			= data->msaClientID;
		m_credentials.msaToken.accessToken	= data->msaToken.token;
		m_credentials.msaToken.refreshToken = data->msaToken.refresh_token;
		m_credentials.msaToken.issuedAt		= data->msaToken.issueInstant;
		m_credentials.msaToken.expiresAt	= data->msaToken.notAfter;
		m_credentials.msaToken.metadata		= data->msaToken.extra;
	}

	m_pipelineValid = buildPipeline(action);

	if (!m_pipelineValid)
	{
		qWarning() << "AuthFlow: Pipeline build failed for account type" << static_cast<int>(data->type);
	}

	updateState(AccountTaskState::STATE_CREATED);
}

bool AuthFlow::buildPipeline(Action action)
{
	// Explicit handling of non-MSA accounts
	if (m_legacyData->type == AccountType::Offline)
	{
		qDebug() << "AuthFlow: Offline account does not require authentication pipeline";
		// Offline accounts don't need auth steps - this is valid, not an error
		// The caller should check account type before creating AuthFlow
		return false;
	}

	if (m_legacyData->type != AccountType::MSA)
	{
		qWarning() << "AuthFlow: Unsupported account type:" << static_cast<int>(m_legacyData->type);
		return false;
	}

	// Step 1: Microsoft Authentication
	if (action == Action::DeviceCode)
	{
		auto* deviceCodeStep = new projt::minecraft::auth::DeviceCodeAuthStep(m_credentials);
		connect(deviceCodeStep,
				&projt::minecraft::auth::DeviceCodeAuthStep::deviceCodeReady,
				this,
				&AuthFlow::authorizeWithBrowserWithExtra);
		connect(this, &Task::aborted, deviceCodeStep, &projt::minecraft::auth::DeviceCodeAuthStep::cancel);
		m_steps.append(projt::minecraft::auth::Step::Ptr(deviceCodeStep));
	}
	else
	{
		auto* oauthStep = new projt::minecraft::auth::MicrosoftOAuthStep(m_credentials, action == Action::Refresh);
		connect(oauthStep,
				&projt::minecraft::auth::MicrosoftOAuthStep::browserAuthRequired,
				this,
				&AuthFlow::authorizeWithBrowser);
		m_steps.append(projt::minecraft::auth::Step::Ptr(oauthStep));
	}

	// Step 2: Xbox Live User Token
	m_steps.append(projt::minecraft::auth::Step::Ptr(new projt::minecraft::auth::XboxLiveUserStep(m_credentials)));

	// Step 3: Xbox XSTS Token for Xbox Live services
	m_steps.append(projt::minecraft::auth::Step::Ptr(
		new projt::minecraft::auth::XboxSecurityTokenStep(m_credentials,
														  projt::minecraft::auth::XstsTarget::XboxLive)));

	// Step 4: Xbox XSTS Token for Minecraft services
	m_steps.append(projt::minecraft::auth::Step::Ptr(
		new projt::minecraft::auth::XboxSecurityTokenStep(m_credentials,
														  projt::minecraft::auth::XstsTarget::MinecraftServices)));

	// Step 5: Minecraft Services Login (get access token)
	m_steps.append(
		projt::minecraft::auth::Step::Ptr(new projt::minecraft::auth::MinecraftServicesLoginStep(m_credentials)));

	// Step 6: Xbox Profile (optional, for display - gamertag extraction)
	m_steps.append(projt::minecraft::auth::Step::Ptr(new projt::minecraft::auth::XboxProfileFetchStep(m_credentials)));

	// Step 7: Game Entitlements
	m_steps.append(projt::minecraft::auth::Step::Ptr(new projt::minecraft::auth::GameEntitlementsStep(m_credentials)));

	// Step 8: Minecraft Profile
	m_steps.append(
		projt::minecraft::auth::Step::Ptr(new projt::minecraft::auth::MinecraftProfileFetchStep(m_credentials)));

	// Step 9: Skin Download
	m_steps.append(projt::minecraft::auth::Step::Ptr(new projt::minecraft::auth::SkinDownloadStep(m_credentials)));

	qDebug() << "AuthFlow: Built pipeline with" << m_steps.size() << "steps";
	return true;
}

void AuthFlow::executeTask()
{
	// Handle offline accounts - they don't need authentication
	if (m_legacyData->type == AccountType::Offline)
	{
		qDebug() << "AuthFlow: Offline account - no authentication required, succeeding immediately";
		if (m_legacyData)
		{
			m_legacyData->accountState = AccountState::Online;
		}
		updateState(AccountTaskState::STATE_SUCCEEDED, tr("Offline account ready"));
		return;
	}

	// Early fail for invalid pipeline (non-offline accounts)
	if (!m_pipelineValid)
	{
		failWithState(AccountTaskState::STATE_FAILED_HARD,
					  tr("Failed to build authentication pipeline for this account type"));
		return;
	}

	// Sanity check: empty pipeline should not succeed silently
	if (m_steps.isEmpty())
	{
		qWarning() << "AuthFlow: Pipeline is empty after successful build - this is a bug";
		failWithState(AccountTaskState::STATE_FAILED_HARD, tr("Authentication pipeline is empty (internal error)"));
		return;
	}

	updateState(AccountTaskState::STATE_WORKING, tr("Initializing"));
	executeNextStep();
}

void AuthFlow::executeNextStep()
{
	// Check abort flag before starting new step
	if (m_aborted)
	{
		qDebug() << "AuthFlow: Skipping next step - flow was aborted";
		return;
	}

	if (!Task::isRunning())
	{
		return;
	}

	if (m_steps.isEmpty())
	{
		m_currentStep.reset();
		succeed();
		return;
	}

	m_currentStep = m_steps.front();
	m_steps.pop_front();

	qDebug() << "AuthFlow:" << m_currentStep->description();

	connect(m_currentStep.get(), &projt::minecraft::auth::Step::completed, this, &AuthFlow::onStepCompleted);

	m_currentStep->execute();
}

void AuthFlow::onStepCompleted(projt::minecraft::auth::StepResult result, QString message)
{
	// Map step result to flow state
	// Note: StepResult::Succeeded means "step succeeded, continue flow"
	// The flow itself only succeeds when all steps complete (pipeline empty)
	const auto flowState = stepResultToFlowState(result);

	if (updateState(flowState, message))
	{
		executeNextStep();
	}
}

void AuthFlow::succeed()
{
	// Sync new credentials back to legacy AccountData
	if (m_legacyData)
	{
		m_legacyData->msaClientID			 = m_credentials.msaClientId;
		m_legacyData->msaToken.token		 = m_credentials.msaToken.accessToken;
		m_legacyData->msaToken.refresh_token = m_credentials.msaToken.refreshToken;
		m_legacyData->msaToken.issueInstant	 = m_credentials.msaToken.issuedAt;
		m_legacyData->msaToken.notAfter		 = m_credentials.msaToken.expiresAt;
		m_legacyData->msaToken.extra		 = m_credentials.msaToken.metadata;
		m_legacyData->msaToken.validity		 = toValidity(m_credentials.msaToken.validity);

		m_legacyData->userToken.token		 = m_credentials.xboxUserToken.accessToken;
		m_legacyData->userToken.issueInstant = m_credentials.xboxUserToken.issuedAt;
		m_legacyData->userToken.notAfter	 = m_credentials.xboxUserToken.expiresAt;
		m_legacyData->userToken.extra		 = m_credentials.xboxUserToken.metadata;
		m_legacyData->userToken.validity	 = toValidity(m_credentials.xboxUserToken.validity);

		// xboxApiToken receives gamertag from XboxProfileFetchStep via xboxServiceToken.metadata
		m_legacyData->xboxApiToken.token		= m_credentials.xboxServiceToken.accessToken;
		m_legacyData->xboxApiToken.issueInstant = m_credentials.xboxServiceToken.issuedAt;
		m_legacyData->xboxApiToken.notAfter		= m_credentials.xboxServiceToken.expiresAt;
		m_legacyData->xboxApiToken.extra		= m_credentials.xboxServiceToken.metadata;
		m_legacyData->xboxApiToken.validity		= toValidity(m_credentials.xboxServiceToken.validity);

		m_legacyData->mojangservicesToken.token		   = m_credentials.minecraftServicesToken.accessToken;
		m_legacyData->mojangservicesToken.issueInstant = m_credentials.minecraftServicesToken.issuedAt;
		m_legacyData->mojangservicesToken.notAfter	   = m_credentials.minecraftServicesToken.expiresAt;
		m_legacyData->mojangservicesToken.extra		   = m_credentials.minecraftServicesToken.metadata;
		m_legacyData->mojangservicesToken.validity	   = toValidity(m_credentials.minecraftServicesToken.validity);

		m_legacyData->yggdrasilToken.token		  = m_credentials.minecraftAccessToken.accessToken;
		m_legacyData->yggdrasilToken.issueInstant = m_credentials.minecraftAccessToken.issuedAt;
		m_legacyData->yggdrasilToken.notAfter	  = m_credentials.minecraftAccessToken.expiresAt;
		m_legacyData->yggdrasilToken.validity	  = toValidity(m_credentials.minecraftAccessToken.validity);

		m_legacyData->minecraftProfile.id			= m_credentials.profile.id;
		m_legacyData->minecraftProfile.name			= m_credentials.profile.name;
		m_legacyData->minecraftProfile.skin.id		= m_credentials.profile.skin.id;
		m_legacyData->minecraftProfile.skin.url		= m_credentials.profile.skin.url;
		m_legacyData->minecraftProfile.skin.variant = m_credentials.profile.skin.variant;
		m_legacyData->minecraftProfile.skin.data	= m_credentials.profile.skin.imageData;
		m_legacyData->minecraftProfile.validity		= toValidity(m_credentials.profile.validity);
		m_legacyData->minecraftProfile.currentCape	= m_credentials.profile.activeCapeId;

		// Sync capes
		m_legacyData->minecraftProfile.capes.clear();
		for (auto it = m_credentials.profile.capes.begin(); it != m_credentials.profile.capes.end(); ++it)
		{
			const auto& capeIn = it.value();
			Cape capeOut;
			capeOut.id	  = capeIn.id;
			capeOut.url	  = capeIn.url;
			capeOut.alias = capeIn.alias;
			capeOut.data  = capeIn.imageData;
			m_legacyData->minecraftProfile.capes.insert(capeIn.id, capeOut);
		}

		m_legacyData->minecraftEntitlement.ownsMinecraft	= m_credentials.entitlements.ownsMinecraft;
		m_legacyData->minecraftEntitlement.canPlayMinecraft = m_credentials.entitlements.canPlayMinecraft;
		m_legacyData->minecraftEntitlement.validity			= toValidity(m_credentials.entitlements.validity);

		m_legacyData->validity_ = Validity::Certain;
	}

	updateState(AccountTaskState::STATE_SUCCEEDED, tr("Finished all authentication steps"));
}

void AuthFlow::failWithState(AccountTaskState state, const QString& reason)
{
	if (m_legacyData)
	{
		m_legacyData->errorString = reason;
	}
	updateState(state, reason);
}

bool AuthFlow::updateState(AccountTaskState newState, const QString& reason)
{
	m_taskState = newState;
	setDetails(reason);

	switch (newState)
	{
		case AccountTaskState::STATE_CREATED:
			setStatus(tr("Waiting..."));
			if (m_legacyData)
			{
				m_legacyData->errorString.clear();
			}
			return true;

		case AccountTaskState::STATE_WORKING:
			setStatus(m_currentStep ? m_currentStep->description() : tr("Working..."));
			if (m_legacyData)
			{
				m_legacyData->accountState = AccountState::Working;
			}
			return true;

		case AccountTaskState::STATE_SUCCEEDED:
			setStatus(tr("Authentication task succeeded."));
			if (m_legacyData)
			{
				m_legacyData->accountState = AccountState::Online;
			}
			emitSucceeded();
			return false;

		case AccountTaskState::STATE_OFFLINE:
			setStatus(tr("Failed to contact the authentication server."));
			if (m_legacyData)
			{
				m_legacyData->errorString  = reason;
				m_legacyData->accountState = AccountState::Offline;
			}
			emitFailed(reason);
			return false;

		case AccountTaskState::STATE_DISABLED:
			setStatus(tr("Client ID has changed. New session needs to be created."));
			if (m_legacyData)
			{
				m_legacyData->errorString  = reason;
				m_legacyData->accountState = AccountState::Disabled;
			}
			emitFailed(reason);
			return false;

		case AccountTaskState::STATE_FAILED_SOFT:
			setStatus(tr("Encountered an error during authentication."));
			if (m_legacyData)
			{
				m_legacyData->errorString  = reason;
				m_legacyData->accountState = AccountState::Errored;
			}
			emitFailed(reason);
			return false;

		case AccountTaskState::STATE_FAILED_HARD:
			setStatus(tr("Failed to authenticate. The session has expired."));
			if (m_legacyData)
			{
				m_legacyData->errorString  = reason;
				m_legacyData->accountState = AccountState::Expired;
			}
			emitFailed(reason);
			return false;

		case AccountTaskState::STATE_FAILED_GONE:
			setStatus(tr("Failed to authenticate. The account no longer exists."));
			if (m_legacyData)
			{
				m_legacyData->errorString  = reason;
				m_legacyData->accountState = AccountState::Gone;
			}
			emitFailed(reason);
			return false;

		default:
			setStatus(tr("..."));
			const QString error = tr("Unknown account task state: %1").arg(static_cast<int>(newState));
			if (m_legacyData)
			{
				m_legacyData->accountState = AccountState::Errored;
			}
			emitFailed(error);
			return false;
	}
}

AccountTaskState AuthFlow::stepResultToFlowState(projt::minecraft::auth::StepResult result) noexcept
{
	// StepResult::Continue and StepResult::Succeeded both mean "step completed successfully"
	// The distinction is semantic: Continue hints more steps may follow, Succeeded suggests finality.
	// At the flow level, both translate to STATE_WORKING until the pipeline is exhausted.
	//
	// Future: If we add optional/best-effort steps, we may want a StepResult::Skipped that
	// also maps to STATE_WORKING but logs differently.

	switch (result)
	{
		case projt::minecraft::auth::StepResult::Continue:
		case projt::minecraft::auth::StepResult::Succeeded: return AccountTaskState::STATE_WORKING;

		case projt::minecraft::auth::StepResult::Offline: return AccountTaskState::STATE_OFFLINE;

		case projt::minecraft::auth::StepResult::SoftFailure: return AccountTaskState::STATE_FAILED_SOFT;

		case projt::minecraft::auth::StepResult::HardFailure: return AccountTaskState::STATE_FAILED_HARD;

		case projt::minecraft::auth::StepResult::Disabled: return AccountTaskState::STATE_DISABLED;

		case projt::minecraft::auth::StepResult::Gone: return AccountTaskState::STATE_FAILED_GONE;
	}

	return AccountTaskState::STATE_FAILED_HARD;
}

Validity AuthFlow::toValidity(projt::minecraft::auth::TokenValidity validity) noexcept
{
	switch (validity)
	{
		case projt::minecraft::auth::TokenValidity::None: return Validity::None;
		case projt::minecraft::auth::TokenValidity::Assumed: return Validity::Assumed;
		case projt::minecraft::auth::TokenValidity::Certain: return Validity::Certain;
	}
	return Validity::None;
}

bool AuthFlow::abort()
{
	// Set abort flag to prevent new steps from starting
	m_aborted = true;

	qDebug() << "AuthFlow: Abort requested";

	// Cancel current step BEFORE emitting aborted (to prevent use-after-free)
	// The emitAborted() signal may cause this object to be destroyed
	if (m_currentStep)
	{
		// Disconnect to prevent callbacks after abort
		disconnect(m_currentStep.get(), nullptr, this, nullptr);
		m_currentStep->cancel();
		m_currentStep.reset();
	}

	// Clear remaining steps
	m_steps.clear();

	emitAborted();

	return true;
}