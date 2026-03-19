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
#include "InstanceCreationTask.h"

#include <QDebug>
#include <QFile>

#include "FileSystem.h"
#include "minecraft/MinecraftLoadAndCheck.h"
#include "tasks/SequentialTask.h"

bool InstanceCreationTask::abort()
{
	if (!canAbort())
		return false;

	m_abort = true;
	if (m_gameFilesTask)
		return m_gameFilesTask->abort();

	return true;
}

void InstanceCreationTask::executeTask()
{
	setAbortable(true);

	if (updateInstance())
	{
		emitSucceeded();
		return;
	}

	// When the user aborted in the update stage.
	if (m_abort)
	{
		emitAborted();
		return;
	}

	m_createdInstance = createInstance();
	if (!m_createdInstance)
	{
		if (m_abort)
			return;

		qWarning() << "Instance creation failed!";
		if (!m_error_message.isEmpty())
		{
			qWarning() << "Reason: " << m_error_message;
			emitFailed(tr("Error while creating new instance:\n%1").arg(m_error_message));
		}
		else
		{
			emitFailed(tr("Error while creating new instance."));
		}

		return;
	}

	// If this is set, it means we're updating an instance. So, we now need to remove the
	// files scheduled to, and we'd better not let the user abort in the middle of it, since it'd
	// put the instance in an invalid state.
	if (shouldOverride())
	{
		bool deleteFailed = false;

		setAbortable(false);
		setStatus(tr("Removing old conflicting files..."));
		qDebug() << "Removing old files";

		for (const QString& path : m_files_to_remove)
		{
			if (!QFile::exists(path))
				continue;

			qDebug() << "Removing" << path;

			if (!QFile::remove(path))
			{
				qCritical() << "Could not remove" << path;
				deleteFailed = true;
			}
		}

		if (deleteFailed)
		{
			emitFailed(tr("Failed to remove old conflicting files."));
			return;
		}
	}

	if (m_abort)
		return;

	m_createdInstance->saveNow();

	setAbortable(true);
	setAbortButtonText(tr("Skip"));
	qDebug() << "Downloading game files";

	auto updateTasks = m_createdInstance->createUpdateTask();
	if (updateTasks.isEmpty())
	{
		emitSucceeded();
		return;
	}

	auto task = makeShared<SequentialTask>();
	task->addTask(makeShared<MinecraftLoadAndCheck>(m_createdInstance.get(), Net::Mode::Online));
	for (const auto& updateTask : updateTasks)
	{
		task->addTask(updateTask);
	}
	connect(task.get(),
			&Task::finished,
			this,
			[this, task]
			{
				if (task->wasSuccessful() || m_abort)
				{
					emitSucceeded();
				}
				else
				{
					emitFailed(tr("Could not download game files: %1").arg(task->failReason()));
				}
			});
	connect(task.get(), &Task::progress, this, &InstanceCreationTask::setProgress);
	connect(task.get(), &Task::stepProgress, this, &InstanceCreationTask::propagateStepProgress);
	connect(task.get(), &Task::status, this, &InstanceCreationTask::setStatus);
	connect(task.get(), &Task::details, this, &InstanceCreationTask::setDetails);

	setDetails(tr("Downloading game files"));
	m_gameFilesTask = task;
	m_gameFilesTask->start();
}
