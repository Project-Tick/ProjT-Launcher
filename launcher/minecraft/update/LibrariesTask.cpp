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
#include "LibrariesTask.h"

#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"

#include "Application.h"

LibrariesTask::LibrariesTask(MinecraftInstance* inst)
{
	m_inst = inst;
}

void LibrariesTask::executeTask()
{
	setStatus(tr("Downloading required library files..."));
	qDebug() << m_inst->name() << ": downloading libraries";
	MinecraftInstance* inst = (MinecraftInstance*)m_inst;

	// Build a list of URLs that will need to be downloaded.
	auto components = inst->getPackProfile();
	auto profile	= components->getProfile();

	NetJob::Ptr job{ new NetJob(tr("Libraries for instance %1").arg(inst->name()), APPLICATION->network()) };
	downloadJob.reset(job);

	auto metacache = APPLICATION->metacache();

	auto processArtifactPool =
		[this, inst, metacache](const QList<LibraryPtr>& pool, QStringList& errors, const QString& localPath)
	{
		for (auto lib : pool)
		{
			if (!lib)
			{
				emitFailed(tr("Null jar is specified in the metadata, aborting."));
				return false;
			}
			auto dls = lib->getDownloads(inst->runtimeContext(), metacache.get(), errors, localPath);
			for (auto dl : dls)
			{
				downloadJob->addNetAction(dl);
			}
		}
		return true;
	};

	QStringList failedLocalLibraries;
	QList<LibraryPtr> libArtifactPool;
	libArtifactPool.append(profile->getLibraries());
	libArtifactPool.append(profile->getNativeLibraries());
	libArtifactPool.append(profile->getMavenFiles());
	for (auto agent : profile->getAgents())
	{
		libArtifactPool.append(agent->library());
	}
	libArtifactPool.append(profile->getMainJar());
	processArtifactPool(libArtifactPool, failedLocalLibraries, inst->getLocalLibraryPath());

	QStringList failedLocalJarMods;
	processArtifactPool(profile->getJarMods(), failedLocalJarMods, inst->jarModsDir());

	if (!failedLocalJarMods.empty() || !failedLocalLibraries.empty())
	{
		downloadJob.reset();
		QString failed_all = (failedLocalLibraries + failedLocalJarMods).join("\n");
		emitFailed(tr("Some artifacts marked as 'local' are missing their files:\n%1\n\nYou need to either add the "
					  "files, or removed the "
					  "packages that require them.\nYou'll have to correct this problem manually.")
					   .arg(failed_all));
		return;
	}

	connect(downloadJob.get(), &NetJob::succeeded, this, &LibrariesTask::emitSucceeded);
	connect(downloadJob.get(), &NetJob::failed, this, &LibrariesTask::jarlibFailed);
	connect(downloadJob.get(), &NetJob::aborted, this, &LibrariesTask::emitAborted);
	connect(downloadJob.get(), &NetJob::progress, this, &LibrariesTask::progress);
	connect(downloadJob.get(), &NetJob::stepProgress, this, &LibrariesTask::propagateStepProgress);

	downloadJob->start();
}

bool LibrariesTask::canAbort() const
{
	return true;
}

void LibrariesTask::jarlibFailed(QString reason)
{
	emitFailed(tr("Game update failed: it was impossible to fetch the required libraries.\nReason:\n%1").arg(reason));
}

bool LibrariesTask::abort()
{
	if (downloadJob)
	{
		return downloadJob->abort();
	}
	else
	{
		qWarning() << "Prematurely aborted LibrariesTask";
	}
	return true;
}
