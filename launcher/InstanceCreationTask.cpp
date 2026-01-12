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
#include "InstanceCreationTask.h"

#include <QDebug>
#include <QFile>
#include "FileSystem.h"

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

	if (!createInstance())
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
	if (!m_abort)
		emitSucceeded();
}
