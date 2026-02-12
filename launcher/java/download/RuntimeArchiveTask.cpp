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
#include "java/download/RuntimeArchiveTask.hpp"

#include <QDir>
#include <QFile>
#include <memory>
#include <quazip.h>

#include "Application.h"
#include "MMCZip.h"
#include "Untar.h"
#include "net/ChecksumValidator.h"
#include "net/NetJob.h"

namespace projt::java
{
	RuntimeArchiveTask::RuntimeArchiveTask(QUrl url, QString finalPath, QString checksumType, QString checksumHash)
		: m_url(url),
		  m_final_path(std::move(finalPath)),
		  m_checksum_type(std::move(checksumType)),
		  m_checksum_hash(std::move(checksumHash))
	{}

	void RuntimeArchiveTask::executeTask()
	{
		setStatus(tr("Downloading Java"));

		MetaEntryPtr entry = APPLICATION->metacache()->resolveEntry("java", m_url.fileName());

		auto download = makeShared<NetJob>(QString("JRE::DownloadJava"), APPLICATION->network());
		auto action	  = Net::Download::makeCached(m_url, entry);
		if (!m_checksum_hash.isEmpty() && !m_checksum_type.isEmpty())
		{
			auto hashType = QCryptographicHash::Algorithm::Sha1;
			if (m_checksum_type == "sha256")
			{
				hashType = QCryptographicHash::Algorithm::Sha256;
			}
			action->addValidator(new Net::ChecksumValidator(hashType, QByteArray::fromHex(m_checksum_hash.toUtf8())));
		}
		download->addNetAction(action);
		auto fullPath = entry->getFullPath();

		connect(download.get(), &Task::failed, this, &RuntimeArchiveTask::emitFailed);
		connect(download.get(), &Task::progress, this, &RuntimeArchiveTask::setProgress);
		connect(download.get(), &Task::stepProgress, this, &RuntimeArchiveTask::propagateStepProgress);
		connect(download.get(), &Task::status, this, &RuntimeArchiveTask::setStatus);
		connect(download.get(), &Task::details, this, &RuntimeArchiveTask::setDetails);
		connect(download.get(), &Task::aborted, this, &RuntimeArchiveTask::emitAborted);
		connect(download.get(), &Task::succeeded, this, [this, fullPath] { extractRuntime(fullPath); });
		m_task = download;
		m_task->start();
	}

	void RuntimeArchiveTask::extractRuntime(QString input)
	{
		setStatus(tr("Extracting Java"));
		if (input.endsWith("tar"))
		{
			setStatus(tr("Extracting Java (Progress is not reported for tar archives)"));
			QFile in(input);
			if (!in.open(QFile::ReadOnly))
			{
				emitFailed(tr("Unable to open supplied tar file."));
				return;
			}
			if (!Tar::extract(&in, QDir(m_final_path).absolutePath()))
			{
				emitFailed(tr("Unable to extract supplied tar file."));
				return;
			}
			emitSucceeded();
			return;
		}
		if (input.endsWith("tar.gz") || input.endsWith("taz") || input.endsWith("tgz"))
		{
			setStatus(tr("Extracting Java (Progress is not reported for tar archives)"));
			if (!GZTar::extract(input, QDir(m_final_path).absolutePath()))
			{
				emitFailed(tr("Unable to extract supplied tar file."));
				return;
			}
			emitSucceeded();
			return;
		}
		if (input.endsWith("zip"))
		{
			auto zip = std::make_shared<QuaZip>(input);
			if (!zip->open(QuaZip::mdUnzip))
			{
				emitFailed(tr("Unable to open supplied zip file."));
				return;
			}
			auto files = zip->getFileNameList();
			if (files.isEmpty())
			{
				emitFailed(tr("No files were found in the supplied zip file."));
				return;
			}
			m_task = makeShared<MMCZip::ExtractZipTask>(zip, m_final_path, files[0]);

			auto progressStep = std::make_shared<TaskStepProgress>();
			connect(m_task.get(),
					&Task::finished,
					this,
					[this, progressStep]
					{
						progressStep->state = TaskStepState::Succeeded;
						stepProgress(*progressStep);
					});

			connect(m_task.get(), &Task::succeeded, this, &RuntimeArchiveTask::emitSucceeded);
			connect(m_task.get(), &Task::aborted, this, &RuntimeArchiveTask::emitAborted);
			connect(m_task.get(),
					&Task::failed,
					this,
					[this, progressStep](QString reason)
					{
						progressStep->state = TaskStepState::Failed;
						stepProgress(*progressStep);
						emitFailed(reason);
					});
			connect(m_task.get(), &Task::stepProgress, this, &RuntimeArchiveTask::propagateStepProgress);

			connect(m_task.get(),
					&Task::progress,
					this,
					[this, progressStep](qint64 current, qint64 total)
					{
						progressStep->update(current, total);
						stepProgress(*progressStep);
					});
			connect(m_task.get(),
					&Task::status,
					this,
					[this, progressStep](QString status)
					{
						progressStep->status = status;
						stepProgress(*progressStep);
					});
			m_task->start();
			return;
		}

		emitFailed(tr("Could not determine archive type!"));
	}

	bool RuntimeArchiveTask::abort()
	{
		auto aborted = canAbort();
		if (m_task)
			aborted = m_task->abort();
		return aborted;
	}
} // namespace projt::java
