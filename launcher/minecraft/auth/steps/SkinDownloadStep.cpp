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

#include "SkinDownloadStep.hpp"

#include <QNetworkRequest>

#include "Application.h"

namespace projt::minecraft::auth
{

	SkinDownloadStep::SkinDownloadStep(Credentials& credentials) noexcept : Step(credentials)
	{}

	QString SkinDownloadStep::description() const
	{
		return tr("Downloading player skin.");
	}

	void SkinDownloadStep::execute()
	{
		// Skip if no skin URL available
		if (m_credentials.profile.skin.url.isEmpty())
		{
			emit completed(StepResult::Continue, tr("No skin to download."));
			return;
		}

		const QUrl url(m_credentials.profile.skin.url);

		m_response = std::make_shared<QByteArray>();
		m_request  = Net::Download::makeByteArray(url, m_response);

		m_task = NetJob::Ptr::create(QStringLiteral("SkinDownload"), APPLICATION->network());
		m_task->setAskRetry(false);
		m_task->addNetAction(m_request);

		connect(m_task.get(), &Task::finished, this, &SkinDownloadStep::onRequestCompleted);
		m_task->start();
	}

	void SkinDownloadStep::onRequestCompleted()
	{
		// Skin download is optional - always continue regardless of result
		if (m_request->error() == QNetworkReply::NoError)
		{
			m_credentials.profile.skin.imageData = *m_response;
			emit completed(StepResult::Continue, tr("Got player skin."));
		}
		else
		{
			qWarning() << "Failed to download skin:" << m_request->errorString();
			emit completed(StepResult::Continue, tr("Skin download failed (continuing)."));
		}
	}

} // namespace projt::minecraft::auth
