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
#include "AssetUpdateTask.h"

#include "BuildConfig.h"
#include "launch/LaunchStage.hpp"
#include "minecraft/AssetsUtils.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "net/ChecksumValidator.h"

#include "Application.h"

#include "net/ApiDownload.h"

AssetUpdateTask::AssetUpdateTask(MinecraftInstance* inst)
{
	m_inst = inst;
}

void AssetUpdateTask::executeTask()
{
	setStatus(tr("Updating assets index..."));
	auto components	  = m_inst->getPackProfile();
	auto profile	  = components->getProfile();
	auto assets		  = profile->getMinecraftAssets();
	QUrl indexUrl	  = assets->url;
	QString localPath = assets->id + ".json";
	auto job		  = makeShared<NetJob>(tr("Asset index for %1").arg(m_inst->name()), APPLICATION->network());

	auto metacache = APPLICATION->metacache();
	auto entry	   = metacache->resolveEntry("asset_indexes", localPath);
	entry->setStale(true);
	auto hexSha1 = assets->sha1.toLatin1();
	qDebug() << "Asset index SHA1:" << hexSha1;
	auto dl = Net::ApiDownload::makeCached(indexUrl, entry);
	dl->addValidator(new Net::ChecksumValidator(QCryptographicHash::Sha1, assets->sha1));
	job->addNetAction(dl);

	downloadJob.reset(job);

	connect(downloadJob.get(), &NetJob::succeeded, this, &AssetUpdateTask::assetIndexFinished);
	connect(downloadJob.get(), &NetJob::failed, this, &AssetUpdateTask::assetIndexFailed);
	connect(downloadJob.get(), &NetJob::aborted, this, &AssetUpdateTask::emitAborted);
	connect(downloadJob.get(), &NetJob::progress, this, &AssetUpdateTask::progress);
	connect(downloadJob.get(), &NetJob::stepProgress, this, &AssetUpdateTask::propagateStepProgress);

	qDebug() << m_inst->name() << ": Starting asset index download";
	downloadJob->start();
}

bool AssetUpdateTask::canAbort() const
{
	return true;
}

void AssetUpdateTask::assetIndexFinished()
{
	AssetsIndex index;
	qDebug() << m_inst->name() << ": Finished asset index download";

	auto components = m_inst->getPackProfile();
	auto profile	= components->getProfile();
	auto assets		= profile->getMinecraftAssets();

	QString asset_fname = "assets/indexes/" + assets->id + ".json";
	// NOTE: Current validation is done via AssetsUtils::loadAssetsIndexJson.
	// Future improvement: Implement a generic validator based on JSON schema for more robust checks.
	if (!AssetsUtils::loadAssetsIndexJson(assets->id, asset_fname, index))
	{
		auto metacache = APPLICATION->metacache();
		auto entry	   = metacache->resolveEntry("asset_indexes", assets->id + ".json");
		metacache->evictEntry(entry);
		emitFailed(tr("Failed to read the assets index!"));
	}

	auto job = index.getDownloadJob();
	if (job)
	{
		QString resourceURLStr = APPLICATION->settings()->get("ResourceURL").toString();
		QString source		   = tr("Mojang");
		if (!resourceURLStr.isEmpty() && resourceURLStr != BuildConfig.DEFAULT_RESOURCE_BASE)
		{
			source = QUrl(resourceURLStr).host();
		}
		setStatus(tr("Getting the asset files from %1...").arg(source));
		downloadJob = job;
		connect(downloadJob.get(), &NetJob::succeeded, this, &AssetUpdateTask::emitSucceeded);
		connect(downloadJob.get(), &NetJob::failed, this, &AssetUpdateTask::assetsFailed);
		connect(downloadJob.get(), &NetJob::aborted, this, &AssetUpdateTask::emitAborted);
		connect(downloadJob.get(), &NetJob::progress, this, &AssetUpdateTask::progress);
		connect(downloadJob.get(), &NetJob::stepProgress, this, &AssetUpdateTask::propagateStepProgress);
		downloadJob->start();
		return;
	}
	emitSucceeded();
}

void AssetUpdateTask::assetIndexFailed(QString reason)
{
	qDebug() << m_inst->name() << ": Failed asset index download";
	emitFailed(tr("Failed to download the assets index:\n%1").arg(reason));
}

void AssetUpdateTask::assetsFailed(QString reason)
{
	emitFailed(tr("Failed to download assets:\n%1").arg(reason));
}

bool AssetUpdateTask::abort()
{
	if (downloadJob)
	{
		return downloadJob->abort();
	}
	else
	{
		qWarning() << "Prematurely aborted AssetUpdateTask";
	}
	return true;
}
