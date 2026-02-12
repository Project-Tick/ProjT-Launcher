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

#include "LogUploadTask.h"

#include <QMessageBox>
#include <QUrl>

#include "Application.h"
#include "net/NetJob.h"
#include "net/PasteUpload.h"
#include "ui/dialogs/CustomMessageBox.h"

constexpr int MaxMclogsLines	 = 25000;
constexpr int InitialMclogsLines = 10000;
constexpr int FinalMclogsLines	 = 14900;

static QString truncateLogForMclogs(const QString& logContent)
{
	QStringList lines = logContent.split("\n");
	if (lines.size() > MaxMclogsLines)
	{
		QString truncatedLog = lines.mid(0, InitialMclogsLines).join("\n");
		truncatedLog += "\n\n\n\n\n\n\n\n\n\n"
						"------------------------------------------------------------\n"
						"----------------------- Log truncated ----------------------\n"
						"------------------------------------------------------------\n"
						"----- Middle portion omitted to fit mclo.gs size limits ----\n"
						"------------------------------------------------------------\n"
						"\n\n\n\n\n\n\n\n\n\n";
		truncatedLog += lines.mid(lines.size() - FinalMclogsLines - 1).join("\n");
		return truncatedLog;
	}
	return logContent;
}

LogUploadTask::LogUploadTask(const QString& name, const QString& logContent, QWidget* parentWidget)
	: Task(),
	  m_name(name),
	  m_logContent(logContent),
	  m_parentWidget(parentWidget)
{
	setObjectName("LogUploadTask");
}

void LogUploadTask::executeTask()
{
	auto pasteType		= static_cast<PasteUpload::PasteType>(APPLICATION->settings()->get("PastebinType").toInt());
	auto baseURL		= APPLICATION->settings()->get("PastebinCustomAPIBase").toString();
	bool shouldTruncate = false;

	if (baseURL.isEmpty())
		baseURL = PasteUpload::PasteTypes[pasteType].defaultBase;

	// Confirm upload with user
	if (auto url = QUrl(baseURL); url.isValid())
	{
		auto response = CustomMessageBox::selectable(m_parentWidget,
													 tr("Confirm Upload"),
													 tr("You are about to upload \"%1\" to %2.\n"
														"You should double-check for personal information.\n\n"
														"Are you sure?")
														 .arg(m_name, url.host()),
													 QMessageBox::Warning,
													 QMessageBox::Yes | QMessageBox::No,
													 QMessageBox::No)
							->exec();

		if (response != QMessageBox::Yes)
		{
			emitAborted();
			return;
		}

		// Check for mclo.gs truncation
		if (baseURL == "https://api.mclo.gs" && m_logContent.count("\n") > MaxMclogsLines)
		{
			auto truncateResponse =
				CustomMessageBox::selectable(
					m_parentWidget,
					tr("Confirm Truncation"),
					tr("The log has %1 lines, exceeding mclo.gs' limit of %2.\n"
					   "The launcher can keep the first %3 and last %4 lines, trimming the middle.\n\n"
					   "If you choose 'No', mclo.gs will only keep the first %2 lines, cutting off "
					   "potentially useful info like crashes at the end.\n\n"
					   "Proceed with truncation?")
						.arg(m_logContent.count("\n"))
						.arg(MaxMclogsLines)
						.arg(InitialMclogsLines)
						.arg(FinalMclogsLines),
					QMessageBox::Warning,
					QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
					QMessageBox::No)
					->exec();

			if (truncateResponse == QMessageBox::Cancel)
			{
				emitAborted();
				return;
			}
			shouldTruncate = truncateResponse == QMessageBox::Yes;
		}
	}

	QString textToUpload = shouldTruncate ? truncateLogForMclogs(m_logContent) : m_logContent;

	// Create upload job
	m_uploadJob = NetJob::Ptr(new NetJob("Log Upload", APPLICATION->network()));

	auto pasteJob = new PasteUpload(textToUpload, baseURL, pasteType);
	m_uploadJob->addNetAction(Net::NetRequest::Ptr(pasteJob));

	connect(m_uploadJob.get(),
			&Task::succeeded,
			this,
			[this, pasteJob]()
			{
				m_uploadedUrl = pasteJob->pasteLink();
				onUploadSucceeded();
			});

	connect(m_uploadJob.get(), &Task::failed, this, &LogUploadTask::onUploadFailed);
	connect(m_uploadJob.get(), &Task::aborted, this, [this]() { emitAborted(); });

	// Start the upload
	m_uploadJob->start();
}

void LogUploadTask::onUploadSucceeded()
{
	if (m_uploadedUrl.isEmpty())
	{
		emitFailed(tr("Upload succeeded but returned empty URL"));
		return;
	}

	emitSucceeded();
}

void LogUploadTask::onUploadFailed(QString reason)
{
	emitFailed(reason);
}
